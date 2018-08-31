/*
 *  IR receiver, USB wakeup, motherboard switch wakeup, wakeup timer,
 *  USB HID keyboard device, eeprom emulation
 *
 *  Copyright (C) 2014-2018 Joerg Riechardt
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 */

#include "stm32f10x.h"
#include "usb_hid.h"
#include "irmpmain.h"
#include "eeprom.h"
#include "st_link_leds.h"
#include "config.h" /* CooCox workaround */
#ifdef DEBUG
#include <stdio.h>
#endif

#define BYTES_PER_QUERY	(HID_IN_BUFFER_SIZE - 4)
/* after plugging in, it takes some time, until SOF's are being sent to the device */
#define SOF_TIMEOUT 500

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

const char firmware[] = FW_STR;

const char supported_protocols[] = {
#if IRMP_SUPPORT_SIRCS_PROTOCOL==1
IRMP_SIRCS_PROTOCOL,
#endif
#if IRMP_SUPPORT_NEC_PROTOCOL==1
IRMP_NEC_PROTOCOL,
#endif
#if IRMP_SUPPORT_SAMSUNG_PROTOCOL==1
IRMP_SAMSUNG_PROTOCOL,
#endif
#if IRMP_SUPPORT_KASEIKYO_PROTOCOL==1
IRMP_KASEIKYO_PROTOCOL,
#endif
#if IRMP_SUPPORT_JVC_PROTOCOL==1
IRMP_JVC_PROTOCOL,
#endif
#if IRMP_SUPPORT_NEC16_PROTOCOL==1
IRMP_NEC16_PROTOCOL,
#endif
#if IRMP_SUPPORT_NEC42_PROTOCOL==1
IRMP_NEC42_PROTOCOL,
#endif
#if IRMP_SUPPORT_MATSUSHITA_PROTOCOL==1
IRMP_MATSUSHITA_PROTOCOL,
#endif
#if IRMP_SUPPORT_DENON_PROTOCOL==1
IRMP_DENON_PROTOCOL,
#endif
#if IRMP_SUPPORT_RC5_PROTOCOL==1
IRMP_RC5_PROTOCOL,
#endif
#if IRMP_SUPPORT_RC6_PROTOCOL==1
IRMP_RC6_PROTOCOL,
IRMP_RC6A_PROTOCOL,
#endif
#if IRMP_SUPPORT_IR60_PROTOCOL==1
IRMP_IR60_PROTOCOL,
#endif
#if IRMP_SUPPORT_GRUNDIG_PROTOCOL==1
IRMP_GRUNDIG_PROTOCOL,
#endif
#if IRMP_SUPPORT_SIEMENS_PROTOCOL==1
IRMP_SIEMENS_PROTOCOL,
#endif
#if IRMP_SUPPORT_NOKIA_PROTOCOL==1
IRMP_NOKIA_PROTOCOL,
#endif
#if IRMP_SUPPORT_BOSE_PROTOCOL==1
IRMP_BOSE_PROTOCOL,
#endif
#if IRMP_SUPPORT_KATHREIN_PROTOCOL==1
IRMP_KATHREIN_PROTOCOL,
#endif
#if IRMP_SUPPORT_NUBERT_PROTOCOL==1
IRMP_NUBERT_PROTOCOL,
#endif
#if IRMP_SUPPORT_FAN_PROTOCOL==1
IRMP_FAN_PROTOCOL,
#endif
#if IRMP_SUPPORT_SPEAKER_PROTOCOL==1
IRMP_SPEAKER_PROTOCOL,
#endif
#if IRMP_SUPPORT_BANG_OLUFSEN_PROTOCOL==1
IRMP_BANG_OLUFSEN_PROTOCOL,
#endif
#if IRMP_SUPPORT_RECS80_PROTOCOL==1
IRMP_RECS80_PROTOCOL,
#endif
#if IRMP_SUPPORT_RECS80EXT_PROTOCOL==1
IRMP_RECS80EXT_PROTOCOL,
#endif
#if IRMP_SUPPORT_THOMSON_PROTOCOL==1
IRMP_THOMSON_PROTOCOL,
#endif
#if IRMP_SUPPORT_NIKON_PROTOCOL==1
IRMP_NIKON_PROTOCOL,
#endif
#if IRMP_SUPPORT_NETBOX_PROTOCOL==1
IRMP_NETBOX_PROTOCOL,
#endif
#if IRMP_SUPPORT_ORTEK_PROTOCOL==1
IRMP_ORTEK_PROTOCOL,
#endif
#if IRMP_SUPPORT_TELEFUNKEN_PROTOCOL==1
IRMP_TELEFUNKEN_PROTOCOL,
#endif
#if IRMP_SUPPORT_FDC_PROTOCOL==1
IRMP_FDC_PROTOCOL,
#endif
#if IRMP_SUPPORT_RCCAR_PROTOCOL==1
IRMP_RCCAR_PROTOCOL,
#endif
#if IRMP_SUPPORT_ROOMBA_PROTOCOL==1
IRMP_ROOMBA_PROTOCOL,
#endif
#if IRMP_SUPPORT_RUWIDO_PROTOCOL==1
IRMP_RUWIDO_PROTOCOL,
#endif
#if IRMP_SUPPORT_A1TVBOX_PROTOCOL==1
IRMP_A1TVBOX_PROTOCOL,
#endif
#if IRMP_SUPPORT_LEGO_PROTOCOL==1
IRMP_LEGO_PROTOCOL,
#endif
#if IRMP_SUPPORT_RCMM_PROTOCOL==1
IRMP_RCMM32_PROTOCOL,
IRMP_RCMM24_PROTOCOL,
IRMP_RCMM12_PROTOCOL,
#endif
#if IRMP_SUPPORT_LGAIR_PROTOCOL==1
IRMP_LGAIR_PROTOCOL,
#endif
#if IRMP_SUPPORT_SAMSUNG48_PROTOCOL==1
IRMP_SAMSUNG48_PROTOCOL,
#endif
#if IRMP_SUPPORT_MERLIN_PROTOCOL==1
IRMP_MERLIN_PROTOCOL,
#endif
#if IRMP_SUPPORT_PENTAX_PROTOCOL==1
IRMP_PENTAX_PROTOCOL,
#endif
#if IRMP_SUPPORT_S100_PROTOCOL==1
IRMP_S100_PROTOCOL,
#endif
#if IRMP_SUPPORT_ACP24_PROTOCOL==1
IRMP_ACP24_PROTOCOL,
#endif
#if IRMP_SUPPORT_TECHNICS_PROTOCOL==1
IRMP_TECHNICS_PROTOCOL,
#endif
#if IRMP_SUPPORT_PANASONIC_PROTOCOL==1
IRMP_PANASONIC_PROTOCOL,
#endif
#if IRMP_SUPPORT_MITSU_HEAVY_PROTOCOL==1
IRMP_MITSU_HEAVY,
#endif
#if IRMP_SUPPORT_TECHNICS_PROTOCOL==1
IRMP_TECHNICS_PROTOCOL,
#endif
#if IRMP_SUPPORT_VINCENT_PROTOCOL==1
IRMP_VINCENT_PROTOCOL,
#endif
#if IRMP_SUPPORT_SAMSUNGAH_PROTOCOL==1
IRMP_SAMSUNGAH_PROTOCOL,
#endif
#if IRMP_SUPPORT_RADIO1_PROTOCOL==1
IRMP_RADIO1_PROTOCOL,
#endif
0
};

__IO uint8_t PrevXferComplete = 1;
uint32_t AlarmValue = 0xFFFFFFFF;
volatile unsigned int systicks = 0;
volatile unsigned int sof_timeout = 0;
volatile unsigned int i = 0;
uint8_t Reboot = 0;
volatile uint32_t boot_flag __attribute__((__section__(".noinit")));
volatile int send_ir_on_delay = -1;

void delay_ms(unsigned int msec)
{
	systicks = 0;
	while (systicks <= msec);
}

void LED_Switch_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
#ifdef BlueDeveloperBoard
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
#endif
#if (defined(ST_Link) || defined(BlackDeveloperBoard))
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
	/* disable SWD, so pins are available */
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable, ENABLE);
#endif /* ST_Link */
#if defined(StickLink) || defined(GreenLink)
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
#endif /* StickLink */
	/* start with wakeup and reset switch off */
#ifdef SimpleCircuit
	GPIO_WriteBit(WAKEUP_PORT, WAKEUP_PIN, Bit_SET);
#else
	GPIO_WriteBit(WAKEUP_PORT, WAKEUP_PIN, Bit_RESET);
#endif /* SimpleCircuit */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
#ifndef ST_Link
	GPIO_InitStructure.GPIO_Pin = LED_PIN;
	GPIO_Init(LED_PORT, &GPIO_InitStructure);
#endif /* ST_Link */
	GPIO_InitStructure.GPIO_Pin = EXTLED_PIN;
	GPIO_Init(EXTLED_PORT, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = WAKEUP_PIN;
#ifdef SimpleCircuit
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
#endif /* SimpleCircuit */
	GPIO_Init(WAKEUP_PORT, &GPIO_InitStructure);
	/* start with LED off */
#ifdef ST_Link
	LED_deinit();
#else
	/* on the blue and black developer board and on the green ST-Link stick the LED lights, when pulled low */
#if !(defined(BlueDeveloperBoard) || defined(BlackDeveloperBoard) || defined(BlackDeveloperBoardTest) || defined(GreenLink))
	GPIO_WriteBit(LED_PORT, LED_PIN, Bit_RESET);
#else
	GPIO_WriteBit(LED_PORT, LED_PIN, Bit_SET);
#endif
#ifdef EXTLED_PORT
	GPIO_WriteBit(EXTLED_PORT, EXTLED_PIN, Bit_RESET);
#endif
#endif /* ST_Link */
}

void blink_LED(void)
{
#ifdef ST_Link
		red_on();
#ifdef EXTLED_PORT
		EXTLED_PORT->ODR ^= EXTLED_PIN;
#endif
		delay_ms(25);
		LED_deinit();
#ifdef EXTLED_PORT
		EXTLED_PORT->ODR ^= EXTLED_PIN;
#endif
#else
	LED_PORT->ODR ^= LED_PIN;
#ifdef EXTLED_PORT
	EXTLED_PORT->ODR ^= EXTLED_PIN;
#endif
	delay_ms(25);
	LED_PORT->ODR ^= LED_PIN;
#ifdef EXTLED_PORT
	EXTLED_PORT->ODR ^= EXTLED_PIN;
#endif
#endif /* ST_Link */
}

/* buf[0 ... 5] -> eeprom[virt_addr ... virt_addr + 2] */
/* buffer: 012345 -> arguments for Write: (10)(32)(54) -> eeprom: 01,23,45 */
void eeprom_store(uint8_t virt_addr, uint8_t *buf)
{
	EE_WriteVariable(virt_addr, (buf[1] << 8) | buf[0]);
	EE_WriteVariable(virt_addr + 1, (buf[3] << 8) | buf[2]);
	EE_WriteVariable(virt_addr + 2, (buf[5] << 8) | buf[4]);
}

/* eeprom[virt_addr ... virt_addr + 2] -> buf[0-5] */
/* eeprom: 01,23,45 -> Read results: (10)(32)(54) -> buffer: 012345 */
uint8_t eeprom_restore(uint8_t *buf, uint8_t virt_addr)
{
	uint8_t i, retVal = 0;
	for(i=0; i<3; i++) {
		if (EE_ReadVariable(virt_addr + i, (uint16_t *) &buf[2*i])) {
			/* the variable was not found or no valid page was found */
			*((uint16_t *) &buf[2*i]) = 0xFFFF;
			retVal = 1;
		}
	}
	return retVal;
}

/* which address has the received ir-code in eeprom? */
uint8_t get_num_of_irdata(IRMP_DATA *ir)
{
	uint8_t i, idx;
	uint8_t buf[SIZEOF_IR];
	for (i=0; i < NUM_KEYS; i++) {
		idx = SIZEOF_IR/2 * i;
		eeprom_restore(buf, idx);
		/* don't compare flags */
		if (!memcmp(buf, ir, sizeof(buf) - 1))
			return i;
	}
	return 0xFF;
}

/* put key into eeprom at address num */
void put_key(uint16_t key, uint8_t num)
{
	EE_WriteVariable(NUM_KEYS * SIZEOF_IR/2 + num, key);
}

/* get key at address num from eeprom */
uint16_t get_key(uint8_t num)
{
	uint16_t key;
	if (EE_ReadVariable(NUM_KEYS * SIZEOF_IR/2 + num, &key)) {
		/* the variable was not found or no valid page was found */
		key = 0xFFFF;
	}
	return key;
}

void store_wakeup(IRMP_DATA *ir)
{
	uint8_t idx;
	uint8_t tmp[SIZEOF_IR];
	uint8_t zeros[SIZEOF_IR] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
	idx = NUM_KEYS * (SIZEOF_IR/2 + 1);
	eeprom_restore(tmp, idx);
	if (!memcmp(tmp, zeros, SIZEOF_IR)) {
		/* store received wakeup IRData in first wakeup slot */
		eeprom_store(idx, (uint8_t *) ir);
		fast_toggle();
	}
}

void Systick_Init(void)
{
	/* 1ms */
	SysTick_Config((SysCtlClockGet()/1000));
}

void SysTick_Handler(void)
{
	systicks++;
	if (sof_timeout != SOF_TIMEOUT)
		sof_timeout++;
	if (i == 999) {
		if (AlarmValue)
			AlarmValue--;
		if (send_ir_on_delay > 0)
			send_ir_on_delay--;
		i = 0;
	} else {
		i++;
	}
}

/* Reset the counter with every "StartOfFrame" event,
 * sent by active host at fullspeed every 1ms */
void SOF_Callback(void)
{
	sof_timeout = 0;
}

uint8_t host_running(void)
{
	return (sof_timeout != SOF_TIMEOUT);
}

void Wakeup(void)
{
	AlarmValue = 0xFFFFFFFF;
	if(host_running())
		return;
	/* USB wakeup */
	Resume(RESUME_START);
	/* motherboard power switch: WAKEUP_PIN short high (resp. low in case of SimpleCircuit) */
#ifdef SimpleCircuit
	GPIO_WriteBit(WAKEUP_PORT, WAKEUP_PIN, Bit_RESET);
#else
	GPIO_WriteBit(WAKEUP_PORT, WAKEUP_PIN, Bit_SET);
#endif /* SimpleCircuit */
	delay_ms(500);
#ifdef SimpleCircuit
	GPIO_WriteBit(WAKEUP_PORT, WAKEUP_PIN, Bit_SET);
#else
	GPIO_WriteBit(WAKEUP_PORT, WAKEUP_PIN, Bit_RESET);
#endif /* SimpleCircuit */
	fast_toggle();
	/* let software know, PC was powered on by firmware, TODO make configurable & use Eeprom */
	send_ir_on_delay = 90;
}

uint8_t store_new_irdata(uint8_t num)
{
	uint8_t loop, ret = 0;
	IRMP_DATA new_IRData;
	uint8_t tmp[SIZEOF_IR];
	irmp_get_data(&new_IRData); // flush input of irmp data
	//blink_LED();
	/* 5 seconds to press button on remote */
	for(loop=0; loop < 50; loop++) {
		delay_ms(100);
		if (irmp_get_data(&new_IRData)) {
			new_IRData.flags = 0;
			/* store received IRData at address num */
			eeprom_store(num, (uint8_t *) &new_IRData);
			/* validate stored value in eeprom */
			eeprom_restore(tmp, num);
			if (memcmp(&new_IRData, tmp, sizeof(tmp)))
				ret = 1;
			blink_LED();
			return ret;
		}
	}
	return ret;
}

int8_t get_handler(uint8_t *buf)
{
	/* number of valid bytes in buf, -1 signifies error */
	int8_t ret = 3;
	uint8_t idx;
	switch ((enum command) buf[2]) {
	case CMD_CAPS:
		/* in first query we give information about slots and depth */
		if (!buf[3]) {
			buf[3] = NUM_KEYS;
			buf[4] = 0; //unused TODO
			buf[5] = WAKE_SLOTS;
			ret += 3;
			break;
		}
		/* in later queries we give information about supported protocols and firmware */
		idx = BYTES_PER_QUERY * (buf[3] - 1);
		if (idx < sizeof(supported_protocols)) {
			strncpy((char *) &buf[3], &supported_protocols[idx], BYTES_PER_QUERY);
			/* actually this is not true for the last transmission,
			 * but it doesn't matter since it's NULL terminated
			 */
			ret = HID_IN_BUFFER_SIZE-1;
			break;
		}
		if (idx >= sizeof(firmware) + (sizeof(supported_protocols) / BYTES_PER_QUERY + 1) * BYTES_PER_QUERY)
			return -1;
		strncpy((char *) &buf[3], &firmware[idx - (sizeof(supported_protocols) / BYTES_PER_QUERY + 1) * BYTES_PER_QUERY], BYTES_PER_QUERY);
		ret = HID_IN_BUFFER_SIZE-1;
		break;
	case CMD_ALARM:
		/* AlarmValue -> buf[3-6] */
		memcpy(&buf[3], &AlarmValue, sizeof(AlarmValue));
		ret += sizeof(AlarmValue);
		break;
	case CMD_IRDATA:
		idx = SIZEOF_IR/2 * buf[3];
		eeprom_restore(&buf[3], idx);
		ret += SIZEOF_IR;
		break;
	case CMD_KEY:
		*((uint16_t*)&buf[3]) = get_key(buf[3]);
		ret += 2;
		break;
	case CMD_WAKE:
		idx = NUM_KEYS * (SIZEOF_IR/2 + 1) + SIZEOF_IR/2 * buf[3];
		eeprom_restore(&buf[3], idx);
		ret += SIZEOF_IR;
		break;
	default:
		ret = -1;
	}
	return ret;
}

int8_t set_handler(uint8_t *buf)
{
	/* number of valid bytes in buf, -1 signifies error */
	int8_t ret = 3;
	uint8_t idx;
	uint8_t tmp[SIZEOF_IR];
	switch ((enum command) buf[2]) {
	case CMD_ALARM:
		memcpy(&AlarmValue, &buf[3], sizeof(AlarmValue));
		break;
	case CMD_IRDATA:
		idx = SIZEOF_IR/2 * buf[3];
		eeprom_store(idx, &buf[4]);
		/* validate stored value in eeprom */
		eeprom_restore(tmp, idx);
		if (memcmp(&buf[4], tmp, sizeof(tmp)))
			ret = -1;
	case CMD_KEY:
		put_key((buf[5] << 8) | buf[4], buf[3]);
		/* validate stored value in eeprom */
		if(!(get_key(buf[3]) == ((buf[5] << 8) | buf[4])))
			ret = -1;
		break;
	case CMD_WAKE:
		idx = NUM_KEYS * (SIZEOF_IR/2 + 1) + SIZEOF_IR/2 * buf[3];
		eeprom_store(idx, &buf[4]);
		/* validate stored value in eeprom */
		eeprom_restore(tmp, idx);
		if (memcmp(&buf[4], tmp, sizeof(tmp)))
			ret = -1;
		break;
	case CMD_REBOOT:
		Reboot = 1;
		break;
	case CMD_IRDATA_REMOTE:
		idx = SIZEOF_IR/2 * buf[3];
		if(store_new_irdata(idx))
			ret = -1;
	case CMD_WAKE_REMOTE:
		idx = NUM_KEYS * (SIZEOF_IR/2 + 1) + SIZEOF_IR/2 * buf[3];
		if(store_new_irdata(idx))
			ret = -1;
		break;
	default:
		ret = -1;
	}
	return ret;
}

int8_t reset_handler(uint8_t *buf)
{
	/* number of valid bytes in buf, -1 signifies error */
	int8_t ret = 3;
	uint8_t idx;
	uint8_t zeros[SIZEOF_IR] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
	switch ((enum command) buf[2]) {
	case CMD_ALARM:
		AlarmValue = 0xFFFFFFFF;
		break;
	case CMD_IRDATA:
		idx = SIZEOF_IR/2 * buf[3];
		eeprom_store(idx, zeros);
	case CMD_KEY:
		put_key(0xFFFF, buf[3]);
		break;
	case CMD_WAKE:
		idx = NUM_KEYS * (SIZEOF_IR/2 + 1) + SIZEOF_IR/2 * buf[3];
		eeprom_store(idx, zeros);
		break;
	default:
		ret = -1;
	}
	return ret;
}

/* is received ir-code in one of the wakeup-slots except last one? wakeup if true */
void check_wakeups(IRMP_DATA *ir)
{
	if(host_running())
		return;
	uint8_t i, idx;
	uint8_t buf[SIZEOF_IR];
	for (i=0; i < WAKE_SLOTS - 1; i++) {
		idx = NUM_KEYS * (SIZEOF_IR/2 + 1) + i * SIZEOF_IR/2;
		if (!eeprom_restore(buf, idx)) {
			if (!memcmp(buf, ir, sizeof(buf)))
				Wakeup();
		}
	}
}

void reboot(void)
{
	boot_flag = 0x12094444;// let bootloader know reset is from here
	fast_toggle();
	NVIC_SystemReset();
}

/* is received ir-code in the last wakeup-slot? reboot µC if true */
void check_reboot(IRMP_DATA *ir)
{
	uint8_t idx;
	uint8_t buf[SIZEOF_IR];
	idx = NUM_KEYS * (SIZEOF_IR/2 + 1) + (WAKE_SLOTS - 1) * SIZEOF_IR/2;
	if (!eeprom_restore(buf, idx)) {
		if (!memcmp(buf, ir, sizeof(buf)))
			reboot();
	}
}


void USB_DISC_release(void)
{
#if defined(Bootloader) && defined(PullDown) || defined(Maple)
	/* bootloader must activate disconnect, here we release the disconnect */
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(USB_DISC_RCC_APB2Periph, ENABLE);
	GPIO_InitStructure.GPIO_Pin = USB_DISC_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(USB_DISC_PORT, &GPIO_InitStructure);
#if defined(Maple)
	GPIO_WriteBit(USB_DISC_PORT, USB_DISC_PIN, Bit_RESET);
#else
	GPIO_WriteBit(USB_DISC_PORT, USB_DISC_PIN, Bit_SET);
#endif
#endif
}

void USB_Reset(void)
{
#if defined(Bootloader) && !defined(PullDown) && !defined(Maple)
	/* disable USB */
	PowerOff();
	/* USB reset by pulling USBDP shortly low. A pullup resistor is needed, most
	 * boards have it. */
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_WriteBit(GPIOA, GPIO_Pin_12, Bit_RESET);
	delay_ms(15);
#endif
}

void led_callback (uint_fast8_t on)
{
#ifdef ST_Link
	if (on) {
			red_on();
	} else {
			LED_deinit();
	}
#else
	LED_PORT->ODR ^= LED_PIN;
#endif /* ST_Link */
#ifdef EXTLED_PORT
	EXTLED_PORT->ODR ^= EXTLED_PIN;
#endif
}

void send_magic(void)
{
	uint8_t magic[SIZEOF_IR] = {0xFF, 0x00, 0x00, 0x00, 0x00, 0x00};
	USB_HID_SendData(REPORT_ID_IR, magic, SIZEOF_IR);
	send_ir_on_delay = -1;
}

int main(void)
{
	uint8_t buf[HID_OUT_BUFFER_SIZE-1];
	uint8_t kbd_buf[3] = {0};
	IRMP_DATA myIRData;
	int8_t ret;

	LED_Switch_init();
	Systick_Init();
	USB_Reset();
	USB_HID_Init();
	USB_DISC_release();
	IRMP_Init();
	FLASH_Unlock();
	EE_Init();
	irmp_set_callback_ptr (led_callback);

	while (1) {
		if (!AlarmValue)
			Wakeup();

		if (!send_ir_on_delay)
			send_magic();

		/* test if USB is connected to PC, sendtransfer is complete and configuration command is received */
		if (USB_HID_GetStatus() == CONFIGURED && PrevXferComplete && USB_HID_ReceiveData(buf) == RX_READY && buf[0] == STAT_CMD) {

			switch ((enum access) buf[1]) {
			case ACC_GET:
				ret = get_handler(buf);
				break;
			case ACC_SET:
				ret = set_handler(buf);
				break;
			case ACC_RESET:
				ret = reset_handler(buf);
				break;
			default:
				ret = -1;
			}

			if (ret == -1) {
				buf[0] = STAT_FAILURE;
				ret = 3;
			} else {
				buf[0] = STAT_SUCCESS;
			}

			/* send configuration data */
			USB_HID_SendData(REPORT_ID_CONFIG, buf, ret);
			blink_LED();
			if(Reboot)
				reboot();
		}

		/* poll IR-data */
		if (irmp_get_data(&myIRData)) {
			myIRData.flags = myIRData.flags & IRMP_FLAG_REPETITION;
			if (!(myIRData.flags)) {
				store_wakeup(&myIRData);
				check_wakeups(&myIRData);
				check_reboot(&myIRData);
			}

			/* send IR-data */
			kbd_buf[0] = get_key(get_num_of_irdata(&myIRData)) >> 8; // modifier
			kbd_buf[2] = get_key(get_num_of_irdata(&myIRData)) & 0xFF; // key
			USB_HID_SendData(REPORT_ID_IR, kbd_buf, sizeof(kbd_buf));
			delay_ms(30);
			kbd_buf[0] = 0;
			kbd_buf[2] = 0;
			USB_HID_SendData(REPORT_ID_IR, kbd_buf, sizeof(kbd_buf)); // release TODO implement better repeat
		}
	}
}
