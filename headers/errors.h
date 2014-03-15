#ifndef errors_GUARD
#define errors_GUARD

#include "LPC13xx.h"

#define E_OK 					0x8
#define E_ACCESS_DENIED 		0xa

/* rfid errors */
#define E_RFID_TIMEOUT 			0x0 // 000
#define E_RFID_CORRUPT 			0x1 // 001
#define E_RFID_CHECKSUM 		0x2 // 010

/* UART errors */
#define E_UART_OVERFLOW 		0x3 // 011

/* XBEE wireless errors */
#define E_XBEE_INIT_FAILURE		0x4 // 100
#define E_XBEE_DATA_TIMEOUT 	0x5 // 101
#define E_XBEE_RESPONCE_TIMEOUT 0x6 // 110
#define E_RANDOM_MISSMATCH 		0x7 // 111

/* LEDS */
#define LED_OK_PIN  6
#define LED_ERROR_PIN  5
#define LED_DENY_PIN  4


#endif
