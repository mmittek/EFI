#ifndef _HAVE_USART_H_
#define _HAVE_USART_H_

#define USART_FOSC 16000000 // Clock Speed
#define USART_BAUD 57600

void USART_Init();
void USART_Transmit( unsigned char data );
void USART_Print(char *s);


#endif // _HAVE_USART_H_
