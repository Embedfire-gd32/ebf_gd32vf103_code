/*!
    \file  main.c
    \brief keyboard polling mode
    
    \version 2019-6-5, V1.0.0, demo for GD32VF103
*/

/*
    Copyright (c) 2019, GigaDevice Semiconductor Inc.

    Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice, this 
       list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright notice, 
       this list of conditions and the following disclaimer in the documentation 
       and/or other materials provided with the distribution.
    3. Neither the name of the copyright holder nor the names of its contributors 
       may be used to endorse or promote products derived from this software without 
       specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT 
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR 
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
OF SUCH DAMAGE.
*/

#include "gd32vf103.h"
#include "gd32vf103v_eval.h"
#include "systick.h"

void led_init(void);
void led_flash(int times);

/***********野火专用 LED*******************/

#define LED1_PORT    GPIOB
#define LED1_CLK     RCU_GPIOB
#define LED1_PIN     GPIO_PIN_0

#define LED2_PORT    GPIOB
#define LED2_CLK     RCU_GPIOB
#define LED2_PIN     GPIO_PIN_1

#define LED3_PORT    GPIOB
#define LED3_CLK     RCU_GPIOB
#define LED3_PIN     GPIO_PIN_5

#define LED_OFF      gpio_bit_set(LED1_PORT, LED1_PIN);\
					 gpio_bit_set(LED2_PORT, LED2_PIN);\
					 gpio_bit_set(LED3_PORT, LED3_PIN)

#define LED_ON       gpio_bit_reset(LED1_PORT, LED1_PIN);\
					 gpio_bit_reset(LED2_PORT, LED2_PIN);\
					 gpio_bit_reset(LED3_PORT, LED3_PIN)

#define LED1_ON      gpio_bit_reset(LED1_PORT, LED1_PIN);
#define LED1_OFF     gpio_bit_set(LED1_PORT, LED1_PIN);
#define LED1_TOGGLE	 GPIO_OCTL(LED1_PORT) ^= LED1_PIN

#define LED2_ON      gpio_bit_reset(LED2_PORT, LED2_PIN);
#define LED2_OFF     gpio_bit_set(LED2_PORT, LED2_PIN);
#define LED2_TOGGLE	 GPIO_OCTL(LED2_PORT) ^= LED2_PIN

#define LED3_ON      gpio_bit_reset(LED3_PORT, LED3_PIN);
#define LED3_OFF     gpio_bit_set(LED3_PORT, LED3_PIN);
#define LED3_TOGGLE	 GPIO_OCTL(LED3_PORT) ^= LED3_PIN

/***********野火专用 KEY*******************/

#define KEY1_PORT    GPIOA
#define KEY1_CLK     RCU_GPIOA
#define KEY1_PIN     GPIO_PIN_0

#define KEY2_PORT    GPIOC
#define KEY2_CLK     RCU_GPIOC
#define KEY2_PIN     GPIO_PIN_13

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
	/* enable the LED clock */
	rcu_periph_clock_enable(LED1_CLK);

	/* configure LED GPIO port */
	gpio_init(LED1_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, LED1_PIN | LED2_PIN | LED3_PIN);
    
    /* enable the KEY_B clock */
    rcu_periph_clock_enable(KEY1_CLK);
    rcu_periph_clock_enable(KEY2_CLK);
    rcu_periph_clock_enable(RCU_AF);

    /* configure button pin as input */
    gpio_init(KEY1_PORT, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, KEY1_PIN);
    gpio_init(KEY2_PORT, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, KEY2_PIN);
    
    LED_OFF;

    while(1){
        /* check whether the button is pressed */
        if(SET ==  gpio_input_bit_get(KEY1_PORT, KEY1_PIN)){
            delay_1ms(50);
            LED1_TOGGLE;
        }

        /* check whether the button is pressed */
		if(SET ==  gpio_input_bit_get(KEY2_PORT, KEY2_PIN)){
			delay_1ms(50);
			LED2_TOGGLE;
		}
}
}

/*!
    \brief      initialize the LEDs
    \param[in]  none
    \param[out] none
    \retval     none
*/
void led_init(void)
{
    /* enable the LED clock */
   rcu_periph_clock_enable(RCU_GPIOC);
   rcu_periph_clock_enable(RCU_GPIOE);
   /* configure LED GPIO port */
   gpio_init(GPIOC, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_0 | GPIO_PIN_2);
   gpio_init(GPIOE, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_0 | GPIO_PIN_1);

   /* turn off LEDs */
   gpio_bit_reset(GPIOC, GPIO_PIN_0 | GPIO_PIN_2);
   gpio_bit_reset(GPIOE, GPIO_PIN_0 | GPIO_PIN_1);
}

/*!
    \brief      flash the LEDs for test
    \param[in]  times: times to flash the LEDs
    \param[out] none
    \retval     none
*/
void led_flash(int times)
{
    int i;
    for(i=0; i<times; i++){
        /* delay 400 ms */
        delay_1ms(400);

        /* turn on LEDs */
        gpio_bit_set(GPIOC, GPIO_PIN_0 | GPIO_PIN_2);
        gpio_bit_set(GPIOE, GPIO_PIN_0 | GPIO_PIN_1);

        /* delay 400 ms */
        delay_1ms(400);
        /* turn off LEDs */
        gpio_bit_reset(GPIOC, GPIO_PIN_0 | GPIO_PIN_2);
        gpio_bit_reset(GPIOE, GPIO_PIN_0 | GPIO_PIN_1);
    }
}

