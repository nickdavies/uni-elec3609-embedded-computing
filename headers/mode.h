#ifndef mode_GUARD
#define mode_GUARD

#ifdef __USE_CMSIS
#include "LPC13xx.h"
#endif

/* Modes */
#define MODE_RFID	0
#define MODE_XBEE   1

/* mode pins */
#define MODE_IO_PORT LPC_GPIO0->DATA
#define MODE_IO_PIN	7

uint8_t get_mode();
void set_mode(uint8_t mode);

uint8_t current_mode;

#endif
