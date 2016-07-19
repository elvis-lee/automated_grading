
#include "stm32f4xx.h"
#include "stm32f4_discovery.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"

//=====string handle functions declare=====
int strlen(char *s);
int strcmp(const char* s1, const char* s2);
char *strcpy(char *dst, char *src);
uint8_t *strcat(uint8_t *dest, const uint8_t *src);

//=====fram and data structure=====


static void pins_setup(void);
extern __IO uint32_t TimingDelay;


typedef struct __attribute__((packed)){
	uint8_t type;
	uint32_t time;
	uint16_t val;
	uint8_t checksum;
} uart_data_t;

typedef struct __attribute__((packed)){
  uint8_t start;
  uart_data_t data;
  uint8_t stop;
} uart_frame_t;
