#include <stm32f4xx.h>
#include <stm32f4xx_rcc.h>
#include <stm32f4xx_gpio.h>
#include <stm32f4xx_usart.h>
#include <misc.h>
#include "uart.h"

int RxOverflow = 0;

// TxPrimed is used to signal that Tx send buffer needs to be primed
// to commence sending -- it is cleared by the IRQ, set by uart_write

static int TxPrimed = 0;


/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////

int QueueFull(struct Queue *q)
{
	return (((q->pWR + 1) % QUEUE_SIZE) == q->pRD);
}

int QueueEmpty(struct Queue *q)
{
	return (q->pWR == q->pRD);
}



int Enqueue(struct Queue *q, const uint8_t *data, uint16_t len)
{
	int i;
	for (i = 0; !QueueFull(q) && (i < len); i++)
	{
		q->q[q->pWR] = data[i];
		q->pWR = ((q->pWR + 1) ==  QUEUE_SIZE) ? 0 : q->pWR + 1;
	}
	if (!TxPrimed)
	{
		TxPrimed = 1;
		USART_ITConfig(USART1, USART_IT_TXE, ENABLE);

	}
	return i;
}

int Dequeue(struct Queue *q, uint8_t *data, uint16_t len)
{
	int i;
	for (i = 0; !QueueEmpty(q) && (i < len); i++)
	{
		data[i] = q->q[q->pRD];
		q->pRD = ((q->pRD + 1) ==  QUEUE_SIZE) ? 0 : q->pRD + 1;
	}
	return i;
}

int pack_avail(struct Queue *q)
{
	return (q->pWR >= q->pRD) ? q->pWR - q->pRD : q->pWR + QUEUE_SIZE - q->pRD;
}



void InitQueue(struct Queue *q)
{
	q->pRD = 0;
	q->pWR = 0;
}


int  uart_open (uint8_t uart, uint32_t baud, uint32_t flags)
{
  USART_InitTypeDef USART_InitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;

  if (uart == 1) {

    // get things to a known state
    USART_DeInit(USART1);

    // Enable clock for GPIOB
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

    GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_USART1);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_USART1);

    // Turn on clocks for USART1
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
 
    // Configure TX pin
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF; //GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // Configure RX pin 
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF; //GPIO_Mode_IN;//GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // Configure the UART
    USART_StructInit(&USART_InitStructure);
    USART_InitStructure.USART_BaudRate = baud;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART1, &USART_InitStructure);


    // Enable RX Interrupt.  TX interrupt enabled in send routine
    USART_ClearITPendingBit(USART1, USART_IT_RXNE);
    //disable Transmit Data Register empty interrupt
    USART_ITConfig(USART1, USART_IT_TXE, DISABLE);

    //enable Receive Data register not empty interrupt
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

    InitQueue(&UART1_RXq);
    InitQueue(&UART1_TXq);

    // Configure NVIC
    /* Configure the NVIC Preemption Priority Bits */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
    /* Enable the USART1 Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority =0;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 5;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);


    // Enable USART1
    USART_Cmd(USART1, ENABLE);


    return 0;
  }
  return 1;  // only handle UART2
}





void USART1_IRQHandler(void)
{
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
	{
		uint8_t data;

		// clear the interrupt
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);

		// buffer the data (or toss it if there's no room
		// Flow control is supposed to prevent this
		data = USART_ReceiveData(USART1) & 0xff;
		 if (!Enqueue(&UART1_RXq, &data, 1))
			RxOverflow = 1;

	}

	if(USART_GetITStatus(USART1, USART_IT_TXE) != RESET)
	{
		// Write one byte to the transmit data register
		uint8_t data;

		if (Dequeue(&UART1_TXq, &data, 1))
		{
			USART_SendData(USART1, data);
		}
		else
		{
			// if we have nothing to send, disable the interrupt
			// and wait for a kick
			USART_ITConfig(USART1, USART_IT_TXE, DISABLE);
			TxPrimed = 0;
		}
	}
}











