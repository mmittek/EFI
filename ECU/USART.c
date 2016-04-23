#include <avr/io.h>

#include "hack.h"

#include "USART.h"

const unsigned int USART_MYUBRR =USART_FOSC/16/USART_BAUD-1;

void USART_Init( )
{
	/*Set baud rate */
	UBRR0H = (unsigned char)(USART_MYUBRR>>8);
	UBRR0L = (unsigned char)USART_MYUBRR;
	//Enable receiver and transmitter */
	UCSR0B = (1<<RXEN0)|(1<<TXEN0);
	/* Set frame format: 8data, 2stop bit */
	UCSR0C = (3<<UCSZ00);
}




void USART_Transmit( unsigned char data )
{
	/* Wait for empty transmit buffer */
	while ( !( UCSR0A & (1<<UDRE0)) )
	;
	/* Put data into buffer, sends the data */
	UDR0 = data;
}

void USART_Print(char *s) {
	uint16_t i=0;
	while(s[i] != 0) {
		USART_Transmit(s[i]);
		i++;
	}
}
