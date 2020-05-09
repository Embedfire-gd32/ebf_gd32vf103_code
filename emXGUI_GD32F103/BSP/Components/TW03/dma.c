
#include "bsp.h"
#include "dma.h"
#include "delay.h"

u16 DMA1_MEM_LEN;//����DMAÿ�����ݴ��͵ĳ��� 		    
//DMA1�ĸ�ͨ������
//����Ĵ�����ʽ�ǹ̶���,���Ҫ���ݲ�ͬ��������޸�
//�Ӵ洢��->����ģʽ/8λ���ݿ��/�洢������ģʽ
//DMA_CHx:DMAͨ��CHx
//cpar:�����ַ
//cmar:�洢����ַ
//cndtr:���ݴ�����  
//dir: 0 :����->�洢�� 1:�洢��->����
void DMA_Config(DMA_Channel_TypeDef*DMA_CHx,u32 cpar,u32 cmar,u16 cndtr,u8 dir)
{
 	
 //	delay_ms(1);			//�ȴ�DMAʱ���ȶ�
 	DMA_CHx->CPAR=cpar;  	//DMA1 �����ַ 
	DMA_CHx->CMAR=(u32)cmar;//DMA1,�洢����ַ
	DMA1_MEM_LEN=cndtr;     //����DMA����������
	DMA_CHx->CNDTR=cndtr;   //DMA1,����������
	DMA_CHx->CCR=0X00000000;//��λ
	DMA_CHx->CCR|=(dir<<4);  //�Ӵ洢����
	DMA_CHx->CCR|=0<<5;  //��ͨģʽ
	DMA_CHx->CCR|=0<<6;  //�����ַ������ģʽ
	DMA_CHx->CCR|=1<<7;  //�洢������ģʽ
	DMA_CHx->CCR|=0<<8;  //�������ݿ��Ϊ8λ
	DMA_CHx->CCR|=0<<10; //�洢�����ݿ��8λ
	DMA_CHx->CCR|=1<<12; //�е����ȼ�
	DMA_CHx->CCR|=0<<14; //�Ǵ洢�����洢��ģʽ
} 
//����һ��DMA����
void DMA_Enable(DMA_Channel_TypeDef*DMA_CHx)
{
	DMA_CHx->CCR&=~(1<<0);       //�ر�DMA���� 
	DMA_CHx->CNDTR=DMA1_MEM_LEN; //DMA1,���������� 
	DMA_CHx->CCR|=1<<0;          //����DMA����
}	  


