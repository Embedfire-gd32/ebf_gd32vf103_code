#include "sys.h"
#include "usart.h"
#include "hw_config.h"
u8 UART_Reg_Status = UART_NO_DATA;


#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 
	/* Whatever you require here. If the only file you are using is */ 
	/* standard output using printf() for debugging, no file handling */ 
	/* is required. */ 
}; 
/* FILE is typedef’ d in stdio.h. */ 
FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
_sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{      
//#ifdef _DEBUG_
	while((USART1->SR&0X40)==0);//循环发送,直到发送完毕   
	USART1->DR = (u8) ch;      
//#endif
	return ch;
}
#endif 
//end
//////////////////////////////////////////////////////////////////


//串口1中断服务程序
//注意,读取USARTx->SR能避免莫名其妙的错误   	
u8 USART_RX_BUF[MAX_IN_OUT_DATA_SIZE]; 
//接收状态
//bit15，接收完成标志
//bit14，接收到第一个0x0d
//bit13，接收到第一个0x0a
//bit12，接收到第二个0x0d
//bit9~0，接收到的有效字节数目
u16 USART_RX_STA=0;       //接收状态标记	  
  
void USART1_IRQCallback(void)
{
	u8 res;	    
	if(USART1->SR&(1<<5))//接收到数据
	{	 
		res=USART1->DR; 
		if((USART_RX_STA&0x8000)==0)//接收未完成
		{
			if(USART_RX_STA&0x1000)  //接收到第二个0x0d
			{
				if(res!=0x0a)
				{
					USART_RX_STA=0;//接收错误,重新开始
				}
				else 
				{
					USART_RX_STA|=0x8000;	//接收完成了 
					USART_RX_BUF[USART_RX_STA&UART_DATA_MASK]=res;
					USART_RX_STA++;
					SetUARTStatus(UART_DATA_IN);
				}
			}
			else if(USART_RX_STA&0x2000)	  //接收到第一个0x0a
			{
				 if(res!=0x0d)
				{
					USART_RX_STA &= ~0x4000;		//说明不是结束，继续接收
					USART_RX_STA &= ~0x2000;
				}
				else 
				{
					USART_RX_STA|=0x1000;	//接收到第二个0x0d 
				}
				USART_RX_BUF[USART_RX_STA&UART_DATA_MASK]=res;
				USART_RX_STA++;
			}
			else if(USART_RX_STA&0x4000)//接收到了第一个0x0d
			{
				if(res!=0x0a)
				{
					USART_RX_STA &= ~0x4000;		//说明不是结束，继续接收
				}
				else 
				{
					USART_RX_STA|=0x2000;	//接收到第一个0x0a 
				}
				USART_RX_BUF[USART_RX_STA&UART_DATA_MASK]=res;
				USART_RX_STA++;
			}
			else //还没收到0X0D
			{	
				if(res==0x0d)			//接收到第一个0x0d
				{
					USART_RX_STA|=0x4000;	 
					USART_RX_BUF[USART_RX_STA&UART_DATA_MASK]=res;
					USART_RX_STA++;
				}
				else
				{
					USART_RX_BUF[USART_RX_STA&UART_DATA_MASK]=res;
					USART_RX_STA++;
					if((USART_RX_STA&UART_DATA_MASK)>=MAX_IN_OUT_DATA_SIZE)USART_RX_STA=0;//接收数据错误,重新开始接收	  
				}		 
			}
		} 							     
	}  											 
} 
			
  
void USART2_IRQCallback(void)
{
	u8 res;	    
	if(USART2->SR&(1<<5))//接收到数据
	{	 
		res=USART2->DR; 
		if((USART_RX_STA&0x8000)==0)//接收未完成
		{
			if(USART_RX_STA&0x1000)  //接收到第二个0x0d
			{
				if(res!=0x0a)
				{
					USART_RX_STA=0;//接收错误,重新开始
				}
				else 
				{
					USART_RX_STA|=0x8000;	//接收完成了 
					USART_RX_BUF[USART_RX_STA&UART_DATA_MASK]=res;
					USART_RX_STA++;
					SetUARTStatus(UART_DATA_IN);
				}
			}
			else if(USART_RX_STA&0x2000)	  //接收到第一个0x0a
			{
				 if(res!=0x0d)
				{
					USART_RX_STA &= ~0x4000;		//说明不是结束，继续接收
					USART_RX_STA &= ~0x2000;
				}
				else 
				{
					USART_RX_STA|=0x1000;	//接收到第二个0x0d 
				}
				USART_RX_BUF[USART_RX_STA&UART_DATA_MASK]=res;
				USART_RX_STA++;
			}
			else if(USART_RX_STA&0x4000)//接收到了第一个0x0d
			{
				if(res!=0x0a)
				{
					USART_RX_STA &= ~0x4000;		//说明不是结束，继续接收
				}
				else 
				{
					USART_RX_STA|=0x2000;	//接收到第一个0x0a 
				}
				USART_RX_BUF[USART_RX_STA&UART_DATA_MASK]=res;
				USART_RX_STA++;
			}
			else //还没收到0X0D
			{	
				if(res==0x0d)			//接收到第一个0x0d
				{
					USART_RX_STA|=0x4000;	 
					USART_RX_BUF[USART_RX_STA&UART_DATA_MASK]=res;
					USART_RX_STA++;
				}
				else
				{
					USART_RX_BUF[USART_RX_STA&UART_DATA_MASK]=res;
					USART_RX_STA++;
					if((USART_RX_STA&UART_DATA_MASK)>=MAX_IN_OUT_DATA_SIZE)USART_RX_STA=0;//接收数据错误,重新开始接收	  
				}		 
			}
		} 		 									     
	}  											 
} 


void USART_TX(u8 *str, u16 len)
{
	 u16 i;

	 for(i = 0;i < len;i ++)
	 {
#ifdef EN_USART1_RX
		while((USART1->SR&0X40)==0);//循环发送,直到发送完毕   
		USART1->DR = (u8)*(str+i);
#endif
#ifdef EN_USART2_RX
		while((USART2->SR&0X40)==0);//循环发送,直到发送完毕   
		USART2->DR = (u8)*(str+i);
#endif
	 }
}		
		
//初始化IO 串口3 打印串口
//pclk2:PCLK2时钟频率(Mhz)
//bound:波特率

void uart3_init(u32 pclk2,u32 bound)
{  	 
	float temp;
	u16 mantissa;
	u16 fraction;	   
	temp=(float)(pclk2*1000000)/(bound*16);//得到USARTDIV
	mantissa=temp;				 //得到整数部分
	fraction=(temp-mantissa)*16; //得到小数部分	 
    mantissa<<=4;
	mantissa+=fraction; 
	RCC->APB2ENR|=1<<3;   //使能PORTB口时钟  
	RCC->APB1ENR|=1<<18;  //使能串口3时钟
	GPIOB->CRH&=0XFFF000FF; 
	GPIOB->CRH|=0X00038B00;//IO状态设置 PB10 TX PB11 TX
//	GPIOB->ODR|=0<<12; 
	RCC->APB1RSTR|=1<<18;   //复位串口3
	RCC->APB1RSTR&=~(1<<18);//停止复位	   	   
	//波特率设置

 	USART3->BRR=mantissa; // 波特率设置	 
	USART3->CR1|=0X200C;  //0：一个起始位，8个数据位，n个停止位,无校验位.

	//使能接收中断

	USART3->CR1|=1<<8;    //PE中断使能
//	USART3->CR1|=1<<5;    //接收缓冲区非空中断使能	 
//	MY_NVIC_Init(3,3,USART1_IRQChannel,2);//组2，最低优先级    	
}

//初始化IO 串口1
//pclk2:PCLK2时钟频率(Mhz)
//bound:波特率
//CHECK OK
//091209
void uart_init(u32 pclk2,u32 bound)
{  	 
	float temp;
	u16 mantissa;
	u16 fraction;	   
	temp=(float)(pclk2*1000000)/(bound*16);//得到USARTDIV
	mantissa=temp;				 //得到整数部分
	fraction=(temp-mantissa)*16; //得到小数部分	 
    mantissa<<=4;
	mantissa+=fraction; 
	RCC->APB2ENR|=1<<2;   //使能PORTA口时钟  
#ifdef EN_USART1_RX
	RCC->APB2ENR|=1<<14;  //使能串口1时钟
#endif
#ifdef EN_USART2_RX
	RCC->APB1ENR|=1<<17;  //使能串口2时钟

#endif
#ifdef EN_USART1_RX
	GPIOA->CRH&=0XFFFFF00F; 
	GPIOA->CRH|=0X000008B0;//IO状态设置
#endif
#ifdef EN_USART2_RX
	GPIOA->CRL&=0XFFFF00FF; 
	GPIOA->CRL|=0X00008B00;//IO状态设置
#endif								   
#ifdef EN_USART1_RX		  
	RCC->APB2RSTR|=1<<14;   //复位串口1
	RCC->APB2RSTR&=~(1<<14);//停止复位	   	   
#endif
#ifdef EN_USART2_RX
	RCC->APB1RSTR|=1<<17;   //复位串口2
	RCC->APB1RSTR&=~(1<<17);//停止复位
#endif
	//波特率设置
#ifdef EN_USART1_RX		  
 	USART1->BRR=mantissa; // 波特率设置	 
	USART1->CR1|=0X200C;  //0：一个起始位，8个数据位，n个停止位,无校验位.
#endif
#ifdef EN_USART2_RX
 	USART2->BRR=mantissa; // 波特率设置	 
	USART2->CR1|=0X200C;  //0：一个起始位，8个数据位，n个停止位,无校验位.
#endif

#if OPEN_CTS
	USART2->CR3 |= 1<<9;	//CTSE 使能
#endif

	//使能接收中断
#ifdef EN_USART1_RX
	USART1->CR1|=1<<8;    //PE中断使能
	USART1->CR1|=1<<5;    //接收缓冲区非空中断使能	 
	MY_NVIC_Init(3,3,USART1_IRQChannel,2);//组2，最低优先级    	
#endif
#ifdef EN_USART2_RX
	USART2->CR1|=1<<8;    //PE中断使能
	USART2->CR1|=1<<5;    //接收缓冲区非空中断使能	    	
	MY_NVIC_Init(3,3,USART2_IRQChannel,2);//组2，最低优先级    	
#endif

	
}

void SetUARTStatus(u8 status)
{
   UART_Reg_Status |= status;
}

void ClearUARTStatus(u8 status)
{
	UART_Reg_Status &= ~status;	
}

u8 GetUARTStatus(void)
{
	return 	UART_Reg_Status;
}
