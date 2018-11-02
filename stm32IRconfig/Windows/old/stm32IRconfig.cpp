/**********************************************************************************************************
	stm32config: configure and IRMP_STM32_KBD

	Copyright (C) 2014-2018 Jörg Riechardt

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

enum __attribute__ ((__packed__)) access {
	ACC_GET,
	ACC_SET,
	ACC_RESET
};

enum __attribute__ ((__packed__)) command {
	CMD_CAPS,
	CMD_ALARM,
	CMD_IRDATA,
	CMD_KEY,
	CMD_WAKE,
	CMD_REBOOT,
	CMD_IRDATA_REMOTE,
	CMD_WAKE_REMOTE
};

enum __attribute__ ((__packed__)) status {
	STAT_CMD,
	STAT_SUCCESS,
	STAT_FAILURE
};

hid_device *handle;
unsigned char inBuf[17];
unsigned char outBuf[17];

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

static void read_stm32() {
	int i;
	int retVal;
	retVal = hid_read(handle, inBuf, sizeof(inBuf));
	if (retVal < 0) {
		printf("read error\n");
	} else {
		printf("read %d bytes:\n\t", retVal);
		for (i = 0; i < retVal; i++)
			printf("%02hx ", inBuf[i]);
		puts("\n");
	}
} 

static void write_stm32() {
	int i;
	int retVal;
	retVal = hid_write(handle, outBuf, sizeof(outBuf));
	if (retVal < 0) {
		printf("write error: %ls\n", hid_error(handle));
	} else {
		printf("written %d bytes:\n\t", retVal);
		for (i = 0; i < retVal; i++)
			printf("%02hx ", outBuf[i]);
		puts("\n");
	}
}

void write_and_check() {
	write_stm32();
	#ifdef WIN32
	Sleep(2);
	#else
	usleep(2000);
	#endif
	read_stm32();
	while (inBuf[0] == 0x01)
		read_stm32();
	if (inBuf[1] == STAT_SUCCESS) {
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
	uint8_t k, l, idx;
	unsigned int n;
	int jump_to_firmware;
	jump_to_firmware = 0;

#ifdef WIN32
	UNREFERENCED_PARAMETER(argc);
	UNREFERENCED_PARAMETER(argv);
#endif

	if (hid_init())
		return -1;

	open_stm32();

	outBuf[0] = 0x03; // Report ID Configuration, PC->STM32
	outBuf[1] = STAT_CMD;

cont:	printf("program eeprom: wakeups, IR-data and keys (p)\nprogram eeprom: wakeups and IR-data with remote control (q)\nget eeprom: wakeups, IR-data, keys and capabilities (g)\nreset: wakeups, IR-data, keys and alarm (r)\nset alarm (s)\nget alarm (a)\nreboot (b)\nexit (x)\n");
	scanf("%s", &c);

	switch (c) {

	case 'p':
prog:		printf("set wakeup(w)\nset IR-data(i)\nset key(k)\n");
		scanf("%s", &d);
		memset(&outBuf[2], 0, 15);
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
			write_and_check();
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
			write_and_check();
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
			outBuf[idx++] = kk & 0xFF;
			outBuf[idx++] = (kk>>8) & 0xFF;
			write_and_check();
			break;
		default:
			goto prog;
		}
		break;

	case 'q':
Prog:		printf("set wakeup with remote control(w)\nset IR-data with remote control(i)\n");
		scanf("%s", &d);
		memset(&outBuf[2], 0, 15);
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
			goto Prog;
		}
		write_and_check();
		break;

	case 'g':
get:		printf("get wakeup(w)\nget IR-data (i)\nget key(k)\nget caps(c)\n");
		scanf("%s", &d);
		memset(&outBuf[2], 0, 15);
		idx = 2;
		outBuf[idx++] = ACC_GET;
		switch (d) {
		case 'w':
			printf("enter wakeup number (starting with 0)\n");
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
		case 'c':
			outBuf[idx++] = CMD_CAPS;
			for (l = 0; l < 20; l++) { // for safety stop after 20 loops
				outBuf[idx] = l;
				write_stm32();
				#ifdef WIN32
				Sleep(2);
				#else
				usleep(2000);
				#endif
				read_stm32();
				while (inBuf[0] == 0x01)
					read_stm32();
				if (!l) { // first query for slots and depth
					printf("number of keys: %u\n", inBuf[4]);
					//printf("macro_depth: %u\n", inBuf[5]);
					printf("number of wakeups (including reboot): %u\n", inBuf[6]);
				} else {
					if(!jump_to_firmware) { // queries for supported_protocols
						printf("protocols: ");
						for (k = 4; k < 17; k++) {
							if (!inBuf[k]) {
								printf("\n\n");
								jump_to_firmware = 1;
								goto again;
							}
							printf("%u ", inBuf[k]);
						}
					} else { // queries for firmware
						printf("firmware: ");
						for (k = 4; k < 17; k++) {
							if (!inBuf[k]) {
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
		write_and_check();
out:		break;

	case 'r':
reset:		printf("reset wakeup(w)\nreset IR-data(i)\nreset key(k)\nreset alarm(a)\n");
		scanf("%s", &d);
		memset(&outBuf[2], 0, 15);
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
		case 'a':
			outBuf[idx++] = CMD_ALARM;
			break;
		default:
			goto reset;
		}
		write_and_check();
		break;

	case 's':
		memset(&outBuf[2], 0, 15);
		idx = 2;
		outBuf[idx++] = ACC_SET;
		outBuf[idx++] = CMD_ALARM;
		printf("enter alarm\n");
		scanf("%I64x", &i);
		memcpy(&outBuf[idx++], &i, 4);
		write_and_check();
		break;

	case 'a':
		memset(&outBuf[2], 0, 15);
		idx = 2;
		outBuf[idx++] = ACC_GET;
		outBuf[idx++] = CMD_ALARM;
		write_and_check();
		break;

	case 'b':
		memset(&outBuf[2], 0, 15);
		idx = 2;
		outBuf[idx++] = ACC_SET;
		outBuf[idx++] = CMD_REBOOT;
		write_and_check();
		#ifdef WIN32
		Sleep(2500);
		#else
		usleep(2500000);
		#endif
		hid_close(handle);
		open_stm32();
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
