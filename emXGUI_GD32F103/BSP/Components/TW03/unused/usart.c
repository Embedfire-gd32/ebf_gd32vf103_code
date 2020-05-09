#include "sys.h"
#include "usart.h"
#include "hw_config.h"
u8 UART_Reg_Status = UART_NO_DATA;


#if 1
#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 
	/* Whatever you require here. If the only file you are using is */ 
	/* standard output using printf() for debugging, no file handling */ 
	/* is required. */ 
}; 
/* FILE is typedef�� d in stdio.h. */ 
FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
_sys_exit(int x) 
{ 
	x = x; 
} 
//�ض���fputc���� 
int fputc(int ch, FILE *f)
{      
//#ifdef _DEBUG_
	while((USART1->SR&0X40)==0);//ѭ������,ֱ���������   
	USART1->DR = (u8) ch;      
//#endif
	return ch;
}
#endif 
//end
//////////////////////////////////////////////////////////////////


//����1�жϷ������
//ע��,��ȡUSARTx->SR�ܱ���Ī������Ĵ���   	
u8 USART_RX_BUF[MAX_IN_OUT_DATA_SIZE]; 
//����״̬
//bit15��������ɱ�־
//bit14�����յ���һ��0x0d
//bit13�����յ���һ��0x0a
//bit12�����յ��ڶ���0x0d
//bit9~0�����յ�����Ч�ֽ���Ŀ
u16 USART_RX_STA=0;       //����״̬���	  
  
void USART1_IRQCallback(void)
{
	u8 res;	    
	if(USART1->SR&(1<<5))//���յ�����
	{	 
		res=USART1->DR; 
		if((USART_RX_STA&0x8000)==0)//����δ���
		{
			if(USART_RX_STA&0x1000)  //���յ��ڶ���0x0d
			{
				if(res!=0x0a)
				{
					USART_RX_STA=0;//���մ���,���¿�ʼ
				}
				else 
				{
					USART_RX_STA|=0x8000;	//��������� 
					USART_RX_BUF[USART_RX_STA&UART_DATA_MASK]=res;
					USART_RX_STA++;
					SetUARTStatus(UART_DATA_IN);
				}
			}
			else if(USART_RX_STA&0x2000)	  //���յ���һ��0x0a
			{
				 if(res!=0x0d)
				{
					USART_RX_STA &= ~0x4000;		//˵�����ǽ�������������
					USART_RX_STA &= ~0x2000;
				}
				else 
				{
					USART_RX_STA|=0x1000;	//���յ��ڶ���0x0d 
				}
				USART_RX_BUF[USART_RX_STA&UART_DATA_MASK]=res;
				USART_RX_STA++;
			}
			else if(USART_RX_STA&0x4000)//���յ��˵�һ��0x0d
			{
				if(res!=0x0a)
				{
					USART_RX_STA &= ~0x4000;		//˵�����ǽ�������������
				}
				else 
				{
					USART_RX_STA|=0x2000;	//���յ���һ��0x0a 
				}
				USART_RX_BUF[USART_RX_STA&UART_DATA_MASK]=res;
				USART_RX_STA++;
			}
			else //��û�յ�0X0D
			{	
				if(res==0x0d)			//���յ���һ��0x0d
				{
					USART_RX_STA|=0x4000;	 
					USART_RX_BUF[USART_RX_STA&UART_DATA_MASK]=res;
					USART_RX_STA++;
				}
				else
				{
					USART_RX_BUF[USART_RX_STA&UART_DATA_MASK]=res;
					USART_RX_STA++;
					if((USART_RX_STA&UART_DATA_MASK)>=MAX_IN_OUT_DATA_SIZE)USART_RX_STA=0;//�������ݴ���,���¿�ʼ����	  
				}		 
			}
		} 							     
	}  											 
} 
			
  
void USART2_IRQCallback(void)
{
	u8 res;	    
	if(USART2->SR&(1<<5))//���յ�����
	{	 
		res=USART2->DR; 
		if((USART_RX_STA&0x8000)==0)//����δ���
		{
			if(USART_RX_STA&0x1000)  //���յ��ڶ���0x0d
			{
				if(res!=0x0a)
				{
					USART_RX_STA=0;//���մ���,���¿�ʼ
				}
				else 
				{
					USART_RX_STA|=0x8000;	//��������� 
					USART_RX_BUF[USART_RX_STA&UART_DATA_MASK]=res;
					USART_RX_STA++;
					SetUARTStatus(UART_DATA_IN);
				}
			}
			else if(USART_RX_STA&0x2000)	  //���յ���һ��0x0a
			{
				 if(res!=0x0d)
				{
					USART_RX_STA &= ~0x4000;		//˵�����ǽ�������������
					USART_RX_STA &= ~0x2000;
				}
				else 
				{
					USART_RX_STA|=0x1000;	//���յ��ڶ���0x0d 
				}
				USART_RX_BUF[USART_RX_STA&UART_DATA_MASK]=res;
				USART_RX_STA++;
			}
			else if(USART_RX_STA&0x4000)//���յ��˵�һ��0x0d
			{
				if(res!=0x0a)
				{
					USART_RX_STA &= ~0x4000;		//˵�����ǽ�������������
				}
				else 
				{
					USART_RX_STA|=0x2000;	//���յ���һ��0x0a 
				}
				USART_RX_BUF[USART_RX_STA&UART_DATA_MASK]=res;
				USART_RX_STA++;
			}
			else //��û�յ�0X0D
			{	
				if(res==0x0d)			//���յ���һ��0x0d
				{
					USART_RX_STA|=0x4000;	 
					USART_RX_BUF[USART_RX_STA&UART_DATA_MASK]=res;
					USART_RX_STA++;
				}
				else
				{
					USART_RX_BUF[USART_RX_STA&UART_DATA_MASK]=res;
					USART_RX_STA++;
					if((USART_RX_STA&UART_DATA_MASK)>=MAX_IN_OUT_DATA_SIZE)USART_RX_STA=0;//�������ݴ���,���¿�ʼ����	  
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
		while((USART1->SR&0X40)==0);//ѭ������,ֱ���������   
		USART1->DR = (u8)*(str+i);
#endif
#ifdef EN_USART2_RX
		while((USART2->SR&0X40)==0);//ѭ������,ֱ���������   
		USART2->DR = (u8)*(str+i);
#endif
	 }
}		
		
//��ʼ��IO ����3 ��ӡ����
//pclk2:PCLK2ʱ��Ƶ��(Mhz)
//bound:������

void uart3_init(u32 pclk2,u32 bound)
{  	 
	float temp;
	u16 mantissa;
	u16 fraction;	   
	temp=(float)(pclk2*1000000)/(bound*16);//�õ�USARTDIV
	mantissa=temp;				 //�õ���������
	fraction=(temp-mantissa)*16; //�õ�С������	 
    mantissa<<=4;
	mantissa+=fraction; 
	RCC->APB2ENR|=1<<3;   //ʹ��PORTB��ʱ��  
	RCC->APB1ENR|=1<<18;  //ʹ�ܴ���3ʱ��
	GPIOB->CRH&=0XFFF000FF; 
	GPIOB->CRH|=0X00038B00;//IO״̬���� PB10 TX PB11 TX
//	GPIOB->ODR|=0<<12; 
	RCC->APB1RSTR|=1<<18;   //��λ����3
	RCC->APB1RSTR&=~(1<<18);//ֹͣ��λ	   	   
	//����������

 	USART3->BRR=mantissa; // ����������	 
	USART3->CR1|=0X200C;  //0��һ����ʼλ��8������λ��n��ֹͣλ,��У��λ.

	//ʹ�ܽ����ж�

	USART3->CR1|=1<<8;    //PE�ж�ʹ��
//	USART3->CR1|=1<<5;    //���ջ������ǿ��ж�ʹ��	 
//	MY_NVIC_Init(3,3,USART1_IRQChannel,2);//��2��������ȼ�    	
}

//��ʼ��IO ����1
//pclk2:PCLK2ʱ��Ƶ��(Mhz)
//bound:������
//CHECK OK
//091209
void uart_init(u32 pclk2,u32 bound)
{  	 
	float temp;
	u16 mantissa;
	u16 fraction;	   
	temp=(float)(pclk2*1000000)/(bound*16);//�õ�USARTDIV
	mantissa=temp;				 //�õ���������
	fraction=(temp-mantissa)*16; //�õ�С������	 
    mantissa<<=4;
	mantissa+=fraction; 
	RCC->APB2ENR|=1<<2;   //ʹ��PORTA��ʱ��  
#ifdef EN_USART1_RX
	RCC->APB2ENR|=1<<14;  //ʹ�ܴ���1ʱ��
#endif
#ifdef EN_USART2_RX
	RCC->APB1ENR|=1<<17;  //ʹ�ܴ���2ʱ��

#endif
#ifdef EN_USART1_RX
	GPIOA->CRH&=0XFFFFF00F; 
	GPIOA->CRH|=0X000008B0;//IO״̬����
#endif
#ifdef EN_USART2_RX
	GPIOA->CRL&=0XFFFF00FF; 
	GPIOA->CRL|=0X00008B00;//IO״̬����
#endif								   
#ifdef EN_USART1_RX		  
	RCC->APB2RSTR|=1<<14;   //��λ����1
	RCC->APB2RSTR&=~(1<<14);//ֹͣ��λ	   	   
#endif
#ifdef EN_USART2_RX
	RCC->APB1RSTR|=1<<17;   //��λ����2
	RCC->APB1RSTR&=~(1<<17);//ֹͣ��λ
#endif
	//����������
#ifdef EN_USART1_RX		  
 	USART1->BRR=mantissa; // ����������	 
	USART1->CR1|=0X200C;  //0��һ����ʼλ��8������λ��n��ֹͣλ,��У��λ.
#endif
#ifdef EN_USART2_RX
 	USART2->BRR=mantissa; // ����������	 
	USART2->CR1|=0X200C;  //0��һ����ʼλ��8������λ��n��ֹͣλ,��У��λ.
#endif

#if OPEN_CTS
	USART2->CR3 |= 1<<9;	//CTSE ʹ��
#endif

	//ʹ�ܽ����ж�
#ifdef EN_USART1_RX
	USART1->CR1|=1<<8;    //PE�ж�ʹ��
	USART1->CR1|=1<<5;    //���ջ������ǿ��ж�ʹ��	 
	MY_NVIC_Init(3,3,USART1_IRQChannel,2);//��2��������ȼ�    	
#endif
#ifdef EN_USART2_RX
	USART2->CR1|=1<<8;    //PE�ж�ʹ��
	USART2->CR1|=1<<5;    //���ջ������ǿ��ж�ʹ��	    	
	MY_NVIC_Init(3,3,USART2_IRQChannel,2);//��2��������ȼ�    	
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
