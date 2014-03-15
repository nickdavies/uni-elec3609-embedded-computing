#ifndef rfid_GUARD
#define rfid_GUARD

#define RFID_STX 0x02
#define RFID_ETX 0x03

#define RFID_ASCII_DATA_LENGTH 10
#define RFID_FULL_READ_LENGTH 16
/* should be 1/2 of the value above */
#define RFID_HEX_DATA_LENGTH 5

struct RFID_DATA_READ {
	/* Start Symbol */
	uint8_t STX;
		
	/* Data ASCII */
	uint8_t data[RFID_ASCII_DATA_LENGTH];
	/* Checksum */
	uint8_t checksum[2];

	/* CRAP */
    uint8_t CR;
    uint8_t LF;
	/* Stop Symbol */
	uint8_t ETX;
};

struct RFID_DATA {
	/* Data HEX */
	uint8_t data[RFID_HEX_DATA_LENGTH];
	/* Checksum */
	uint8_t checksum;
};

uint8_t read_rfid(struct RFID_DATA* rfid_data);
uint8_t verify_rfid_checksum(struct RFID_DATA in);

#endif
