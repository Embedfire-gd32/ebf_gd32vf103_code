#ifndef __USART_H
#define __USART_H
#include <stm32f10x_map.h>
#include <stm32f10x_nvic.h>
#include "stdio.h"	 
//////////////////////////////////////////////////////////////////////////////////	 
 

#define UART_NO_DATA	0x01
#define UART_DATA_IN	0x02		//STM32收到uart数据
#define UART_DATA_OUT	0x04		//STM32有uart数据需要下传

#define UART_DATA_MASK 	0X3FF	//数据位0-9

//如果想串口中断接收，请不要注释以下宏定义
#define EN_USART1_RX //使能串口1接收
#define EN_USART2_RX  //使能串口2接收
void uart_init(u32 pclk2,u32 bound);
void USART1_IRQCallback(void);
void USART2_IRQCallback(void);
void USART_TX(u8 *str, u16 len);
void SetUARTStatus(u8 status);
void ClearUARTStatus(u8 status);
u8 GetUARTStatus(void);

#define OPEN_CTS	1

#endif	   
















