/**
  ******************************************************************************
  * @file    bsp_led.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   STM32 SD卡 usb mass storage （模拟U盘）
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火 F103-霸道 STM32 开发板
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */

#include "gd32vf103.h"
#include "systick.h"
#include <stdio.h>
#include "./key/bsp_key.h"
#include "./led/bsp_led.h"
#include "./usart/bsp_debug_usart.h"
#include "./eeprom/i2c.h"
#include "./eeprom/at24cxx.h"
#include "./flash/gd25qxx.h"
#include "./sdcard/sdcard_test.h"
#include "./sdcard/bsp_spi_sdcard.h"
#include "./lcd/bsp_ili9341_lcd.h"
#include "./palette/palette.h"
#include "./lcd/bsp_xpt2046_lcd.h"

uint8_t i = 0;
uint8_t textbuf[14] = {0};
uint32_t ILI9341_ID = 0;

uint8_t lcd_flag = 0;
uint8_t touch_flag = 0;
uint8_t flash_flag = 0;
uint8_t eeprom_flag = 0;
uint8_t sdio_flag = 0;

extern volatile uint8_t KEY_DOWN;

static void SPI_Flash_Text(void);
static void LCD_Text(void);
static void EEPROM_Text(void);
static void KEY_Text(void);
//static void USB_Text(void);
static void Periph_Display(void);


void USB_Delay(__IO uint32_t nCount)
{
	for(; nCount != 0; nCount--);
}

int main(void)
{
	uint32_t i = 0;

	uint32_t flash_id = 0;

	LED_GPIO_Config();
	Key_GPIO_Config();
	Debug_USART_Config(EVAL_COM0);
	i2c_eeprom_init();
	spi_flash_init();
	SD_Init();

	ILI9341_Init();
	//触摸屏初始化
	XPT2046_Init();
	//从FLASH里获取校正参数，若FLASH无参数，则使用模式3进行校正
	Calibrate_or_Get_TouchParaWithFlash(3,0);

	//其中0、3、5、6 模式适合从左至右显示文字，
	//不推荐使用其它模式显示文字	其它模式显示文字会有镜像效果
	//其中 6 模式为大部分液晶例程的默认显示方向
	ILI9341_GramScan ( 3 );


	//绘制触摸画板界面
	Palette_Init(LCD_SCAN_MODE);

	/* LCD 初始化 */


	printf("\r\n *********** F103核心板综合测试程序 *********** \r\n");

	/* SD卡 测试 */
	SD_Test();
	/* EEPROM 测试 */
	EEPROM_Text();
	/* SPI FLASH 测试 */
	SPI_Flash_Text();
	/* LCD 测试 */
	LCD_Text();

	/* 按键 初始化 */

	/* TIM6 初始化 */
//	BASIC_TIM_Init();

	/* 显示测试结果 */
	Periph_Display();

  while (1)
  {
    //按键测试
	KEY_Text();
	//触摸检测
    XPT2046_TouchEvenHandler();

    i++;
    if (i > 0xFFFF)
    {
    	LED1_TOGGLE;
    	LED2_TOGGLE;
    	LED3_TOGGLE;
    	i = 0;
    }
  }
}

/**
  * @brief  SPI FLASH测试
  * @param  无
  * @param  无
  * @retval 无
  */
static void SPI_Flash_Text(void)
{
	if (spi_flash_read_id() == sFLASH_ID)
//	if(SPI_FLASH_ReadID() == sFLASH_ID)
	{
		flash_flag = 1;
		printf("SPI FLASH: OK\r\n");
	}
	else
	{
		printf("SPI FLASH: FAILED\r\n");
	}

}

/**
  * @brief  LCD和触摸板测试
  * @param  无
  * @param  无
  * @retval 无
  */
static void LCD_Text(void)
{
	if(ILI9341_ReadID() == 0x9341)
	{
		//触摸屏初始化
		XPT2046_Init();
		//从FLASH里获取校正参数，若FLASH无参数，则使用模式3进行校正
		Calibrate_or_Get_TouchParaWithFlash(3,0);
		lcd_flag = 1;
		touch_flag = 1;
		printf("LCD: OK\r\n");
		printf("TOUCH PAD: OK\r\n");

		ILI9341_GramScan ( 3 );
		//LCD_SetFont(&Font16x24);
		//绘制触摸画板界面
		Palette_Init(LCD_SCAN_MODE);
	}
	else
	{
		//__set_PRIMASK(1);//关总中断
		printf("LCD: FAILED\r\n");
		printf("TOUCH PAD: FAILED\r\n");
		// __set_PRIMASK(0);//开总中断
	}
}

/**
  * @brief  EEPROM测试
  * @param  无
  * @param  无
  * @retval 无
  */
static void EEPROM_Text(void)
{
//	if(ee_CheckOk() ==1)
//	{
//		eeprom_flag = 1;
//		printf("EEPROM：OK\r\n");
//	}
//	else
//	{
//		printf("EEPROM：FAILED\r\n");
//	}
	i2c_24c02_test();
}

/**
  * @brief  按键测试
  * @param  无
  * @param  无
  * @retval 无
  */
static void KEY_Text(void)
{
	switch(KEY_DOWN)
	{
		case 1:
			switch(i)
			{
				case 0:
					LED1_TOGGLE;
					ILI9341_BackLed_Control(DISABLE);
					printf("KEY 1 is pressed\r\n");
					KEY_DOWN = 0;
					i++;
				break;
				case 1:
					LED1_TOGGLE;
					ILI9341_BackLed_Control(ENABLE);
					printf("KEY 1 is pressed\r\n");
					KEY_DOWN = 0;
					i = 0;
				break;
			}break;
		case 2:
			switch(i)
			{
				case 0:
					LED2_TOGGLE;
					ILI9341_BackLed_Control(DISABLE);
					printf("KEY 2 is pressed\r\n");
					KEY_DOWN = 0;
					i++;
				break;
				case 1:
					LED2_TOGGLE;
					ILI9341_BackLed_Control(ENABLE);
					printf("KEY 2 is pressed\r\n");
					KEY_DOWN = 0;
					i = 0;
				break;
			}break;
		default: break;
	}
}

/**
  * @brief  测试结果显示
  * @param  无
  * @param  无
  * @retval 无
  */
static void Periph_Display(void)
{
	LCD_SetFont(&Font8x16);
	if(lcd_flag)
	{
		LCD_SetColors(BLACK,WHITE);
		ILI9341_DispString_EN(80, 0,"LCD:OK");
	}
	if(touch_flag)
	{
		LCD_SetColors(BLACK,WHITE);
		ILI9341_DispString_EN(80, 24,"TOUCH PAD:OK");
	}
	else
	{
		LCD_SetColors(RED,WHITE);
		ILI9341_DispString_EN(80, 24,"TOUCH PAD:FAILED");
	}
	if(flash_flag)
	{
		LCD_SetColors(BLACK,WHITE);
		ILI9341_DispString_EN(80, 48,"SPI FLASH:OK");
	}
	else
	{
		LCD_SetColors(RED,WHITE);
		ILI9341_DispString_EN(80, 48,"SPI FLASH:FAILED");
	}
	if(eeprom_flag)
	{
		LCD_SetColors(BLACK,WHITE);
		ILI9341_DispString_EN(80, 72,"EEPROM: OK");
	}
	else
	{
		LCD_SetColors(RED,WHITE);
		ILI9341_DispString_EN(80, 72,"EEPROM: FAILED");
	}
	if(sdio_flag)
	{
		LCD_SetColors(BLACK,WHITE);
		ILI9341_DispString_EN(80, 94,"SDIO: OK");
	}
	else
	{
		LCD_SetColors(RED,WHITE);
		ILI9341_DispString_EN(80, 94,"SDIO: FAILED");
	}
}

/* -------------------------------------end of file -------------------------------------------- */


/*!
    \file  main.c
    \brief running led
    
    \version 2019-6-5, V1.0.0, firmware for GD32VF103
*/

/*
    Copyright (c) 2019, GigaDevice Semiconductor Inc.

    Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice, this 
       list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright notice, 
       this list of conditions and the following disclaimer in the documentation 
       and/or other materials provided with the distribution.
    3. Neither the name of the copyright holder nor the names of its contributors 
       may be used to endorse or promote products derived from this software without 
       specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT 
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR 
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
OF SUCH DAMAGE.
*/




///*!
//    \brief      main function
//    \param[in]  none
//    \param[out] none
//    \retval     none
//*/
//
//int main(void)
//{
//	uint32_t flash_id = 0;
//
//	LED_GPIO_Config();
//	Key_GPIO_Config();
//	Debug_USART_Config(EVAL_COM0);
//	i2c_eeprom_init();
//	spi_flash_init();
//	SD_Init();
//
//	ILI9341_Init();
//	//触摸屏初始化
//	XPT2046_Init();
//	//从FLASH里获取校正参数，若FLASH无参数，则使用模式3进行校正
//	Calibrate_or_Get_TouchParaWithFlash(3,0);
//
//	//其中0、3、5、6 模式适合从左至右显示文字，
//	//不推荐使用其它模式显示文字	其它模式显示文字会有镜像效果
//	//其中 6 模式为大部分液晶例程的默认显示方向
//	ILI9341_GramScan ( 3 );
//
//
//	//绘制触摸画板界面
//	Palette_Init(LCD_SCAN_MODE);
//
//    while(1){
//
////    	LED_ON;
////		delay_1ms(200);
////		LED_OFF;
////		delay_1ms(200);
////
////    	if (Key_Scan(KEY1_PORT, KEY1_GPIO_PIN) == KEY_ON)
////    	{
////    		LED_ON;
////    		delay_1ms(2000);
////    		LED_OFF;
////    	}
////
////    	printf("/***********欢迎使用野火开发板***********/\r\n");
////
////
////    	/* get flash id */
////		flash_id = spi_flash_read_id();
////		printf("\n\rThe Flash_ID:0x%X\n\r",flash_id);
////
////		SD_Test();
////
////		LCD_Test();
////		if(I2C_OK == i2c_24c02_test())
////		{
////			printf("/*********** EEPROM 测试成功 ***********/\r\n");
////		}
//
//		//触摸检测函数，本函数至少10ms调用一次
//		XPT2046_TouchEvenHandler();
//		delay_1ms(10);
//
//    }
//}

