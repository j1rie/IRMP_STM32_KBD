/**********************************************************************************************************
	stm32IRconfig: configure and monitor IRMP_STM32_KBD

	Copyright (C) 2014-2022 Joerg Riechardt

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

static int stm32fd = -1;
uint8_t inBuf[64];
uint8_t outBuf[64];
unsigned int in_size, out_size;

static bool open_stm32(const char *devicename) {
	stm32fd = open(devicename, O_RDWR );
	if (stm32fd == -1) {
		printf("error opening stm32 device: %s\n",strerror(errno));
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

static void write_stm32(int out_size) {
	int retVal;
	retVal = write(stm32fd, outBuf, out_size);
	if (retVal < 0) {
		printf("write error\n");
	} else {
		printf("written %d bytes:\n\t", retVal);
		for (int i = 0; i < out_size; i++)
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

	outBuf[2] = ACC_GET;
	outBuf[3] = CMD_CAPS;
	outBuf[4] = 0;
	write(stm32fd, outBuf, 5);
	usleep(3000);
	read(stm32fd, inBuf, 9);
	while (inBuf[0] == 0x01)
		retValm = read(stm32fd, inBuf, 9);
	in_size = inBuf[7] ? inBuf[7] : 17;
	out_size = inBuf[8] ? inBuf[8] : 17;
	printf("hid in report count: %u\n", in_size);
	printf("hid out report count: %u\n", out_size);
	if(!inBuf[7] || !inBuf[8])
		printf("old firmware!\n");
	puts("");

cont:	printf("program eeprom: wakeups, IR-data, keys and repeat (p)\nprogram eeprom: wakeups and IR-data with remote control (q)\nget eeprom: wakeups, IR-data, keys, repeat and capabilities (g)\nreset: wakeups, IR-data, keys, repeat, alarm and eeprom (r)\nset alarm (s)\nget alarm (a)\nreboot (b)\nmonitor until ^C (m)\nexit (x)\n");
	scanf("%s", &c);

	switch (c) {

	case 'p':
prog:		printf("set wakeup(w)\nset IR-data(i)\nset key(k)\nset repeat(r)\n");
		scanf("%s", &d);
		memset(&outBuf[2], 0, out_size - 2);
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
			write_and_check(idx, 4);
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
			write_and_check(idx, 4);
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
			write_and_check(idx, 4);
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
			write_and_check(idx, 4);
			break;
		default:
			goto prog;
		}
		break;

	case 'q':
Prog:		printf("set wakeup with remote control(w)\nset IR-data with remote control(i)\n");
		scanf("%s", &d);
		memset(&outBuf[2], 0, out_size - 2);
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
		write_and_check(idx, 4);
		break;

	case 'g':
get:		printf("get wakeup(w)\nget IR-data (i)\nget key(k)\nget repeat(r)\nget caps(c)\n");
		scanf("%s", &d);
		memset(&outBuf[2], 0, out_size - 2);
		idx = 2;
		outBuf[idx++] = ACC_GET;
		switch (d) {
		case 'w':
			printf("enter wakeup number (starting with 0)\n");
			scanf("%" SCNx8 "", &n);
			outBuf[idx++] = CMD_WAKE;
			outBuf[idx++] = n;
			l = 10;
			break;
		case 'i':
			printf("enter IR-data number (starting with 0)\n");
			scanf("%" SCNx8 "", &n);
			outBuf[idx++] = CMD_IRDATA;
			outBuf[idx++] = n;
			l = 10;
			break;
		case 'k':
			printf("enter key number (starting with 0)\n");
			scanf("%" SCNx8 "", &n);
			outBuf[idx++] = CMD_KEY;
			outBuf[idx++] = n;
			l = 6;
			break;
		case 'r':
			printf("get repeat delay(0)\nget repeat period(1)\nget repeat timeout(2)\n");
			scanf("%" SCNx8 "", &n);
			outBuf[idx++] = CMD_REPEAT;
			outBuf[idx++] = n;
			l = 6;
			break;
		case 'c':
			jump_to_firmware = 0;
			outBuf[idx++] = CMD_CAPS;
			for (l = 0; l < 20; l++) { // for safety stop after 20 loops
				outBuf[idx] = l;
				write_stm32(idx+1);
				usleep(3000);
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
		write_and_check(idx, l);
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

	case 's':
		memset(&outBuf[2], 0, out_size - 2);
		idx = 2;
		outBuf[idx++] = ACC_SET;
		outBuf[idx++] = CMD_ALARM;
		printf("enter alarm\n");
		scanf("%" SCNx64 "", &i);
		memcpy(&outBuf[idx], &i, 4);
		write_and_check(idx + 4, 4);
		break;

	case 'a':
		memset(&outBuf[2], 0, out_size - 2);
		idx = 2;
		outBuf[idx++] = ACC_GET;
		outBuf[idx++] = CMD_ALARM;
		write_and_check(idx, 8);
		break;

	case 'b':
		memset(&outBuf[2], 0, out_size - 2);
		idx = 2;
		outBuf[idx++] = ACC_SET;
		outBuf[idx++] = CMD_REBOOT;
		write_and_check(idx, 4);
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
		retValm = read(stm32fd, inBuf, in_size);
		if (retValm >= 0) {
			printf("read %d bytes:\n\t", retValm);
			for (l = 0; l < 5; l++)
				printf("%02hhx ", inBuf[l]);
			printf("\n");
			printf("modifier|key: %02hhx%02hhx\n\n", inBuf[1],inBuf[3]);
			printf("\n");
		}
	}

exit:	if (stm32fd >= 0) close(stm32fd);
	return 0;
}
