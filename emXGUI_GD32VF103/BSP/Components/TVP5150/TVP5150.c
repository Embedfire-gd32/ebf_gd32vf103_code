
#include "BSP.h"
//#include "TVP5150.h"


/*=========================================================================================*/
/*
 TVP5150内部寄存器通过I2C总线进行设置。I2C总线地址由I2CSEL引脚的高低电平决定。
 I2CSEL=‘1’，写地址为0xBA；I2CSEL=‘0’，写地址为0xB8。

 TVP5150芯片只有8位数据总线，其输出格式有两种：8-bit 4:2:2格式和8-bit ITU-R BT.656格式。

   两种格式的区别：8-bit 4:2:2格式需要输出行、场同步信号，数据总线上只有图像数据；
 ITU-R BT.656格式不需要输出行、场同步信号，该格式的行、场起始与结束都是通过嵌入到图像数据中的标志码来指示的，因此该格式又称为嵌入同步方式。

 */
/*=========================================================================================*/

#define	TVP_WT_ADDR	0xBA
#define	TVP_RD_ADDR	0xBB

static const struct i2c_ops *pIIC=NULL;



/*=========================================================================================*/

static BOOL write_reg(u8 reg,u8 val)
{
	pIIC->Open();

	pIIC->Start();
	if(!pIIC->WriteByte(TVP_WT_ADDR,IIC_ACK))//发送器件地址+写命令
	{
		pIIC->Stop();

		pIIC->Close();
		return FALSE;
	}
	pIIC->WriteByte((reg&0xFF),IIC_ACK);//写寄存器地址L
	pIIC->WriteByte(val&0XFF,IIC_ACK);	//发送数据

	pIIC->Stop();

	pIIC->Close();
	return TRUE;

}

static u8 read_reg(u8 reg)
{
	u8 val=0;

	pIIC->Open();

	pIIC->Start();
	if(!pIIC->WriteByte(TVP_WT_ADDR,IIC_ACK))	//发送写命令
	{
		goto exit;
	}

	pIIC->WriteByte(reg&0XFF,IIC_ACK);   	//发送低8位地址

	pIIC->Start();
	pIIC->WriteByte(TVP_RD_ADDR,IIC_ACK);   	//发送读命令

	val =pIIC->ReadByte(IIC_NoACK);

exit:
    pIIC->Stop();

    pIIC->Close();
    return val;
}



/*=========================================================================================*/

BOOL TVP5150_Init(void)
{
	write_reg(0x00,0x01);
	write_reg(0x00,0x00);

	write_reg(0x03,0x6F);
	write_reg(0x04,0xC0);
	write_reg(0x0D,0xC0);

	//亮度调节
	write_reg(0x09,0x80);

	//色彩饱和度
	write_reg(0x0A,0xD0);

	//色彩调节,增大变绿
	write_reg(0x0B,0x04);

	//对比度调节
	write_reg(0x0C,0xB0);
	write_reg(0x0E,0x03);
	write_reg(0x1B,0x13);

    return TRUE;
}

BOOL	TVP5150_Mount(const struct i2c_ops *iic)
{
	pIIC =iic;
	return TRUE;
}

/*=========================================================================================*/


