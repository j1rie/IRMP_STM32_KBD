/**********************************************************************************************************  
    stm32kbdstatusled: switch statusled on/off on IRMP_STM32

    Copyright (C) 2014-2024 Joerg Riechardt

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

************************************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <unistd.h>
#include <sys/un.h>
#include <errno.h>
#include <fcntl.h>

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

static int stm32fd = -1;
uint8_t inBuf[64];
uint8_t outBuf[64];

static bool open_stm32(const char *devicename) {
	stm32fd = open(devicename, O_RDWR);
	if (stm32fd == -1) {
		printf("error opening stm32 device: %s\n",strerror(errno));
		return false;
	}
	//printf("opened stm32 device\n");
	return true;
}

static void read_stm32() {
	int retVal;
	retVal = read(stm32fd, inBuf, sizeof(inBuf));
	if (retVal < 0) {
	    printf("read error\n");
        } /*else {
                printf("read %d bytes:\n\t", retVal);
                for (int i = 0; i < retVal; i++)
                        printf("%02hhx ", inBuf[i]);
                puts("\n");
        }*/
} 

static void write_stm32() {
	int retVal;
	retVal = write(stm32fd, outBuf, sizeof(outBuf));
	if (retVal < 0) {
	    printf("write error\n");
        } /*else {
                printf("written %d bytes:\n\t", retVal);
                for (int i = 0; i < retVal; i++)
                        printf("%02hhx ", outBuf[i]);
                puts("\n");
        }*/
}

int main(int argc, char *argv[]) {
	
	uint8_t led_state;
	int opt = 0;
	char *dvalue = NULL;
	char *svalue = NULL;
	
	while ((opt = getopt(argc, argv, "d:s:")) != -1) {
	    switch (opt) {
	    case 'd':
		dvalue = optarg;
		break;
	    case 's':
		svalue = optarg;
		break;
	    default:
		break;
	    }
	}

	open_stm32(dvalue != NULL ? dvalue : "/dev/irmp_stm32_kbd");
        outBuf[0] = REPORT_ID_CONFIG_OUT;
	outBuf[1] = STAT_CMD;

	if (svalue != NULL) {
	    outBuf[2] = ACC_SET;
	    outBuf[3] = CMD_STATUSLED;
	    led_state = strtoul(svalue, NULL, 0);
	    outBuf[4] = led_state;
	    write_stm32();
	    usleep(3000);
	    read_stm32();
	    while (inBuf[0] == REPORT_ID_KBD)
		read_stm32();
	}
	
	if (stm32fd >= 0) close(stm32fd);
	return 0;
}
