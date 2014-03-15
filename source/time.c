
#include "time.h"

void setTimeOut(uint32_t ms){

	LPC_TMR32B1->TCR = 0x2;
	LPC_TMR32B1->MCR = (1 << 2);
	LPC_TMR32B1->MR0 = ms * ((SystemCoreClock/LPC_SYSCON->SYSAHBCLKDIV) / 1000);
	LPC_TMR32B1->TCR = 0x1;

}

uint8_t timedOut(void){
	return !(LPC_TMR32B1->TCR & 0x1);
}

void delayms(uint32_t ms){
	setTimeOut(ms);

	volatile uint32_t i = 0;
	while(!timedOut()){
		i++;
	}
}
