#ifndef xbee_GUARD
#define xbee_GUARD

#ifdef __USE_CMSIS
#include "LPC13xx.h"
#endif

#include "rfid.h"

/* packet details */
#define XB_DATAGRAM_SIZE 10

/* public functions */
uint8_t send_recieve_data(struct RFID_DATA * send);

uint8_t send_at_command(uint8_t* cmd);
uint8_t enter_at_command_mode(void);

#endif
