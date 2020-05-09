#include <string.h>

#include "bsp.h"
#include "sys.h"
#include "spi.h"
#include "hw_config.h"
#include "delay.h"	
#include "exti.h"
//#include "usart.h"

u8 SPI_Data_Out[MAX_IN_OUT_DATA_SIZE];	//输出存储
u8 SPI_Data_In[MAX_IN_OUT_DATA_SIZE];	//输入存储
u8 SPI_Reg_Status = SPI_NO_DATA;
//SPI口初始化
//这里针是对SPI1的初始化
void SPIx_Init(void)
{	 
	RCC->APB2ENR|=1<<2;       //PORTA时钟使能 	 
	RCC->APB2ENR|=1<<12;      //SPI1时钟使能 
	RCC->AHBENR|=1<<0;		//开启DMA1时钟
		   
	//这里是针对SPI口初始化
	GPIOA->CRL&=0X0F0FFFFF; 
	GPIOA->CRL|=0XB0B00000;//PA5.7复用 	    
	GPIOA->ODR|=0X5<<5;    //PA5.7推挽输出
	
	GPIOA->CRL&=0XF0FFFFFF; 
	GPIOA->CRL|=0X08000000;//PA6复用 	     上拉输入

	GPIOA->CRL&=0XFFF0FFFF;
	GPIOA->CRL|=0X00030000;//PA.4推挽输出
	GPIOA->ODR|=1<<4;      //PA.4输出高 
	
		
	SPI1->CR1|=0<<10;//全双工模式	
	SPI1->CR1|=1<<9; //软件nss管理
	SPI1->CR1|=1<<8;  

	SPI1->CR1|=1<<2; //SPI主机
	SPI1->CR1|=0<<11;//8bit数据格式	

	SPI1->CR1&=0xFFFFFFFD; //空闲模式下SCK为0 CPOL=0
	SPI1->CR1&=0XFFFFFFFE; //数据采样从第一个时间边沿开始,CPHA=0  
//	SPI1->CR1|=7<<3; //Fsck=Fcpu/256
	SPIx_SetSpeed(SPI_SPEED_4);
	SPI1->CR1|=0<<7; //MSBfirst   
	SPI1->CR1|=1<<6; //SPI设备使能

	memset(SPI_Data_In, 0, sizeof(SPI_Data_In));
	delay_ms(100);
//	SPIx_ReceiveData(SPI_Data_In);	//先取一次数据，下位机复位可能就有数据产生，中断已经置成低电平，ST需要沿触发所以检测不到，主动读一次

	EXTIX_Init();

}   
//SPI 速度设置函数
//SpeedSet:
//SPI_SPEED_2   2分频   (SPI 36M@sys 72M)
//SPI_SPEED_8   8分频   (SPI 9M@sys 72M)
//SPI_SPEED_16  16分频  (SPI 4.5M@sys 72M)
//SPI_SPEED_256 256分频 (SPI 281.25K@sys 72M)
void SPIx_SetSpeed(u8 SpeedSet)
{
	SPI1->CR1&=0XFFC7;//Fsck=Fcpu/256
	switch(SpeedSet)
	{
		case SPI_SPEED_2://二分频
			SPI1->CR1|=0<<3;//Fsck=Fpclk/2=36Mhz
			break;
		case SPI_SPEED_4://四分频
			SPI1->CR1|=1<<3;//Fsck=Fpclk/4=18Mhz
			break;
		case SPI_SPEED_8://八分频
			SPI1->CR1|=2<<3;//Fsck=Fpclk/8=9Mhz
			break;
		case SPI_SPEED_16://十六分频
			SPI1->CR1|=3<<3;//Fsck=Fpclk/16=4.5Mhz
			break;
		case SPI_SPEED_64://六十四分频
			SPI1->CR1|=5<<3;//Fsck=Fpclk/64=1.125Mhz
			break;

		case SPI_SPEED_256://256分频
			SPI1->CR1|=7<<3;//Fsck=Fpclk/16=281.25Khz
			break;
	}		 
	SPI1->CR1|=1<<6; //SPI设备使能	  
} 
//SPIx 读写一个字节
//TxData:要写入的字节
//返回值:读取到的字节
u8 SPIx_ReadWriteByte(u8 TxData)
{		
	u16 retry=0;	
	u16 ret;

	while((SPI1->SR&1<<1)==0)//等待发送区空	
	{
		retry++;
		if(retry>200)return 0;
	}			   
	SPI1->DR=TxData;	 	  //发送一个byte 
	retry=0;
	while((SPI1->SR&1<<0)==0) //等待接收完一个byte  
	{
		retry++;
		if(retry>200)return 0;
	}	  						    
	ret = SPI1->DR;          //返回收到的数据			    

	return ret;
}

u16 SPIx_ReadWriteWord(u16 TxData)
{		
	u8 retry=0;				 
	while((SPI1->SR&1<<1)==0)//等待发送区空	
	{
		retry++;
		if(retry>200)return 0;
	}			  
	SPI1->DR=TxData;	 	  //发送一个byte 
	retry=0;
	while((SPI1->SR&1<<0)==0) //等待接收完一个byte  
	{
		retry++;
		if(retry>200)return 0;
	}	  						    
	return SPI1->DR;          //返回收到的数据			    
}

//接收数据和命令的流程一样
#if 0
u32 SPIx_ReceiveData(u8 *RXBuf)
{
	u16 temp = 0;
	u16 i;
	
	if(0 == RXBuf)
		return 0;
	
	SPI_NSS	 = 0;
	SPIx_ReadWriteByte(0x06);//查询SPI_INT_HOST_STTS
	temp |= SPIx_ReadWriteByte(0xff);					//读寄存器，字节序为小端
	temp |= SPIx_ReadWriteByte(0xff) << 8;
	SPI_NSS	 = 1;
 //	delay_ms(1);
	//hy_debug("\nrx status= %x\r\n",temp);
	
	if((temp != 0xffff) && (temp & 0x01))	//数据或命令已经准备好
	{
		SPI_NSS	 = 0;
		temp = 0;
		SPIx_ReadWriteByte(0x02);//查询RX_DAT_LEN                                                                                                                                                                                                             
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
			SPIx_ReadWriteByte(0x00);	//读数据命令
			for(i = 0;i < temp - 4; i++)
			{
				*(RXBuf + i) = SPIx_ReadWriteByte(0xff);
				hy_debug("[%d]=[%02x]\r\n",i,*(RXBuf + i));
			}
			SPI_NSS	 = 1;
			//delay_ms(1);
			SPI_NSS	 = 0;
			SPIx_ReadWriteByte(0x10);	//最后一次读数据命令
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
	SPIx_ReadWriteByte(SPI_REG_INT_STTS);		//查询SPI_INT_HOST_STTS
	temp |= SPIx_ReadWriteByte(0xff);			//读寄存器，字节序为小端
	temp |= SPIx_ReadWriteByte(0xff) << 8;
	SPI_NSS= (1);
	if((temp != 0xffff) && (temp & 0x01))	//数据或命令已经准备好
	{
		SPI_NSS=(0);
		SPIx_ReadWriteByte(SPI_REG_RX_DAT_LEN);	//查询RX_DAT_LEN
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
				SPIx_ReadWriteByte(SPI_CMD_RX_DATA);	//读数据命令
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
				SPIx_ReadWriteByte(SPI_CMD_RX_DATA);	//读数据命令
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

//用户数据下行
//ret = 0 发送失败
//ret = 1 发送成功
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
		SPIx_ReadWriteByte(0x03);//查询TX_BUFF_AVAILgai 0x03
		temp |= SPIx_ReadWriteByte(0xff);					//读寄存器，字节序为小端
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
		SPIx_ReadWriteByte(0x80);	//写数据命令
		for(i = 0; i < DataLen - 4; i ++)
		{
			SPIx_ReadWriteByte(*(TXBuf + i));
		}
		SPI_NSS	 = 1;
		//delay_ms(1);
		SPI_NSS	 = 0;
		SPIx_ReadWriteByte(0x90);	//最后一次写数据命令
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

//命令下行
//ret = 0 发送失败
//ret = 1 发送成功
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
		SPIx_ReadWriteByte(0x03);//查询TX_BUFF_AVAIL
		temp |= SPIx_ReadWriteByte(0xff);					//读寄存器，字节序为小端
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
		SPIx_ReadWriteByte(0x91);	//写命令命令
#if SPI_USE_DMA
		//SPI1->DR = 0x91;
		DMA_Config(DMA1_Channel3, (u32)&SPI1->DR, (u32)TXBuf, CmdLen, 1);		
		SPI1->CR2 |= (0X1<<7);	//TXE enable
		SPI1->CR2 |= (0X1<<1);	//TXDMA enable
		DMA_Enable(DMA1_Channel3);

		timeout=SPI_DATATIMEOUT;
		while(1)
		{
			if((DMA1->ISR&0X200)/* && (0 == (SPI1->SR&0x80))*/)	//传输完成
			{
				DMA1->IFCR|=(0x7<<8);

				DMA1_Channel3->CCR&=~(1<<0);       //关闭DMA传输
				DMA1_Channel3->CCR = 0X00000000;
				DMA1_Channel3->CPAR=0;  	//DMA1 外设地址 
				DMA1_Channel3->CMAR=0;		//DMA1,存储器地址
				RCC->AHBENR&=(~0x1);		//开启DMA1时钟
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
		while(!(SPI1->SR & 0x02))	//等待TXE = 1
		{
			timeout --;
			if(0 == timeout)
			{
				hy_debug("\r\ntx spi TXE timeout\r\n");
				SPI_NSS	 = 1;
				return 0;
			}
		}

		while(SPI1->SR & 0x80)	//等待busy = 0
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
		//SPI1->CR1&=~(1<<6); //SPI设备使能
	
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
		SPIx_ReadWriteByte(0x91);	//最后一次写命令命令
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
	//测试精简指令
		memset(SPI_Data_Out, 0x5a, sizeof(SPI_Data_Out));
		memset(SPI_Data_In, 0, sizeof(SPI_Data_In));
		SPI_Data_Out[0] = 0xaa;	//SYN
		SPI_Data_Out[1] = 0x01;	//TYPE 命令端口传输 jj
		SPI_Data_Out[2] = 0x00;	//
		SPI_Data_Out[3] = 0x04;	//数据长度 4byte
		SPI_Data_Out[4] = 0x00;	//序号0
		SPI_Data_Out[5] = 0x00;	//FLG
		SPI_Data_Out[6] = 0x00;	//DA
		SPI_Data_Out[7] = 0X05;	//CHK
/*获取版本号*/
#if 1
		SPI_Data_Out[8] = 0X01;	//精简指令获取版本号
		SPI_Data_Out[9] = 0X07;	
		SPI_Data_Out[10] = 0X00;	
		SPI_Data_Out[11] = 0X00;	
		SPI_Data_Out[12] = 0X08;	//CHK	
#endif		
/*扫描网络*/		
#if 0
		SPI_Data_Out[8] = 0X01;	//精简指令扫描网络
		SPI_Data_Out[9] = 0X22;	
		SPI_Data_Out[10] = 0X00;	
		SPI_Data_Out[11] = 0X00;	
		SPI_Data_Out[12] = 0X23;	//CHK	
#endif
		SPI_Data_Out[13] = 0X00;	//paddind
		SPI_Data_Out[14] = 0X00;	//paddind
		SPI_Data_Out[15] = 0X00;	//paddind

#if 0
	SPIx_TransferCmd(SPI_Data_Out, 16);	//传输长度4的整数倍
#endif
	
//	delay_ms(10);
	

	
//	SPIx_ReceiveData(SPI_Data_In);
	
}

void TestSPI_AT(void)
{
	//测试AT指令
		memset(SPI_Data_Out, 0, sizeof(SPI_Data_Out));
		memset(SPI_Data_In, 0, sizeof(SPI_Data_In));
		SPI_Data_Out[0] = 0xaa;	//SYN
		SPI_Data_Out[1] = 0x02;	//TYPE 命令端口传输 at
		SPI_Data_Out[2] = 0x00;	//
		SPI_Data_Out[3] = 0x08;	//数据长度 8byte
		SPI_Data_Out[4] = 0x00;	//序号0
		SPI_Data_Out[5] = 0x00;	//FLG
		SPI_Data_Out[6] = 0x00;	//DA
		SPI_Data_Out[7] = 0X0a;	//CHK

		SPI_Data_Out[8] = 'A';	//at+指令获取版本号
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

	SPIx_TransferCmd(SPI_Data_Out, 32); //传输长度4的整数倍
	
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
























