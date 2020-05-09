#include <string.h>

#include "bsp.h"
#include "sys.h"
#include "spi.h"
#include "hw_config.h"
#include "delay.h"	
#include "exti.h"
//#include "usart.h"

u8 SPI_Data_Out[MAX_IN_OUT_DATA_SIZE];	//����洢
u8 SPI_Data_In[MAX_IN_OUT_DATA_SIZE];	//����洢
u8 SPI_Reg_Status = SPI_NO_DATA;
//SPI�ڳ�ʼ��
//�������Ƕ�SPI1�ĳ�ʼ��
void SPIx_Init(void)
{	 
	RCC->APB2ENR|=1<<2;       //PORTAʱ��ʹ�� 	 
	RCC->APB2ENR|=1<<12;      //SPI1ʱ��ʹ�� 
	RCC->AHBENR|=1<<0;		//����DMA1ʱ��
		   
	//���������SPI�ڳ�ʼ��
	GPIOA->CRL&=0X0F0FFFFF; 
	GPIOA->CRL|=0XB0B00000;//PA5.7���� 	    
	GPIOA->ODR|=0X5<<5;    //PA5.7�������
	
	GPIOA->CRL&=0XF0FFFFFF; 
	GPIOA->CRL|=0X08000000;//PA6���� 	     ��������

	GPIOA->CRL&=0XFFF0FFFF;
	GPIOA->CRL|=0X00030000;//PA.4�������
	GPIOA->ODR|=1<<4;      //PA.4����� 
	
		
	SPI1->CR1|=0<<10;//ȫ˫��ģʽ	
	SPI1->CR1|=1<<9; //���nss����
	SPI1->CR1|=1<<8;  

	SPI1->CR1|=1<<2; //SPI����
	SPI1->CR1|=0<<11;//8bit���ݸ�ʽ	

	SPI1->CR1&=0xFFFFFFFD; //����ģʽ��SCKΪ0 CPOL=0
	SPI1->CR1&=0XFFFFFFFE; //���ݲ����ӵ�һ��ʱ����ؿ�ʼ,CPHA=0  
//	SPI1->CR1|=7<<3; //Fsck=Fcpu/256
	SPIx_SetSpeed(SPI_SPEED_4);
	SPI1->CR1|=0<<7; //MSBfirst   
	SPI1->CR1|=1<<6; //SPI�豸ʹ��

	memset(SPI_Data_In, 0, sizeof(SPI_Data_In));
	delay_ms(100);
//	SPIx_ReceiveData(SPI_Data_In);	//��ȡһ�����ݣ���λ����λ���ܾ������ݲ������ж��Ѿ��óɵ͵�ƽ��ST��Ҫ�ش������Լ�ⲻ����������һ��

	EXTIX_Init();

}   
//SPI �ٶ����ú���
//SpeedSet:
//SPI_SPEED_2   2��Ƶ   (SPI 36M@sys 72M)
//SPI_SPEED_8   8��Ƶ   (SPI 9M@sys 72M)
//SPI_SPEED_16  16��Ƶ  (SPI 4.5M@sys 72M)
//SPI_SPEED_256 256��Ƶ (SPI 281.25K@sys 72M)
void SPIx_SetSpeed(u8 SpeedSet)
{
	SPI1->CR1&=0XFFC7;//Fsck=Fcpu/256
	switch(SpeedSet)
	{
		case SPI_SPEED_2://����Ƶ
			SPI1->CR1|=0<<3;//Fsck=Fpclk/2=36Mhz
			break;
		case SPI_SPEED_4://�ķ�Ƶ
			SPI1->CR1|=1<<3;//Fsck=Fpclk/4=18Mhz
			break;
		case SPI_SPEED_8://�˷�Ƶ
			SPI1->CR1|=2<<3;//Fsck=Fpclk/8=9Mhz
			break;
		case SPI_SPEED_16://ʮ����Ƶ
			SPI1->CR1|=3<<3;//Fsck=Fpclk/16=4.5Mhz
			break;
		case SPI_SPEED_64://��ʮ�ķ�Ƶ
			SPI1->CR1|=5<<3;//Fsck=Fpclk/64=1.125Mhz
			break;

		case SPI_SPEED_256://256��Ƶ
			SPI1->CR1|=7<<3;//Fsck=Fpclk/16=281.25Khz
			break;
	}		 
	SPI1->CR1|=1<<6; //SPI�豸ʹ��	  
} 
//SPIx ��дһ���ֽ�
//TxData:Ҫд����ֽ�
//����ֵ:��ȡ�����ֽ�
u8 SPIx_ReadWriteByte(u8 TxData)
{		
	u16 retry=0;	
	u16 ret;

	while((SPI1->SR&1<<1)==0)//�ȴ���������	
	{
		retry++;
		if(retry>200)return 0;
	}			   
	SPI1->DR=TxData;	 	  //����һ��byte 
	retry=0;
	while((SPI1->SR&1<<0)==0) //�ȴ�������һ��byte  
	{
		retry++;
		if(retry>200)return 0;
	}	  						    
	ret = SPI1->DR;          //�����յ�������			    

	return ret;
}

u16 SPIx_ReadWriteWord(u16 TxData)
{		
	u8 retry=0;				 
	while((SPI1->SR&1<<1)==0)//�ȴ���������	
	{
		retry++;
		if(retry>200)return 0;
	}			  
	SPI1->DR=TxData;	 	  //����һ��byte 
	retry=0;
	while((SPI1->SR&1<<0)==0) //�ȴ�������һ��byte  
	{
		retry++;
		if(retry>200)return 0;
	}	  						    
	return SPI1->DR;          //�����յ�������			    
}

//�������ݺ����������һ��
#if 0
u32 SPIx_ReceiveData(u8 *RXBuf)
{
	u16 temp = 0;
	u16 i;
	
	if(0 == RXBuf)
		return 0;
	
	SPI_NSS	 = 0;
	SPIx_ReadWriteByte(0x06);//��ѯSPI_INT_HOST_STTS
	temp |= SPIx_ReadWriteByte(0xff);					//���Ĵ������ֽ���ΪС��
	temp |= SPIx_ReadWriteByte(0xff) << 8;
	SPI_NSS	 = 1;
 //	delay_ms(1);
	//hy_debug("\nrx status= %x\r\n",temp);
	
	if((temp != 0xffff) && (temp & 0x01))	//���ݻ������Ѿ�׼����
	{
		SPI_NSS	 = 0;
		temp = 0;
		SPIx_ReadWriteByte(0x02);//��ѯRX_DAT_LEN                                                                                                                                                                                                             
		temp |= SPIx_ReadWriteByte(0xff);
		temp |= SPIx_ReadWriteByte(0xff) << 8;
		SPI_NSS	 = 1;
		//delay_ms(1);

		if(temp > 0)
		{
			if(temp%4)
			{
				temp = ((temp+3)/4) << 2;
			}
			hy_debug("\nlen = %d\r\n",temp);
			SPI_NSS	 = 0;
			SPIx_ReadWriteByte(0x00);	//����������
			for(i = 0;i < temp - 4; i++)
			{
				*(RXBuf + i) = SPIx_ReadWriteByte(0xff);
				hy_debug("[%d]=[%02x]\r\n",i,*(RXBuf + i));
			}
			SPI_NSS	 = 1;
			//delay_ms(1);
			SPI_NSS	 = 0;
			SPIx_ReadWriteByte(0x10);	//���һ�ζ���������
			for(i = temp - 4;i < temp; i++)
			{
				*(RXBuf + i) = SPIx_ReadWriteByte(0xff);
			}
			hy_debug("[%d]=[%02x]\r\n",i,*(RXBuf + i));
			SPI_NSS	 = 1;
		//	
			return temp;
		}
	}
	return 0;
}
#else
u32 SPIx_ReceiveData(u8 *RXBuf)
{
	u16 temp = 0;
	u16 i;
	//WM_SPI_RX_DESC* rxdesc;
	u8 tempdata;
	SPI_NSS	= 0;;
	//UserLength=
	SPIx_ReadWriteByte(SPI_REG_INT_STTS);		//��ѯSPI_INT_HOST_STTS
	temp |= SPIx_ReadWriteByte(0xff);			//���Ĵ������ֽ���ΪС��
	temp |= SPIx_ReadWriteByte(0xff) << 8;
	SPI_NSS= (1);
	if((temp != 0xffff) && (temp & 0x01))	//���ݻ������Ѿ�׼����
	{
		SPI_NSS=(0);
		SPIx_ReadWriteByte(SPI_REG_RX_DAT_LEN);	//��ѯRX_DAT_LEN
		//ZPF_DBG("len is %d\r\n",UserLength)
		temp |= SPIx_ReadWriteByte(0xff);
		temp |= SPIx_ReadWriteByte(0xff) << 8;
		//ZPF_DBG("\r\n&&receive %d\r\n",temp);
		SPI_NSS=(1);

		if(temp > 0)
		{
			if(temp % 4)
			{
				temp = ((temp + 3) / 4) << 2;
			}
					hy_debug("\nlen = %d\r\n",temp);
			//ZPF_DBG("kevin debug rx len = %d\r\n",temp);
			//rxdesc = SPIGetRxBuff(temp);
			//if(temp < 1024)
			//{
				SPI_NSS=(0);
				SPIx_ReadWriteByte(SPI_CMD_RX_DATA);	//����������
				for(i = 0; i < temp; i++)
				{
					*(RXBuf+ i) = SPIx_ReadWriteByte(0xff);
					printf("[%d]=[%02x]\r\n", i, *(RXBuf + i));
				}
				//printf("\r\n");
				SPI_NSS=(1);
				//AppSendMsg(MSG_SPI, (INT32U)rxdesc);
			//}
			/*
			else
			{
				SPI_NSS = (0);
				SPIx_ReadWriteByte(SPI_CMD_RX_DATA);	//����������
				for(i = 0; i < temp; i++)
				{
					tempdata = SPIx_ReadWriteByte(0xff);
					//ZPF_DBG("Other[%d]=[%x]\r\n", i, *(RXBuf + i));
				}
				SPI_NSS=(1);
			}
				*/	
			return temp;
		}
	}
	return 0;
}
#endif
u32 testspispeedcnt = 0;

//�û���������
//ret = 0 ����ʧ��
//ret = 1 ���ͳɹ�
u8 SPIx_TransferData(u8 *TXBuf, u16 DataLen)
{
	u16 temp = 0;
	u16 i;
	u16 retry=0;
	
	if(0 == TXBuf)
		return 0;
	hy_debug("\r\nentry transfer data\r\n");
	SPI_NSS	 = 0;
	
	while((temp != 0xffff) && (0 == (temp & 0x01)))	
	{
		retry ++;
		SPIx_ReadWriteByte(0x03);//��ѯTX_BUFF_AVAILgai 0x03
		temp |= SPIx_ReadWriteByte(0xff);					//���Ĵ������ֽ���ΪС��
		temp |= SPIx_ReadWriteByte(0xff) << 8;
		if(retry > 20000)
			return 0;
	}
	SPI_NSS	 = 1;
	hy_debug("\nstatus = %x\n",temp);
	//delay_ms(1);
	if(DataLen > 0)
	{
		SPI_NSS	 = 0;
		SPIx_ReadWriteByte(0x80);	//д��������
		for(i = 0; i < DataLen - 4; i ++)
		{
			SPIx_ReadWriteByte(*(TXBuf + i));
		}
		SPI_NSS	 = 1;
		//delay_ms(1);
		SPI_NSS	 = 0;
		SPIx_ReadWriteByte(0x90);	//���һ��д��������
		for(;i < DataLen;i ++)
		{
			SPIx_ReadWriteByte(*(TXBuf + i));
		}
		hy_debug("\r\ntransfer data over\r\n");
		SPI_NSS	 = 1;
		
	}	

	testspispeedcnt ++;
	return	1;
}

//��������
//ret = 0 ����ʧ��
//ret = 1 ���ͳɹ�
u8 SPIx_TransferCmd(u8 *TXBuf, u16 CmdLen)
{
	u16 temp = 0;
	u16 i;
	u16 retry=0;
	
	if(0 == TXBuf)
		return 0;
	hy_debug("\nSPIx_TransferCmd\n");

	SPI_NSS	 = 0;
	
	while((temp != 0xffff) && (0 == (temp & 0x02)))	
	{
		retry ++;
		SPIx_ReadWriteByte(0x03);//��ѯTX_BUFF_AVAIL
		temp |= SPIx_ReadWriteByte(0xff);					//���Ĵ������ֽ���ΪС��
		temp |= SPIx_ReadWriteByte(0xff) << 8;
		if(retry > 20000)
			return 0;
	}
	hy_debug("\nTX_BUFF_AVAIL = %d,cmdlen=%d\r\n",temp,CmdLen);
	SPI_NSS	 = 1;
	//delay_ms(1);
//while(1)
{
	//delay_us(100);
	if(CmdLen > 0)
	{
		if(CmdLen%4)
			CmdLen = ((CmdLen+3)/4)<<2;
		SPI_NSS	 = 0;
		SPIx_ReadWriteByte(0x91);	//д��������
#if SPI_USE_DMA
		//SPI1->DR = 0x91;
		DMA_Config(DMA1_Channel3, (u32)&SPI1->DR, (u32)TXBuf, CmdLen, 1);		
		SPI1->CR2 |= (0X1<<7);	//TXE enable
		SPI1->CR2 |= (0X1<<1);	//TXDMA enable
		DMA_Enable(DMA1_Channel3);

		timeout=SPI_DATATIMEOUT;
		while(1)
		{
			if((DMA1->ISR&0X200)/* && (0 == (SPI1->SR&0x80))*/)	//�������
			{
				DMA1->IFCR|=(0x7<<8);

				DMA1_Channel3->CCR&=~(1<<0);       //�ر�DMA����
				DMA1_Channel3->CCR = 0X00000000;
				DMA1_Channel3->CPAR=0;  	//DMA1 �����ַ 
				DMA1_Channel3->CMAR=0;		//DMA1,�洢����ַ
				RCC->AHBENR&=(~0x1);		//����DMA1ʱ��
				break;
			}
			timeout--;
			if(0 == timeout)
			{
				hy_debug("\r\ntx dma timeout\r\n");
				SPI_NSS	 = 1;
				return 0;
			}
				
		}
		
		timeout=SPI_DATATIMEOUT;
		while(!(SPI1->SR & 0x02))	//�ȴ�TXE = 1
		{
			timeout --;
			if(0 == timeout)
			{
				hy_debug("\r\ntx spi TXE timeout\r\n");
				SPI_NSS	 = 1;
				return 0;
			}
		}

		while(SPI1->SR & 0x80)	//�ȴ�busy = 0
		{
			timeout --;
			if(0 == timeout)
			{
				hy_debug("\r\ntx spi BUSY timeout\r\n");
				SPI_NSS	 = 1;
				return 0;
			}
		}
		SPI1->CR2 &= ~(0X1<<1);	//TXDMA disable
		//SPI1->CR1&=~(1<<6); //SPI�豸ʹ��
	
		 //delay_ms(1);
#else
		for(i = 0; i < CmdLen; i ++)
		{	 hy_debug("%02x ",*(TXBuf + i));
			SPIx_ReadWriteByte(*(TXBuf + i));
		}

#endif

		SPI_NSS	 = 1;
		//delay_us(1);
#if  0//!SPI_USE_DMA
		SPI_NSS	 = 0;
		SPIx_ReadWriteByte(0x91);	//���һ��д��������
		for(i = CmdLen - 4;i < CmdLen; i ++)
		{
			//hy_debug("=%x",*(TXBuf + i));
			SPIx_ReadWriteByte(*(TXBuf + i));
		}
		SPI_NSS	 = 1;
#endif		
	}	
}
	return	1;	
}


void TestSPISpeed(void)
{
	int i;
	int ret;
	
	memset(SPI_Data_Out, 0xaa, sizeof(SPI_Data_Out));
	 testspispeedcnt = 0;
	printf("\r\n\rstart test spi speed\n\r");
	for(i = 0;i < 10000;i ++)
	{
		ret = SPIx_TransferData(SPI_Data_Out, 1024);
		if(0 == ret)
			break;
		//delay_ms(5);
	}
	printf("\r\n\rtest speed stop ,loop=%d\n\r",testspispeedcnt);
}

void TestSPI_JJ(void)
{
	//u16 temp = 0;
	//���Ծ���ָ��
		memset(SPI_Data_Out, 0x5a, sizeof(SPI_Data_Out));
		memset(SPI_Data_In, 0, sizeof(SPI_Data_In));
		SPI_Data_Out[0] = 0xaa;	//SYN
		SPI_Data_Out[1] = 0x01;	//TYPE ����˿ڴ��� jj
		SPI_Data_Out[2] = 0x00;	//
		SPI_Data_Out[3] = 0x04;	//���ݳ��� 4byte
		SPI_Data_Out[4] = 0x00;	//���0
		SPI_Data_Out[5] = 0x00;	//FLG
		SPI_Data_Out[6] = 0x00;	//DA
		SPI_Data_Out[7] = 0X05;	//CHK
/*��ȡ�汾��*/
#if 1
		SPI_Data_Out[8] = 0X01;	//����ָ���ȡ�汾��
		SPI_Data_Out[9] = 0X07;	
		SPI_Data_Out[10] = 0X00;	
		SPI_Data_Out[11] = 0X00;	
		SPI_Data_Out[12] = 0X08;	//CHK	
#endif		
/*ɨ������*/		
#if 0
		SPI_Data_Out[8] = 0X01;	//����ָ��ɨ������
		SPI_Data_Out[9] = 0X22;	
		SPI_Data_Out[10] = 0X00;	
		SPI_Data_Out[11] = 0X00;	
		SPI_Data_Out[12] = 0X23;	//CHK	
#endif
		SPI_Data_Out[13] = 0X00;	//paddind
		SPI_Data_Out[14] = 0X00;	//paddind
		SPI_Data_Out[15] = 0X00;	//paddind

#if 0
	SPIx_TransferCmd(SPI_Data_Out, 16);	//���䳤��4��������
#endif
	
//	delay_ms(10);
	

	
//	SPIx_ReceiveData(SPI_Data_In);
	
}

void TestSPI_AT(void)
{
	//����ATָ��
		memset(SPI_Data_Out, 0, sizeof(SPI_Data_Out));
		memset(SPI_Data_In, 0, sizeof(SPI_Data_In));
		SPI_Data_Out[0] = 0xaa;	//SYN
		SPI_Data_Out[1] = 0x02;	//TYPE ����˿ڴ��� at
		SPI_Data_Out[2] = 0x00;	//
		SPI_Data_Out[3] = 0x08;	//���ݳ��� 8byte
		SPI_Data_Out[4] = 0x00;	//���0
		SPI_Data_Out[5] = 0x00;	//FLG
		SPI_Data_Out[6] = 0x00;	//DA
		SPI_Data_Out[7] = 0X0a;	//CHK

		SPI_Data_Out[8] = 'A';	//at+ָ���ȡ�汾��
		SPI_Data_Out[9] = 'T';	
		SPI_Data_Out[10] = '+';	
		SPI_Data_Out[11] = 'Q';	
		SPI_Data_Out[12] = 'V';
		SPI_Data_Out[13] = 'E';
		SPI_Data_Out[14] = 'R';
		SPI_Data_Out[15] = '\r';
	
		SPI_Data_Out[16] = 0X0B;	//CHK	
		SPI_Data_Out[17] = 0X00;	//paddind
		SPI_Data_Out[18] = 0X00;	//paddind
		SPI_Data_Out[19] = 0X00;	//paddind

	SPIx_TransferCmd(SPI_Data_Out, 32); //���䳤��4��������
	
	delay_ms(10);
}


void SetSPIStatus(u8 status)
{
   SPI_Reg_Status |= status;
}

void ClearSPIStatus(u8 status)
{
	SPI_Reg_Status &= ~status;	
}

u8 GetSPIStatus(void)
{
	return 	SPI_Reg_Status;
}
























