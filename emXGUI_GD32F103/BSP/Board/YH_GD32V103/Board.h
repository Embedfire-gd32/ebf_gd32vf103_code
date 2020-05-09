#ifndef		__BOARD_H___
#define		__BOARD_H___

#ifdef	__cplusplus
extern "C"{
#endif

#include <stdio.h>
#include <stdarg.h>




#include "def.h"

#if 0
#include "Common/drv_inc/my_iic.h"
#include "Common/drv_inc/my_uart.h"

//#include "Common/SYS/sys.h"
//#include "Common/SDMMC/SDMMC.h"
//#include "Common/I2S/bsp_i2s.h"
#include "Common/DMA/DMA_LCD.h"

//#include "Components/WM8978/WM8978.h"

#include "X_GUI_Drv/GUI_Drv_Cfg.h"
#endif

/*=========================================================================================*/

#define	KB	(1024)
#define	MB	(1024*KB)

/*=========================================================================================*/

#define	OS_TIMER			TIMER1
#define	OS_TimerIRQn		TIMER1_IRQn
#define	OS_TimerIRQHandler	TIMER1_IRQHandler

#define	OS_TICK_HZ			200

/*=========================================================================================*/

#define	CCM_BASE				0x10000000
#define	CCM_SIZE				(64*KB)

#define	ONCHIP_SRAM1_BASE		0x20000000
#define	ONCHIP_SRAM1_SIZE		(112*KB)

#define	ONCHIP_SRAM2_BASE		0x2001C000
#define	ONCHIP_SRAM2_SIZE		(16*KB)

#define	EXMC_BANK0_BASE			0x60000000
#define	EXMC_BANK1_BASE			0x64000000
#define	EXMC_BANK2_BASE			0x68000000
#define	EXMC_BANK3_BASE			0x6C000000


#define	EXT_LCD_BASE			EXMC_BANK0_BASE

#define	EXT_LCD_FSMC_BANK		EXMC_BANK0_NORSRAM_REGION0

//#define RES_BASE 0x00200000

/*=========================================================================================*/

extern const struct	i2c_ops I2C_GPD7_GPD3;
extern const struct	i2c_ops I2C_GPB8_GPB9;

/*=========================================================================================*/

void* dma_mem_alloc(u32 size);
void  dma_mem_free(void *p);

/*=========================================================================================*/


extern const char TargetBoard_IO[];

/*=========================================================================================*/

#ifdef	__cplusplus
}
#endif

#endif	/*__BOARD_H___*/
