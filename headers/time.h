#ifndef time_GUARD
#define time_GUARD
#ifdef __USE_CMSIS
#include "LPC13xx.h"
#endif

/* Timeings for transmissions */
#define RFID_READ_TIMEOUT 200
#define XBEE_RESPONCE_TIMEOUT 1000
#define XBEE_INCOMPLETE_TIMEOUT 300

/* Led Timings */
#define ACCESS_ON_TIME 200
#define ACCESS_OFF_TIME 100
#define ACCESS_COUNT 10

/* error LEDS */
#define ERROR_NOTIFY_TIME 2000
#define ERROR_NOTIFY_ERROR_GAP 1000
#define ERROR_DISPLAY_TIME 2000
#define ERROR_END_GAP 1000
#define ERROR_END_OF_ERROR 500

/* Times for Inits */
#define XBEE_GUARD_TIMES 1000
/* Normal response time <15ms so this is easily enough time */
#define XBEE_COMMAND_TIMEOUT 50
#define END_OF_INIT_LED_ON_TIME 1000

/* This is not an accurate delay and hence is not in ms */
#define CLOCK_INIT_PAUSE_LENGTH 10000
#define EXTERNAL_COMPONENTS_BOOT_WAIT 500



/* functions */
void setTimeOut(uint32_t ms);

uint8_t timedOut(void);

void delayms(uint32_t ms);
#endif
