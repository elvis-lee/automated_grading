#ifndef _UART_H
#define _UART_H


#include <stm32f4xx_usart.h>

#define QUEUE_SIZE 64
# define HIGH_WATER ( QUEUE_SIZE - 6)

//#define HWFLOWCTRL

uint8_t _getchar(void);
uint8_t _putchar(const uint8_t c);


int uart_open(uint8_t uart, uint32_t baud, uint32_t flags);
int uart_close(uint8_t uart);
uint16_t uart_read (uint8_t uart, uint8_t *buf, uint16_t nbyte);
uint16_t uart_write(uint8_t uart, const uint8_t *buf, uint16_t nbyte);

#endif





/*
// uart.h


#ifndef UART_H_
#define UART_H_

#include <stm32f10x.h>


int uart_open ( USART_TypeDef * USARTx , uint32_t baud , uint32_t flags);
int uart_close ( USART_TypeDef * USARTx );
int uart_putc (int c, USART_TypeDef * USARTx );
int uart_getc ( USART_TypeDef * USARTx );

int uart_puts (char* string  , USART_TypeDef * USARTx );
char* uart_gets (USART_TypeDef * USARTx );



#endif
*/
