
#include "BSP.h"
//#include "NT99141.h"


/*=========================================================================================*/

#define	NT_WT_ADDR	0x54
#define	NT_RD_ADDR	0x55

static const struct i2c_ops *pIIC=NULL;

/*=========================================================================================*/

struct NT_RegValue
{
    u16    u16RegAddr;            /* Sensor Register Address */
    u8     u8Value;            /* Sensor Register Data */
};

/* NT99141, VGA, YUV422 */
#define NT99141_YUV422
//#define NT99141_RGB565

static struct NT_RegValue g_sNT99141_VGA_RegValue[]= {
    //{0x3021, 0x60},
#if 1
    /* [Inti] */
    {0x3109, 0x04},{0x3040, 0x04},{0x3041, 0x02},{0x3042, 0xFF},{0x3043, 0x08},
    {0x3052, 0xE0},{0x305F, 0x33},{0x3100, 0x07},{0x3106, 0x03},
    {0x3108, 0x00},{0x3110, 0x22},{0x3111, 0x57},{0x3112, 0x22},{0x3113, 0x55},
    {0x3114, 0x05},{0x3135, 0x00},{0x32F0, 0x01},{0x306a,0x01},
    // Initial AWB Gain */
    {0x3290, 0x01},{0x3291, 0x80},{0x3296, 0x01},{0x3297, 0x73},
    /* CA Ratio */
    {0x3250, 0x80},{0x3251, 0x03},{0x3252, 0xFF},{0x3253, 0x00},{0x3254, 0x03},
    {0x3255, 0xFF},{0x3256, 0x00},{0x3257, 0x50},
    /* Gamma */
    {0x3270, 0x00},{0x3271, 0x0C},{0x3272, 0x18},{0x3273, 0x32},{0x3274, 0x44},
    {0x3275, 0x54},{0x3276, 0x70},{0x3277, 0x88},{0x3278, 0x9D},{0x3279, 0xB0},
    {0x327A, 0xCF},{0x327B, 0xE2},{0x327C, 0xEF},{0x327D, 0xF7},{0x327E, 0xFF},
    /* Color Correction */
    {0x3302, 0x00},{0x3303, 0x40},{0x3304, 0x00},{0x3305, 0x96},{0x3306, 0x00},
    {0x3307, 0x29},{0x3308, 0x07},{0x3309, 0xBA},{0x330A, 0x06},{0x330B, 0xF5},
    {0x330C, 0x01},{0x330D, 0x51},{0x330E, 0x01},{0x330F, 0x30},{0x3310, 0x07},
    {0x3311, 0x16},{0x3312, 0x07},{0x3313, 0xBA},
    /* EExt */
    {0x3326, 0x02},{0x32F6, 0x0F},{0x32F9, 0x42},{0x32FA, 0x24},{0x3325, 0x4A},
    {0x3330, 0x00},{0x3331, 0x0A},{0x3332, 0xFF},{0x3338, 0x30},{0x3339, 0x84},
    {0x333A, 0x48},{0x333F, 0x07},
    /* Auto Function */
    {0x3360, 0x10},{0x3361, 0x18},{0x3362, 0x1f},{0x3363, 0x37},{0x3364, 0x80},
    {0x3365, 0x80},{0x3366, 0x68},{0x3367, 0x60},{0x3368, 0x30},{0x3369, 0x28},
    {0x336A, 0x20},{0x336B, 0x10},{0x336C, 0x00},{0x336D, 0x20},{0x336E, 0x1C},
    {0x336F, 0x18},{0x3370, 0x10},{0x3371, 0x38},{0x3372, 0x3C},{0x3373, 0x3F},
    {0x3374, 0x3F},{0x338A, 0x34},{0x338B, 0x7F},{0x338C, 0x10},{0x338D, 0x23},
    {0x338E, 0x7F},{0x338F, 0x14},{0x3375, 0x0A},{0x3376, 0x0C},{0x3377, 0x10},
    {0x3378, 0x14},
    {0x3012, 0x02},{0x3013, 0xD0},{0x3060, 0x01},
#endif
    /* MCLK:12M hz, PCLK:12M hz, FPS:30 [YUYV_640x480_30Fps] */
    {0x32BF, 0x60},{0x32C0, 0x5A},{0x32C1, 0x5A},{0x32C2, 0x5A},{0x32C3, 0x00},
    {0x32C4, 0x20},{0x32C5, 0x20},{0x32C6, 0x20},{0x32C7, 0x00},{0x32C8, 0xDD},
    {0x32C9, 0x5A},{0x32CA, 0x7A},{0x32CB, 0x7A},{0x32CC, 0x7A},{0x32CD, 0x7A},
    {0x32DB, 0x7B},{0x32E0, 0x02},{0x32E1, 0x80},{0x32E2, 0x01},{0x32E3, 0xE0},
    {0x32E4, 0x00},{0x32E5, 0x80},{0x32E6, 0x00},{0x32E7, 0x80},{0x3200, 0x3E},
    {0x3201, 0x0F},{0x3028, 0x24},{0x3029, 0x20},{0x302A, 0x10},{0x3022, 0x25},
    {0x3023, 0x24},{0x3002, 0x00},{0x3003, 0xA4},{0x3004, 0x00},{0x3005, 0x04},
    {0x3006, 0x04},{0x3007, 0x63},{0x3008, 0x02},{0x3009, 0xD3},{0x300A, 0x06},
    {0x300B, 0x8B},{0x300C, 0x02},{0x300D, 0xE0},{0x300E, 0x03},{0x300F, 0xC0},
    {0x3010, 0x02},{0x3011, 0xD0},{0x32B8, 0x3F},{0x32B9, 0x31},{0x32BB, 0x87},
    {0x32BC, 0x38},{0x32BD, 0x3C},{0x32BE, 0x34},{0x3201, 0x7F},{0x3021, 0x06},
    {0x3060, 0x01},
};

static void Delay(u32 nCount)
{
    volatile u32 i;
    for(; nCount!=0; nCount--)
    {
        for(i=0; i<200; i++);
    }
}

static void SnrReset(void)
{
#if 0
    /* GPIOI7 reset:    H->L->H     */
    outpw(REG_SYS_GPI_MFPL,(inpw(REG_SYS_GPI_MFPL) & ~0xF0000000));
    outpw((GPIO_BA+0x200),(inpw(GPIO_BA+0x200) | 0x0080)); /* GPIOI7 Output mode */
    outpw((GPIO_BA+0x204),(inpw(GPIO_BA+0x204) | 0x0080)); /* GPIOI7 Output to high */
    Delay(100);
    outpw((GPIO_BA+0x204),(inpw(GPIO_BA+0x204) & ~0x0080)); /* GPIOI7 Output to low */
    Delay(100);
    outpw((GPIO_BA+0x204),(inpw(GPIO_BA+0x204) | 0x0080)); /* GPIOI7 Output to high */
#endif
}

static void SnrPowerDown(BOOL bIsEnable)
{
#if 0
    /* GPI0 power down, HIGH for power down */
    outpw( REG_SYS_GPI_MFPL,(inpw(REG_SYS_GPI_MFPL) & ~0x0000000F));
    outpw((GPIO_BA+0x200),(inpw(GPIO_BA+0x200) | 0x0001)); /* GPIOI0 Output mode */
    outpw((GPIO_BA+0x204),(inpw(GPIO_BA+0x204) &~ 0x0001)); /* GPIOI0 Output to low */

    if(bIsEnable)
    {
        outpw((GPIO_BA+0x204),(inpw(GPIO_BA+0x204) | 0x0001)); /* GPIOI0 Output to high */
    }
    else
    {
        outpw((GPIO_BA+0x204),(inpw(GPIO_BA+0x204) &~ 0x0001)); /* GPIOI0 Output to low */
    }
#endif
}

/*=========================================================================================*/

static BOOL write_reg(u16 reg,u8 val)
{
	pIIC->Open();

	pIIC->Start();
	if(!pIIC->WriteByte(NT_WT_ADDR,IIC_ACK))//·¢ËÍÆ÷¼þµØÖ·+Ð´ÃüÁî
	{
		pIIC->Stop();

		pIIC->Close();
		return FALSE;
	}
	pIIC->WriteByte((reg>>8),IIC_ACK);//Ð´¼Ä´æÆ÷µØÖ·H
	pIIC->WriteByte((reg&0xFF),IIC_ACK);//Ð´¼Ä´æÆ÷µØÖ·L
	pIIC->WriteByte(val&0XFF,IIC_ACK);	//·¢ËÍÊý¾Ý

	pIIC->Stop();

	pIIC->Close();
	return TRUE;

}

static u8 read_reg(u16 reg)
{
	u8 val=0;

	pIIC->Open();

	pIIC->Start();
	if(!pIIC->WriteByte(NT_WT_ADDR,IIC_ACK))	//·¢ËÍÐ´ÃüÁî
	{
		goto exit;
	}

	pIIC->WriteByte(reg>>8,IIC_ACK);   		//·¢ËÍµÍ8Î»µØÖ·
	pIIC->WriteByte(reg&0XFF,IIC_ACK);   	//·¢ËÍµÍ8Î»µØÖ·

	pIIC->Start();
	pIIC->WriteByte(NT_RD_ADDR,IIC_ACK);   	//·¢ËÍ¶ÁÃüÁî

	val =pIIC->ReadByte(IIC_NoACK);

exit:
    pIIC->Stop();

    pIIC->Close();
    return val;
}

#if 0
static BOOL write_reg(u16 reg,u8 val)
{
    // 3-Phase(ID address, register address, data(8bits)) write transmission
    volatile u8 u32Delay = 0x50;
    BOOL res=FALSE;

    SWI2C_SendStart();
    while(u32Delay--);

    if(!SWI2C_WriteByte(NT_WT_ADDR,I2C_Ack)) 	goto exit;
    if(!SWI2C_WriteByte((reg>>8),I2C_Ack)) 	goto exit;
    if(!SWI2C_WriteByte((reg&0xFF),I2C_Ack)) 	goto exit;
    if(!SWI2C_WriteByte(val,I2C_Ack)) 		goto exit;
exit:
	SWI2C_SendStop();
	return res;
}

static u8 read_reg(u16 reg)
{

    volatile u8 u32Delay = 0x50;
    u8 val=0;

    SWI2C_SendStart();
    while(u32Delay--);

    if(!SWI2C_WriteByte(NT_WT_ADDR,I2C_Ack)) 	goto exit;
    if(!SWI2C_WriteByte((reg>>8),I2C_Ack)) 		goto exit;
    if(!SWI2C_WriteByte((reg&0xFF),I2C_Ack)) 	goto exit;

    SWI2C_SendStart();
    SWI2C_WriteByte(NT_RD_ADDR,I2C_Ack);
    val=SWI2C_ReadByte(I2C_NoAck);

exit:
	SWI2C_SendStop();
	return val;

}
#endif

/*=========================================================================================*/

#define	Reset_Register	0x3021
#define	XAddr_Start_H	0x3002
#define	XAddr_Start_L	0x3003
#define	YAddr_Start_H	0x3004
#define	YAddr_Start_L	0x3005
#define	XAddr_End_H		0x3006
#define	XAddr_End_L		0x3007
#define	YAddr_End_H		0x3008
#define	YAddr_End_L		0x3009
#define	Line_Length_Pck_H	0x300A
#define	Line_Length_Pck_L	0x300B
#define	Frame_Leggth_Line_H	0x300C
#define	Frame_Leggth_Line_L	0x300D
#define	X_Output_Size_H		0x300E
#define	X_Output_Size_L		0x300F
#define	Y_Output_Size_H		0x3010
#define	Y_Output_Size_L		0x3011
#define	Mode_Control_0		0x3200
#define	Mode_Control_1		0x3201
#define	Read_Mode_0			0x3022
#define	Read_Mode_1			0x3023
#define	Read_Mode_2			0x3024
#define	Scaler_OutSizeX_H	0x32E0
#define	Scaler_OutSizeX_L	0x32E1
#define	Scaler_OutSizeY_H	0x32E2
#define	Scaler_OutSizeY_L	0x32E3


#define	Output_Format		0x32F0 //reg
//Output format
#define	YCbCr		(0<<4)
#define	RGB565		(1<<4)
#define	RGB555		(2<<4)
#define	RGB444x		(3<<4)
#define	RGBx444		(4<<4)
#define	RAW_10Bits	(5<<4)
#define	CCIR656		(6<<4)
#define	JPEG		(7<<4)

#define	RB_Swap		(1<<1)
#define	BYTE_Swap	(1<<0)


void NT99141_Set_OutSize(u16 w,u16 h)
{
	u8 dev_addr=0x54;

	u8 val;

//	val =read_reg(Mode_Control_1);
//	val |= (1<<0); //Scaling_Down Disable
//	write_reg(Mode_Control_1,val);


	////
	write_reg(XAddr_Start_H,2>>8);
	write_reg(XAddr_Start_L,2&0xFF);
	write_reg(XAddr_End_H,(2+1280)>>8);
	write_reg(XAddr_End_L,(2+1280)&0xFF);

	write_reg(YAddr_Start_H,2>>8);
	write_reg(YAddr_Start_L,2&0xFF);
	write_reg(YAddr_End_H,(2+720)>>8);
	write_reg(YAddr_End_L,(2+720)&0xFF);

	////OutputSize
	write_reg(X_Output_Size_H,1280>>8);
	write_reg(X_Output_Size_L,1280&0x00FF);
	write_reg(Y_Output_Size_H,720>>8);
	write_reg(Y_Output_Size_L,720&0x00FF);

	////ScaleSize(1:1)
	write_reg(Scaler_OutSizeX_H,w>>8);
	write_reg(Scaler_OutSizeX_L,w&0x00FF);
	write_reg(Scaler_OutSizeY_H,h>>8);
	write_reg(Scaler_OutSizeY_L,h&0x00FF);

}

/*=========================================================================================*/

BOOL NT99141_Init(void)
{
    u32 i;
    u8 u8DeviceID=0x54;
    u8 u8ID[2]= {0};
    u8 val;


    SnrReset();
    SnrPowerDown(FALSE);
    Delay(100);

    //sysprintf("NT_RegNum=%3d\n",sizeof(g_sNT99141_VGA_RegValue)/sizeof(struct NT_RegValue));
    for(i=0; i<sizeof(g_sNT99141_VGA_RegValue)/sizeof(struct NT_RegValue); i++)
    {
    	write_reg(g_sNT99141_VGA_RegValue[i].u16RegAddr,g_sNT99141_VGA_RegValue[i].u8Value);
    }


    u8ID[0]=read_reg(0x3000);  /* Chip_Version_H 0x14 */
    u8ID[1]=read_reg(0x3001);  /* Chip_Version_L 0x10 */
    //sysprintf("Sensor Chip_Version_H = 0x%02x(0x14) Chip_Version_L = 0x%02x(0x10)\n", u8ID[0],u8ID[1]);
    if(u8ID[0]!=0x14 || u8ID[1]!=0x10 )
    {
        //sysprintf("NT99141 init failed!!\n");
        return FALSE;
    }

	write_reg(Output_Format,YCbCr|BYTE_Swap);

    write_reg(Mode_Control_1,
    		 (1<<6) //Scaling_Down_En
			|(1<<5) //AE_En
			|(1<<4) //AWB_En
			|(1<<3) //Noise_Reduction_En
			|(1<<2) //Edge_Enhance_En
			|(1<<1) //Color_Correct_En
			|(1<<0) //Special_Effect_En
			);

    write_reg(Read_Mode_0,
    		 (1<<5) //Y_Even_Inc
    		|(1<<2) //X_Even_Inc
			|(1<<1) //Mirror_Horizontal
			|(1<<0) //Flip_Vertical
			);

	write_reg(Read_Mode_1,
			 (1<<5) //Y_Odd_Inc
			|(1<<2) //X_Odd_Inc
			|(0<<0) //X_Bin_En
			);


    return TRUE;
}



void NT99141_Start(void)
{
	/* Enable GPIO Clock */
//	outpw(REG_CLK_PCLKEN0,inpw(REG_CLK_PCLKEN0)|(1<<3));
	/* switch I2C pin function, to do... */
//	SWI2C_Open(eDRVGPIO_GPIOB,eDRVGPIO_PIN0,eDRVGPIO_GPIOB,eDRVGPIO_PIN1,Delay);

	if(!NT99141_Init())
	{

	}

	NT99141_Set_OutSize(640,480);
}

