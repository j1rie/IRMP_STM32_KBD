/**********************************************************************************************************
	stm32kbdIRconfig: configure IRMP_STM32_KBD

	Copyright (C) 2014-2022 Joerg Riechardt

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
	CMD_EEPROM_RESET
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
			printf("%02x ", inBuf[i]);
		puts("\n");
	}
} 

static void write_stm32(int out_size) {
	int retVal;
	retVal = hid_write(handle, outBuf, out_size);
	if (retVal < 0) {
		printf("write error: %ls\n", hid_error(handle));
	} else {
		printf("written %d bytes:\n\t", retVal);
		for (int i = 0; i < out_size; i++)
			printf("%02x ", outBuf[i]);
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
	char c, d;
	uint8_t k, l, idx, eeprom_lines;
	unsigned int n;
	int jump_to_firmware;

#ifdef WIN32
	UNREFERENCED_PARAMETER(argc);
	UNREFERENCED_PARAMETER(argv);
#endif

	if (hid_init())
		return -1;

	open_stm32();

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
	hid_read(handle, inBuf, 9);
	while (inBuf[0] == 0x01)
		hid_read(handle, inBuf, 9);
	eeprom_lines = inBuf[4];
	in_size = inBuf[7] ? inBuf[7] : 17;
	out_size = inBuf[8] ? inBuf[8] : 17;
	printf("hid in report count: %u\n", in_size);
	printf("hid out report count: %u\n", out_size);
	if(!inBuf[7] || !inBuf[8])
		printf("old firmware!\n");
	puts("");

cont:	printf("set eeprom: wakeups, IR-data, keys, repeat and alarm (s)\nset eeprom by remote: wakeups and IR-data (q)\nget eeprom: wakeups, IR-data, keys, repeat, alarm, capabilities and eeprom (g)\nreset: wakeups, IR-data, keys, repeat, alarm and eeprom (r)\nreboot (b)\nmonitor until ^C (m)\nexit (x)\n");
	scanf("%s", &c);

	switch (c) {

	case 's':
set:		printf("set wakeup(w)\nset IR-data(i)\nset key(k)\nset repeat(r)\nset alarm(a)\n");
		scanf("%s", &d);
		memset(&outBuf[2], 0, out_size - 2);
		idx = 2;
		outBuf[idx++] = ACC_SET;
		switch (d) {
		case 'w':
			printf("enter wakeup number (starting with 0)\n");
			scanf("%u", &n);
			outBuf[idx++] = CMD_WAKE;
			outBuf[idx++] = n;
			printf("enter IRData (protocoladdresscommandflag)\n");
			scanf("%I64x", &i);
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
			scanf("%u", &n);
			outBuf[idx++] = CMD_IRDATA;
			outBuf[idx++] = n;
			printf("enter IRData (protocoladdresscommandflag)\n");
			scanf("%I64x", &i);
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
			scanf("%u", &n);
			outBuf[idx++] = CMD_KEY;
			outBuf[idx++] = n;
			printf("enter key (KEY_xxx)\n");
			scanf("%s", &c);
			for(l=0; l < lines; l++) {
			    if(!strcmp(mapusb[l].key, &c)) {
				kk = mapusb[l].usb_hid_key;
				break;
			    }
			}
			outBuf[idx++] = kk;
			printf("enter modifier (KEY_xxx)\n");
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
			scanf("%u", &n);
			outBuf[idx++] = CMD_REPEAT;
			outBuf[idx++] = n;
			printf("enter value (dec)\n");
			scanf("%hu", &kk);
			outBuf[idx++] = kk & 0xFF;
			outBuf[idx++] = (kk>>8) & 0xFF;
			write_and_check(idx, 4);
			break;
		case 'a':
			outBuf[idx++] = CMD_ALARM;
			printf("enter alarm\n");
			scanf("%I64x", &i);
			memcpy(&outBuf[idx], &i, 4);
			write_and_check(idx + 4, 4);
			break;
		default:
			goto set;
		}
		break;

	case 'q':
Set:		printf("set wakeup with remote control(w)\nset IR-data with remote control(i)\n");
		scanf("%s", &d);
		memset(&outBuf[2], 0, out_size - 2);
		idx = 2;
		outBuf[idx++] = ACC_SET;
		switch (d) {
		case 'w':
			printf("enter wakeup number (starting with 0)\n");
			scanf("%u", &n);
			outBuf[idx++] = CMD_WAKE_REMOTE;
			outBuf[idx++] = n;
			break;
		case 'i':
			printf("enter IR-data number (starting with 0)\n");
			scanf("%u", &n);
			outBuf[idx++] = CMD_IRDATA_REMOTE;
			outBuf[idx++] = n;
			break;
		default:
			goto Set;
		}
		write_and_check(idx, 4);
		break;

	case 'g':
get:		printf("get wakeup(w)\nget IR-data (i)\nget key(k)\nget repeat(r)\nget caps(c)\nget alarm(a)\nget eeprom(e)\n");
		scanf("%s", &d);
		memset(&outBuf[2], 0, out_size - 2);
		idx = 2;
		outBuf[idx++] = ACC_GET;
		switch (d) {
		case 'w':
			printf("enter wakeup number (starting with 0)\n");
			scanf("%u", &n);
			outBuf[idx++] = CMD_WAKE;
			outBuf[idx++] = n;
			write_and_check(idx, 10);
			break;
		case 'i':
			printf("enter IR-data number (starting with 0)\n");
			scanf("%u", &n);
			outBuf[idx++] = CMD_IRDATA;
			outBuf[idx++] = n;
			write_and_check(idx, 10);
			break;
		case 'k':
			printf("enter key number (starting with 0)\n");
			scanf("%u", &n);
			outBuf[idx++] = CMD_KEY;
			outBuf[idx++] = n;
			write_and_check(idx, 6);
			break;
		case 'r':
			printf("get repeat delay(0)\nget repeat period(1)\nget repeat timeout(2)\n");
			scanf("%u", &n);
			outBuf[idx++] = CMD_REPEAT;
			outBuf[idx++] = n;
			write_and_check(idx, 6);
			break;
		case 'a':
			outBuf[idx++] = CMD_ALARM;
			write_and_check(idx, 8);
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
				for(n=0; n < lines-1; n++) {
					if(mapusb[n].usb_hid_key == inBuf[5]) {
						printf("%s|", mapusb[n].key);
					}
				}
				for(n=0; n < lines-1; n++) {
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
				read_stm32(in_size, l == 0 ? 7 : in_size);
				while (inBuf[0] == 0x01)
					read_stm32(in_size, l == 0 ? 7 : in_size);
				if (!l) { // first query for slots and depth
					printf("number of keys: %u\n", inBuf[4]);
					//printf("macro_depth: %u\n", inBuf[5]);
					printf("number of wakeups (including reboot): %u\n", inBuf[6]);
				} else {
					if(!jump_to_firmware) { // queries for supported_protocols
						printf("protocols: ");
						for (k = 4; k < in_size; k++) {
							if (!inBuf[k]) { // NULL termination
								printf("\n\n");
								jump_to_firmware = 1;
								goto again;
							}
							printf("%u ", inBuf[k]);
						}
					} else { // queries for firmware
						printf("firmware: ");
						for (k = 4; k < in_size; k++) {
							if (!inBuf[k]) { // NULL termination
								printf("\n\n");
								goto out;
							}
							printf("%c", inBuf[k]);
						}
					}
				}
				printf("\n\n");
again:			;
			}
			break;
		default:
			goto get;
		}
out:
		break;

	case 'r':
reset:		printf("reset wakeup(w)\nreset IR-data(i)\nreset key(k)\nreset repeat(r)\nreset alarm(a)\nreset eeprom(e)\n");
		scanf("%s", &d);
		memset(&outBuf[2], 0, out_size - 2);
		idx = 2;
		outBuf[idx++] = ACC_RESET;
		switch (d) {
		case 'w':
			printf("enter slot number (starting with 0)\n");
			scanf("%u", &n);
			outBuf[idx++] = CMD_WAKE;
			outBuf[idx++] = n;
			break;
		case 'i':
			printf("enter IR-data number (starting with 0)\n");
			scanf("%u", &n);
			outBuf[idx++] = CMD_IRDATA;
			outBuf[idx++] = n;
			break;
		case 'k':
			printf("enter key number (starting with 0)\n");
			scanf("%u", &n);
			outBuf[idx++] = CMD_KEY;
			outBuf[idx++] = n;
			break;
		case 'r':
			printf("reset repeat delay(0)\nreset repeat period(1)\nreset repeat timeout(2)\n");
			scanf("%u", &n);
			outBuf[idx++] = CMD_REPEAT;
			outBuf[idx++] = n;
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

	case 'b':
		memset(&outBuf[2], 0, out_size - 2);
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

	case 'x':
		goto exit;
		break;

	default:
		goto cont;
	}

	goto cont;

exit:	hid_close(handle);

	/* Free static HIDAPI objects. */
	hid_exit();

#ifdef WIN32
	Sleep(2);
#endif

	return 0;
}
