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

#define RECONNECTDELAY 3000 // ms

class cIrmpRemote : public cRemote, private cThread {
private:
  bool Connect(void);
  virtual void Action(void) override;
  bool Stop();
  bool Ready();
  int fd;
public:
  cIrmpRemote(const char *Name);
};

cIrmpRemote::cIrmpRemote(const char *Name)
:cRemote(Name)
,cThread("IRMP_KBD remote control")
{
  Connect();
  Start();
}

bool cIrmpRemote::Connect()
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
  if (fd)
    close(fd);
  return true;
}

bool cIrmpRemote::Ready(void)
{
  return fd >= 0;
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
  bool release_needed = false;
  bool repeat = false;
  bool pair = false;
  uint16_t timeout = 160, code = 0, code2 = 0, lastcode = 0, lastcode2 = 0;

  while(Running()){

    bool ready = fd >= 0 && cFile::FileReady(fd, timeout); // implizit mindestens 100 ms!!!
    int ret = ready ? safe_read(fd, &event, sizeof(event)) : -1;

    if (fd < 0 || ready && ret <= 0) {
	esyslog("ERROR: irmphidkbd connection broken, trying to reconnect every %.1f seconds", float(RECONNECTDELAY) / 1000);
	if (fd >= 0)
	    close(fd);
	fd = -1;
	while (Running() && fd < 0) {
	    cCondWait::SleepMs(RECONNECTDELAY);
	    if (Connect()) {
		isyslog("reconnected to irmphidkbd");
		break;
	    }
	}
    }


    if (ready && ret > 0) {
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

				int Delta = ThisTime.Elapsed(); // the time between two consecutive events
				if (debug) printf("Delta: %d\n", Delta);
				if(!pair){
					ThisTime.Set();
					if(code != lastcode) { // new key // ??
						if (debug) printf("Neuer\n");
						if (repeat) {
							Put(lastcode, false, true); // generated release for previous repeated key
							printf("put release for %d\n", lastcode);
							if (pair) {// hier immer falsch!
								Put(evkeys[lastcode2], false, true);
							}
						}
						if(!pair) { // hier immer richtig!
							lastcode = event.code;
						} else {
							lastcode2 = event.code;
						}
						repeat = false;
						FirstTime.Set();
					} else { // repeat
						if (debug) printf("Repeat\n");
						if (FirstTime.Elapsed() < (uint)Setup.RcRepeatDelay || LastTime.Elapsed() < (uint)Setup.RcRepeatDelta) {
							if (debug) printf("continue\n\n");
							continue; // don't send key
						}
						repeat = true;
						}
					}

				/* send key */
					if(!pair) {
						code = event.code;
					} else {
						code2 = event.code;
					}
					if(debug) printf("delta send: %ld\n", LastTime.Elapsed());
					LastTime.Set();
					Put(evkeys[event.code], repeat);
					release_needed = true;
			}
		}

		/* send release */
		if (ThisTime.Elapsed() > timeout && release_needed && repeat) {
			release_needed = false;
			if(debug) printf("delta release: %ld timeout: %d code: %s\n", ThisTime.Elapsed(), timeout, evkeys[lastcode]);
			cRemote::Put(evkeys[lastcode], false, true);
			if (pair) {
			if(debug) printf("delta release: %ld timeout: %d code: %s\n", ThisTime.Elapsed(), timeout, evkeys[lastcode2]);
				Put(evkeys[lastcode2], false, true);
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
