#ifndef __KEY_H
#define	__KEY_H

#include "gd32vf103.h"

//���Ŷ���
/*******************************************************/
#define KEY1_PORT         GPIOA
#define KEY1_GPIO_CLK     RCU_GPIOA
#define KEY1_GPIO_PIN     GPIO_PIN_0

#define KEY2_PORT         GPIOC
#define KEY2_GPIO_CLK     RCU_GPIOC
#define KEY2_GPIO_PIN     GPIO_PIN_13
/*******************************************************/

 /** �������±��ú�
	* ��������Ϊ�ߵ�ƽ������ KEY_ON=1�� KEY_OFF=0
	* ����������Ϊ�͵�ƽ���Ѻ����ó�KEY_ON=0 ��KEY_OFF=1 ����
	*/
#define KEY_ON	SET
#define KEY_OFF	RESET

void Key_GPIO_Config(void);
uint8_t Key_Scan(uint32_t GPIOx, uint32_t GPIO_Pin);

#endif /* __LED_H */

