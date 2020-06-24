/**********************************************************************************************************
	stm32IRconfig: configure and monitor IRMP_STM32_KBD

	Copyright (C) 2014-2020 Joerg Riechardt

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
	CMD_REPEAT
};

enum status {
	STAT_CMD,
	STAT_SUCCESS,
	STAT_FAILURE
};

enum report_id {
	REPORT_ID_IR = 1,
	REPORT_ID_CONFIG_IN = 2,
	REPORT_ID_CONFIG_OUT = 3
};

static int stm32fd = -1;
uint8_t inBuf[17];
uint8_t outBuf[17];

static bool open_stm32(const char *devicename) {
	stm32fd = open(devicename, O_RDWR );
	if (stm32fd == -1) {
		printf("error opening stm32 device: %s\n",strerror(errno));
		return false;
	}
	printf("opened stm32 device\n");
	return true;
}

static void read_stm32() {
	int retVal;
	retVal = read(stm32fd, inBuf, sizeof(inBuf));
	if (retVal < 0) {
		printf("read error\n");
	} else {
		printf("read %d bytes:\n\t", retVal);
		for (int i = 0; i < retVal; i++)
			printf("%02hhx ", inBuf[i]);
		puts("\n");
	}
} 

static void write_stm32() {
	int retVal;
	retVal = write(stm32fd, outBuf, sizeof(outBuf));
	if (retVal < 0) {
		printf("write error\n");
	} else {
		printf("written %d bytes:\n\t", retVal);
		for (int i = 0; i < retVal; i++)
			printf("%02hhx ", outBuf[i]);
		puts("\n");
	}
}

void write_and_check() {
	write_stm32();
	usleep(2000);
	read_stm32(); // blocking per default, waits until data arrive
	while (inBuf[0] == REPORT_ID_IR)
		read_stm32();
	if (inBuf[1] == STAT_SUCCESS) {
		puts("*****************************OK********************************\n");
	} else {
		puts("***************************ERROR*******************************\n");
	}
}

int main(int argc, const char **argv) {

	uint64_t i;
	uint16_t kk = 0x0000;
	char c, d;
	uint8_t n, k, l, idx;
	int retValm, jump_to_firmware;

	open_stm32(argc>1 ? argv[1] : "/dev/irmp_stm32");

	outBuf[0] = REPORT_ID_CONFIG_OUT;
	outBuf[1] = STAT_CMD;

cont:	printf("program eeprom: wakeups, IR-data, keys and repeat (p)\nprogram eeprom: wakeups and IR-data with remote control (q)\nget eeprom: wakeups, IR-data, keys, repeat and capabilities (g)\nreset: wakeups, IR-data, keys, repeat and alarm (r)\nset alarm (s)\nget alarm (a)\nreboot (b)\nmonitor until ^C (m)\nexit (x)\n");
	scanf("%s", &c);

	switch (c) {

	case 'p':
prog:		printf("set wakeup(w)\nset IR-data(i)\nset key(k)\nset repeat(r)\n");
		scanf("%s", &d);
		memset(&outBuf[2], 0, 15);
		idx = 2;
		outBuf[idx++] = ACC_SET;
		switch (d) {
		case 'w':
			printf("enter wakeup number (starting with 0)\n");
			scanf("%" SCNx8 "", &n);
			outBuf[idx++] = CMD_WAKE;
			outBuf[idx++] = n;
			printf("enter IRData (protocoladdresscommandflag)\n");
			scanf("%" SCNx64 "", &i);
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
			scanf("%" SCNx8 "", &n);
			outBuf[idx++] = CMD_IRDATA;
			outBuf[idx++] = n;
			printf("enter IRData (protocoladdresscommandflag)\n");
			scanf("%" SCNx64 "", &i);
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
			scanf("%" SCNx8 "", &n);
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
		case 'r':
			printf("set repeat delay(0)\nset repeat period(1)\nset repeat timeout(2)\n");
			scanf("%" SCNx8 "", &n);
			outBuf[idx++] = CMD_REPEAT;
			outBuf[idx++] = n;
			printf("enter value (dec)\n");
			scanf("%" SCNu16 "", &kk);
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
			scanf("%" SCNx8 "", &n);
			outBuf[idx++] = CMD_WAKE_REMOTE;
			outBuf[idx++] = n;
			break;
		case 'i':
			printf("enter IR-data number (starting with 0)\n");
			scanf("%" SCNx8 "", &n);
			outBuf[idx++] = CMD_IRDATA_REMOTE;
			outBuf[idx++] = n;
			break;
		default:
			goto Prog;
		}
		write_and_check();
		break;

	case 'g':
get:		printf("get wakeup(w)\nget IR-data (i)\nget key(k)\nget repeat(r)\nget caps(c)\n");
		scanf("%s", &d);
		memset(&outBuf[2], 0, 15);
		idx = 2;
		outBuf[idx++] = ACC_GET;
		switch (d) {
		case 'w':
			printf("enter wakeup number (starting with 0)\n");
			scanf("%" SCNx8 "", &n);
			outBuf[idx++] = CMD_WAKE;
			outBuf[idx++] = n;
			break;
		case 'i':
			printf("enter IR-data number (starting with 0)\n");
			scanf("%" SCNx8 "", &n);
			outBuf[idx++] = CMD_IRDATA;
			outBuf[idx++] = n;
			break;
		case 'k':
			printf("enter key number (starting with 0)\n");
			scanf("%" SCNx8 "", &n);
			outBuf[idx++] = CMD_KEY;
			outBuf[idx++] = n;
			break;
		case 'r':
			printf("get repeat delay(0)\nget repeat period(1)\nget repeat timeout(2)\n");
			scanf("%" SCNx8 "", &n);
			outBuf[idx++] = CMD_REPEAT;
			outBuf[idx++] = n;
			break;
		case 'c':
			jump_to_firmware = 0;
			outBuf[idx++] = CMD_CAPS;
			for (l = 0; l < 20; l++) { // for safety stop after 20 loops
				outBuf[idx] = l;
				write_stm32();
				usleep(2000);
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
							if (!inBuf[k]) { // NULL termination
								printf("\n\n");
								jump_to_firmware = 1;
								goto again;
							}
							printf("%u ", inBuf[k]);
						}
					} else { // queries for firmware
						printf("firmware: ");
						for (k = 4; k < 17; k++) {
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
		write_and_check();
out:
		break;

	case 'r':
reset:		printf("reset wakeup(w)\nreset IR-data(i)\nreset key(k)\nreset repeat(r)\nreset alarm(a)\n");
		scanf("%s", &d);
		memset(&outBuf[2], 0, 15);
		idx = 2;
		outBuf[idx++] = ACC_RESET;
		switch (d) {
		case 'w':
			printf("enter slot number (starting with 0)\n");
			scanf("%" SCNx8 "", &n);
			outBuf[idx++] = CMD_WAKE;
			outBuf[idx++] = n;
			break;
		case 'i':
			printf("enter IR-data number (starting with 0)\n");
			scanf("%" SCNx8 "", &n);
			outBuf[idx++] = CMD_IRDATA;
			outBuf[idx++] = n;
			break;
		case 'k':
			printf("enter key number (starting with 0)\n");
			scanf("%" SCNx8 "", &n);
			outBuf[idx++] = CMD_KEY;
			outBuf[idx++] = n;
			break;
		case 'r':
			printf("reset repeat delay(0)\nreset repeat period(1)\nreset repeat timeout(2)\n");
			scanf("%" SCNx8 "", &n);
			outBuf[idx++] = CMD_REPEAT;
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
		scanf("%" SCNx64 "", &i);
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
		close(stm32fd);
		usleep(1900000);
		for(l=0;l<6;l++) {
			if(open_stm32(argc>1 ? argv[1] : "/dev/irmp_stm32") == true)
				break;
			usleep(100000);
		}
		break;

	case 'm':
		goto monit;
		break;

	case 'x':
		goto exit;
		break;

	default:
		goto cont;
	}

	goto cont;

monit:	while(true) {
		retValm = read(stm32fd, inBuf, sizeof(inBuf));
		if (retValm >= 0) {
			printf("read %d bytes:\n\t", retValm);
			for (l = 0; l < retValm; l++)
				printf("%02hhx ", inBuf[l]);
			printf("\n");
			printf("converted to protocoladdresscommandflag:\n\t");
			printf("%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx", inBuf[1],inBuf[3],inBuf[2],inBuf[5],inBuf[4],inBuf[6]);
			printf("\n\n");
		}
	}

exit:	if (stm32fd >= 0) close(stm32fd);
	return 0;
}
