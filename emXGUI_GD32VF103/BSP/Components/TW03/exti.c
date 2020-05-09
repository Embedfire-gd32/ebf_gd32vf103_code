#include <string.h>

#include "bsp.h"
#include "sys.h"

#include "exti.h"
#include "delay.h"
//#include "usart.h"
#include "hw_config.h"
#include "spi.h"


extern u8 SPI_Data_In[MAX_IN_OUT_DATA_SIZE];	//输入存储

#define	EXTI2_IRQChannel	0

//外部中断2服务程序
void EXTI2_IRQCallbk(void)
{
//	LED1 = 0;
	hy_debug("\r\nexti2 irq\r\n");
	SetSPIStatus(SPI_DATA_IN);		//SPI接收到新数据
#if 0		//不在中断中接收数据，防止数据刚接收完，应用程序还没有处理完数据又来了新的中断把之前的数据覆盖了
	memset(SPI_Data_In, 0, sizeof(SPI_Data_In));
	SPIx_ReceiveData(SPI_Data_In);
	//SetSPIStatus(SPI_DATA_IN);		//SPI接收到新数据
#endif	
	EXTI->PR=1<<2;  //清除LINE2上的中断标志位
}

//外部中断3服务程序
void EXTI3_IRQCallbk(void)
{
//	LED1 = 0;
	hy_debug("\nexti3 irq\n");
	memset(SPI_Data_In, 0, sizeof(SPI_Data_In));
	SPIx_ReceiveData(SPI_Data_In);
	
	EXTI->PR=1<<3;  //清除LINE3上的中断标志位 
}

//外部中断初始化程序
//初始化PD2为中断输入.
void EXTIX_Init(void)
{
#if 0	
	//PD2
	RCC->APB2ENR|=1<<5;     //使能PORTD时钟

	GPIOD->CRL&=0XFFFFFF0F;//PD2设置成输入	  
	GPIOD->CRL|=0X00000080; 				   
	GPIOD->ODR|=1<<1;	   //PD2上拉
	
	if(0 == (GPIOD->IDR & 0x0002))	//中断IO为低，说明有数据
	{
		SPIx_ReceiveData(SPI_Data_In);
	}

	Ex_NVIC_Config(GPIO_D,1,FTIR);//下降沿触发

	MY_NVIC_Init(2,2,EXTI2_IRQChannel,2);    //抢占2，子优先级2，组2
#endif
#if 1	
//PA2
	RCC->APB2ENR|=1<<2;     //使能PORTA时钟
	GPIOA->CRL&=0XFFFFF0FF;//PA2
	GPIOA->CRL|=0X00000800; 				   
	GPIOA->ODR|=1<<2;	   //PA2
	//PD1
	
	if(0 == (GPIOA->IDR & 0x0004))	//中断IO为低，说明有数据
	{	
		hy_debug("\n\rget spi init irq data\n\r");
		SPIx_ReceiveData(SPI_Data_In);
	}

	Ex_NVIC_Config(GPIO_A,2,FTIR);//下降沿触发
	MY_NVIC_Init(2,2,EXTI2_IRQChannel,2);    //抢占2，子优先级2，组2
#endif	
}












