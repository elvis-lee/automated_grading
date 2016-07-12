
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
uint8_t pk_ptr[14]="";
uint8_t i=0;
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
                          if (strlen(pk_ptr)>=9)
                          {
                             if (pk_ptr[0]=='S' && pk_ptr[8]=='E')
                              uart_write(myUSART,pk_ptr,9);
                             else uart_write(myUSART,"ErrorPack",9);
                               for (i=0;i<5;i++)
                                pk_ptr[i]=pk_ptr[i+9];
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

uint8_t* strcat(uint8_t* dest,const uint8_t* src){
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
