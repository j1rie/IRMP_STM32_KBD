/*
 * irmphidkbd.c: An IRMP HID keyboard plugin for the Video Disk Recorder
 *
 * Copyright (C) 20025-2025 Joerg Riechardt <J.Riechardt@gmx.de>
 *
 */

#include <vdr/plugin.h>
#include <linux/input.h>
#include <vdr/i18n.h>
#include <vdr/remote.h>
#include <vdr/thread.h>
#include "input-event-codes.h"

static const char *VERSION        = "0.0.1";
static const char *DESCRIPTION    = tr("Send keypresses from IRMP HID-KBD-Device to VDR");

const char* kbd_device = "/dev/irmp_stm32_kbd_event";

uint8_t debug = 1;

class cIrmpRemote : public cRemote, private cThread {
private:
  virtual void Action(void) override;
public:
  cIrmpRemote(const char *Name);
  virtual bool Initialize(void) override;
  virtual bool Stop();
  int fd;
};

cIrmpRemote::cIrmpRemote(const char *Name)
:cRemote(Name)
{
  Initialize();
  Start();
}

bool cIrmpRemote::Initialize()
{
  fd = open(kbd_device, O_RDONLY | O_NONBLOCK);
  if(fd == -1){
    printf("Cannot open %s. %s.\n", kbd_device, strerror(errno));
    //return false;
  } else {
    printf("opened %s\n", kbd_device);
  }

  /*if(ioctl(fd, EVIOCGRAB, 1)){
    printf("Cannot grab %s. %s.\n", kbd_device, strerror(errno));
  } else {
    if(debug) printf("Grabbed %s!\n", kbd_device);
  }*/

  return true;
}

bool cIrmpRemote::Stop()
{
  //ioctl(fd, EVIOCGRAB, 0);
  if (fd) // ??
	close(fd);
  return true;
}

void cIrmpRemote::Action(void)
{
  if(debug) printf("action!\n");
  struct input_event event;
  uint8_t magic_key = 173;
  uint8_t only_once = 1;
  uint8_t release_needed = 0, repeat = 0, pair = 0, skip = 0;
  long int this_time = 0, last_sent = 0, last_received = 0;
  uint16_t timeout = 160, code = 0, code2 = 0;
  struct timespec now; // TODO unstellen auf VDR's cTimeMs
  int Delta; // the time between two subsequent LIRC events
  int RepeatRate = 100000;

	while(1){
		usleep(1000); // don't eat too much cpu
		if (fd < 0) {
			fd = open(kbd_device, O_RDONLY | O_NONBLOCK);
			if(fd == -1){
				printf("Cannot open %s. %s.\n", kbd_device, strerror(errno));
				sleep(10);
				continue;
			} else {
				printf("opened %s\n", kbd_device);
			}
		}

		if (read(fd, &event, sizeof(event)) != -1) {
			if (event.type == EV_KEY && event.value == 1) { // keypress
				if(debug) printf("read %ld %d %d --- ", this_time, event.code, pair);
				clock_gettime(CLOCK_MONOTONIC, &now);
				this_time = now.tv_sec * 1000 + now.tv_nsec / 1000 / 1000;
				pair = (this_time - last_received < 10)? 1 : 0; // modifier+key, TODO: process modifier
				if(only_once && event.code == magic_key) {
					FILE *out = fopen("/var/log/started_by_IRMP_STM32_KBD", "a");
					time_t date = time(NULL);
					struct tm *ts = localtime(&date);
					fprintf(out, "%s", asctime(ts));
					fclose(out);
					only_once = 0;
				}
				Delta = this_time - last_received;
				if (debug) printf("Delta: %d\n", Delta);
				if (RepeatRate > Delta)
					RepeatRate = Delta; // determine repeat rate
				if(!pair){
					last_received = this_time;
					if(Delta > timeout || Delta > RepeatRate * 11 / 10) { // new key
						printf("Neuer\n");
						repeat = 0;
						skip = 0;
					} else { // repeat
						printf("Repeat\n");
						if( (!repeat && (this_time - last_sent < Setup.RcRepeatDelay)) || (repeat && (this_time - last_sent) < Setup.RcRepeatDelta)) {
							skip = 1;
							continue; // don't send key
						} else {
							repeat = 1;
							skip = 0;
							timeout = Delta * 11 / 10; // 10 % more should be enough

						}
					}
				}

				/* send key */
				if (!skip){
					if(!pair) {
						code = event.code;
					} else {
						code2 = event.code;
					}
					if(debug) printf("delta: %ld ", this_time - last_sent); /// TODO checken!!!
					cRemote::Put(evkeys[event.code], repeat);
					last_sent = this_time;
					if(debug) printf("put code: %s, %s\n", evkeys[event.code], repeat ? "repeat" : "first");
					release_needed = 1;
				}
			}
		}

		/* send release */
		clock_gettime(CLOCK_MONOTONIC, &now);
		this_time = now.tv_sec * 1000 + now.tv_nsec / 1000 / 1000;
		if((this_time - last_received > timeout) && release_needed) { // && repeat ??
			release_needed = 0;
			cRemote::Put(evkeys[code], false, true);
			if(debug) printf("put  %ld %d %d --- put code: %s, release\n\n", this_time, code, pair, evkeys[code]);
			if (pair) {
				cRemote::Put(evkeys[code2], false, true);
				if(debug) printf("put  %ld %d %d     put code: %s, release\n\n", this_time, code2, pair, evkeys[code2]);
			}
		}
	}
}

class cPluginIrmphidkbd : public cPlugin {
public:
  cPluginIrmphidkbd(void);
  virtual ~cPluginIrmphidkbd() override;
  virtual const char *Version(void) override { return VERSION; }
  virtual const char *Description(void) override { return DESCRIPTION; }
  virtual const char *CommandLineHelp(void) override;
  virtual bool ProcessArgs(int argc, char *argv[]) override;
  virtual bool Start(void) override;
};

cPluginIrmphidkbd::cPluginIrmphidkbd(void)
{
}

cPluginIrmphidkbd::~cPluginIrmphidkbd()
{
}

const char *cPluginIrmphidkbd::CommandLineHelp(void)
{
  return "  input event device (/dev/input/event... )\n"
         "  default /dev/irmp_stm32_kbd_event\n";
}

bool cPluginIrmphidkbd::ProcessArgs(int argc, char *argv[])
{
  if(argc > 1) kbd_device = argv[1];

  return true;
}

bool cPluginIrmphidkbd::Start(void)
{
  new cIrmpRemote("IRMP_KBD");
  return true;
}


VDRPLUGINCREATOR(cPluginIrmphidkbd); // Don't touch this!
