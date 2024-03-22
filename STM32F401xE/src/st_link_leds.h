#ifndef __ST_LINK_LEDS_H
#define __ST_LINK_LEDS_H

#include <stdint.h>
extern volatile unsigned int systicks;
void delay_ms(unsigned int msec);
void LED_deinit(void);
void fast_toggle(void);
void both_on(void);
void red_on(void);
void yellow_short_on(void);
void statusled_write(uint8_t led_state);

#endif /* __ST_LINK_LEDS_H */
