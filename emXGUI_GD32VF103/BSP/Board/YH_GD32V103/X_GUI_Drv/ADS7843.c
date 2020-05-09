#include  "gd32vf103.h"

#include	"Include.h"
#include	"GUI_Drv.h"
#include	"x_file.h"


/*=====================================================================*/
// CS   - PD13
// SCLK - PE0
// MOSI - PE2
// MISO - PE3
// IRQ  - PE4

#define	CS_PORT		GPIOD
#define	CS_PIN		GPIO_PIN_13

#define	SCLK_PORT	GPIOE
#define	SCLK_PIN	GPIO_PIN_0

#define	MOSI_PORT	GPIOE
#define	MOSI_PIN	GPIO_PIN_2

#define	MISO_PORT	GPIOE
#define	MISO_PIN	GPIO_PIN_3

#define	IRQ_PORT	GPIOE
#define	IRQ_PIN		GPIO_PIN_4



// CS   - PB0
#define	CS_0()		GPIO_BC(CS_PORT)  |= CS_PIN
#define	CS_1()		GPIO_BOP(CS_PORT) &= CS_PIN

// SCLK - PB14
#define	SCLK_0()	GPIO_BC(SCLK_PORT)  |= SCLK_PIN
#define	SCLK_1()	GPIO_BOP(SCLK_PORT) &= SCLK_PIN

// MOSI - PB1
#define	MOSI_0()	GPIO_BC(MOSI_PORT)  |= MOSI_PIN
#define	MOSI_1()	GPIO_BOP(MOSI_PORT) &= MOSI_PIN

// MISO - PB2
#define	MISO()		((GPIO_ISTAT(MISO_PORT) & MISO_PIN) != 0)

// IRQ  - PF11
#define TP_IRQ()	((GPIO_ISTAT(IRQ_PORT) & IRQ_PIN) != 0)

/*=====================================================================*/

static BOOL	ADS7843_HardInit(void)
{
	rcu_periph_clock_enable(RCU_GPIOD);
	rcu_periph_clock_enable(RCU_GPIOE);

	gpio_init(CS_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, CS_PIN);
	gpio_init(SCLK_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, SCLK_PIN);
	gpio_init(MOSI_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, MOSI_PIN);
	gpio_init(MISO_PORT, GPIO_MODE_IPU, GPIO_OSPEED_50MHZ, MISO_PIN);
	gpio_init(IRQ_PORT, GPIO_MODE_IPU, GPIO_OSPEED_50MHZ, IRQ_PIN);

	//DebugPuts("ADS7843_Init,\r\n");

	return TRUE;
}

/*=====================================================================*/
// A/D ͨ��ѡ�������ֺ͹����Ĵ���
#define	CHX 	0x90 	//ͨ��Y+��ѡ�������	//0x94
#define	CHY 	0xD0	//ͨ��X+��ѡ�������	//0xD4

/*=====================================================================*/

static void  delay_us(int t)
{
	volatile int i;

	while(t-->0)
	{
		for(i=0;i<2;i++);
	}
}

/*=====================================================================*/

static BOOL	ADS7843_IsPenDown(void)
{
	if(!TP_IRQ())
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
		
}

/*=====================================================================*/

static	void ADS7843_WrByte(U8 dat)
{
	int  i, j ;
	
    SCLK_0();
	for( i = 0; i < 8; i++ )
	{
		
		if(dat&0x80)			 //ʱ������������DIN
		{
			MOSI_1();	
		}
		else
		{
			MOSI_0();
		}
		 	
		dat <<=1;
		
		delay_us(1);
		SCLK_0();			//��ʼ����������
		delay_us(1);
		SCLK_1();			//ʱ�����壬һ��8��
		delay_us(1);
		SCLK_0();			//ʱ�����壬һ��8��
		
	}

}




//���뷨����
static	void InsertSort(int *A, int p, int r)
{
    int i,j;
    int key;
    for(i=p+1; i<=r; i++)
    {
        key = A[i];
        j = i-1;
        while (j >= 0 && A[j] > key)
        {
            A[j+1] = A[j];
            j--;
        }
        A[j+1] = key;
    }
}


//�Ӵ�����IC��ȡadcֵ
//CMD:ָ��
//����ֵ:����������
static u16 ADS7843_ReadAD(u8 CMD)
{
	u8 count=0;
	u16 dat=0;

	SCLK_0();		//������ʱ��
	MOSI_0(); 		//����������
	CS_0(); 		//ѡ�д�����IC

	ADS7843_WrByte(CMD);
	delay_us(6);	//ADS7843��ת��ʱ���Ϊ6us

	SCLK_0(); 	 	//��1��ʱ�ӣ����BUSY
	delay_us(1);
	SCLK_1();
	delay_us(1);
	SCLK_0();

	for(count=0;count<16;count++)//����16λ����,ֻ�и�12λ��Ч
	{
		dat <<= 1;

		SCLK_0();	//�½�����Ч
		delay_us(1);
 		SCLK_1();
 		if(MISO())
 		{
 			dat |= 1;
 		}
	}

	dat >>= 4;  //ֻ�и�12λ��Ч.
	CS_1();		//�ͷ�Ƭѡ

	return dat;
}

////////
#define	COUNT	12
static	int ad_dat[COUNT];

static BOOL	ADS7843_GetXY(u16 *X_Addata,u16 *Y_Addata)
{

	U16	i,j,x_dat,y_dat;


//	while( ADS7843_IsBusy());			//���BUSY���ȴ�ֱ��ת����ϣ�������Բ���

	////X
	for(i=0;i<COUNT;i++)
	{
		ad_dat[i] =ADS7843_ReadAD(CHX);
	}

	InsertSort(ad_dat,0,COUNT-1);
	i=COUNT>>1;
	x_dat  = ad_dat[i++];
	x_dat += ad_dat[i];
	x_dat >>= 1;
	
	////Y
	for(i=0;i<COUNT;i++)
	{
		ad_dat[i] =ADS7843_ReadAD(CHY);
	}

	InsertSort(ad_dat,0,COUNT-1);
	i=COUNT>>1;
	y_dat  = ad_dat[i++];
	y_dat += ad_dat[i];
	y_dat >>= 1;

	////
	if(0)
	{
		char buf[40];
		x_sprintf(buf,"ts_ad: %04XH,%04XH.\r\n",x_dat,y_dat);
		DebugPuts(buf);
	}
	
	*X_Addata = x_dat;
	*Y_Addata = y_dat;
	
	return TRUE;

}

/*============================================================================*/

static int ts_x=0,ts_y=0;
static int ts_down=0;

static BOOL Init(void)
{
	ts_down=0;
	ADS7843_HardInit();
	return TRUE;
}

static int GetState(void)
{
	if(ADS7843_IsPenDown())
	{
		ts_down =1;
		return 1;
	}
	ts_down=0;
	return 0;
}

static BOOL GetSample(POINT *pt)
{
	if(ts_down==1)
	{
		u16 x,y;

		ADS7843_GetXY(&x,&y);
		pt->x =x;
		pt->y =y;
		return TRUE;
	}
	return FALSE;
}

static BOOL GetPoint(POINT *pt)
{
	if(GetSample(pt))
	{
#if	GUI_TOUCHSCREEN_CALIBRATE
		TouchPanel_TranslatePoint(LCD_XSIZE,LCD_YSIZE,pt);
#endif
		return TRUE;
	}
	return	TRUE;
}



static BOOL LoadCfg(TS_CFG_DATA *cfg)
{
#if 0
	X_FILE *fp=NULL;
	int i;

	////

	fp=x_fopen("B:ts_cfg.bin","rb");
	if(fp!=NULL)
	{

		i=x_fread((char*)cfg,sizeof(TS_CFG_DATA),1,fp);
		DebugPutHex(0,32,(void*)cfg);

		x_fclose(fp);
		////
		return TRUE;
	}

	cfg->LUx =0x10;
	cfg->LUy =0x10;

	cfg->RUx =0x20;
	cfg->RUy =0x10;

	cfg->RDx =0x20;
	cfg->RDy =0x20;

	cfg->LDx =0x10;
	cfg->LDy =0x20;

	DebugPrintf("ts_load_config Error\r\n");
	return FALSE;

#endif
}

static BOOL SaveCfg(TS_CFG_DATA *cfg)
{
#if 0
	X_FILE *fp;

		////

	DebugPrintf("ts_save_cfg\n");

	fp=x_fopen("B:ts_cfg.bin","wb+");
	if(fp!=NULL)
	{

		x_fwrite((u8*)cfg,sizeof(TS_CFG_DATA),1,fp);
		x_fclose(fp);
		return TRUE;
	}
	////

#endif
	return FALSE;
}

/*============================================================================*/

const GUI_TOUCH_DEV TouchDev_ADS7843=
{
	.Init 		=Init,
	.GetState 	=GetState,
	.GetSample	=GetSample,
	.GetPoint	=GetPoint,
	.LoadCfg	=LoadCfg,
	.SaveCfg	=SaveCfg,
};

/*============================================================================*/

/*=====================================================================*/

