/*
 * Copyright (C) 2014-2025 Joerg Riechardt
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#ifndef __CONFIG_H
#define __CONFIG_H

/* NUM_KEYS * (SIZEOF_IR + 2) + WAKE_SLOTS * SIZEOF_IR + 3 * 2 + 1 <= 512
 * MACRO_SLOTS x (MACRO_DEPTH + 1) * SIZEOF_IR <= 511
 * Eeprom page size = (1024 - 1) 8bit variables,
 * Eeprom partition 1: NUM_KEYS IRMP_DATA
 * Eeprom partition 2: NUM_KEYS keys
 * Eeprom partition 3: WAKE_SLOTS IRMP_DATA
 * Eeprom partition 4: repeat_delay, repeat_period, repeat_timeout, send_after_wakeup
 * Eeprom partition 5 starting at 512: MACRO_SLOTS x (MACRO_DEPTH + 1) IRMP_DATA
 */
#define NUM_KEYS   61
#define MACRO_SLOTS	8
#define MACRO_DEPTH	8
#define WAKE_SLOTS	2 // at least 2: wakeup and reboot
#define SIZEOF_IR	6


/* TODO GPIO x Logging (irmp.c) */

#define WAKEUP_GPIO		27 /* GPIO 27 (Pico(2) Pin 32, One+Zero Pin 27, Seeed XIAO RP2350 D1) */
#define EXTLED_GPIO		4  /* GPIO 4  (Pico(2) Pin 6,  One+Zero Pin 4,  Seeed XIAO RP2350 D9) */
#define IR_IN_GPIO		28 /* GPIO 28 (Pico(2) Pin 34, One+Zero Pin 28, Seeed XIAO RP2350 D2) */
#define IR_OUT_GPIO		6  /* GPIO 6  (Pico(2) Pin 9,  One+Zero Pin 6,  Seeed XIAO RP2350 D4) */
#define STATUSLED_GPIO		2  /* GPIO 2  (Pico(2) Pin 4,  One+Zero Pin 2,  Seeed XIAO RP2350 D8) */
#define IS_RGBW			false
#ifdef PICO_DEFAULT_WS2812_PIN
#define WS2812_PIN		PICO_DEFAULT_WS2812_PIN /* GPIO16 (One+Zero Onboard), GPIO22 (Seeed XIAO RP2350 Onboard) */
#else
// default to pin 16 if the board doesn't have a default WS2812 pin defined
#define WS2812_PIN		16 /* GPIO 16 (Pico(2) Pin 21) */
#endif
// pio pins need to be consecutive
#define WS2812_PIN_2		WS2812_PIN - 1  /* GPIO 15 (Pico(2) Pin 20, One+Zero Pin 15), GPIO 21 (Seeed XIAO RP2350 D11) */
#define NUM_PIXELS		64
#define APA106_PIN		0  /* GPIO 0  (Pico(2) Pin 1,  One+Zero Pin 0,  Seeed XIAO RP2350 D6 */

#endif /* __CONFIG_H */
