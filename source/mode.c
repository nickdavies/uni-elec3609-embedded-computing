
#include "mode.h"
#include "IO.h"

uint8_t get_mode(){
	return current_mode;
}

void set_mode(uint8_t mode){

	MODE_IO_PORT &= (0 << MODE_IO_PIN);
	MODE_IO_PORT |= (mode << MODE_IO_PIN);
	current_mode = mode;

}

