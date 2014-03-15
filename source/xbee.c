#include "errors.h"
#include "time.h"
#include "xbee.h"
#include "crypto.h"
#include "rfid.h"
#include "uart.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* private functions */
static uint8_t send_data(struct RFID_DATA * send, uint32_t * random);
static uint8_t recieve_data(uint32_t * random);

static uint8_t encode_random(uint8_t * full_datagram, struct RFID_DATA * datastream, uint8_t * random);
static uint8_t decode_random(uint8_t * full_datagram, struct RFID_DATA * data_out, uint8_t * random);


uint8_t send_recieve_data(struct RFID_DATA * send){
	
	uint8_t status;
	uint32_t random;

	random = rand();

	status = send_data(send, &random);
	if (status != E_OK){ return status; }

	return recieve_data(&random);

}

uint8_t send_at_command(uint8_t* cmd){
	volatile uint32_t i, responce = 0;

	uart_dest_buffer = (uint8_t *)&responce;
	uart_max_read = 3;

	UARTSend(cmd, strlen( (char*)cmd));
	setTimeOut(XBEE_COMMAND_TIMEOUT);
	while( !timedOut() && uart_max_read ){ i++; }
	if ( timedOut() ){
			return E_XBEE_INIT_FAILURE;
	}

	if (responce != 0x0d4b4f){
			return E_XBEE_INIT_FAILURE;
	}

	return E_OK;
}

uint8_t enter_at_command_mode(void){

	volatile uint32_t i, responce = 0;

	uart_dest_buffer = (uint8_t *)&responce;
	uart_max_read = 3;

	delayms(XBEE_GUARD_TIMES);

	UARTSend((uint8_t*)"+++", 3);

	delayms(XBEE_GUARD_TIMES);

	setTimeOut(XBEE_COMMAND_TIMEOUT);

	while( !timedOut() && uart_max_read ){ i++; }
	if ( timedOut() ){
			return E_XBEE_INIT_FAILURE;
	}

	if (responce != 0x0d4b4f){
		return E_XBEE_INIT_FAILURE;
	}

	return E_OK;

}

static uint8_t send_data(struct RFID_DATA * send, uint32_t * random){
	uint8_t status;
	uint8_t full_datagram[XB_DATAGRAM_SIZE];

	status = encode_random(full_datagram, send, (uint8_t*)random);

	if (status != E_OK){ return status; }

#if CRYPTO_ENABLE_ENCRYPTION
	status = encrypt(CRYPTO_PASSWORD, full_datagram, XB_DATAGRAM_SIZE);
	if (status != E_OK){ return status; }
#endif

	UARTSend(full_datagram, XB_DATAGRAM_SIZE);

	return E_OK;

}

static uint8_t recieve_data(uint32_t * random){

	/* data data input */
	uint8_t full_datagram[XB_DATAGRAM_SIZE];
	struct RFID_DATA read;
	uint8_t status;

	/* Busy waiting counters */
	volatile uint32_t i, full_length;

	/* We are waiting for XB_DATAGRAM_SIZE Bytes input */
	uart_dest_buffer = full_datagram;
	uart_max_read = XB_DATAGRAM_SIZE;

	/* wait for start of transmission */
	full_length = XB_DATAGRAM_SIZE;
	setTimeOut(XBEE_RESPONCE_TIMEOUT);

	/* wait for any data */
	while( !timedOut() && uart_max_read == full_length){ i++; }
	/* If transmission didnt start in time, timeout */
	if ( timedOut() ){
			return E_XBEE_RESPONCE_TIMEOUT;
	}

	/* Timeout if all data isnt read */
	setTimeOut(XBEE_INCOMPLETE_TIMEOUT);
	while( !timedOut() && uart_max_read ){ i++; }
	/* If we did not receive as much data as we wanted */
	if ( timedOut() ){
		return E_XBEE_DATA_TIMEOUT;
	}

#if CRYPTO_ENABLE_ENCRYPTION
	status = decrypt(CRYPTO_PASSWORD, full_datagram, XB_DATAGRAM_SIZE);
	if (status != E_OK){ return status; }
#endif

	decode_random(full_datagram, &read, (uint8_t*)&i);
	if (i != *random){
		return E_RANDOM_MISSMATCH;
	}

	status = verify_rfid_checksum(read);
	if (status != E_OK){ return status; }

	return (read.data[0] * 8 +
			read.data[1] * 4 +
			read.data[2] * 2 +
			read.data[3]
			);
}

/*
 * r_i = Random data
 * d_i = data
 * c_i = checksum
 *
 * SIZE:  1  1  1  1  1  1  1  1  1  1 = 10
 * DATA: r0 d0 d1 d2 d3 d4 r1 c1 r2 r3
 *
 */
static uint8_t encode_random(uint8_t * full_datagram, struct RFID_DATA * datastream, uint8_t * random){

	uint8_t * data = datastream->data;

	*(full_datagram++) = *(random++);
	*(full_datagram++) = *(data++);
	*(full_datagram++) = *(data++);
	*(full_datagram++) = *(data++);
	*(full_datagram++) = *(data++);
	*(full_datagram++) = *(data++);
	*(full_datagram++) = *(random++);
	*(full_datagram++) = datastream->checksum;
	*(full_datagram++) = *(random++);
	*(full_datagram++) = *(random++);

	return E_OK;
}

static uint8_t decode_random(uint8_t * full_datagram, struct RFID_DATA * data_out, uint8_t * random){

	uint8_t * data = data_out->data;

	*(random++) = *(full_datagram++);
	*(data++) = *(full_datagram++);
	*(data++) = *(full_datagram++);
	*(data++) = *(full_datagram++);
	*(data++) = *(full_datagram++);
	*(data++) = *(full_datagram++);
	*(random++) = *(full_datagram++);
	data_out->checksum = *(full_datagram++);
	*(random++) = *(full_datagram++);
	*(random++) = *(full_datagram++);

	return E_OK;
}
