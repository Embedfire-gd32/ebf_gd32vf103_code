
#include "Include.h"
#include "JoyStick.h"

/*=========================================================================================*/

void ARD_PinInit(int pin,int dir);
int ARD_PinGet(int pin);
u16 ARD_AnlogGet(int ch);

#define	KEY_A_GET()	ARD_PinGet(2)
#define	KEY_B_GET()	ARD_PinGet(3)
#define	KEY_C_GET()	ARD_PinGet(4)
#define	KEY_D_GET()	ARD_PinGet(5)
#define	KEY_E_GET()	ARD_PinGet(6)
#define	KEY_F_GET()	ARD_PinGet(7)
#define	KEY_K_GET()	ARD_PinGet(8)

/*=========================================================================================*/

#define	VK_A	(1<<0)
#define	VK_B	(1<<1)
#define	VK_C	(1<<2)
#define	VK_D	(1<<3)
#define	VK_E	(1<<4)
#define	VK_F	(1<<5)
#define	VK_K	(1<<6)

/*=========================================================================================*/


void JoyStick_HardInit(void)
{
	GPIO_InitTypeDef gpio_init;

	//For STM32F412 DISC
	/*
	 *  KEY_A - ARD D2 - PG13
	 *  KEY_B - ARD D3 - PF4
	 *  KEY_C - ARD_D4 - PG12
	 *  KEY_D - ARD_D5 - PF10
	 *  KEY_E - ARD_D6 - PF3
	 *  KEY_F - ARD_D7 - PG11
	 *  KEY_K - ARD_D8 - PG10
	 *  X     - ARD_A0 - PA1/ADC1_IN1
	 *  Y     - ARD_A1 - PC1/ADC1_IN11
	 */

	ARD_HardInit();

	ARD_PinInit(2,0);
	ARD_PinInit(3,0);
	ARD_PinInit(4,0);
	ARD_PinInit(5,0);
	ARD_PinInit(6,0);
	ARD_PinInit(7,0);
	ARD_PinInit(8,0);
}

/*=========================================================================================*/

int UART6_Open(u32 baud);
int UART6_Write(const u8 *buf,int size);
int UART6_Read(u8 *buf,int size);
void UART6_Close(void);


static void __puts(const char *s)
{
	UART6_Write(s,x_strlen(s));
}

static void __motor_set(s8 a,s8 b)
{
	s8 buf[4];
	int i;

	i=3;
	while(i-- > 0)
	{
		buf[0] =0xF0;
		buf[1] =0x5A;
		buf[2] =a;
		buf[3] =b;
		UART6_Write(buf,4);

		UART6_Read(buf,2);
		if(buf[0]=='O' && buf[1]=='K')
		{
			break;
		}
		mk_msleep(20);
	}

}

static u8 key_val_cur=0;
static u8 key_val_lost=0;

static u8 get_key_val(u8 *val)
{
	u8 key=0;

	if(KEY_A_GET()==0)
	{
		key |= VK_A;
	}

	if(KEY_B_GET()==0)
	{
		key |= VK_B;
	}

	if(KEY_C_GET()==0)
	{
		key |= VK_C;
	}

	if(KEY_D_GET()==0)
	{
		key |= VK_D;
	}

	if(KEY_E_GET()==0)
	{
		key |= VK_E;
	}

	if(KEY_F_GET()==0)
	{
		key |= VK_F;
	}

	if(KEY_K_GET()==0)
	{
		key |= VK_K;
	}

	*val =key;

	if(key != 0)
	{
		return TRUE;
	}
	return FALSE;

}
/*=========================================================================================*/
static s8 speed=0;

static void KeyDown_Handler(u8 key,u8 key_cnt)
{

	if(key & VK_F)
	{
		if(speed > 0)
		{
			speed -= 5;
		}
	}
	if(key & VK_E)
	{
		if(speed < 100)
		{
			speed += 5;
		}
	}

	speed =MIN(speed,100);
	speed =MAX(speed,0);

	if(key & VK_D) //D - LEFT
	{
		__motor_set(-40,+40);
		return;
	}

	if(key & VK_B) //B - RIGHT
	{
		__motor_set(+40,-40);
		return;
	}

	if(key & VK_A) //A - UP
	{
		__motor_set(speed,speed);
		return;
	}

	if(key & VK_C) //C - DOWN
	{
		__motor_set(-speed,-speed);
		return;
	}


	__motor_set(0,0);

}

static void KeyUp_Hander(u8 key)
{
	__motor_set(0,0);
}

/*=========================================================================================*/
volatile u16 ax0,ay0;
volatile u16 ax,ay;
int xoff,yoff;


void JoyStickMain(void)
{

	u8 key,key_cnt;
	WCHAR buf[128];

	JoyStick_HardInit();

	UART6_Open(115200);
	__puts("UART6_OK.\r\n");

	xoff =0;
	yoff =0;
	speed =0;

	ax0 =ARD_AnlogGet(0);
	ax0 =ARD_AnlogGet(0);
	ax0 >>= 4;

	ay0 =ARD_AnlogGet(1);
	ay0 =ARD_AnlogGet(1);
	ay0 >>= 4;

	key_cnt=0;
	key =0;
	while(1)
	{

		if(get_key_val(&key))
		{
			if(key_cnt < 255)
			{
				key_cnt++;
			}

			if((key_cnt == 2) || (key_cnt&(1<<4)))
			{
				KeyDown_Handler(key,key_cnt);
			}
		}
		else
		{

			if(key_cnt > 0)
			{
				KeyUp_Hander(key);
				key_cnt =0;
			}
			else
			{
				key_cnt=0;
			}
		}

		if(1)
		{
			u16 x,y;

			x =ARD_AnlogGet(0)>>4;
			y =ARD_AnlogGet(1)>>4;

			if(x > ax0)
			{
				xoff =x-ax0;
			}
			else
			{
				xoff =-(ax0-x);
			}
			xoff =MIN(xoff,127);
			xoff =MAX(xoff,-128);

			if(y > ay0)
			{
				yoff =y-ay0;
			}
			else
			{
				yoff =-(ay0-y);
			}
			yoff =MIN(yoff,127);
			yoff =MAX(yoff,-128);

		}

		//speed = yoff>0 ? yoff:-yoff;


		if(1)
		{
			HDC hdc;
			RECT rc;

			x_wsprintf(buf,	L"Xoff:%d,Yoff:%d\r\n"
							L"Speed:%d\r\n",

							xoff,yoff,speed);

			rc.x =0;
			rc.y =0;
			rc.w =240;
			rc.h =48;

			hdc =GetDC(NULL);

			DrawText(hdc,buf,-1,&rc,DT_LEFT|DT_CENTER|DT_BKGND);
			ReleaseDC(NULL,hdc);
			//SYS_msleep(50);
		}

		SYS_msleep(30);

	}

}

/*=========================================================================================*/
