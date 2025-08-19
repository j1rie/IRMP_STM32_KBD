/*
 * irmphidkbd.c: An IRMP HID keyboard plugin for the Video Disk Recorder
 *
 * Copyright (C) 20025-2025 Joerg Riechardt <J.Riechardt@gmx.de>
 *
 */

#include <vdr/plugin.h>
//#include <linux/input.h>
#include <vdr/i18n.h>
#include <vdr/remote.h>
#include <vdr/thread.h>
#include "usb_hid_keys.h"

static const char *VERSION        = "0.0.2";
static const char *DESCRIPTION    = tr("Send keypresses from IRMP HID-KBD-Device to VDR");

const char* kbd_device = "/dev/irmp_stm32_kbd";

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

cString get_key_from_hex(uint8_t hex) {
  for(int i = 0; i < lines; i++) {
    if (hex == mapusb[i].usb_hid_key)
      return mapusb[i].key;
  }
  return "error";
}

void cIrmpRemote::Action(void)
{
  cTimeMs FirstTime;
  cTimeMs LastTime;
  cTimeMs ThisTime;
  cTimeMs LoopTime;
  uint8_t buf[64];
  uint8_t magic_key = 173; // testen!
  uint8_t only_once = 1;
  bool release_needed = false;
  bool repeat = false;
  int timeout = -1;
  cString mod_key = "";
  cString mod = "";
  cString key = "";
  cString last_mod_key = "";
  int RepeatRate = 118;

  if(debug) printf("irmphidkbd action!\n");

  while(Running()){

    //if(debug) printf("loop timeout: %d, elapsed: %ld\n", timeout, LoopTime.Elapsed());
    LoopTime.Set();
    bool ready = fd >= 0 && cFile::FileReady(fd, timeout); // implizit mindestens 100 ms!!!
    int ret = ready ? safe_read(fd, buf, sizeof(buf)) : -1;

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
            if (!buf[1] && !buf[3]) { // release
                if (timeout >= (int)LoopTime.Elapsed()) {
                    timeout -= LoopTime.Elapsed(); // aber mind. 25
                    //if(debug) printf("release, new timeout: %d, LoopTime.Elapsed(): %ld\n\n", timeout, LoopTime.Elapsed());
                }
              continue;
            } // keypress
            mod = get_key_from_hex(buf[1]);
            key = get_key_from_hex(buf[3]);
            mod_key = mod;
            mod_key.Append("|");
            mod_key.Append(key);
            //if(debug) printf("mod_key: %s\n", (const char*)mod_key);

            if(only_once && buf[3] == magic_key) {
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
            if (debug && !Delta) printf("ACHTUNG: Delta: %d *****************************************************\n", Delta); // kommt nicht vor, oder?
            if (RepeatRate > Delta)
                RepeatRate = Delta; // autodetect repeat rate
            ThisTime.Set();
            timeout = Setup.RcRepeatTimeout ? Setup.RcRepeatTimeout : RepeatRate * 103 / 100 + 1;  // 3 % + 1 should presumably be enough
            if (debug) printf("mod_key: %s, last_mod_key: %s, toggle: %d, timeout: %d\n", (const char*)mod_key, (const char*)last_mod_key, Setup.RcTogglingProtocol, timeout);
            if (strcmp(mod_key, last_mod_key) != 0) {// new key
                if (debug) printf("Neuer\n");
                if (repeat) {
                    if (debug) printf("put release for %s\n", (const char*)last_mod_key);
                    Put(last_mod_key, false, true); // generated release for previous repeated key
                }
                last_mod_key = mod_key;
                repeat = false;
                FirstTime.Set();
            } else { // repeat
                if (debug) printf("Repeat\n");
                //if (RepeatRate > Delta)
                    //RepeatRate = Delta; // autodetect repeat rate
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

            /* send key */
            if(debug) printf("delta send: %ld\n", LastTime.Elapsed());
            LastTime.Set();
            Put(mod_key, repeat);
            release_needed = true;

    } else { // no key within timeout
            if (release_needed && repeat) {
                if(debug) printf("put release for %s, delta %ld\n", (const char *)last_mod_key, ThisTime.Elapsed());
                Put(last_mod_key, false, true);
            }
            release_needed = false;
            repeat = false;
            last_mod_key = "";
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
