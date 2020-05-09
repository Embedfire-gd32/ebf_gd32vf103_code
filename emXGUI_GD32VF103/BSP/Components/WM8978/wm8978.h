#ifndef __WM8978_H
#define __WM8978_H

#include "BSP.h"

#pragma pack(1)
typedef struct
{							  
	u8 mvol;		//����(����������һ���),��Χ:0~63
	u8 cfreq[5];	//����Ƶ��ѡ������,ÿ��Ԫ�ؿ�ѡ��ΧΪ0~3,�Ҵ����Ƶ�ʸ�����ͬ 
					//[0]��ӦƵ��:80,105,135,175
					//[1]��ӦƵ��:230,300,385,500
					//[2]��ӦƵ��:650,850,1100,1400
					//[3]��ӦƵ��:1800,2400,3200,4100
					//[4]��ӦƵ��:5300,6900,9000,11700
	u8 freqval[5];	//����Ƶ�������
	u8 d3;			//3d����	
	u8 speakersw;	//�������ȿ���,0,�ر�;1,�� 
	u8 saveflag; 	//�����־,0X0A,�������;����,����δ����	   
}_wm8978_obj;

#pragma pack()

extern _wm8978_obj wm8978set;//WM8978������
 
//���AD0��(4��)�ӵ�,IIC��ַΪ0X4A(���������λ).
//�����V3.3,��IIC��ַΪ0X4B(���������λ).
#define WM8978_ADDR				0X1A	//WM8978��������ַ,�̶�Ϊ0X1A 
 
#define EQ1_80Hz		0X00
#define EQ1_105Hz		0X01
#define EQ1_135Hz		0X02
#define EQ1_175Hz		0X03

#define EQ2_230Hz		0X00
#define EQ2_300Hz		0X01
#define EQ2_385Hz		0X02
#define EQ2_500Hz		0X03

#define EQ3_650Hz		0X00
#define EQ3_850Hz		0X01
#define EQ3_1100Hz		0X02
#define EQ3_14000Hz		0X03

#define EQ4_1800Hz		0X00
#define EQ4_2400Hz		0X01
#define EQ4_3200Hz		0X02
#define EQ4_4100Hz		0X03

#define EQ5_5300Hz		0X00
#define EQ5_6900Hz		0X01
#define EQ5_9000Hz		0X02
#define EQ5_11700Hz		0X03

//// WM8978 format
#define	WM8978_LSB			0
#define	WM8978_MSB			1
#define	WM8978_Phlips_I2S	2
#define	WM8978_PCM			3

//// WM8978 data width
#define	WM8978_16bit	0
#define	WM8978_20bit	1
#define	WM8978_24bit	2
#define	WM8978_32bit	3

void wm8978_read_para(_wm8978_obj * wm8978dev);
void wm8978_save_para(_wm8978_obj * wm8978dev);

BOOL WM8978_Init(const struct i2c_ops *iic);

void WM8978_ADDA_Cfg(u8 dacen,u8 adcen);
void WM8978_Input_Cfg(u8 micen,u8 lineinen,u8 auxen);
void WM8978_Output_Cfg(u8 dacen,u8 bpsen);
void WM8978_MIC_Gain(u8 gain);
void WM8978_LINEIN_Gain(u8 gain);
void WM8978_AUX_Gain(u8 gain);
//u8 WM8978_Write_Reg(u8 reg,u16 val);
//u16 WM8978_Read_Reg(u8 reg);
void WM8978_HPvol_Set(u8 voll,u8 volr);
void WM8978_SPKvol_Set(u8 volx);
void WM8978_I2S_Cfg(u8 fmt,u8 len);
void WM8978_3D_Set(u8 depth);
void WM8978_EQ_3D_Dir(u8 dir); 
void WM8978_EQ1_Set(u8 cfreq,u8 gain); 
void WM8978_EQ2_Set(u8 cfreq,u8 gain);
void WM8978_EQ3_Set(u8 cfreq,u8 gain);
void WM8978_EQ4_Set(u8 cfreq,u8 gain);
void WM8978_EQ5_Set(u8 cfreq,u8 gain);

#endif





















