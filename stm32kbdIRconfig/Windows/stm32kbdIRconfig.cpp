/**********************************************************************************************************
	stm32kbdIRconfig: configure IRMP_STM32_KBD

	Copyright (C) 2014-2024 Joerg Riechardt

	based on work by Alan Ott
	Copyright 2010  Alan Ott

	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

***********************************************************************************************************/

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <wchar.h>
#include <string.h>
#include <stdlib.h>
#include "hidapi.h"
#include <stdint.h>
#include <inttypes.h>

// Headers needed for sleeping.
#ifdef _WIN32
	#include <windows.h>
#else
	#include <unistd.h>
#endif
#include "usb_hid_keys.h"

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

hid_device *handle;
uint8_t inBuf[64];
uint8_t outBuf[64];
unsigned int in_size, out_size;

static bool open_stm32() {
	// Open the device using the VID, PID.
	handle = hid_open(0x1209, 0x4445, NULL);
	if (!handle) {
		printf("error opening stm32 device\n");
		return false;
	}
	printf("opened stm32 device\n");
	return true;
}

static void read_stm32(int in_size, int show_len) {
	int retVal;
	retVal = hid_read(handle, inBuf, in_size);
	if (retVal < 0) {
		printf("read error\n");
	} else {
		printf("read %d bytes:\n\t", retVal);
		for (int i = 0; i < show_len; i++)
			printf("%02x ", (unsigned int)inBuf[i]);
		puts("\n");
	}
} 

static void write_stm32(int idx) {
	int retVal;
	retVal = hid_write(handle, outBuf, idx);
	if (retVal < 0) {
		printf("write error: %ls\n", hid_error(handle));
	} else {
		printf("written %d bytes:\n\t", retVal);
		for (int i = 0; i < idx; i++)
			printf("%02x ", (unsigned int)outBuf[i]);
		puts("\n");
	}
}

void write_and_check(int idx, int show_len) {
	write_stm32(idx);
	#ifdef WIN32
	Sleep(3);
	#else
	usleep(3000);
	#endif
	read_stm32(in_size, show_len); // blocking per default, waits until data arrive
	while (inBuf[0] == REPORT_ID_KBD)
		read_stm32(in_size, show_len);
	if((inBuf[0] == REPORT_ID_CONFIG_IN) && (inBuf[1] == STAT_SUCCESS) && (inBuf[2] == outBuf[2]) && (inBuf[3] == outBuf[3])) {
		puts("*****************************OK********************************\n");
	} else {
		puts("***************************ERROR*******************************\n");
	}
}

int main(int argc, char* argv[])
{
	uint64_t i;
	uint16_t kk = 0x0000;
	char c, d, e;
	uint8_t l, idx, eeprom_lines;
	int8_t k;
	unsigned int s, m;
	int retValm, jump_to_firmware;

#ifdef WIN32
	UNREFERENCED_PARAMETER(argc);
	UNREFERENCED_PARAMETER(argv);
#endif

	if (hid_init())
		return -1;

	open_stm32();

	unsigned char descriptor[HID_API_MAX_REPORT_DESCRIPTOR_SIZE];
	int res = 0;

	res = hid_get_report_descriptor(handle, descriptor, sizeof(descriptor));
	if (res < 0) {
		printf("Error: Unable to get Report Descriptor\n");
		return -1;
	} else {
		printf("Report Descriptor Size: %d\n", res);
		printf("Report Descriptor:");
		for (int i = 0; i < res; i++) {
			printf(" %02x", descriptor[i]);
		}
		printf("\n");
	}

	/* Get Report count */
	for(int n = 0; n < res - 2; n++) {
		if(descriptor[n] == 0x95 && descriptor[n+2] == 0x81){ // REPORT_COUNT, INPUT
			in_size = descriptor[n+1] + 1;
		}
		if(descriptor[n] == 0x95 && descriptor[n+2] == 0x91){ // REPORT_COUNT, OUTPUT
			out_size = descriptor[n+1] + 1;
			break;
		}
	}

	outBuf[0] = REPORT_ID_CONFIG_OUT;
	outBuf[1] = STAT_CMD;
	outBuf[2] = ACC_GET;
	outBuf[3] = CMD_CAPS;
	outBuf[4] = 0;
	hid_write(handle, outBuf, 5);
	#ifdef WIN32
	Sleep(3);
	#else
	usleep(3000);
	#endif
	hid_read(handle, inBuf, in_size);
	while (inBuf[0] == 0x01)
		hid_read(handle, inBuf, in_size);
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

	#ifdef WIN32
cont:	printf("set: wakeups, IR-data, keys, repeat, alarm, commit on RP2xxx, statusled and neopixel(s)\nset by remote: wakeups, macros and IR-data (q)\nget: wakeups, macros, IR-data, keys, repeat, alarm, capabilities, eeprom and raw eeprom from RP2xxx (g)\nreset: wakeups, macros, IR-data, keys, repeat, alarm and eeprom (r)\nsend IR (i)\nreboot (b)\nhid test (h)\nneopixel test (n)exit (x)\n");
	#else
cont:	printf("set: wakeups, IR-data, keys, repeat, alarm, commit on RP2xxx, statusled and neopixel(s)\nset by remote: wakeups, macros and IR-data (q)\nget: wakeups, macros, IR-data, keys, repeat, alarm, capabilities, eeprom and raw eeprom from RP2xxx (g)\nreset: wakeups, macros, IR-data, keys, repeat, alarm and eeprom (r)\nsend IR (i)\nreboot (b)\nmonitor until ^C (m)\nhid test (h)neopixel test (n)\nexit (x)\n");
	#endif
	scanf("%s", &c);

	switch (c) {

	case 's':
set:		printf("set wakeup(w)\nset macro(m)\nset IR-data(i)\nset key(k)\nset repeat(r)\nset alarm(a)\ncommit on RP2xxx(c)\nstatusled(s)\nneopixel(n)\n");
		scanf("%s", &d);
		memset(&outBuf[2], 0, sizeof(outBuf) - 2);
		idx = 2;
		outBuf[idx++] = ACC_SET;
		switch (d) {
		case 'w':
			printf("enter wakeup number (starting with 0)\n");
			scanf("%u", &s);
			outBuf[idx++] = CMD_WAKE;
			outBuf[idx++] = s;
			printf("enter IRData (protocoladdresscommandflag)\n");
			scanf("%llx", &i);
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
			scanf("%u", &m);
			outBuf[idx++] = CMD_MACRO;
			outBuf[idx++] = m;    // (m+1)-th macro
			printf("enter slot number, 0 for trigger\n");
			scanf("%u", &s);
			outBuf[idx++] = s;    // (s+1)-th slot
			printf("enter IRData (protocoladdresscommandflag)\n");
			scanf("%llx", &i);
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
			scanf("%u", &m);
			outBuf[idx++] = CMD_IRDATA;
			outBuf[idx++] = m;
			printf("enter IRData (protocoladdresscommandflag)\n");
			scanf("%llx", &i);
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
			scanf("%u", &s);
			outBuf[idx++] = CMD_KEY;
			outBuf[idx++] = s;
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
			scanf("%u", &s);
			outBuf[idx++] = CMD_REPEAT;
			outBuf[idx++] = s;
			printf("enter value (dec)\n");
			scanf("%hu", &kk);
			outBuf[idx++] = kk & 0xFF;
			outBuf[idx++] = (kk>>8) & 0xFF;
			write_and_check(idx, 4);
			break;
		case 'a':
			outBuf[idx++] = CMD_ALARM;
			printf("enter alarm\n");
			scanf("%llx", &i);
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
			scanf("%u", &s);
			outBuf[idx++] = s;
			write_and_check(idx, 4);
			break;
		case 'n':
			outBuf[idx++] = CMD_NEOPIXEL;
			printf("enter led number (starting with 1)\n");
			scanf("%u", &s);
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
				printf("enter red in hex\n");
				scanf("%u", &s);
				outBuf[idx++] = s;
				printf("enter green in hex\n");
				scanf("%u", &s);
				outBuf[idx++] = s;
				printf("enter blue in hex\n");
				scanf("%u", &s);
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
			scanf("%u", &s);
			outBuf[idx++] = CMD_WAKE_REMOTE;
			outBuf[idx++] = s;
			break;
		case 'm':
			printf("enter macro number (starting with 0)\n");
			scanf("%u", &m);
			outBuf[idx++] = CMD_MACRO_REMOTE;
			outBuf[idx++] = m;    // (m+1)-th macro
			printf("enter slot number, 0 for trigger\n");
			scanf("%u", &s);
			outBuf[idx++] = s;    // (s+1)-th slot
			break;
		case 'i':
			printf("enter IR-data number (starting with 0)\n");
			scanf("%u", &s);
			outBuf[idx++] = CMD_IRDATA_REMOTE;
			outBuf[idx++] = s;
			break;
		default:
			goto Set;
		}
		write_and_check(idx, 4);
		break;

	case 'g':
get:		printf("get wakeup(w)\nget macro(m)\nget IR-data (i)\nget key(k)\nget repeat(r)\nget caps(c)\nget alarm(a)\nget eeprom(e)\nget raw eeprom from RP2xxx(p)\n");
		scanf("%s", &d);
		memset(&outBuf[2], 0, sizeof(outBuf) - 2);
		idx = 2;
		outBuf[idx++] = ACC_GET;
		switch (d) {
		case 'w':
			printf("enter wakeup number (starting with 0)\n");
			scanf("%u", &s);
			outBuf[idx++] = CMD_WAKE;
			outBuf[idx++] = s;
			write_and_check(idx, 10);
			break;
		case 'm':
			printf("enter macro number (starting with 0)\n");
			scanf("%u", &m);
			outBuf[idx++] = CMD_MACRO;
			outBuf[idx++] = m;    // (m+1)-th macro
			printf("enter slot number, 0 for trigger\n");
			scanf("%u", &s);
			outBuf[idx++] = s;    // (s+1)-th slot
			write_and_check(idx, 10);
			break;
		case 'i':
			printf("enter IR-data number (starting with 0)\n");
			scanf("%u", &s);
			outBuf[idx++] = CMD_IRDATA;
			outBuf[idx++] = s;
			write_and_check(idx, 10);
			break;
		case 'k':
			printf("enter key number (starting with 0)\n");
			scanf("%u", &s);
			outBuf[idx++] = CMD_KEY;
			outBuf[idx++] = s;
			write_and_check(idx, 6);
			break;
		case 'r':
			printf("get repeat delay(0)\nget repeat period(1)\nget repeat timeout(2)\n");
			scanf("%u", &s);
			outBuf[idx++] = CMD_REPEAT;
			outBuf[idx++] = s;
			write_and_check(idx, 6);
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
				#ifdef WIN32
				Sleep(3);
				#else
				usleep(3000);
				#endif
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
				hid_write(handle, outBuf, idx);
				#ifdef WIN32
				Sleep(3);
				#else
				usleep(3000);
				#endif
				hid_read(handle, inBuf, 10);
				for (int i = 4; i < 10; i++)
					printf("%02x", inBuf[i]);
				printf(" ");
				idx = 3;
				outBuf[idx++] = CMD_KEY;
				outBuf[idx++] = l;
				#ifdef WIN32
				Sleep(3);
				#else
				usleep(3000);
				#endif
				hid_write(handle, outBuf, idx);
				#ifdef WIN32
				Sleep(3);
				#else
				usleep(3000);
				#endif
				hid_read(handle, inBuf, 6);
				for(int n=0; n < lines-1; n++) {
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
				#ifdef WIN32
				Sleep(3);
				#else
				usleep(3000);
				#endif
				idx = 3;
			}
			goto out;
			break;
		case 'p':
			outBuf[idx++] = CMD_EEPROM_GET_RAW;
			for(k = 31; k >= 0; k--) { // FLASH_SECTOR_SIZE * nr_sectors / size
				outBuf[idx] = k;
				for(l = 0; l < 16; l++) { // size / 32
					outBuf[idx+1] = l;
					hid_write(handle, outBuf, idx+2);
					#ifdef WIN32
					Sleep(3);
					#else
					usleep(3000);
					#endif
					retValm = hid_read(handle, inBuf, in_size);
					if (retValm < 0) {
						printf("read error\n");
					} else {
						for (int i = 4; i < 36; i++)
							printf("%02x ", inBuf[i]);
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
reset:		printf("reset wakeup(w)\nreset macro slot(m)\nreset IR-data(i)\nreset key(k)\nreset repeat(r)\nreset alarm(a)\nreset eeprom(e)\n");
		scanf("%s", &d);
		memset(&outBuf[2], 0, sizeof(outBuf) - 2);
		idx = 2;
		outBuf[idx++] = ACC_RESET;
		switch (d) {
		case 'w':
			printf("enter slot number (starting with 0)\n");
			scanf("%u", &s);
			outBuf[idx++] = CMD_WAKE;
			outBuf[idx++] = s;
			break;
		case 'm':
			printf("enter macro number (starting with 0)\n");
			scanf("%u", &m);
			outBuf[idx++] = CMD_MACRO;
			outBuf[idx++] = m;    // (m+1)-th macro
			printf("enter slot number, 0 for trigger\n");
			scanf("%u", &s);
			outBuf[idx++] = s;    // (s+1)-th slot
			break;
		case 'i':
			printf("enter IR-data number (starting with 0)\n");
			scanf("%u", &s);
			outBuf[idx++] = CMD_IRDATA;
			outBuf[idx++] = s;
			break;
		case 'k':
			printf("enter key number (starting with 0)\n");
			scanf("%u", &s);
			outBuf[idx++] = CMD_KEY;
			outBuf[idx++] = s;
			break;
		case 'r':
			printf("reset repeat delay(0)\nreset repeat period(1)\nreset repeat timeout(2)\n");
			scanf("%u", &s);
			outBuf[idx++] = CMD_REPEAT;
			outBuf[idx++] = s;
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
		scanf("%llx", &i);
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
		hid_close(handle);
		#ifdef WIN32
		Sleep(1900);
		#else
		usleep(1900000);
		#endif
		for(l=0;l<6;l++) {
			if(open_stm32() == true)
				break;
			#ifdef WIN32
			Sleep(100);
			#else
			usleep(100000);
			#endif
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
			#ifdef WIN32
			Sleep(1000);
			#else
			usleep(1000000);
			#endif
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
			#ifdef WIN32
			Sleep(1000);
			#else
			usleep(1000000);
			#endif
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
		retValm = hid_read(handle, inBuf, in_size);
		if (retValm >= 0) {
			printf("read %d bytes:\n\t", retValm);
			for (l = 0; l < 64; l++)
				printf("%02x ", inBuf[l]);
			printf("\n");
			printf("modifier|key: %02hhx%02x\n\n", inBuf[1],inBuf[3]);
			printf("\n");
		}
	}

exit:	hid_close(handle);

	/* Free static HIDAPI objects. */
	hid_exit();

#ifdef WIN32
	Sleep(2);
#endif

	return 0;
}
