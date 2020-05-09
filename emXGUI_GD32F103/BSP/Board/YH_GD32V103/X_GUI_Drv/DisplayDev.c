
#include  "gd32vf103.h"

#include "Include.h"
#include "GUI_Drv.h"

/*============================================================================*/

#define	LCD_RST_GPIO	GPIOE
#define	LCD_RST_PIN		GPIO_PIN_1
static  void LCD_RST_GPIO_Init(void)
{
	rcu_periph_clock_enable(RCU_GPIOE);
	gpio_init(LCD_RST_GPIO, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, LCD_RST_PIN);
}


#define	LCD_BL_GPIO		GPIOD
#define	LCD_BL_PIN		GPIO_PIN_12
static  void LCD_BL_GPIO_Init(void)
{
	rcu_periph_clock_enable(RCU_GPIOD);
	gpio_init(LCD_BL_GPIO, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, LCD_BL_PIN);
}

/*============================================================================*/

void	LCD_Init(void);

void LCD_HardInit(void)
{
	LCD_RST_GPIO_Init();
	LCD_BL_GPIO_Init();

	gpio_bit_set(LCD_BL_GPIO,LCD_BL_PIN);

	gpio_bit_set(LCD_RST_GPIO,LCD_RST_PIN);
	GUI_msleep(50);
	gpio_bit_reset(LCD_RST_GPIO,LCD_RST_PIN);
	GUI_msleep(50);
	gpio_bit_set(LCD_RST_GPIO,LCD_RST_PIN);
	GUI_msleep(50);

	LCD_Init();
}


void LCD_BkLight(int on)
{
	if(on)
	{
		gpio_bit_reset(LCD_BL_GPIO,LCD_BL_PIN);
	}
	else
	{
		gpio_bit_set(LCD_BL_GPIO,LCD_BL_PIN);
	}
}

/*===================================================================================*/


#if 0

BOOL	LCD_SetRotate(int rotate)
{

	return FALSE;
}

/*===================================================================================*/

BOOL	LCD_SetRotate(int rotate);

BOOL	gdrvSetRotate(int rotate)
{
	BOOL res=FALSE;

	switch(rotate)
	{
		case ROTATE_0:
			if(LCD_SetRotate(ROTATE_0))
			{
				disp_rotate =rotate;

				surf_screen.Width =LCD_XSIZE;
				surf_screen.Height =LCD_YSIZE;
				surf_screen.WidthBytes =surf_screen.Width*2;

				disp_xmax =surf_screen.Width-1;
				disp_ymax =surf_screen.Height-1;

				res =TRUE;
			}
			break;
			////

		case ROTATE_90:
			if(LCD_SetRotate(ROTATE_90))
			{
				disp_rotate =rotate;

				surf_screen.Width =LCD_YSIZE;
				surf_screen.Height =LCD_XSIZE;
				surf_screen.WidthBytes =surf_screen.Width*2;

				disp_xmax =surf_screen.Width-1;
				disp_ymax =surf_screen.Height-1;

				res =TRUE;
			}
			break;
			////

		case ROTATE_180:
			if(LCD_SetRotate(ROTATE_180))
			{
				disp_rotate =rotate;

				surf_screen.Width =LCD_XSIZE;
				surf_screen.Height =LCD_YSIZE;
				surf_screen.WidthBytes =surf_screen.Width*2;

				disp_xmax =surf_screen.Width-1;
				disp_ymax =surf_screen.Height-1;

				res =TRUE;
			}
			break;
			////

		case ROTATE_270:
			if(LCD_SetRotate(ROTATE_270))
			{
				disp_rotate =rotate;

				surf_screen.Width =LCD_YSIZE;
				surf_screen.Height =LCD_XSIZE;
				surf_screen.WidthBytes =surf_screen.Width*2;

				disp_xmax =surf_screen.Width-1;
				disp_ymax =surf_screen.Height-1;

				res =TRUE;
			}
			break;
			////

		default:
			 break;

	}
	return res;
}
#endif

