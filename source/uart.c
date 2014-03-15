#include "uart.h"
#include "errors.h"

void UART_IRQHandler(void)
{
	uint8_t IIRValue, LSRValue;
	uint8_t Dummy;

	IIRValue = LPC_UART->IIR;

	IIRValue >>= 1;			/* skip pending bit in IIR */
	IIRValue &= 0x07;			/* check bit 1~3, interrupt identification */
	if (IIRValue == IIR_RLS)	/* Receive Line Status */
	{
		LSRValue = LPC_UART->LSR;
		/* Receive Line Status */
		if (LSRValue & (LSR_OE | LSR_PE | LSR_FE | LSR_RXFE | LSR_BI))
		{
			/* There are errors or break interrupt */
			/* Read LSR will clear the interrupt */
			UARTStatus = LSRValue;
			Dummy = LPC_UART->RBR;	/* Dummy read on RX to clear
									   interrupt, then bail out */
			return;
		}
		if (LSRValue & LSR_RDR)	/* Receive Data Ready */
		{
			if (uart_max_read){
				*uart_dest_buffer++ = LPC_UART->RBR;
				uart_max_read--;
			} else {
				UARTStatus = E_UART_OVERFLOW;
				UARTStatus = LSRValue;
				Dummy = LPC_UART->RBR;	/* Dummy read on RX to clear
										   interrupt, then bail out */
				return;
			}
		}
	}
	else if (IIRValue == IIR_RDA)	/* Receive Data Available */
	{
		if (uart_max_read){
			*uart_dest_buffer++ = LPC_UART->RBR;
			uart_max_read--;
		} else {
			UARTStatus = E_UART_OVERFLOW;
			UARTStatus = LSRValue;
			Dummy = LPC_UART->RBR;	/* Dummy read on RX to clear
									   interrupt, then bail out */
			return;
		}
	}
	else if (IIRValue == IIR_CTI)	/* Character timeout indicator */
	{
		/* Character Time-out indicator */
		UARTStatus |= 0x100;		/* Bit 9 as the CTI error */
	}
	else if (IIRValue == IIR_THRE)	/* THRE, transmit holding register empty */
	{
		/* THRE interrupt */
		LSRValue = LPC_UART->LSR;		/* Check status in the LSR to see if
										   valid data in U0THR or not */
		if (LSRValue & LSR_THRE)
		{
			UARTTxEmpty = 1;
		}
		else
		{
			UARTTxEmpty = 0;
		}
	}
	return;
}

void UARTSend(uint8_t *BufferPtr, uint32_t Length){

  while ( Length != 0 ) {
	  /* THRE status, contain valid data */
	  while ( !(LPC_UART->LSR & LSR_THRE) );
	  LPC_UART->THR = *BufferPtr;
      BufferPtr++;
      Length--;
  }
  return;
}

