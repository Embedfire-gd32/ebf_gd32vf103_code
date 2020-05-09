/**
  ******************************************************************************
  * @file    bsp_led.c
  * @author  fire
  * @version V1.0
  * @date    2019-xx-xx
  * @brief   led应用函数接口
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火  GD32F103 核心板
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */
  
#include "./led/bsp_led.h"

 /**
  * @brief  初始化控制LED的IO
  * @param  无
  * @retval 无
  */
void LED_GPIO_Config(void)
{		
    /* enable the LED clock */
    rcu_periph_clock_enable(LED1_CLK);

    /* configure LED GPIO port */
    gpio_init(LED1_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, LED1_PIN | LED2_PIN | LED3_PIN);	

    LED_OFF;    // 关闭全部 LED
}
/*********************************************END OF FILE**********************/
