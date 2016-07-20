
#include "stm32f4xx.h"
#include "stm32f4_discovery.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#define ALL_PINS (GPIO_Pin_1 |GPIO_Pin_2 |GPIO_Pin_3 |GPIO_Pin_4 |GPIO_Pin_5 |GPIO_Pin_6 |GPIO_Pin_7 |GPIO_Pin_8 |GPIO_Pin_9 |GPIO_Pin_10 |GPIO_Pin_11 |GPIO_Pin_12 |GPIO_Pin_13 |GPIO_Pin_14 |GPIO_Pin_15 | LED4_PIN | LED5_PIN | LED3_PIN | LED6_PIN) // all pins


//=====fram and data structure=====
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

//=====pin setup=====
static void pins_setup(void);





