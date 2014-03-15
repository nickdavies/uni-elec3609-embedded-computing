#include <stdio.h>

#include "errors.h"
#include "init.h"
#include "mode.h"
#include "rfid.h"
#include "time.h"
#include "xbee.h"

static void grant_access(void);
static void deny_access(void);
static void show_error(uint8_t e);

int main(void){
	
	uint8_t status;
	struct RFID_DATA rfid_id;

	status = sys_init();

	if (status == E_OK){
		while(1){
			set_mode(MODE_RFID);
			status = read_rfid(&rfid_id);

			if (status == E_OK){
				set_mode(MODE_XBEE);

				status = send_recieve_data(&rfid_id);
				if (status == E_OK){
					grant_access();
				} else if (status == E_ACCESS_DENIED){
					deny_access();
				} else {
					/* error sending/recieving responce from server 
						NOT including access denied */
					show_error(status);
				}
			} else {
				/* error reading RFID */
				show_error(status);
			}
		}
	} else {
		while (1){
			show_error(status);
		}
	}
}

static void grant_access(void){
	uint8_t i;

	LPC_GPIO2->DATA = 0;
	for (i = 0; i != ACCESS_COUNT; i++){
		LPC_GPIO2->DATA |= (1 << LED_OK_PIN);
		delayms(ACCESS_ON_TIME);
		LPC_GPIO2->DATA &= ~(1 << LED_OK_PIN);
		delayms(ACCESS_OFF_TIME);
	}

}

static void deny_access(void){
	uint8_t i;

	LPC_GPIO2->DATA = 0;
	for (i = 0; i != ACCESS_COUNT; i++){
			LPC_GPIO2->DATA |= (1 << LED_DENY_PIN);
			delayms(ACCESS_ON_TIME);
			LPC_GPIO2->DATA &= ~(1 << LED_DENY_PIN);
			delayms(ACCESS_OFF_TIME);
	}

}

static void show_error(uint8_t e){
	LPC_GPIO2->DATA = 0;
	LPC_GPIO2->DATA |= (1 << LED_ERROR_PIN);
	delayms(ERROR_NOTIFY_TIME);

	LPC_GPIO2->DATA = 0;
	delayms(ERROR_NOTIFY_ERROR_GAP);

	if (0x4 & e){
		LPC_GPIO2->DATA |= (1 << LED_OK_PIN);
	}
	if (0x2 & e){
		LPC_GPIO2->DATA |= (1 << LED_ERROR_PIN);
	}
	if (0x1 & e){
		LPC_GPIO2->DATA |= (1 << LED_DENY_PIN);
	}
	delayms(ERROR_DISPLAY_TIME);
	LPC_GPIO2->DATA = 0;
	delayms(ERROR_END_GAP);
	LPC_GPIO2->DATA |= (1 << LED_OK_PIN);
	delayms(ERROR_END_OF_ERROR);
	LPC_GPIO2->DATA = 0;
}
