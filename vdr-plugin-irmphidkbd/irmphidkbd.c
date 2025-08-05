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
  void Action(void);
  bool Ready();
  int fd;
public:
  cIrmpRemote(const char *Name);
  ~cIrmpRemote();
};

cIrmpRemote::cIrmpRemote(const char *Name)
:cRemote(Name)
,cThread("IRMP_KBD remote control")
{
  Connect();
  Start();
}

cIrmpRemote::~cIrmpRemote()
{
  //ioctl(fd, EVIOCGRAB, 0);
  int fh = fd;
  fd = -1;
  Cancel();
  if (fh >= 0)
     close(fh);
}

bool cIrmpRemote::Connect()
{
  fd = open(kbd_device, O_RDONLY | O_NONBLOCK);
  if(fd == -1){
    if(debug) printf("Cannot open %s. %s.\n", kbd_device, strerror(errno));
    esyslog("Cannot open %s. %s.\n", kbd_device, strerror(errno));
    return false;
  } else {
    if(debug) printf("opened %s\n", kbd_device);
    isyslog("opened %s\n", kbd_device);
  }

  /*if(ioctl(fd, EVIOCGRAB, 1)){
    if(debug) printf("Cannot grab %s. %s.\n", kbd_device, strerror(errno));
  } else {
    if(debug) printf("Grabbed %s!\n", kbd_device);
  }*/

  return true;
}

bool cIrmpRemote::Ready(void)
{
  return fd >= 0;
}

void cIrmpRemote::Action(void)
{
  cTimeMs FirstTime;
  cTimeMs LastTime;
  cTimeMs ThisTime;
  cTimeMs LoopTime;
  struct input_event event;
  uint8_t magic_key = 173;
  uint8_t only_once = 1;
  bool release_needed = false;
  bool repeat = false;
  bool pair = false;
  int timeout = -1;
  int RepeatRate = 118; // greatest repeat rate we know so far (except Sky)
  uint16_t code = 0, lastcode = 0;
  //uint16_t code2 = 0, lastcode2 = 0;

  if(debug) printf("irmphidkbd action!\n");

  while(Running()){

    //printf("loop timeout: %d, elapsed: %ld\n", timeout, LoopTime.Elapsed());
    LoopTime.Set();
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
            if (event.type != EV_KEY || event.value != 1) { // unbrauchbar
                if (timeout >= (int)LoopTime.Elapsed()) {
                    timeout -= LoopTime.Elapsed(); // aber mind. 25
                    //if(debug) printf("unusable event, new timeout: %d\n", timeout);
                }
                continue;
            }
            code = event.code;
            pair = (FirstTime.Elapsed() < 10)? 1 : 0; // modifier+key, TODO: process modifier
            if(only_once && code == magic_key) {
                if(debug) printf("magic\n");
                FILE *out = fopen("/var/log/started_by_IRMP_STM32_KBD", "a");
                time_t date = time(NULL);
                struct tm *ts = localtime(&date);
                fprintf(out, "%s", asctime(ts));
                fclose(out);
                only_once = 0;
            }
            if(!pair){
                int Delta = ThisTime.Elapsed(); // the time between two consecutive events
                if (debug) printf("Delta: %d\n", Delta);
                if (RepeatRate > Delta && Delta > 0)
                    RepeatRate = Delta; // autodetect repeat rate
                ThisTime.Set();
                timeout = Setup.RcRepeatTimeout ? Setup.RcRepeatTimeout : RepeatRate * 103 / 100 + 1;  // 3 % + 1 should presumably be enough
                if (debug) printf("code: %s, lastcode: %s, toggle: %d, timeout: %d, pair: %d\n", evkeys[code], evkeys[lastcode], Setup.RcTogglingProtocol, timeout, pair);
                if(code != lastcode) { // new key
                    if (debug) printf("Neuer\n");
                    if (repeat) {
                        Put(lastcode, false, true); // generated release for previous repeated key
                        if (debug) printf("put release for %d\n", lastcode);
                    }
                    lastcode = code;
                    repeat = false;
                    FirstTime.Set();
                } else { // repeat
                    if (debug) printf("Repeat\n");
                    if (FirstTime.Elapsed() < (uint)Setup.RcRepeatDelay) {
                        if (debug) printf("continue Delay\n\n");
                        continue; // repeat function kicks in after a short delay
                    }
                    if (LastTime.Elapsed() < (uint)Setup.RcRepeatDelta) {
                        if (debug)  printf("continue Delta\n\n");
                        continue; // skip same keys coming in too fast
                    }
                    repeat = true;
                }
            } else {
                if (debug) printf("pair!\n");
            }

            /* send key */
            if(!pair) {
                //code = code;
            } else {
                //code2 = code;
            }
            if(debug) printf("delta send: %ld\n", LastTime.Elapsed());
            LastTime.Set();
            Put(evkeys[code], repeat);
            release_needed = true;
    } else { // no key within timeout
            if (release_needed && repeat) {
                if(debug) printf("put release for %s, delta %ld\n", evkeys[lastcode], ThisTime.Elapsed());
                Put(evkeys[lastcode], false, true);
                /*if (pair) {
                    if(debug) printf("put release for %s, delta %ld\n", evkeys[lastcode2], ThisTime.Elapsed());
                    Put(evkeys[lastcode2], false, true);
                }*/
            }
            release_needed = false;
            repeat = false;
            lastcode = 0;
            timeout = -1;
            if (debug) printf("reset\n");
    }
    if (debug) printf("\n");
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
