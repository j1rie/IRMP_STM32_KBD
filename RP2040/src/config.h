/*
 * Copyright (C) 2014-2023 Joerg Riechardt
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#ifndef __CONFIG_H
#define __CONFIG_H

/* NUM_KEYS * (SIZEOF_IR/2 + 1) + WAKE_SLOTS * SIZEOF_IR/2 <= 255
 * Eeprom page size = (512 - 1) 8bit variables,
 * Eeprom partition 1: NUM_KEYS IRMP_DATA
 * Eeprom partition 2: NUM_KEYS keys
 * Eeprom partition 3: WAKE_SLOTS IRMP_DATA
 * Eeprom partition 4: repeat_delay, repeat_period, repeat_timeout
 */
#define NUM_KEYS   61
#define WAKE_SLOTS	2 // at least 2: wakeup and reboot
#define SIZEOF_IR	6


/* TODO GPIO x Logging (irmp.c) */

#define WAKEUP_GPIO		27 /* GPIO 2  (Pin 32)  */
#define RESET_GPIO		3  /* GPIO 3  (Pin 5)  */
#define EXTLED_GPIO		4  /* GPIO 4  (Pin 6)  */
#define IR_IN_GPIO		28 /* GPIO 28 (Pin 34) */
#define IR_OUT_GPIO		6  /* GPIO 6  (Pin 9)  */
#define WAKEUP_RESET_GPIO	7  /* GPIO 7  (Pin 10) */
#define LED_GPIO		8  /* GPIO 8  (Pin 11) */

#endif /* __CONFIG_H */
