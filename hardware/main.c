
#include "uart.h"
#include <stm32f4xx.h>


// please redefine HSE_VALUE and PLL_M following this link:
//http://stm32f4-discovery.com/2015/01/properly-set-clock-speed-stm32f4xx-devices/

// ----- main() ---------------------------------------------------------------

// Sample pragmas to cope with warnings. Please note the related line at
// the end of this function, used to pop the compiler diagnostics status.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wmissing-declarations"
#pragma GCC diagnostic ignored "-Wreturn-type"

//#define myUSART USART1
#define myUSART 1


uint8_t buf[QUEUE_SIZE];
uint8_t nbyte = 32; //size_t

uint8_t cmd[16];
uint8_t cmdi; //size_t

uint8_t cmd_feedback[32];

// avoid newlib
int strlen(char *s);
int strcmp(const char* s1, const char* s2);
char *strcpy(char *dst, char *src);
uint8_t *strcat(uint8_t *dest, const uint8_t *src);

//added by Hao
typedef struct tagoutput
{ 
uint8_t type;
uint32_t time;
uint16_t val;
uint8_t checksum;
} output;
output out[10000];
output out_tem;
uint8_t tem[8];
uint8_t *pack_push(uint8_t *ptr);
output pack_pop();
void load(output data, uint8_t* c);
uint8_t pk_ptr[14]="";
uint8_t i=0;
uint8_t flag=0;

//

int main(int argc, char* argv[])
{
	// At this stage the system clock should have already been configured
	// at high speed.
	//trace_printf("System clock: %uHz\n", SystemCoreClock);

	uart_open(myUSART,1000000,0);
	uint8_t ri;
	cmdi = 0;

	// prompt
	uart_write(myUSART, "elvis is here ", strlen("elvis is here "));

	// Infinite loop
	while (1)
	{		ri = uart_read(myUSART, buf, nbyte);
		        if (ri>0)
                         {
                          strcmp(pk_ptr,strcat(pk_ptr,buf));
                          if (strlen(pk_ptr)>=10)
                          {
                             if (pk_ptr[0]=='S' && pk_ptr[9]=='E')
                               {pack_push(pk_ptr);
                                out_tem = pack_pop();
                                load(out_tem,tem);
                                uart_write(myUSART,"S",1);
                                uart_write(myUSART,tem,8);  
                                uart_write(myUSART,"E",1);
                               }
                             else uart_write(myUSART,"ErrorPack!",10);
                               for (i=0;i<4;i++)
                                pk_ptr[i]=pk_ptr[i+10];
                          }
                         }
    
                       
	}
	uart_close(myUSART);
        
}




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


uint8_t *pack_push(uint8_t *ptr)
{  
  out[++flag].type=ptr[1];
  out[flag].time=*((uint32_t*)(ptr+2));
  out[flag].val=*((uint16_t*)(ptr+6));
  out[flag].checksum=*(ptr+8);
  return "PackStored";
}

output pack_pop()
{  
   output r;
   r.type=out[flag].type;
   r.time=out[flag].time;
   r.val=out[flag].val;
   r.checksum=out[flag--].checksum;
  return r;
}

void load(output data, uint8_t *c)
{ 
 c[0]=data.type;
 c[1]=*((uint8_t*)(&data.time));
 c[2]=*(((uint8_t*)(&data.time))+1);
 c[3]=*(((uint8_t*)(&data.time))+2);
 c[4]=*(((uint8_t*)(&data.time))+3);
 c[5]=*(((uint8_t*)(&data.val))+0);
 c[6]=*(((uint8_t*)(&data.val))+1);
 c[7]=*((uint8_t*)(&data.checksum));
 
}

#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t* file, uint32_t line)
{
    // Infinite loop
    // Use GDB to find out why we're here 
    while (1);
}
#endif



#pragma GCC diagnostic pop

// ----------------------------------------------------------------------------
