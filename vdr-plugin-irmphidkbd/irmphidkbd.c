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
  cTimeMs FirstTime;
  cTimeMs LastTime;
  cTimeMs ThisTime;
  struct input_event event;
  uint8_t magic_key = 173;
  uint8_t only_once = 1;
  bool pressed = false;
  bool repeat = false;
  bool pair = false;
  uint16_t timeout = 160, code = 0, code2 = 0;
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
				//if(debug) printf("read %ld %d %d --- ", this_time, event.code, pair);
				pair = (LastTime.Elapsed() < 10)? 1 : 0; // modifier+key, TODO: process modifier // ??
				if(only_once && event.code == magic_key) {
					if(debug) printf("magic\n");
					FILE *out = fopen("/var/log/started_by_IRMP_STM32_KBD", "a");
					time_t date = time(NULL);
					struct tm *ts = localtime(&date);
					fprintf(out, "%s", asctime(ts));
					fclose(out);
					only_once = 0;
				}
				int Delta = ThisTime.Elapsed();
				if (debug) printf("Delta: %d\n", Delta);
				if (RepeatRate > Delta)
					RepeatRate = Delta; // determine repeat rate
				if(!pair){
					ThisTime.Set();
					if(Delta > timeout || Delta > RepeatRate * 11 / 10) { // new key // ??
						if (debug) printf("Neuer\n");
						pressed = true;
						repeat = false;
						FirstTime.Set();
					} else { // repeat
						if (debug) printf("Repeat\n");
						if (FirstTime.Elapsed() < (uint)Setup.RcRepeatDelay || LastTime.Elapsed() < (uint)Setup.RcRepeatDelta) {
							if (debug) printf("continue\n\n");
							continue; // don't send key
						} else {
							pressed = true;
							repeat = true;
							timeout = Delta * 3 / 2; // 11 / 10; // 10 % more should be enough
						}
					}
				}

				/* send key */
				if (pressed){
					if(!pair) {
						code = event.code;
					} else {
						code2 = event.code;
					}
					if(debug) printf("delta send: %ld\n", LastTime.Elapsed());
					LastTime.Set();
					cRemote::Put(evkeys[event.code], repeat);
				}
			}
		}

		/* send release */
		if(ThisTime.Elapsed() > timeout && pressed && repeat) {
			pressed = false;
			if(debug) printf("delta release: %ld\n", ThisTime.Elapsed());
			cRemote::Put(evkeys[code], false, true);
			if (pair) {
				cRemote::Put(evkeys[code2], false, true);
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
