#ifndef uart_GUARD
#define uart_GUARD

#ifdef __USE_CMSIS
#include "LPC13xx.h"
#endif

#include "time.h"

/* UART SETTINGS */
#define UART_BAUDRATE 9600
#define UART_MODEM_ENABLE 0

/* INTERRUPT MODES */
#define UART_TX_INTERRUPT 1
#define UART_RX_INTERRUPT 1

/* INTERRUPT ENABLE VALUES */
#define IER_RBR		0x01
#define IER_THRE	0x02
#define IER_RLS		0x04

/* INTERRUPT TYPE VALUES */
#define IIR_PEND	0x01
#define IIR_RLS		0x03
#define IIR_RDA		0x02
#define IIR_CTI		0x06
#define IIR_THRE	0x01

/* PIN INFORMATION */
#define LSR_RDR		0x01
#define LSR_OE		0x02
#define LSR_PE		0x04
#define LSR_FE		0x08
#define LSR_BI		0x10
#define LSR_THRE	0x20
#define LSR_TEMT	0x40
#define LSR_RXFE	0x80

uint8_t * uart_dest_buffer;
uint32_t uart_max_read;
uint32_t UARTStatus;
uint8_t  UARTTxEmpty;

/* UART FUNCTIONS */

void UART_IRQHandler(void);
void UARTSend(uint8_t *BufferPtr, uint32_t Length);

#endif
