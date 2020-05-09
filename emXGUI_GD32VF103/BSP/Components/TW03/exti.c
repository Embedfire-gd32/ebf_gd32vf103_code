#include <string.h>

#include "bsp.h"
#include "sys.h"

#include "exti.h"
#include "delay.h"
//#include "usart.h"
#include "hw_config.h"
#include "spi.h"


extern u8 SPI_Data_In[MAX_IN_OUT_DATA_SIZE];	//����洢

#define	EXTI2_IRQChannel	0

//�ⲿ�ж�2�������
void EXTI2_IRQCallbk(void)
{
//	LED1 = 0;
	hy_debug("\r\nexti2 irq\r\n");
	SetSPIStatus(SPI_DATA_IN);		//SPI���յ�������
#if 0		//�����ж��н������ݣ���ֹ���ݸս����꣬Ӧ�ó���û�д����������������µ��жϰ�֮ǰ�����ݸ�����
	memset(SPI_Data_In, 0, sizeof(SPI_Data_In));
	SPIx_ReceiveData(SPI_Data_In);
	//SetSPIStatus(SPI_DATA_IN);		//SPI���յ�������
#endif	
	EXTI->PR=1<<2;  //���LINE2�ϵ��жϱ�־λ
}

//�ⲿ�ж�3�������
void EXTI3_IRQCallbk(void)
{
//	LED1 = 0;
	hy_debug("\nexti3 irq\n");
	memset(SPI_Data_In, 0, sizeof(SPI_Data_In));
	SPIx_ReceiveData(SPI_Data_In);
	
	EXTI->PR=1<<3;  //���LINE3�ϵ��жϱ�־λ 
}

//�ⲿ�жϳ�ʼ������
//��ʼ��PD2Ϊ�ж�����.
void EXTIX_Init(void)
{
#if 0	
	//PD2
	RCC->APB2ENR|=1<<5;     //ʹ��PORTDʱ��

	GPIOD->CRL&=0XFFFFFF0F;//PD2���ó�����	  
	GPIOD->CRL|=0X00000080; 				   
	GPIOD->ODR|=1<<1;	   //PD2����
	
	if(0 == (GPIOD->IDR & 0x0002))	//�ж�IOΪ�ͣ�˵��������
	{
		SPIx_ReceiveData(SPI_Data_In);
	}

	Ex_NVIC_Config(GPIO_D,1,FTIR);//�½��ش���

	MY_NVIC_Init(2,2,EXTI2_IRQChannel,2);    //��ռ2�������ȼ�2����2
#endif
#if 1	
//PA2
	RCC->APB2ENR|=1<<2;     //ʹ��PORTAʱ��
	GPIOA->CRL&=0XFFFFF0FF;//PA2
	GPIOA->CRL|=0X00000800; 				   
	GPIOA->ODR|=1<<2;	   //PA2
	//PD1
	
	if(0 == (GPIOA->IDR & 0x0004))	//�ж�IOΪ�ͣ�˵��������
	{	
		hy_debug("\n\rget spi init irq data\n\r");
		SPIx_ReceiveData(SPI_Data_In);
	}

	Ex_NVIC_Config(GPIO_A,2,FTIR);//�½��ش���
	MY_NVIC_Init(2,2,EXTI2_IRQChannel,2);    //��ռ2�������ȼ�2����2
#endif	
}












