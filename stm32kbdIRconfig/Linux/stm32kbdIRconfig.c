/**********************************************************************************************************
	stm32kbdIRconfig: configure and monitor IRMP_STM32_KBD

	Copyright (C) 2014-2025 Joerg Riechardt

	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

***********************************************************************************************************/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sysexits.h>
#include <sys/stat.h>
#include <errno.h>
#include <inttypes.h>
#include <termios.h>
#include <fcntl.h>
#include "usb_hid_keys.h"
#include <sys/ioctl.h>
#include <linux/hidraw.h>

enum access {
	ACC_GET,
	ACC_SET,
	ACC_RESET
};

enum command {
	CMD_CAPS,
	CMD_ALARM,
	CMD_IRDATA,
	CMD_KEY,
	CMD_WAKE,
	CMD_REBOOT,
	CMD_IRDATA_REMOTE,
	CMD_WAKE_REMOTE,
	CMD_REPEAT,
	CMD_EEPROM_RESET,
	CMD_EEPROM_COMMIT,
	CMD_EEPROM_GET_RAW,
	CMD_HID_TEST,
	CMD_STATUSLED,
	CMD_EMIT,
	CMD_NEOPIXEL,
	CMD_MACRO,
	CMD_MACRO_REMOTE,
	CMD_SEND_AFTER_WAKEUP,
};

enum status {
	STAT_CMD,
	STAT_SUCCESS,
	STAT_FAILURE
};

enum report_id {
	REPORT_ID_KBD = 1,
	REPORT_ID_CONFIG_IN = 2,
	REPORT_ID_CONFIG_OUT = 3
};

enum color {
	red,
	green,
	blue,
	yellow,
	white,
	off,
	custom,
	strong_red,
	orange,
	purple,
	strong_white
};

#define NUM_PIXELS 64

static int stm32fd = -1;
uint8_t inBuf[64];
uint8_t outBuf[64];
unsigned int in_size, out_size;

static bool open_stm32(const char *devicename) {
	stm32fd = open(devicename, O_RDWR );
	if (stm32fd == -1) {
		printf("error opening stm32 device: %s\n", strerror(errno));
		return false;
	}
	printf("opened stm32 device\n");
	return true;
}

static void read_stm32(int in_size, int show_len) {
	int retVal;
	retVal = read(stm32fd, inBuf, in_size);
	if (retVal < 0) {
		printf("read error\n");
	} else {
		printf("read %d bytes:\n\t", retVal);
		for (int i = 0; i < show_len; i++)
			printf("%02hhx ", inBuf[i]);
		puts("\n");
	}
} 

static void write_stm32(int idx) {
	int retVal;
	retVal = write(stm32fd, outBuf, idx);
	if (retVal < 0) {
		printf("write error\n");
	} else {
		printf("written %d bytes:\n\t", retVal);
		for (int i = 0; i < idx; i++)
			printf("%02hhx ", outBuf[i]);
		puts("\n");
	}
}

void write_and_check(int idx, int show_len) {
	write_stm32(idx);
	usleep(3000);
	read_stm32(in_size, show_len); // blocking per default, waits until data arrive
	while (inBuf[0] == REPORT_ID_KBD)
		read_stm32(in_size, show_len);
	if((inBuf[0] == REPORT_ID_CONFIG_IN) && (inBuf[1] == STAT_SUCCESS) && (inBuf[2] == outBuf[2]) && (inBuf[3] == outBuf[3])) {
		puts("*****************************OK********************************\n");
	} else {
		puts("***************************ERROR*******************************\n");
	}
}

int main(int argc, const char **argv) {

	uint64_t i;
	uint16_t kk = 0x0000;
	char c, d, e;
	uint8_t s, m, l, idx, eeprom_lines;
	int8_t k;
	int retValm, jump_to_firmware, res, desc_size = 0;
	unsigned int n;
	FILE *fp;
	char testfilename[10];
	uint16_t j = 0;
        struct hidraw_report_descriptor rpt_desc;

        memset(&rpt_desc, 0x0, sizeof(rpt_desc));

	open_stm32(argc>1 ? argv[1] : "/dev/irmp_stm32_kbd");

        /* Get Report Descriptor Size */
        res = ioctl(stm32fd, HIDIOCGRDESCSIZE, &desc_size);
        if (res < 0)
                perror("HIDIOCGRDESCSIZE");
        else
                printf("Report Descriptor Size: %d\n", desc_size);

        /* Get Report Descriptor */
        rpt_desc.size = desc_size;
        res = ioctl(stm32fd, HIDIOCGRDESC, &rpt_desc);
        if (res < 0) {
                perror("HIDIOCGRDESC");
        } else {
                printf("Report Descriptor: ");
                for(n = 0; n < rpt_desc.size; n++)
                        printf("%02hhx ", rpt_desc.value[n]);
                puts("");
        }

	/* Get Report count */
	for(n = 0; n < rpt_desc.size - 2; n++) {
		if(rpt_desc.value[n] == 0x95 && rpt_desc.value[n+2] == 0x81){ // REPORT_COUNT, INPUT
			in_size = rpt_desc.value[n+1] + 1;
		}
		if(rpt_desc.value[n] == 0x95 && rpt_desc.value[n+2] == 0x91){ // REPORT_COUNT, OUTPUT
			out_size = rpt_desc.value[n+1] + 1;
			break;
		}
	}

	outBuf[0] = REPORT_ID_CONFIG_OUT;
	outBuf[1] = STAT_CMD;
	outBuf[2] = ACC_GET;
	outBuf[3] = CMD_CAPS;
	outBuf[4] = 0;
	write(stm32fd, outBuf, 5);
	usleep(3000);
	read(stm32fd, inBuf, in_size);
	while (inBuf[0] == 0x01)
		read(stm32fd, inBuf, in_size);
	eeprom_lines = inBuf[4];
	if(in_size != (inBuf[7] ? inBuf[7] : 17))
		printf("warning: hid in report count mismatch: %u %u\n", in_size, inBuf[7] ? inBuf[7] : 17);
	else
		printf("hid in report count: %u\n", in_size);
	if(out_size != (inBuf[8] ? inBuf[8] : 17))
		printf("warning: hid out report count mismatch: %u %u\n", out_size,  inBuf[8] ? inBuf[8] : 17);
	else
		printf("hid out report count: %u\n", out_size);
	if(!inBuf[7] || !inBuf[8])
		printf("old firmware!\n");
	puts("");

cont:	printf("set: wakeups, macros, IR-data, keys, repeat, send_after_wakeup, alarm, commit on RP2xxx, statusled and neopixel(s)\nset by remote: wakeups, macros and IR-data (q)\nget: wakeups, macros, IR-data, keys, repeat, send_after_wakeup, alarm, capabilities, eeprom and raw eeprom from RP2xxx (g)\nreset: wakeups, macros, IR-data, keys, repeat, send_after_wakeup, alarm and eeprom (r)\nsend IR (i)\nreboot (b)\nmonitor until ^C (m)\nrun test (t)\nhid test (h)\nneopixel test (n)\nexit (x)\n");
	scanf("%s", &c);

	switch (c) {

	case 's':
set:		printf("set wakeup(w)\nset macro(m)\nset IR-data(i)\nset key(k)\nset repeat(r)\nset send_after_wakeup(x)\nset alarm(a)\ncommit on RP2xxx(c)\nstatusled(s)\nneopixel(n)\n");
		scanf("%s", &d);
		memset(&outBuf[2], 0, sizeof(outBuf) - 2);
		idx = 2;
		outBuf[idx++] = ACC_SET;
		switch (d) {
		case 'w':
			printf("enter wakeup number (starting with 0)\n");
			scanf("%" SCNx8 "", &m);
			outBuf[idx++] = CMD_WAKE;
			outBuf[idx++] = m;
			printf("enter IRData (protocoladdresscommandflag)\n");
			scanf("%" SCNx64 "", &i);
			outBuf[idx++] = (i>>40) & 0xFF;
			outBuf[idx++] = (i>>24) & 0xFF;
			outBuf[idx++] = (i>>32) & 0xFF;
			outBuf[idx++] = (i>>8) & 0xFF;
			outBuf[idx++] = (i>>16) & 0xFF;
			outBuf[idx++] = i & 0xFF;
			write_and_check(idx, 4);
			break;
		case 'm':
			printf("enter macro number (starting with 0)\n");
			scanf("%" SCNx8 "", &m);
			outBuf[idx++] = CMD_MACRO;
			outBuf[idx++] = m;    // (m+1)-th macro
			printf("enter slot number, 0 for trigger\n");
			scanf("%" SCNx8 "", &s);
			outBuf[idx++] = s;    // (s+1)-th slot
			printf("enter IRData (protocoladdresscommandflag)\n");
			scanf("%" SCNx64 "", &i);
			outBuf[idx++] = (i>>40) & 0xFF;
			outBuf[idx++] = (i>>24) & 0xFF;
			outBuf[idx++] = (i>>32) & 0xFF;
			outBuf[idx++] = (i>>8) & 0xFF;
			outBuf[idx++] = (i>>16) & 0xFF;
			outBuf[idx++] = i & 0xFF;
			write_and_check(idx, 4);
			break;
		case 'i':
			printf("enter IR-data number (starting with 0)\n");
			scanf("%" SCNu8 "", &m);
			outBuf[idx++] = CMD_IRDATA;
			outBuf[idx++] = m;
			printf("enter IRData (protocoladdresscommandflag)\n");
			scanf("%" SCNx64 "", &i);
			outBuf[idx++] = (i>>40) & 0xFF;
			outBuf[idx++] = (i>>24) & 0xFF;
			outBuf[idx++] = (i>>32) & 0xFF;
			outBuf[idx++] = (i>>8) & 0xFF;
			outBuf[idx++] = (i>>16) & 0xFF;
			outBuf[idx++] = i & 0xFF;
			write_and_check(idx, 4);
			break;
		case 'k':
			printf("enter key number (starting with 0)\n");
			scanf("%" SCNu8 "", &m);
			outBuf[idx++] = CMD_KEY;
			outBuf[idx++] = m;
			printf("enter key (KEY_xxx)\n");
			scanf("%s", &c);
			for(l=0; l < lines; l++) {
			    if(!strcmp(mapusb[l].key, &c)) {
				kk = mapusb[l].usb_hid_key;
				break;
			    }
			}
			outBuf[idx++] = kk;
			printf("enter modifier (KEY_xxx or ff for none)\n");
			scanf("%s", &c);
			for(l=0; l < lines; l++) {
			    if(!strcmp(mapusb[l].key, &c)) {
				kk = mapusb[l].usb_hid_key;
				break;
			    }
			}
			outBuf[idx++] = kk;
			write_and_check(idx, 4);
			break;
		case 'r':
			printf("set repeat delay(0)\nset repeat period(1)\nset repeat timeout(2)\n");
			scanf("%" SCNu8 "", &m);
			outBuf[idx++] = CMD_REPEAT;
			outBuf[idx++] = m;
			printf("enter value (dec)\n");
			scanf("%" SCNu16 "", &kk);
			outBuf[idx++] = kk & 0xFF;
			outBuf[idx++] = (kk>>8) & 0xFF;
			write_and_check(idx, 4);
			break;
		case 'x':
			outBuf[idx++] = CMD_SEND_AFTER_WAKEUP;
			printf("enter send_after_delay (dec)\n");
			scanf("%" SCNu8 "", &s);
			outBuf[idx++] = s;
			write_and_check(idx, 4);
			break;
		case 'a':
			outBuf[idx++] = CMD_ALARM;
			printf("enter alarm\n");
			scanf("%" SCNx64 "", &i);
			memcpy(&outBuf[idx], &i, 4);
			write_and_check(idx + 4, 4);
			break;
		case 'c':
			outBuf[idx++] = CMD_EEPROM_COMMIT;
			write_and_check(idx, 4);
			break;
		case 's':
			outBuf[idx++] = CMD_STATUSLED;
			printf("enter 1 for on, 0 for off\n");
			scanf("%" SCNx8 "", &s);
			outBuf[idx++] = s;
			write_and_check(idx, 4);
			break;
		case 'n':
			outBuf[idx++] = CMD_NEOPIXEL;
			printf("enter led number (starting with 1)\n");
			scanf("%" SCNu8 "", &s);
			outBuf[idx++] = 3 * s;
			outBuf[idx++] = (s - 1) / 19;
			outBuf[idx++] = 3 * ((s - 1) % 19) + 1;
			idx += 3 * ((s - 1) % 19);
color: printf("red(r)\ngreen(g)\nblue(b)\nyellow(y)\nwhite(w)\noff(o)\ncustom(c)\nstrong_red(s)\norange(a)\npurple(p)\nstrong_white(x)\n");
			scanf("%s", &e);
			switch (e) {
			case 'r':
				outBuf[idx++] = 3;
				outBuf[idx++] = 0;
				outBuf[idx++] = 0;
				write_and_check(idx, 4);
				break;
			case 'g':
				outBuf[idx++] = 0;
				outBuf[idx++] = 4;
				outBuf[idx++] = 0;
				write_and_check(idx, 4);
				break;
			case 'b':
				outBuf[idx++] = 0;
				outBuf[idx++] = 0;
				outBuf[idx++] = 12;
				write_and_check(idx, 4);
				break;
			case 'y':
				outBuf[idx++] = 4;
				outBuf[idx++] = 2;
				outBuf[idx++] = 0;
				write_and_check(idx, 4);
				break;
			case 'w':
				outBuf[idx++] = 3;
				outBuf[idx++] = 3;
				outBuf[idx++] = 2;
				write_and_check(idx, 4);
				break;
			case 'o':
				outBuf[idx++] = 0;
				outBuf[idx++] = 0;
				outBuf[idx++] = 0;
				write_and_check(idx, 4);
				break;
			case 'c':
				printf("enter red in dec\n");
				scanf("%" SCNu8 "", &s);
				outBuf[idx++] = s;
				printf("enter green in dec\n");
				scanf("%" SCNu8 "", &s);
				outBuf[idx++] = s;
				printf("enter blue in dec\n");
				scanf("%" SCNu8 "", &s);
				outBuf[idx++] = s;
				write_and_check(idx, 4);
				break;
			case 's':
				outBuf[idx++] = 255;
				outBuf[idx++] = 0;
				outBuf[idx++] = 0;
				write_and_check(idx, 4);
				break;
			case 'a':
				outBuf[idx++] = 8;
				outBuf[idx++] = 2;
				outBuf[idx++] = 0;
				write_and_check(idx, 4);
				break;
			case 'p':
				outBuf[idx++] = 8;
				outBuf[idx++] = 0;
				outBuf[idx++] = 8;
				write_and_check(idx, 4);
				break;
			case 'x':
				outBuf[idx++] = 255;
				outBuf[idx++] = 255;
				outBuf[idx++] = 255;
				write_and_check(idx, 4);
				break;
			default:
				goto color;
			}
			break;
		default:
			goto set;
		}
		break;

	case 'q':
Set:		printf("set wakeup with remote control(w)\nset macro with remote control(m)\nset IR-data with remote control(i)\n");
		scanf("%s", &d);
		memset(&outBuf[2], 0, sizeof(outBuf) - 2);
		idx = 2;
		outBuf[idx++] = ACC_SET;
		switch (d) {
		case 'w':
			printf("enter wakeup number (starting with 0)\n");
			scanf("%" SCNx8 "", &m);
			outBuf[idx++] = CMD_WAKE_REMOTE;
			outBuf[idx++] = m;
			break;
		case 'm':
			printf("enter macro number (starting with 0)\n");
			scanf("%" SCNx8 "", &m);
			outBuf[idx++] = CMD_MACRO_REMOTE;
			outBuf[idx++] = m;    // (m+1)-th macro
			printf("enter slot number, 0 for trigger\n");
			scanf("%" SCNx8 "", &s);
			outBuf[idx++] = s;    // (s+1)-th slot
			break;
		case 'i':
			printf("enter IR-data number (starting with 0)\n");
			scanf("%" SCNu8 "", &m);
			outBuf[idx++] = CMD_IRDATA_REMOTE;
			outBuf[idx++] = m;
			break;
		default:
			goto Set;
		}
		write_and_check(idx, 4);
		break;

	case 'g':
get:		printf("get wakeup(w)\nget macro(m)\nget IR-data (i)\nget key(k)\nget repeat(r)\nget send_after_weakeup(x)\nget caps(c)\nget alarm(a)\nget eeprom(e)\nget raw eeprom from RP2xxx(p)\n");
		scanf("%s", &d);
		memset(&outBuf[2], 0, sizeof(outBuf) - 2);
		idx = 2;
		outBuf[idx++] = ACC_GET;
		switch (d) {
		case 'w':
			printf("enter wakeup number (starting with 0)\n");
			scanf("%" SCNx8 "", &m);
			outBuf[idx++] = CMD_WAKE;
			outBuf[idx++] = m;
			write_and_check(idx, 10);
			break;
		case 'm':
			printf("enter macro number (starting with 0)\n");
			scanf("%" SCNx8 "", &m);
			outBuf[idx++] = CMD_MACRO;
			outBuf[idx++] = m;    // (m+1)-th macro
			printf("enter slot number, 0 for trigger\n");
			scanf("%" SCNx8 "", &s);
			outBuf[idx++] = s;    // (s+1)-th slot
			write_and_check(idx, 10);
			break;
		case 'i':
			printf("enter IR-data number (starting with 0)\n");
			scanf("%" SCNu8 "", &m);
			outBuf[idx++] = CMD_IRDATA;
			outBuf[idx++] = m;
			write_and_check(idx, 10);
			break;
		case 'k':
			printf("enter key number (starting with 0)\n");
			scanf("%" SCNu8 "", &m);
			outBuf[idx++] = CMD_KEY;
			outBuf[idx++] = m;
			write_and_check(idx, 6);
			break;
		case 'r':
			printf("get repeat delay(0)\nget repeat period(1)\nget repeat timeout(2)\n");
			scanf("%" SCNu8 "", &m);
			outBuf[idx++] = CMD_REPEAT;
			outBuf[idx++] = m;
			write_and_check(idx, 6);
			break;
		case 'x':
			outBuf[idx++] = CMD_SEND_AFTER_WAKEUP;
			write_and_check(idx, 5);
			break;
		case 'a':
			outBuf[idx++] = CMD_ALARM;
			write_and_check(idx, 8);
			break;
		case 'c':
			jump_to_firmware = 0;
			outBuf[idx++] = CMD_CAPS;
			for (l = 0; l < 20; l++) { // for safety stop after 20 loops
				outBuf[idx] = l;
				write_stm32(idx+1);
				usleep(3000);
				read_stm32(in_size, l == 0 ? 9 : in_size);
				while (inBuf[0] == 0x01)
					read_stm32(in_size, l == 0 ? 9 : in_size);
				if (!l) { // first query for slots and depth
					printf("number of keys: %u\n", inBuf[4]);
					printf("number of wakeups (including reboot): %u\n", inBuf[6]);
					printf("hid in report count: %u\n", inBuf[7]);
					printf("hid out report count: %u\n", inBuf[8]);
					printf("number of macros: %u\n", inBuf[9]);
					printf("macro depth: %u\n", inBuf[10]);
				} else {
					if(!jump_to_firmware) { // queries for supported_protocols
						printf("protocols: ");
						for (s = 4; s < in_size; s++) {
							if (!inBuf[s]) { // NULL termination
								printf("\n\n");
								jump_to_firmware = 1;
								goto again;
							}
							printf("%u ", inBuf[s]);
						}
					} else { // queries for firmware
						printf("firmware: ");
						for (s = 4; s < in_size; s++) {
							if (!inBuf[s]) { // NULL termination
								printf("\n\n");
								goto out;
							}
							printf("%c", inBuf[s]);
						}
					}
				}
				printf("\n\n");
again:			;
			}
			break;
		case 'e':
			for (l = 0; l < eeprom_lines; l++) {
				outBuf[idx++] = CMD_IRDATA;
				outBuf[idx++] = l;
				write(stm32fd, outBuf, idx);
				usleep(3000);
				read(stm32fd, inBuf, 10);
				printf("%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx", inBuf[4],inBuf[6],inBuf[5],inBuf[8],inBuf[7],inBuf[9]);
				printf(" ");
				idx = 3;
				outBuf[idx++] = CMD_KEY;
				outBuf[idx++] = l;
				usleep(3000);
				write(stm32fd, outBuf, idx);
				usleep(3000);
				read(stm32fd, inBuf, 6);
				for(n=0; n < lines-1; n++) {
					if(mapusb[n].usb_hid_key == inBuf[5]) {
						printf("%s|", mapusb[n].key);
					}
				}
				for(int n=0; n < lines-1; n++) {
					if(mapusb[n].usb_hid_key == inBuf[4]) {
						printf("%s", mapusb[n].key);
					}
				}
				printf("\n");
				usleep(3000);
				idx = 3;
			}
			goto out;
			break;
		case 'p':
			outBuf[idx++] = CMD_EEPROM_GET_RAW;
			for(k = 31; k >= 0; k--) { // FLASH_SECTOR_SIZE * nr_sectors / size
				outBuf[idx] = k;
				for(l = 0; l < 32; l++) { // size / 32
					outBuf[idx+1] = l;
					write(stm32fd, outBuf, idx+2);
					usleep(3000);
					retValm = read(stm32fd, inBuf, in_size);
					if (retValm < 0) {
						printf("read error\n");
					} else {
						for (int i = 4; i < 36; i++) // 32
							printf("%02hhx ", inBuf[i]);
					}
				}
				printf("\n");
			}
			break;
		default:
			goto get;
		}
out:
		break;

	case 'r':
reset:		printf("reset wakeup(w)\nreset macro slot(m)\nreset IR-data(i)\nreset key(k)\nreset repeat(r)\nreset send_after_wakeup(x)\nreset alarm(a)\nreset eeprom(e)\n");
		scanf("%s", &d);
		memset(&outBuf[2], 0, sizeof(outBuf) - 2);
		idx = 2;
		outBuf[idx++] = ACC_RESET;
		switch (d) {
		case 'w':
			printf("enter slot number (starting with 0)\n");
			scanf("%" SCNx8 "", &m);
			outBuf[idx++] = CMD_WAKE;
			outBuf[idx++] = m;
			break;
		case 'm':
			printf("enter macro number (starting with 0)\n");
			scanf("%" SCNx8 "", &m);
			outBuf[idx++] = CMD_MACRO;
			outBuf[idx++] = m;    // (m+1)-th macro
			printf("enter slot number, 0 for trigger\n");
			scanf("%" SCNx8 "", &s);
			outBuf[idx++] = s;    // (s+1)-th slot
			break;
		case 'i':
			printf("enter IR-data number (starting with 0)\n");
			scanf("%" SCNu8 "", &m);
			outBuf[idx++] = CMD_IRDATA;
			outBuf[idx++] = m;
			break;
		case 'k':
			printf("enter key number (starting with 0)\n");
			scanf("%" SCNu8 "", &m);
			outBuf[idx++] = CMD_KEY;
			outBuf[idx++] = m;
			break;
		case 'r':
			printf("reset repeat delay(0)\nreset repeat period(1)\nreset repeat timeout(2)\n");
			scanf("%" SCNu8 "", &m);
			outBuf[idx++] = CMD_REPEAT;
			outBuf[idx++] = m;
			break;
		case 'x':
			outBuf[idx++] = CMD_SEND_AFTER_WAKEUP;
			break;
		case 'e':
			outBuf[idx++] = CMD_EEPROM_RESET;
			break;
		case 'a':
			outBuf[idx++] = CMD_ALARM;
			break;
		default:
			goto reset;
		}
		write_and_check(idx, 4);
		break;

	case 'i':
		printf("enter IRData (protocoladdresscommandflag)\n");
		scanf("%" SCNx64 "", &i);
		memset(&outBuf[2], 0, sizeof(outBuf) - 2);
		idx = 2;
		outBuf[idx++] = ACC_SET;
		outBuf[idx++] = CMD_EMIT;
		outBuf[idx++] = (i>>40) & 0xFF;
		outBuf[idx++] = (i>>24) & 0xFF;
		outBuf[idx++] = (i>>32) & 0xFF;
		outBuf[idx++] = (i>>8) & 0xFF;
		outBuf[idx++] = (i>>16) & 0xFF;
		outBuf[idx++] = i & 0xFF;
		write_and_check(idx, 4);
		break;

	case 'b':
		memset(&outBuf[2], 0, sizeof(outBuf) - 2);
		idx = 2;
		outBuf[idx++] = ACC_SET;
		outBuf[idx++] = CMD_REBOOT;
		write_and_check(idx, 4);
		close(stm32fd);
		usleep(1900000);
		for(l=0;l<6;l++) {
			if(open_stm32(argc>1 ? argv[1] : "/dev/irmp_stm32_kbd") == true)
				break;
			usleep(100000);
		}
		break;

	case 'm':
		goto monit;
		break;

	case 'n':
		memset(&outBuf[2], 0, sizeof(outBuf) - 2);
		idx = 2;
		outBuf[idx++] = ACC_SET;
		outBuf[idx++] = CMD_NEOPIXEL;
		outBuf[idx++] = NUM_PIXELS * 3;
		for (m = 0; m < 3; m++) {
			for (s = 0; s < (NUM_PIXELS + 18) / 19; s++) {
				idx = 5;
				outBuf[idx++] = s; // chunk s
				outBuf[idx++] = 0;
				for (i = 0; i < 19; i++) {
					if (s * 19 + i < NUM_PIXELS) {
						outBuf[idx++] = s * 19 + i;
						outBuf[idx++] = 64 - s * 19 - i;
						outBuf[idx++] = 0;
					}
				}
				write_and_check(idx, 4);
			}
			usleep(1000000);
			for (s = 0; s < (NUM_PIXELS + 18) / 19; s++) {
				idx = 5;
				outBuf[idx++] = s; // chunk s
				outBuf[idx++] = 0;
				for (i = 0; i < 19; i++) {
					if (s * 19 + i < NUM_PIXELS) {
						outBuf[idx++] = 64 - s * 19 - i;
						outBuf[idx++] = s * 19 + i;
						outBuf[idx++] = 0;
					}
				}
				write_and_check(idx, 4);
			}
			usleep(1000000);
		}
		for (s = 0; s < (NUM_PIXELS + 18) / 19; s++) {
			idx = 5;
			outBuf[idx++] = s; // chunk s
			outBuf[idx++] = 0;
			for (i = 0; i < 19; i++) {
				if (s * 19 + i < NUM_PIXELS) {
					outBuf[idx++] = 0;
					outBuf[idx++] = 0;
					outBuf[idx++] = 0;
				}
			}
			write_and_check(idx, 4);
		}
		break;

	case 't':
		goto test;
		break;

	case 'x':
		goto exit;
		break;

	case 'h':
		memset(&outBuf[2], 0, sizeof(outBuf) - 2);
		for(l = 2; l < out_size; l++){
			outBuf[l] = l - 1; // ACC_SET ...
		}
		outBuf[3] = CMD_HID_TEST;
		write_and_check(out_size, in_size);
		break;

	default:
		goto cont;
	}

	goto cont;

monit:	while(true) {
		retValm = read(stm32fd, inBuf, in_size);
		if (retValm >= 0) {
			printf("read %d bytes:\n\t", retValm);
			for (l = 0; l < 64; l++)
				printf("%02hhx ", inBuf[l]);
			printf("\n");
			printf("modifier|key: %02hhx%02hhx\n\n", inBuf[1],inBuf[3]);
			printf("\n");
		}
	}

test:	sprintf(testfilename, "test%u", j); printf("write into %s\n", testfilename); // if directory, it needs to exist (or be created)!
	fp = fopen(testfilename, "w");
	while(true) {
		retValm = read(stm32fd, inBuf, in_size);
		if (retValm >= 0) {
			printf("%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx\n", inBuf[1],inBuf[3],inBuf[2],inBuf[5],inBuf[4],inBuf[6]);
			fprintf(fp, "%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx\n", inBuf[1],inBuf[3],inBuf[2],inBuf[5],inBuf[4],inBuf[6]);
			if (inBuf[1] == 0x3c && inBuf[3] == 0 && inBuf[2] == 0 && inBuf[5] == 0 && inBuf[4] == 0x3f && inBuf[6] == 1) { // 3c0000003f01, stopsequence TODO make configurable
				printf("received stopsequence\n");
				fclose(fp);
				j++;
				if (j >= 200) { // TODO make number of tests configurable
					printf("exit\n");
					goto exit;
				}
				goto test;
			}
		}
	}

exit:	if (stm32fd >= 0) close(stm32fd);
	return 0;
}
