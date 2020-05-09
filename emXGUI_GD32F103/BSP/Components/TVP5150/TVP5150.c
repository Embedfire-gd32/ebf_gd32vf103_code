
#include "BSP.h"
//#include "TVP5150.h"


/*=========================================================================================*/
/*
 TVP5150�ڲ��Ĵ���ͨ��I2C���߽������á�I2C���ߵ�ַ��I2CSEL���ŵĸߵ͵�ƽ������
 I2CSEL=��1����д��ַΪ0xBA��I2CSEL=��0����д��ַΪ0xB8��

 TVP5150оƬֻ��8λ�������ߣ��������ʽ�����֣�8-bit 4:2:2��ʽ��8-bit ITU-R BT.656��ʽ��

   ���ָ�ʽ������8-bit 4:2:2��ʽ��Ҫ����С���ͬ���źţ�����������ֻ��ͼ�����ݣ�
 ITU-R BT.656��ʽ����Ҫ����С���ͬ���źţ��ø�ʽ���С�����ʼ���������ͨ��Ƕ�뵽ͼ�������еı�־����ָʾ�ģ���˸ø�ʽ�ֳ�ΪǶ��ͬ����ʽ��

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
	if(!pIIC->WriteByte(TVP_WT_ADDR,IIC_ACK))//����������ַ+д����
	{
		pIIC->Stop();

		pIIC->Close();
		return FALSE;
	}
	pIIC->WriteByte((reg&0xFF),IIC_ACK);//д�Ĵ�����ַL
	pIIC->WriteByte(val&0XFF,IIC_ACK);	//��������

	pIIC->Stop();

	pIIC->Close();
	return TRUE;

}

static u8 read_reg(u8 reg)
{
	u8 val=0;

	pIIC->Open();

	pIIC->Start();
	if(!pIIC->WriteByte(TVP_WT_ADDR,IIC_ACK))	//����д����
	{
		goto exit;
	}

	pIIC->WriteByte(reg&0XFF,IIC_ACK);   	//���͵�8λ��ַ

	pIIC->Start();
	pIIC->WriteByte(TVP_RD_ADDR,IIC_ACK);   	//���Ͷ�����

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

	//���ȵ���
	write_reg(0x09,0x80);

	//ɫ�ʱ��Ͷ�
	write_reg(0x0A,0xD0);

	//ɫ�ʵ���,�������
	write_reg(0x0B,0x04);

	//�Աȶȵ���
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


