#ifndef __LED_H
#define	__LED_H

#include "gd32vf103.h"

//引脚定义
/*******************************************************/
#define LED1_PORT    GPIOB
#define LED1_CLK     RCU_GPIOB
#define LED1_PIN     GPIO_PIN_0

#define LED2_PORT    GPIOB
#define LED2_CLK     RCU_GPIOB
#define LED2_PIN     GPIO_PIN_1

#define LED3_PORT    GPIOB
#define LED3_CLK     RCU_GPIOB
#define LED3_PIN     GPIO_PIN_5

/************************************************************/


#define LED_OFF       gpio_bit_set(LED1_PORT, LED1_PIN);\
											gpio_bit_set(LED2_PORT, LED2_PIN);\
											gpio_bit_set(LED3_PORT, LED3_PIN)

#define LED_ON       gpio_bit_reset(LED1_PORT, LED1_PIN);\
									 	 gpio_bit_reset(LED2_PORT, LED2_PIN);\
										 gpio_bit_reset(LED3_PORT, LED3_PIN)

#define LED1_ON      gpio_bit_reset(LED1_PORT, LED1_PIN);
#define LED1_OFF     gpio_bit_set(LED1_PORT, LED1_PIN);

#define LED2_ON      gpio_bit_reset(LED2_PORT, LED2_PIN);
#define LED2_OFF     gpio_bit_set(LED2_PORT, LED2_PIN);

#define LED3_ON      gpio_bit_reset(LED3_PORT, LED3_PIN);
#define LED3_OFF     gpio_bit_set(LED3_PORT, LED3_PIN);

#define digitalToggle(p,i) {p->ODR ^=i;} //输出反转状态

#define LED1_TOGGLE    GPIO_OCTL(LED1_PORT) ^= LED1_PIN
#define LED2_TOGGLE    GPIO_OCTL(LED2_PORT) ^= LED2_PIN
#define LED3_TOGGLE    GPIO_OCTL(LED2_PORT) ^= LED3_PIN


void LED_GPIO_Config(void);

#endif /* __LED_H */
