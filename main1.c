
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


int main(int argc, char* argv[])
{
	// At this stage the system clock should have already been configured
	// at high speed.
	//trace_printf("System clock: %uHz\n", SystemCoreClock);

	uart_open(myUSART, 9600, 0);
	uint8_t ri;
	cmdi = 0;

	// prompt
	uart_write(myUSART, "# ", strlen("# "));

	// Infinite loop
	while (1)
	{
		// read
		ri = uart_read(myUSART, buf, nbyte);

		if(ri > 0) {

			for(int j=0; j<ri; j++, cmdi++)
				cmd[cmdi] = buf[j];

			// \r = 0x0A, \n = 0x0D
			if(buf[ri-1] == '\r'||  buf[ri-1] == '\n') {

				//echo it back!!
				ri++;
				buf[ri-1] = '\n';
				uart_write(myUSART, buf, ri);

				// replace \r or \n with string end
				cmd[cmdi-1] = '\0';

				cmd_feedback[0] = '\0';
				if(strcmp(cmd, "PLAY") == 0) {
					strcpy(cmd_feedback,"Received PLAY :) \n\r");
				}
				else if(strcmp(cmd, "PAUSE") == 0) {
					strcpy(cmd_feedback,"Received PAUSE :) \n\r");
				}
				else if(strcmp(cmd, "STOP") == 0) {
					strcpy(cmd_feedback,"Received STOP :) \n\r");
				}
				else if(strcmp(cmd, "RESET") == 0) {
					strcpy(cmd_feedback,"Received RESET :) \n\r");
				}

				if(strlen(cmd) > 0)
					uart_write(myUSART, cmd_feedback, strlen(cmd_feedback));

				// clean cmd
				cmd[0] = '\0';
				cmdi = 0;

				// prompt
				uart_write(myUSART, "# ", strlen("# "));
			}
			else {
				//echo it back!!
				uart_write(myUSART, buf, ri);

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
