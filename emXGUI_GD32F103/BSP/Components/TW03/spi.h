#ifndef __SPI_H
#define __SPI_H

#include "bsp.h"
#include "sys.h"

#define SPI_USE_DMA		0		//dma没有调试ok，该宏暂时不能打开

#define SPI_DATATIMEOUT                ((u32)0x0002FFFF)

// SPI总线速度设置 
#define SPI_SPEED_2   0
#define SPI_SPEED_4   1
#define SPI_SPEED_8   2
#define SPI_SPEED_16  3
#define SPI_SPEED_64  4
#define SPI_SPEED_256 5


#define SPI_NSS PAout(4)// PA4

//定义spi数据状态
#define SPI_NO_DATA 	0X01
#define SPI_DATA_OUT 	0X02	//STM32有需要发送的数据
#define SPI_DATA_IN 	0X04	//STM32有接收到新数据

#define SPI_REG_INT_STTS		0x06
#define SPI_REG_RX_DAT_LEN		0x02
#define SPI_REG_TX_BUFF_AVAIL	0x03
#define SPI_CMD_RX_DATA			0x10
#define SPI_CMD_TX_CMD			0x91
#define SPI_CMD_TX_DATA			0x90

						  	    													  
void SPIx_Init(void);			 //初始化SPI口
void SPIx_SetSpeed(u8 SpeedSet); //设置SPI速度   
u8 SPIx_ReadWriteByte(u8 TxData);//SPI总线读写一个字节
u16 SPIx_ReadWriteWord(u16 TxData);
u32 SPIx_ReceiveData(u8 *RXBuf);
u8 SPIx_TransferData(u8 *TXBuf, u16 DataLen);
u8 SPIx_TransferCmd(u8 *TXBuf, u16 CmdLen);
void SetSPIStatus(u8 status);
void ClearSPIStatus(u8 status);
u8 GetSPIStatus(void);

#endif

