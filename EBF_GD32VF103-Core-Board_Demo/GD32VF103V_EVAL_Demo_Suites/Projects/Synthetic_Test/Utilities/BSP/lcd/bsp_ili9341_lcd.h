#ifndef      __BSP_ILI9341_LCD_H
#define	     __BSP_ILI9341_LCD_H


#include  "gd32vf103.h"
#include "./font/fonts.h"
#include "systick.h"


/***************************************************************************************
2^26 =0X0400 0000 = 64MB,ÿ�� BANK ��4*64MB = 256MB
64MB:EXMC_Bank0_NORSRAM1:0X6000 0000 ~ 0X63FF FFFF
64MB:EXMC_Bank0_NORSRAM2:0X6400 0000 ~ 0X67FF FFFF
64MB:EXMC_Bank0_NORSRAM3:0X6800 0000 ~ 0X6BFF FFFF
64MB:EXMC_Bank0_NORSRAM4:0X6C00 0000 ~ 0X6FFF FFFF

ѡ��BANK0-BORSRAM1 ���� TFT����ַ��ΧΪ0X6000 0000 ~ 0X63FF FFFF
EXMC_A16 ��LCD��DC(�Ĵ���/����ѡ��)��
�Ĵ�������ַ = 0X60000000
RAM����ַ = 0X60020000 = 0X60000000+2^16*2 = 0X60000000 + 0X20000 = 0X60020000
��ѡ��ͬ�ĵ�ַ��ʱ����ַҪ���¼���  
****************************************************************************************/

/******************************* ILI9341 ��ʾ���� EXMC �������� ***************************/
//EXMC_Bank0_NORSRAM����LCD��������ĵ�ַ
#define	     EXMC_BANK0_BASE		       (0x60000000)
#define      EXMC_Addr_ILI9341_CMD         ( ( uint32_t ) EXMC_BANK0_BASE )

//EXMC_Bank0_NORSRAM����LCD���ݲ����ĵ�ַ      
#define      EXMC_Addr_ILI9341_DATA        ( ( uint32_t ) 0x60020000 )

//��Ƭѡ���ž�����NOR/SRAM��
#define      EXMC_Bank0_NORSRAMx           EXMC_BANK0_NORSRAM_REGION0



/******************************* ILI9341 ��ʾ��8080ͨѶ���Ŷ��� ***************************/
/******�����ź���******/
//Ƭѡ��ѡ��NOR/SRAM��
#define      ILI9341_CS_CLK                RCU_GPIOD
#define      ILI9341_CS_PORT               GPIOD
#define      ILI9341_CS_PIN                GPIO_PIN_7

//DC���ţ�ʹ��EXMC�ĵ�ַ�źſ��ƣ������ž����˷���LCDʱʹ�õĵ�ַ
//PD11ΪEXMC_A16
#define      ILI9341_DC_CLK                RCU_GPIOD
#define      ILI9341_DC_PORT               GPIOD
#define      ILI9341_DC_PIN                GPIO_PIN_11

//дʹ��
#define      ILI9341_WR_CLK                RCU_GPIOD
#define      ILI9341_WR_PORT               GPIOD
#define      ILI9341_WR_PIN                GPIO_PIN_5

//��ʹ��
#define      ILI9341_RD_CLK                RCU_GPIOD
#define      ILI9341_RD_PORT               GPIOD
#define      ILI9341_RD_PIN                GPIO_PIN_4

//��λ����
#define      ILI9341_RST_CLK               RCU_GPIOE
#define      ILI9341_RST_PORT              GPIOE
#define      ILI9341_RST_PIN               GPIO_PIN_1

//��������
#define      ILI9341_BK_CLK                RCU_GPIOD   
#define      ILI9341_BK_PORT               GPIOD
#define      ILI9341_BK_PIN                GPIO_PIN_12

/********�����ź���***************/
#define      ILI9341_D0_CLK                RCU_GPIOD
#define      ILI9341_D0_PORT               GPIOD
#define      ILI9341_D0_PIN                GPIO_PIN_14

#define      ILI9341_D1_CLK                RCU_GPIOD
#define      ILI9341_D1_PORT               GPIOD
#define      ILI9341_D1_PIN                GPIO_PIN_15

#define      ILI9341_D2_CLK                RCU_GPIOD
#define      ILI9341_D2_PORT               GPIOD
#define      ILI9341_D2_PIN                GPIO_PIN_0

#define      ILI9341_D3_CLK                RCU_GPIOD
#define      ILI9341_D3_PORT               GPIOD
#define      ILI9341_D3_PIN                GPIO_PIN_1

#define      ILI9341_D4_CLK                RCU_GPIOE
#define      ILI9341_D4_PORT               GPIOE
#define      ILI9341_D4_PIN                GPIO_PIN_7

#define      ILI9341_D5_CLK                RCU_GPIOE
#define      ILI9341_D5_PORT               GPIOE
#define      ILI9341_D5_PIN                GPIO_PIN_8

#define      ILI9341_D6_CLK                RCU_GPIOE
#define      ILI9341_D6_PORT               GPIOE
#define      ILI9341_D6_PIN                GPIO_PIN_9

#define      ILI9341_D7_CLK                RCU_GPIOE
#define      ILI9341_D7_PORT               GPIOE
#define      ILI9341_D7_PIN                GPIO_PIN_10

#define      ILI9341_D8_CLK                RCU_GPIOE
#define      ILI9341_D8_PORT               GPIOE
#define      ILI9341_D8_PIN                GPIO_PIN_11

#define      ILI9341_D9_CLK                RCU_GPIOE
#define      ILI9341_D9_PORT               GPIOE
#define      ILI9341_D9_PIN                GPIO_PIN_12

#define      ILI9341_D10_CLK                RCU_GPIOE
#define      ILI9341_D10_PORT               GPIOE
#define      ILI9341_D10_PIN                GPIO_PIN_13

#define      ILI9341_D11_CLK                RCU_GPIOE
#define      ILI9341_D11_PORT               GPIOE
#define      ILI9341_D11_PIN                GPIO_PIN_14

#define      ILI9341_D12_CLK                RCU_GPIOE
#define      ILI9341_D12_PORT               GPIOE
#define      ILI9341_D12_PIN                GPIO_PIN_15

#define      ILI9341_D13_CLK                RCU_GPIOD
#define      ILI9341_D13_PORT               GPIOD
#define      ILI9341_D13_PIN                GPIO_PIN_8

#define      ILI9341_D14_CLK                RCU_GPIOD
#define      ILI9341_D14_PORT               GPIOD
#define      ILI9341_D14_PIN                GPIO_PIN_9

#define      ILI9341_D15_CLK                RCU_GPIOD
#define      ILI9341_D15_PORT               GPIOD
#define      ILI9341_D15_PIN                GPIO_PIN_10

/*************************************** ����Ԥ�� ******************************************/
#define      DEBUG_DELAY()                delay_1ms(50)     // ��Ҫ��ʱ���ܳ�ʼ���ɹ�

/***************************** ILI934 ��ʾ�������ʼ������������� ***************************/
#define      ILI9341_DispWindow_X_Star		    0     //��ʼ���X����
#define      ILI9341_DispWindow_Y_Star		    0     //��ʼ���Y����

#define 			ILI9341_LESS_PIXEL	  							240			//Һ�����϶̷�������ؿ��
#define 			ILI9341_MORE_PIXEL	 								320			//Һ�����ϳ���������ؿ��

//����Һ��ɨ�跽����仯��XY���ؿ��
//����ILI9341_GramScan�������÷���ʱ���Զ�����
extern uint16_t LCD_X_LENGTH,LCD_Y_LENGTH; 

//Һ����ɨ��ģʽ
//������ѡֵΪ0-7
extern uint8_t LCD_SCAN_MODE;

/******************************* ���� ILI934 ��ʾ��������ɫ ********************************/
#define      BACKGROUND		                BLACK   //Ĭ�ϱ�����ɫ

#define      WHITE		 		                  0xFFFF	   //��ɫ
#define      BLACK                         0x0000	   //��ɫ 
#define      GREY                          0xF7DE	   //��ɫ 
#define      BLUE                          0x001F	   //��ɫ 
#define      BLUE2                         0x051F	   //ǳ��ɫ 
#define      RED                           0xF800	   //��ɫ 
#define      MAGENTA                       0xF81F	   //����ɫ�����ɫ 
#define      GREEN                         0x07E0	   //��ɫ 
#define      CYAN                          0x7FFF	   //����ɫ����ɫ 
#define      YELLOW                        0xFFE0	   //��ɫ 
#define      BRED                          0xF81F
#define      GRED                          0xFFE0
#define      GBLUE                         0x07FF



/******************************* ���� ILI934 �������� ********************************/
#define      CMD_SetCoordinateX		 		    0x2A	     //����X����
#define      CMD_SetCoordinateY		 		    0x2B	     //����Y����
#define      CMD_SetPixel		 		          0x2C	     //�������




/********************************** ���� ILI934 ���� ***************************************/
void                     ILI9341_Init                    ( void );
void                     ILI9341_Rst                     ( void );
uint32_t                 ILI9341_ReadID                  ( void );
void                     ILI9341_BackLed_Control         ( ControlStatus enumState );
void                     ILI9341_GramScan                ( uint8_t ucOtion );
void                     ILI9341_OpenWindow              ( uint16_t usX, uint16_t usY, uint16_t usWidth, uint16_t usHeight );
void                     ILI9341_Clear                   ( uint16_t usX, uint16_t usY, uint16_t usWidth, uint16_t usHeight );
void                     ILI9341_SetPointPixel           ( uint16_t usX, uint16_t usY );
uint16_t                 ILI9341_GetPointPixel           ( uint16_t usX , uint16_t usY );
void                     ILI9341_DrawLine                ( uint16_t usX1, uint16_t usY1, uint16_t usX2, uint16_t usY2 );
void                     ILI9341_DrawRectangle           ( uint16_t usX_Start, uint16_t usY_Start, uint16_t usWidth, uint16_t usHeight,uint8_t ucFilled );
void                     ILI9341_DrawCircle              ( uint16_t usX_Center, uint16_t usY_Center, uint16_t usRadius, uint8_t ucFilled );
void                     ILI9341_DispChar_EN             ( uint16_t usX, uint16_t usY, const char cChar );
void                     ILI9341_DispStringLine_EN      ( uint16_t line, char * pStr );
void                     ILI9341_DispString_EN      			( uint16_t usX, uint16_t usY, char * pStr );
void 											ILI9341_DispString_EN_YDir 		(   uint16_t usX,uint16_t usY ,  char * pStr );

void 											LCD_SetFont											(sFONT *fonts);
sFONT 										*LCD_GetFont											(void);
void 											LCD_ClearLine										(uint16_t Line);
void 											LCD_SetBackColor								(uint16_t Color);
void 											LCD_SetTextColor								(uint16_t Color)	;
void 											LCD_SetColors										(uint16_t TextColor, uint16_t BackColor);
void 											LCD_GetColors										(uint16_t *TextColor, uint16_t *BackColor);


#endif /* __BSP_ILI9341_ILI9341_H */


