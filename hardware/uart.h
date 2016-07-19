#ifndef _UART_H
#define _UART_H

#include <stm32f4xx_usart.h>


#define QUEUE_SIZE 600
#define HIGH_WATER ( QUEUE_SIZE - 6)

struct Queue {
  uint16_t pRD, pWR;
  uint8_t  q[QUEUE_SIZE];
};

struct Queue UART1_TXq, UART1_RXq;
int Dequeue(struct Queue *q, uint8_t *data, uint16_t len);
int pack_avail(struct Queue *q);
int Enqueue(struct Queue *q, const uint8_t *data, uint16_t len);
int QueueFull(struct Queue *q);
int QueueEmpty(struct Queue *q);
void InitQueue(struct Queue *q);
uint16_t uart_write(uint8_t uart, const uint8_t *buf, uint16_t nbyte);
int uart_open(uint8_t uart, uint32_t baud, uint32_t flags);

#endif

