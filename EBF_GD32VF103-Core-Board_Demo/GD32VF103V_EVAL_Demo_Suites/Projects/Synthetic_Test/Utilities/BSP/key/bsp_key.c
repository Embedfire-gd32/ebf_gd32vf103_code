/**
  ******************************************************************************
  * @file    bsp_key.c
  * @author  fire
  * @version V1.0
  * @date    2019-xx-xx
  * @brief   ����Ӧ��bsp��ɨ��ģʽ��
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ��  GD32F103 ���İ�
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */ 

#include "./key/bsp_key.h" 

volatile uint8_t KEY_DOWN = 0;

/// ����ȷ����ʱ
void Key_Delay(uint32_t nCount)
{
	for(; nCount != 0; nCount--);
} 

/**
  * @brief  ���ð����õ���I/O��
  * @param  ��
  * @retval ��
  */
void Key_GPIO_Config(void)
{
#if 0
    /* enable the KEY_B clock */
    rcu_periph_clock_enable(KEY1_GPIO_CLK);
    rcu_periph_clock_enable(KEY2_GPIO_CLK);

    /* configure button pin as input */
    gpio_init(KEY1_PORT, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, KEY1_GPIO_PIN);
    gpio_init(KEY2_PORT, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, KEY2_GPIO_PIN);

#else
    /* enable the KEY_B clock */
   rcu_periph_clock_enable(KEY1_GPIO_CLK);
   rcu_periph_clock_enable(KEY2_GPIO_CLK);
   rcu_periph_clock_enable(RCU_AF);

   /* configure button pin as input */
   gpio_init(KEY1_PORT, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, KEY1_GPIO_PIN);
   gpio_init(KEY2_PORT, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, KEY2_GPIO_PIN);

   /* enable the global interrupt */
   eclic_global_interrupt_enable();
   eclic_priority_group_set(ECLIC_PRIGROUP_LEVEL3_PRIO1);

   /* configure button pin as input */
   gpio_init(KEY1_PORT, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, KEY1_GPIO_PIN);
   gpio_init(KEY1_PORT, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, KEY1_GPIO_PIN);

   /* enable and set key EXTI interrupt to the lowest priority */
   eclic_irq_enable(EXTI10_15_IRQn, 1, 1);
   eclic_irq_enable(EXTI0_IRQn, 1, 1);

   /* connect key EXTI line to key GPIO pin */
   gpio_exti_source_select(GPIO_PORT_SOURCE_GPIOC, GPIO_PIN_SOURCE_13);
   gpio_exti_source_select(GPIO_PORT_SOURCE_GPIOA, GPIO_PIN_SOURCE_0);

   /* configure key EXTI line */
   exti_init(EXTI_13, EXTI_INTERRUPT, EXTI_TRIG_FALLING);
   exti_interrupt_flag_clear(EXTI_13);

   exti_init(EXTI_0, EXTI_INTERRUPT, EXTI_TRIG_FALLING);
   exti_interrupt_flag_clear(EXTI_0);
#endif
}


/**
  * @brief   ����Ƿ��а�������     
  *	@param 	GPIOx:����Ķ˿�, x�����ǣ�A...K�� 
	*	@param 	GPIO_PIN:����Ķ˿�λ�� ������GPIO_PIN_x��x������0...15��
  * @retval  ������״̬
  *		@arg KEY_ON:��������
  *		@arg KEY_OFF:����û����
  */
uint8_t Key_Scan(uint32_t GPIOx, uint32_t GPIO_Pin)
{			
	/*����Ƿ��а������� */
	if(gpio_input_bit_get(GPIOx,GPIO_Pin) == KEY_ON )  
	{	 
		/*�ȴ������ͷ� */
		while(gpio_input_bit_get(GPIOx,GPIO_Pin) == KEY_ON);   
		return 	KEY_ON;	 
	}
	else
		return KEY_OFF;
}


/*!
    \brief      this function handles external lines 10 to 15 interrupt request
    \param[in]  none
    \param[out] none
    \retval     none
*/
void EXTI10_15_IRQHandler(void)
{
    if(RESET != exti_interrupt_flag_get(EXTI_13)){
        exti_interrupt_flag_clear(EXTI_13);
       KEY_DOWN = 2;
    }
}

/*!
    \brief      this function handles external lines 10 to 15 interrupt request
    \param[in]  none
    \param[out] none
    \retval     none
*/
void EXTI0_IRQHandler(void)
{
    if(RESET != exti_interrupt_flag_get(EXTI_0)){
        exti_interrupt_flag_clear(EXTI_0);
       KEY_DOWN = 1;
    }
}


/*********************************************END OF FILE**********************/
