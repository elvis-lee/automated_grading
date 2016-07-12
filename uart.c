
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


struct Queue {
  uint16_t pRD, pWR;
  uint8_t  q[QUEUE_SIZE];
};


static struct Queue UART1_TXq, UART1_RXq;


static int QueueFull(struct Queue *q)
{
	return (((q->pWR + 1) % QUEUE_SIZE) == q->pRD);
}

static int QueueEmpty(struct Queue *q)
{
	return (q->pWR == q->pRD);
}

/*static int QueueAvail(struct Queue *q)
{
	return (QUEUE_SIZE + q->pWR - q->pRD) % QUEUE_SIZE;
}*/

static int Enqueue(struct Queue *q, const uint8_t *data, uint16_t len)
{
	int i;
	for (i = 0; !QueueFull(q) && (i < len); i++)
	{
		q->q[q->pWR] = data[i];
		q->pWR = ((q->pWR + 1) ==  QUEUE_SIZE) ? 0 : q->pWR + 1;
	}
	return i;
}

static int Dequeue(struct Queue *q, uint8_t *data, uint16_t len)
{
	int i;
	for (i = 0; !QueueEmpty(q) && (i < len); i++)
	{
		data[i] = q->q[q->pRD];
		q->pRD = ((q->pRD + 1) ==  QUEUE_SIZE) ? 0 : q->pRD + 1;
	}
	return i;
}


static void InitQueue(struct Queue *q)
{
	/*int i;
	for (i = 0; i < QUEUE_SIZE; i++)
	{
		q->q[i] = 0;
	}*/
	q->pRD = 0;
	q->pWR = 0;
}

/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////















/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////


/*int uart_close(uint8_t uart)
{

}*/


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


    // DEBUG
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT; //GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

 
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


#ifdef HWFLOWCTRL
    // Configure CTS pin
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // Configure RTS pin -- software controlled
    GPIO_WriteBit(GPIOA, GPIO_Pin_12, 1); //TODO         // nRTS disabled
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT; //GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
#endif

    // Configure the UART
    USART_StructInit(&USART_InitStructure);
    USART_InitStructure.USART_BaudRate = baud;
#ifdef HWFLOWCTRL
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_CTS;
#else
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
#endif
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
    //NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
    /* Enable the USART1 Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    // Enable USART1
    USART_Cmd(USART1, ENABLE);

#ifdef HWFLOWCTRL
    // nRTS enabled
    GPIO_WriteBit(GPIOA, GPIO_Pin_12, 0);
#endif


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

		#ifdef HWFLOWCTRL
		// If queue is above high water mark, disable nRTS

		if (QueueAvail(&UART1_RXq) > HIGH_WATER)
			GPIO_WriteBit(GPIOA, GPIO_Pin_11, 1);
		#endif
	}

	if(USART_GetITStatus(USART1, USART_IT_TXE) != RESET)
	{
		GPIO_WriteBit(GPIOB, GPIO_Pin_8, 1);
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
		GPIO_WriteBit(GPIOB, GPIO_Pin_8, 0);
	}

}


/*
void USART1_IRQHandler(void)
{
	static int tx_index = 0;
	static int rx_index = 0;

	if (USART_GetITStatus(USART1, USART_IT_TXE) != RESET) // Transmit the string in a loop
	{

		USART_SendData(USART1, StringLoop[tx_index++]);

		if (tx_index >= (sizeof(StringLoop) - 1))
		  tx_index = 0;
	}

	if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) // Received characters modify string
	{
		StringLoop[rx_index++] = USART_ReceiveData(USART1);

		if (rx_index >= (sizeof(StringLoop) - 1))
		  rx_index = 0;
	}
}*/




uint16_t uart_write(uint8_t uart, const uint8_t *buf, uint16_t nbyte)
{
	uint8_t data;
	int i = 0;

	if (uart == 1 && nbyte)
	{
		i = Enqueue(&UART1_TXq, buf, nbyte);

		// if we added something and the Transmitter isn't working
		// give it a kick by turning on the buffer empty interrupt
		if (!TxPrimed)
		{
			TxPrimed = 1;

			// This implementation guarantees that USART_IT_Config
			// is not called simultaneously in the interrupt handler and here.
			USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
		}
	}
	return i;
}

uint16_t uart_read (uint8_t uart, uint8_t *buf, uint16_t nbyte)
{
	int i = 0;

	if (uart == 1)
	{
		i = Dequeue(&UART1_RXq, buf, nbyte);

		#ifdef HWFLOWCTRL
		// If the queue has fallen below high water mark, enable nRTS

		if (QueueAvail(&UART1_RXq) <= HIGH_WATER)
			GPIO_WriteBit(GPIOA, GPIO_Pin_11, 0);
		#endif
	}

	return i;
}






/*
uint8_t _putchar(const uint8_t c)
{
    while (! Enqueue (& UART2_TXq , &c, 1))
		if (! TxPrimed ) {
			TxPrimed = 1;
			USART_ITConfig (USART1 , USART_IT_TXE , ENABLE );
		}
}

uint8_t _getchar(void)
{
    uint8_t data;
    while (! Dequeue (& UART2_RXq , &data, 1));

	return data;
}


uint8_t getchar (void)
{
	uint8_t data;
	while ( Dequeue (& UART2_RXq , &data , 1) != 1);

	#ifdef HWFLOWCTRL
	// If the queue has fallen below high water mark, enable nRTS

	if (QueueAvail(&UAR2_RXq) <= HIGH_WATER)
		GPIO_WriteBit(GPIOD, GPIO_Pin_3, 0);
	#endif

	return data;
}
*/




/*
int uart_open ( USART_TypeDef * USARTx , uint32_t baud , uint32_t flags)
{

	GPIO_InitTypeDef GPIO_InitStruct ;
	GPIO_StructInit (& GPIO_InitStruct );

	// Initialize USART1_Tx
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_5 ; //PIO_Pin_6 ; //GPIO_Pin_5 ;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz ;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP ;
	GPIO_Init (GPIOD , & GPIO_InitStruct );

	// Initialize USART1_RX
	GPIO_InitStruct.GPIO_Pin =  GPIO_Pin_6 ; //GPIO_Pin_5 ;//GPIO_Pin_6 ;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING ;
	GPIO_Init (GPIOD , & GPIO_InitStruct );

	// Remap USART, as USART1 is used as alternate pins on PD8/9
	GPIO_PinRemapConfig(GPIO_Remap_USART1, ENABLE);

	// Connect USART pins to AF
	//GPIO_PinAFConfig(GPIOD, GPIO_PinSource8, GPIO_AF_USART1);
	//GPIO_PinAFConfig(GPIOD, GPIO_PinSource9, GPIO_AF_USART1);

	// see stm32f9x_usart.h
	USART_InitTypeDef USART_InitStructure ;
	// Initialize USART structure
	USART_StructInit (&USART_InitStructure );
	// Modify USART_InitStructure for non - default values , e.g.
	// USART_InitStructure . USART_BaudRate = 38400;
	USART_InitStructure.USART_BaudRate = baud;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx ;
	USART_Init (USARTx ,&USART_InitStructure );
	USART_Cmd (USARTx , ENABLE );


	return 0;
}


int uart_close ( USART_TypeDef * USARTx )
{
	USART_Cmd (USARTx , DISABLE );
	return 0;
}


int uart_putc (int c, USART_TypeDef * USARTx )
{
	while ( USART_GetFlagStatus (USARTx , USART_FLAG_TXE ) == RESET);
	USARTx->DR = (c & 0xff);
	return 0;
}


int uart_getc ( USART_TypeDef * USARTx )
{
	while ( USART_GetFlagStatus (USARTx , USART_FLAG_RXNE ) == RESET);
	return USARTx->DR & 0xff;
}


int uart_puts (char* string  , USART_TypeDef * USARTx )
{
	for(char* it = string; *it; ++it) {
		uart_putc(*it, USARTx);
	}
	return 0;
}

char* uart_gets (USART_TypeDef * USARTx )
{
	return "";
}

*/

