#include "errors.h"
#include "init.h"
#include "uart.h"
#include "mode.h"
#include "time.h"
#include "xbee.h"

#include <stdlib.h>

/* private functions */

/* setup IOCONTRL and pin directions etc. */
static uint8_t IOInit(void);

/* setup basic uart functions */
static uint8_t UARTInit(void);

/* setup XBee module */
static uint8_t XBeeInit(void);


uint8_t sys_init(){

	uint32_t initc;
	uint32_t i = 0;
	uint32_t j = 0;
	uint8_t status;

	status = IOInit();
	if (status != E_OK){ return status; }

	initc = LPC_TMR32B1->TC;

	status = UARTInit();
	if (status != E_OK){ return status; }

	/* inaccurate pause to give clock time to start*/
	for(i = 0; i != CLOCK_INIT_PAUSE_LENGTH; i++){
		j++;
	}

	/* give other components time to power up and become ready for data*/
	delayms(EXTERNAL_COMPONENTS_BOOT_WAIT);

	status = XBeeInit();
	if (status != E_OK){ return status; }


	LPC_GPIO2->DATA |= 0x70;
	delayms(END_OF_INIT_LED_ON_TIME);
	LPC_GPIO2->DATA &= ~(0x70);
	
	srand(LPC_TMR32B1->TC - initc);

	return E_OK;
}

static uint8_t IOInit(){

	/* Clock Init */
	// ACTUAL Frequency = 72000000hz = 72mhz
	LPC_SYSCON->SYSAHBCLKCTRL |= (1<<10);
	LPC_IOCON->PIO1_10 &= ~0x07;
	LPC_IOCON->PIO1_10 |= 0x02;

	/* MODE
	 * grant
	 * deny
	 * DIRECTION
	 */
	LPC_GPIO0->DIR |= (1 << MODE_IO_PIN);
	LPC_GPIO2->DIR |= 0x70;//(1 << LED_OK_PIN) & (1 << LED_ERROR_PIN) & (1 << LED_DENY_PIN);
	LPC_GPIO0->DATA = 0;
	LPC_GPIO2->DATA = 0;

	return E_OK;
}

static uint8_t UARTInit(void) {
	uint32_t Fdiv;
	uint32_t regVal;

	NVIC_DisableIRQ(UART_IRQn);

	LPC_IOCON->PIO1_6 &= ~0x07;    /*  UART I/O config */
	LPC_IOCON->PIO1_6 |= 0x01;     /* UART RXD */
	LPC_IOCON->PIO1_7 &= ~0x07;
	LPC_IOCON->PIO1_7 |= 0x01;     /* UART TXD */
	/* Enable UART clock */
	LPC_SYSCON->SYSAHBCLKCTRL |= (1<<12);
	LPC_SYSCON->UARTCLKDIV = 0x1;     /* divided by 1 */

	LPC_UART->LCR = 0x83;             /* 8 bits, no Parity, 1 Stop bit */
	regVal = LPC_SYSCON->UARTCLKDIV;
	Fdiv = (((SystemCoreClock*LPC_SYSCON->SYSAHBCLKDIV)/regVal)/16)/UART_BAUDRATE ;	/*baud rate */

	LPC_UART->DLM = Fdiv / 256;
	LPC_UART->DLL = Fdiv % 256;
	LPC_UART->LCR = 0x03;		/* DLAB = 0 */
	LPC_UART->FCR = 0x07;		/* Enable and reset TX and RX FIFO. */

#if UART_MODEM_ENABLE
	LPC_UART->MCR = (1 << 4);
#endif

	/* Read to clear the line status. */
	regVal = LPC_UART->LSR;

	/* Ensure a clean start, no data in either TX or RX FIFO. */
	// CodeRed - added parentheses around comparison in operand of &
	while (( LPC_UART->LSR & (LSR_THRE|LSR_TEMT)) != (LSR_THRE|LSR_TEMT) );
	while ( LPC_UART->LSR & LSR_RDR )
	{
		regVal = LPC_UART->RBR;	/* Dump data from RX FIFO */
	}

	/* Enable the UART Interrupt */
	NVIC_EnableIRQ(UART_IRQn);

	LPC_UART->IER = IER_RBR | IER_THRE | IER_RLS;	/* Enable UART interrupt */

	uart_max_read = 0;

	return E_OK;
}

/* setup XBee module */
static uint8_t XBeeInit(){
	uint8_t status = E_ACCESS_DENIED;

	set_mode(MODE_XBEE);

	status = enter_at_command_mode();
	if (status != E_OK){ return status; }

	status = send_at_command((uint8_t*)"ATID4214\r");
	if (status != E_OK){ return status; }

	status = send_at_command((uint8_t*)"ATMY01\r");
	if (status != E_OK){ return status; }

	status = send_at_command((uint8_t*)"ATDL10\r");
	if (status != E_OK){ return status; }

	status = send_at_command((uint8_t*)"ATCN\r");
	if (status != E_OK){ return status; }

	return E_OK;
}

