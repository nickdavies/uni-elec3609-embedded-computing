#include "errors.h"
#include "crypto.h"

#include <stdio.h>

/*
	Encryption algorithm will be as follows:
		pw[] as password bytes
		P[] as plain text bytes
		C[] as ciper bytes
	PW is 1 element longer than P and C will be as long as P

	C[0] = (P[0] ^ PW[0]) ^ PW[1]

	for ( i = 1 to i = len(P) )
		C[i] = (P[i] ^ P[i-1]) ^ PW[i+1]
	Done

	This algorithm is clearly not safe for real use as it
	will leak the key if you have one known P -> C pair.
*/

uint8_t encrypt(uint8_t * pw, uint8_t * data, uint32_t len){
	uint32_t i;

	/* C[0] = (P[0] XOR PW[0]) XOR PW[1] */
	*data = ( (*data) ^ (*pw)) ^ *(pw + 1);
	pw++;

	for( i = 1; i != len; i++){
		data ++;pw ++;
		*data = ( (*data) ^ *(data-1) ) ^ *pw;
	}
	
	return E_OK;

}

uint8_t decrypt(uint8_t * pw, uint8_t * data, uint32_t len){

	uint32_t i;
	uint8_t prev_crypt;
	uint8_t tmp;

	prev_crypt = *data;
	*data = ( *data ^ *(pw + 1)) ^ *(pw);

	pw++;

	for( i = 1; i != len; i++){
    	data++;	pw++;
		tmp = *data;
		*data = (*data ^ *pw) ^ prev_crypt;
		prev_crypt = tmp;

	}
	return E_OK;
}



