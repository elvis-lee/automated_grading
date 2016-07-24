#include "main.h"
#include "uart.h"
#include "string_usr.h"
// please redefine HSE_VALUE and PLL_M following this link:
//http://stm32f4-discovery.com/2015/01/properly-set-clock-speed-stm32f4xx-devices/

#define myUSART 1

//=====define packet byte size=====
#define N 10
//=====define total packet number
#define Npack 6000

//=====external variables=====
extern __IO uint32_t TimingDelay;
//=====buffer=====
uint8_t buf[QUEUE_SIZE];
uint8_t *tem_buf_ptr;
//=====maximum bytes read each time=====
uint16_t nbyte = 10; //size_t 
//=====PINs setting=====
uint16_t pins[] = {GPIO_Pin_0,GPIO_Pin_1, GPIO_Pin_2,GPIO_Pin_3,GPIO_Pin_4,GPIO_Pin_5,GPIO_Pin_6,GPIO_Pin_7,GPIO_Pin_8,GPIO_Pin_9,GPIO_Pin_10,GPIO_Pin_11,GPIO_Pin_12,GPIO_Pin_13,GPIO_Pin_14,GPIO_Pin_15};
//=====data array=====
uart_data_t data_array_out[6000];
uart_data_t data_array_in[6000];
//=====flag for data array=====
__IO uint16_t flag_out= 0 ; 
__IO uint16_t flag_in= 0 ;
//=====push and pop data=====
void pack_push(uint8_t *addr, uint8_t IOtype);
uart_data_t* pack_pop(uint8_t IOtype);
//=====counter slows down buf read=====
uint8_t cnt1 = 0;
//=====temporary test variable=====
uint8_t test1 = 0;
uint8_t pack_rec_all = 0;
uint8_t write_ready_flag =0;


int main(int argc, char* argv[])
{  
    uint32_t ri;
 	uint32_t i;
    
    pins_setup();
	uart_open(myUSART,1000000,0);

    SysTick_Config(SystemCoreClock/200000); 
    NVIC_SetPriority(SysTick_IRQn,0);
    SysTick->CTRL  =  SysTick->CTRL & (~1UL);

	// Infinite loop
	while (1)	
	{  
	  if (cnt1 < 80) //DONT MAKE IT LARGER THAN 200
	   cnt1++;
	  else
	  { cnt1 = 0;
       
       /*=====check priority setting! make sure Systick has higher priority=====
	   if (NVIC_GetPriority( USART1_IRQn ) > NVIC_GetPriority( SysTick_IRQn )) GPIO_SetBits(GPIOD,LED4_PIN);  */


		ri = pack_avail(&UART1_RXq);
		     if (ri >= nbyte) 
			    {    
			       ri = Dequeue(&UART1_RXq,buf,nbyte);
			        for (tem_buf_ptr = buf; tem_buf_ptr - buf < ri ;tem_buf_ptr = tem_buf_ptr + sizeof(uart_frame_t))
			         {	
			   	       pack_push(tem_buf_ptr+1,0);
			         }
			       ri = 0;
			    }

           //=====check if rec all pack=====
		     if (flag_out == Npack)  
		     	{
                    pack_rec_all = 1;
		     	    GPIO_SetBits(GPIOD,LED4_PIN);
                    //echo back the last packet 
		     	    Enqueue(&UART1_TXq,(uint8_t*)pack_pop(0),sizeof(uart_data_t));
		     	}
           //=====enable systick after receiving all pack=====

             if (pack_rec_all)
                {   pack_rec_all = 0;
                    SysTick->CTRL  =  SysTick->CTRL | 1UL;//enable systick
                    GPIO_SetBits(GPIOD,LED5_PIN); 
                }
		}

             if (!flag_out)  
                {     SysTick->CTRL  =  SysTick->CTRL & (~1UL);
                      GPIO_ResetBits(GPIOD,LED6_PIN);
                }	  
/*
             if (flag_in ==5999) 
             	{
             		write_ready_flag = 1;
             	}

             if (write_ready_flag && flag_in > 0)
             	 if (QueueEmpty(&UART1_TXq))  //need improvement later
             		{
             			Enqueue(&UART1_TXq,pack_pop(1),sizeof(uart_data_t));
             		}*/
             	
	}
}	

//=====packet push=====
void pack_push(uint8_t *addr,uint8_t IOtype)
{   
	if (!IOtype)
	{
		flag_out++;
		data_array_out[flag_out] = *(uart_data_t*)addr;
	}

	if (IOtype == 1)
	{
		flag_in++;
		data_array_in[flag_in] = *((uart_data_t*)addr);
	}	
	
	
}

//=====packet pop=====
uart_data_t* pack_pop(uint8_t IOtype)
{
	if (!IOtype)
	{
		uart_data_t* d;
    	d = &(data_array_out[flag_out]);
		flag_out--;
		return d;
	}

	if (IOtype == 1)
	{
		uart_data_t* d;
    	d = &(data_array_in[flag_in]);
		flag_in--;
		return d;
	}	
}


//=====initialize pins=====

static void pins_setup(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
    GPIO_InitStructure.GPIO_Pin   = ALL_PINS;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    /*RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
    GPIO_InitStructure.GPIO_Pin   = ALL_PINS;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOB, &GPIO_InitStructure);*/

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
    GPIO_InitStructure.GPIO_Pin   = ALL_PINS;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOE, &GPIO_InitStructure);
}




#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t* file, uint32_t line)
{
    // Infinite loop
    // Use GDB to find out why we're here 
    while (1);
}
#endif