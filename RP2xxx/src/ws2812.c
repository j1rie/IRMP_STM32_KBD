/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "ws2812.pio.h"
#include "config.h"

void ws2812_init() {
    uint offset0 = pio_add_program(pio0, &ws2812_program);
    uint offset1 = pio_add_program(pio1, &ws2812x_program);
    ws2812_program_init(pio0, 0, offset0, WS2812_PIN, 800000, IS_RGBW);
    ws2812x_program_init(pio1, 0, offset1, WS2812X_PIN, 800000, IS_RGBW);
}
