#include "main.h"
#include "uart.h"
// please redefine HSE_VALUE and PLL_M following this link:
//http://stm32f4-discovery.com/2015/01/properly-set-clock-speed-stm32f4xx-devices/

#define myUSART 1

//=====define packet byte size=====
#define N 10

//=====external variables=====
extern __IO uint32_t TimingDelay;
//=====buffer=====
uint8_t buf[QUEUE_SIZE];
uint8_t *tem_buf_ptr;
//=====maximum bytes read each time=====
uint16_t nbyte = 100; //size_t
//=====PINs setting=====
uint16_t pins[] = {GPIO_Pin_0,GPIO_Pin_1, GPIO_Pin_2,GPIO_Pin_3,GPIO_Pin_4,GPIO_Pin_5,GPIO_Pin_6,GPIO_Pin_7,GPIO_Pin_8,GPIO_Pin_9,GPIO_Pin_10,GPIO_Pin_11,GPIO_Pin_12,GPIO_Pin_13,GPIO_Pin_14,GPIO_Pin_15};
//=====data array=====
uart_data_t data_array[12000];
//=====flag for data array=====
__IO uint16_t flag= 0 ; 
//=====push and pop data=====
void pack_push(uint8_t *addr);
uart_data_t* pack_pop();
//=====counter slows down buf read=====
uint8_t cnt1 = 100;
//=====temporary test variable=====
uint8_t test1 = 0;
uint8_t pack_rec_all = 0;


int main(int argc, char* argv[])
{  
    uint32_t ri;
 	uint32_t i;
    
    pins_setup();
	uart_open(myUSART,1000000,0);

    SysTick_Config(SystemCoreClock/50000); 
    NVIC_SetPriority(SysTick_IRQn,0);
    SysTick->CTRL  =  SysTick->CTRL & (~1UL);
   // SysTick->CTRL  =  SysTick->CTRL | 1UL;  

	// Infinite loop
	while (1)	
	{  
	  if (cnt1 < 100) //DONT MAKE IT LARGER THAN 200
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
			   	       pack_push(tem_buf_ptr);
			         }
			       ri = 0;
			    }

           //=====check if rec all pack=====
		     if (flag >= 11000)  
		     	{
                    pack_rec_all = 1;
		     	    GPIO_SetBits(GPIOD,LED4_PIN); 
                    //echo back the 11000 packet 
		     	    Enqueue(&UART1_TXq,pack_pop(),sizeof(uart_data_t));
		     	}
           //=====enable systick after receiving all pack=====

             if (pack_rec_all)
                {   pack_rec_all = 0;
                    SysTick->CTRL  =  SysTick->CTRL | 1UL;//enable systick
                    GPIO_SetBits(GPIOD,LED5_PIN); 
                   
                      if (!flag)  
                      {  SysTick->CTRL = 0;//disable systick
                         GPIO_ResetBits(GPIOD,LED5_PIN);
                      }
  
                }
		  	 
		}
	}
}	

//=====packet push=====
void pack_push(uint8_t *addr)
{   uart_frame_t *frame;
	flag++;
	frame = ((uart_frame_t*)addr);
	data_array[flag] = (*frame).data;
	
}

//=====packet pop=====
uart_data_t* pack_pop()
{
	uart_data_t* d;
    d = &(data_array[flag]);
	flag--;
	return d;
}


//=====string handle functions=====
int strlen(char *s)
{
    char *p = s;
    while (*p != '\0')
        p++;
    return p - s;
}

char *strcpy(char *dst, char *src)
{
    while((*dst++ = *src++)!= '\0')
        ; // <<== Very important!!!
    return dst;
}

int strcmp(const char* s1, const char* s2)
{
    while(*s1 && (*s1==*s2))
        s1++,s2++;
    return *(const unsigned char*)s1-*(const unsigned char*)s2;
}

uint8_t* strcat(uint8_t* dest,const uint8_t* src)
{
    if(src==""||dest=="") return dest;
    char* temp=dest;
    int i=0;
    int j=0;
    while (dest[i]!='\0')
        i++;
    while((dest[i++]=src[j++])!='\0')
        ;
    return temp;
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
}




#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t* file, uint32_t line)
{
    // Infinite loop
    // Use GDB to find out why we're here 
    while (1);
}
#endif