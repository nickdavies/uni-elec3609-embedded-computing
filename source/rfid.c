#include "errors.h"
#include "rfid.h"
#include "uart.h"
#include "time.h"

/* Private Functions */
static uint8_t read_raw_rfid(struct RFID_DATA_READ* target);
static uint8_t condence_rfid_data(struct RFID_DATA_READ* in, struct RFID_DATA * out);
static uint8_t ASCII_to_HEX(uint8_t ascii, uint8_t* hex);

/* Public Functions */
uint8_t read_rfid(struct RFID_DATA* rfid_data){
	struct RFID_DATA_READ input_data;	
	uint8_t status;

	status = read_raw_rfid(&input_data);
	if( status != E_OK){ return status; }

	status = condence_rfid_data(&input_data, rfid_data);
	if( status != E_OK){ return status; }

	status = verify_rfid_checksum(*rfid_data);
	if( status != E_OK){ return status; }

	return E_OK;
}

/* Private Functions */
static uint8_t read_raw_rfid(struct RFID_DATA_READ* target){

	uint8_t data[16];

	volatile uint32_t i, full_length;

	uart_dest_buffer = data;
	uart_max_read = RFID_FULL_READ_LENGTH;

	/* wait for start of transmission */
	full_length = RFID_FULL_READ_LENGTH;
	while( uart_max_read == full_length){ i++; }

	/* Timeout if all data isnt read */
	setTimeOut(RFID_READ_TIMEOUT);
	while( !timedOut() && uart_max_read ){ i++; }

	if ( timedOut() ){
		return E_RFID_TIMEOUT;
	}

	target->STX = data[0];
	for (i = 0; i != RFID_ASCII_DATA_LENGTH; i++){
		target->data[i] = data[i+1];
	}
	target->checksum[0] = data[11];
	target->checksum[1] = data[12];

	target->CR = data[13];
	target->LF = data[14];
	target->ETX = data[15];

	return E_OK;
}

static uint8_t condence_rfid_data(struct RFID_DATA_READ* in, struct RFID_DATA * out){
	
	uint8_t i;
	uint8_t tmp_L;
	uint8_t tmp_H;

	/* Verify start/stop symbols and \r\n characters
		as a form of error detection */
	if (in -> STX != RFID_STX ||
		in -> ETX != RFID_ETX ||
		in -> CR  != '\r' ||
		in -> LF  != '\n' ){

		return E_RFID_CORRUPT;
	}

	for(i = 0;i != RFID_ASCII_DATA_LENGTH; i += 2){
		if (ASCII_to_HEX(in->data[i], &tmp_H) != E_OK ||
			ASCII_to_HEX(in->data[i+1], &tmp_L) != E_OK){

			return E_RFID_CORRUPT;
		}
    	out -> data[i/2] = (tmp_H << 4) | tmp_L;
	}
	if (ASCII_to_HEX(in->checksum[0], &tmp_H) != E_OK ||
		ASCII_to_HEX(in->checksum[1], &tmp_L) != E_OK){

		return E_RFID_CORRUPT;
	}
	out -> checksum = (tmp_H << 4) | tmp_L;
	return E_OK;
}

uint8_t verify_rfid_checksum(struct RFID_DATA in){
	uint8_t i;
	uint8_t checksum = in.data[0];	

	for(i = 1;i != RFID_HEX_DATA_LENGTH; i++ ){
		checksum ^= in.data[i];
	}

	if( checksum == in.checksum ){
		return E_OK;
	}
	return E_RFID_CHECKSUM;	
}

static uint8_t ASCII_to_HEX(uint8_t ascii, uint8_t* hex){

	if (ascii <= '9' && ascii >= '0'){
    	*hex = ascii - '0';
		return E_OK;
	}

	if (ascii <= 'F' && ascii >= 'A'){
    	*hex = ascii - 'A' + 10;
		return E_OK;
	}

	return E_RFID_CORRUPT;
}

