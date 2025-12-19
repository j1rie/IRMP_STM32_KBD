/*
 * irmphidkbd.c: An IRMP HID keyboard plugin for the Video Disk Recorder
 *
 * Copyright (C) 20025-2025 Joerg Riechardt <J.Riechardt@gmx.de>
 *
 */

#include <vdr/plugin.h>
#include <vdr/i18n.h>
#include <vdr/remote.h>
#include <vdr/thread.h>
#include "usb_hid_keys.h"
#include <locale.h>

static const char *VERSION        = "0.0.6";
static const char *DESCRIPTION    = tr("Send keypresses from IRMP HID-KBD-Device to VDR");

#define DEBUG 1
#define RECONNECTDELAY 3000 // ms
#define REPORT_ID_KBD 1

const char* kbd_device = "/dev/irmp_stm32_kbd";
int fd;
uint8_t buf[64];

class cIrmpRemote : public cRemote, private cThread {
private:
  void Action(void);
  bool Ready();
  cMutex mutex;
  cCondVar keyReceived;
public:
  cIrmpRemote(const char *Name);
  ~cIrmpRemote();
  void Receive() {
    cMutexLock MutexLock(&mutex);
    keyReceived.Broadcast();
  }
};

cIrmpRemote::cIrmpRemote(const char *Name)
:cRemote(Name)
,cThread("IRMP_KBD remote control")
{
  Start();
}

cIrmpRemote::~cIrmpRemote()
{
  Cancel(3);
}

bool cIrmpRemote::Ready(void)
{
  return fd >= 0;
}

char* get_key_from_hex(uint8_t hex) {
  for(int i = 0; i < lines; i++) {
    if (hex == mapusb[i].usb_hid_key)
      return mapusb[i].key;
  }
  return mapusb[lines - 1].key;
}

void cIrmpRemote::Action(void)
{
  cTimeMs FirstTime;
  cTimeMs LastTime;
  cTimeMs ThisTime;
  cString magic_key = "ff|KEY_REFRESH";
  uint8_t only_once = 1;
  bool release_needed = false;
  bool repeat = false;
  int timeout = INT_MAX;
  cString key = "";
  cString lastkey = "";

  if(DEBUG) printf("IrmpRemote action!\n");

  while(Running()){

    cMutexLock MutexLock(&mutex);
    if (keyReceived.TimedWait(mutex, timeout)) { // keypress

            key = cString::sprintf("%s|%s", get_key_from_hex(buf[1]), get_key_from_hex(buf[3])); // modifier|key

            if(only_once && strcmp(key, magic_key) == 0) {
                FILE *out = fopen("/var/log/started_by_IRMP_STM32_KBD", "a");
                setlocale(LC_TIME, "de_DE.UTF-8");
                time_t date = time(NULL);
                struct tm *ts = localtime(&date);
                char outstr[30];
                strftime(outstr, sizeof(outstr), "%a %e. %b %H:%M:%S %Z %Y", ts); // wie date
                fprintf(out, "%s\n", outstr);
                fclose(out);
                isyslog("irmphidkbd: started by IRMP_KBD\n");
                only_once = 0;
            }

            if(strcmp(key, magic_key) == 0) continue; // ignore magic // beide zusammenlegen?
            timeout = buf[59];

            int Delta = ThisTime.Elapsed(); // the time between two consecutive events
            if (DEBUG) printf("Delta: %d\n", Delta);
            ThisTime.Set();

            //timeout = Setup.RcRepeatTimeout ? Setup.RcRepeatTimeout : RepeatRate * 103 / 100 + 1;  // 3 % + 1 should presumably be enough

            if (DEBUG) printf("key: %s, lastkey: %s, timeout: %d\n", (const char*)key, (const char*)lastkey, timeout);

            if (strcmp(key, lastkey) != 0) { // new key
                if (DEBUG) printf("new key\n");
                if (repeat) {
                    if (DEBUG) printf("put release for %s\n", (const char*)lastkey);
                    Put(lastkey, false, true); // generated release for previous repeated key
                }
                lastkey = key;
                repeat = false;
                FirstTime.Set();
            } else { // repeat
                if (DEBUG) printf("repeat\n");
                if (FirstTime.Elapsed() < (uint)Setup.RcRepeatDelay) {
                    if (DEBUG) printf("continue Delay\n\n");
                    continue; // repeat function kicks in after a short delay
                }
                if (LastTime.Elapsed() < (uint)Setup.RcRepeatDelta) {
                    if (DEBUG)  printf("continue Delta\n\n");
                    continue; // skip same keys coming in too fast
                }
                repeat = true;
            }

            /* send key */
            if(DEBUG) printf("delta send: %ld\n", LastTime.Elapsed());
            LastTime.Set();
            if (DEBUG) printf("put %s %s\n", (const char*)key, repeat ? "Repeat" : "");
            Put(key, repeat);
            release_needed = true;

    } else { // no key within timeout
            if (release_needed && repeat) {
                if(DEBUG) printf("put release for %s, delta %ld\n", (const char *)lastkey, ThisTime.Elapsed());
                Put(lastkey, false, true);
            }
            release_needed = false;
            repeat = false;
            lastkey = "";
            timeout = INT_MAX;
            if (DEBUG) printf("reset\n");
    }
    if (DEBUG) printf("\n");
  }
}

cIrmpRemote *myIrmpRemote = NULL;

class cReadIR : public cThread {
private:
  bool Connect(void);
  void Action(void);
protected:
public:
  cReadIR();
  ~cReadIR();
};

cReadIR::cReadIR(void)
{
  Connect();
  Start();
}

cReadIR::~cReadIR()
{
  Cancel();
  //ioctl(fd, EVIOCGRAB, 0);
  if (fd >= 0)
     close(fd);
  fd = -1;
}

bool cReadIR::Connect()
{
  fd = open(kbd_device, O_RDONLY | O_NONBLOCK);
  if(fd == -1){
    if(DEBUG) printf("Cannot open %s. %s.\n", kbd_device, strerror(errno));
    esyslog("Cannot open %s. %s.\n", kbd_device, strerror(errno));
    return false;
  } else {
    if(DEBUG) printf("opened %s\n", kbd_device);
    isyslog("irmphidkbd: opened %s\n", kbd_device);
  }

  /*if(ioctl(fd, EVIOCGRAB, 1)){
    if(DEBUG) printf("Cannot grab %s. %s.\n", kbd_device, strerror(errno));
  } else {
    if(DEBUG) printf("Grabbed %s!\n", kbd_device);
  }*/

  return true;
}

void cReadIR::Action(void)
{
  if(DEBUG) printf("ReadIR action!\n");

  while(Running()){

    bool ready = fd >= 0 && cFile::FileReady(fd, -1); // implizit mindestens 100 ms!!! bei timeout 0, wenn was angekommen ist > 0
    int ret = ready ? safe_read(fd, buf, sizeof(buf)) : -1; //  bei timeout, error -1, bei 0 bytes 0, sonst > 0

    if (fd < 0 || ready && ret <= 0) { // kein fd oder error oder 0 bytes
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

    if (buf[0] == REPORT_ID_KBD && (buf[1] || buf[3])) {
        myIrmpRemote->Receive();
    } else {
        //if(DEBUG) printf("configuration report or release\n");
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
  myIrmpRemote = new cIrmpRemote("IRMP_KBD");
  new cReadIR();
  return true;
}


VDRPLUGINCREATOR(cPluginIrmphidkbd); // Don't touch this!
