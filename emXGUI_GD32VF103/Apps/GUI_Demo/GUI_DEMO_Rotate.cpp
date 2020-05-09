
/*
 *GUI_DEMO_Rotate.cpp
 *2019年3月14日下午10:07:54
 *
 */


#include <string.h>
#include <stdlib.h>
#include <math.h>

#include "emXGUI.h"
#include "emXGUI_Arch.h"
//#include "Kernel.h"
#include "x_libc.h"

typedef	u8	BYTE;

/*============================================================================*/
//定义控件ID
#define	ID_OK		0x1000

static volatile int m0,m1;
static volatile u32 t0,t1;

extern "C"
{

extern const unsigned char gImage_0[];


U32 GUI_GetTickCount();

void X_RoateBitmapDraw(HDC hdc,int cx,int cy,const BITMAP *bm,float theta);
void	gdSetPixelARGB(HDC hdc,int x,int y,U8 a,U8 r,U8 g,U8 b);
}

/*============================================================================*/
#if 0

//#define	Samstride 500

static void BilinearInterpolation(float xPos,float yPos, u8 *src,int src_pitch,const int channels, u8 *dst,const int index)
{
	//双线性插值计算
	int x_floor = xPos;//向下取整
    int y_floor = yPos;//向下取整
/*
	if(x_floor < 0 || x_floor > src_w)
	{
		dst[index] =0;
		return;
	}
	if(y_floor < 0 || y_floor > src_h)
	{
		dst[index] =0;
		return;
	}
*/
	//dst[index] = src[y_floor*src_pitch+x_floor];
	//return;

	int PartX = (xPos - x_floor)*2048;
	int PartY = (yPos - y_floor)*2048;

	int InvX = 2048 - PartX;
	int InvY = 2048 - PartY;

	int TL = y_floor*src_pitch + x_floor*channels;//计算取样点左上角邻近的那个像素点的内存地址索引号
	int BL = TL + src_pitch;//左下角像素点地址

#if 1
	//A
	int Ar1 = InvX*src[BL] + PartX*src[BL+1];
	int Ar2 = InvX*src[TL] + PartX*src[TL+1];
	dst[index] = (PartY*Ar1 + InvY*Ar2)>>22;
#endif
#if 1
	//R
	int Br1 = InvX*src[BL] + PartX*src[BL+1];
	int Br2 = InvX*src[TL] + PartX*src[TL+1];
	dst[index+1] = (PartY*Br1 + InvY*Br2)>>22;
#endif

#if 0
	//G
	int Gr1 = InvX*pdfValue[BL+1] + PartX*pdfValue[BL+4];
	int Gr2 = InvX*pdfValue[TL+1] + PartX*pdfValue[TL+4];
	dstValue[index+2] = (PartY*Gr1 + InvY*Gr2)>>22;

	//B
	int Rr1 = InvX*pdfValue[BL+2] + PartX*pdfValue[BL+5];
	int Rr2 = InvX*pdfValue[TL+2] + PartX*pdfValue[TL+5];
	dstValue[index+3] = (PartY*Rr1 + InvY*Rr2)>>22;
#endif
}


static void BilinearInterpolation_4x4(float ex,float ey,u8 *src,u8 *out,u8 mask)
{

	int PartX = ex*2048;
	int PartY = ey*2048;

	int InvX = 2048 - PartX;
	int InvY = 2048 - PartY;

	int TL = 0;//计算取样点左上角邻近的那个像素点的内存地址索引号.
	int BL = 8;//左下角像素点地址.

#if 1
	if(mask&(1<<3))
	{
	//A
	int Ar1 = InvX*src[BL+0] + PartX*src[BL+4];
	int Ar2 = InvX*src[TL+0] + PartX*src[TL+4];
	out[0] = (PartY*Ar1 + InvY*Ar2)>>22;
	}
#endif

#if 1
	if(mask&(1<<2))
	{
	//R
	int Rr1 = InvX*src[BL+1] + PartX*src[BL+5];
	int Rr2 = InvX*src[TL+1] + PartX*src[TL+5];
	out[1] = (PartY*Rr1 + InvY*Rr2)>>22;
	}
#endif

#if 1
	if(mask&(1<<1))
	{
	//G
	int Gr1 = InvX*src[BL+2] + PartX*src[BL+6];
	int Gr2 = InvX*src[TL+2] + PartX*src[TL+6];
	out[2] = (PartY*Gr1 + InvY*Gr2)>>22;
	}
#endif

#if 1
	if(mask&(1<<0))
	{
	//B
	int Br1 = InvX*src[BL+3] + PartX*src[BL+7];
	int Br2 = InvX*src[TL+3] + PartX*src[TL+7];
	out[3] = (PartY*Br1 + InvY*Br2)>>22;
	}
#endif
}
#endif

#if 0
static void BilinearInterpolation_RGB(float ex,float ey,u8 *src,u8 *out)
{

	int PartX = ex*2048;
	int PartY = ey*2048;

	int InvX = 2048 - PartX;
	int InvY = 2048 - PartY;

	int TL = 0;//计算取样点左上角邻近的那个像素点的内存地址索引号.
	int BL = 8;//左下角像素点地址.


#if 1
	//R
	int Rr1 = InvX*src[BL+1] + PartX*src[BL+5];
	int Rr2 = InvX*src[TL+1] + PartX*src[TL+5];
	out[1] = (PartY*Rr1 + InvY*Rr2)>>22;
#endif

#if 1
	//G
	int Gr1 = InvX*src[BL+2] + PartX*src[BL+6];
	int Gr2 = InvX*src[TL+2] + PartX*src[TL+6];
	out[2] = (PartY*Gr1 + InvY*Gr2)>>22;
#endif

#if 1
	//B
	int Br1 = InvX*src[BL+3] + PartX*src[BL+7];
	int Br2 = InvX*src[TL+3] + PartX*src[TL+7];
	out[3] = (PartY*Br1 + InvY*Br2)>>22;
#endif
}
#endif

static void MakeBilinearAL8(float x,float y, u8 *src,int src_pitch, u8 *dst,const int index)
{
	//双线性插值计算
	int x_floor = x;//向下取整
    int y_floor = y;//向下取整


	int PartX = (x - x_floor)*2048;
	int PartY = (y - y_floor)*2048;

	int InvX = 2048 - PartX;
	int InvY = 2048 - PartY;

	int TL = y_floor*src_pitch + (x_floor);//计算取样点左上角邻近的那个像素点的内存地址索引号
	int BL = TL + src_pitch;//左下角像素点地址

#if 1
	//A
	int Ar1 = InvX*src[BL+0] + PartX*src[BL+1];
	int Ar2 = InvX*src[TL+0] + PartX*src[TL+1];
	dst[index] = (PartY*Ar1 + InvY*Ar2)>>22;

#endif

}

static void MakeBilinearRGB888(float xPos,float yPos, u8 *src,int src_pitch, u8 *out,const int index)
{
	//双线性插值计算
	int x_floor = xPos;//向下取整
    int y_floor = yPos;//向下取整


	int PartX = (xPos - x_floor)*2048;
	int PartY = (yPos - y_floor)*2048;

	int InvX = 2048 - PartX;
	int InvY = 2048 - PartY;

	int TL = y_floor*src_pitch + (x_floor*3);//计算取样点左上角邻近的那个像素点的内存地址索引号
	int BL = TL + src_pitch;//左下角像素点地址

#if 1
	//R
	int Rr1 = InvX*src[BL+0] + PartX*src[BL+3];
	int Rr2 = InvX*src[TL+0] + PartX*src[TL+3];
	out[index+0] = (PartY*Rr1 + InvY*Rr2)>>22;
#endif

#if 1
	//G
	int Gr1 = InvX*src[BL+1] + PartX*src[BL+4];
	int Gr2 = InvX*src[TL+1] + PartX*src[TL+4];
	out[index+1] = (PartY*Gr1 + InvY*Gr2)>>22;
#endif

#if 1
	//B
	int Br1 = InvX*src[BL+2] + PartX*src[BL+6];
	int Br2 = InvX*src[TL+2] + PartX*src[TL+6];
	out[index+2] = (PartY*Br1 + InvY*Br2)>>22;
#endif
}

static void MakeBilinearARGB8888(float xPos,float yPos, u8 *src,int src_pitch, u8 *out,const int index)
{
	//双线性插值计算.
	int x_floor = xPos;//向下取整.
    int y_floor = yPos;//向下取整.


	int PartX = (xPos - x_floor)*2048;
	int PartY = (yPos - y_floor)*2048;

	int InvX = 2048 - PartX;
	int InvY = 2048 - PartY;

	int TL = y_floor*src_pitch + (x_floor*4);//计算取样点左上角邻近的那个像素点的内存地址索引号.
	int BL = TL + src_pitch;//左下角像素点地址.

#if 1
	//A
	int Ar1 = InvX*src[BL+0] + PartX*src[BL+4];
	int Ar2 = InvX*src[TL+0] + PartX*src[TL+4];
	out[index+0] = (PartY*Ar1 + InvY*Ar2)>>22;

#endif

#if 1
	//R
	int Rr1 = InvX*src[BL+1] + PartX*src[BL+5];
	int Rr2 = InvX*src[TL+1] + PartX*src[TL+5];
	out[index+1] = (PartY*Rr1 + InvY*Rr2)>>22;
#endif

#if 1
	//G
	int Gr1 = InvX*src[BL+2] + PartX*src[BL+6];
	int Gr2 = InvX*src[TL+2] + PartX*src[TL+6];
	out[index+2] = (PartY*Gr1 + InvY*Gr2)>>22;
#endif

#if 1
	//B
	int Br1 = InvX*src[BL+3] + PartX*src[BL+7];
	int Br2 = InvX*src[TL+3] + PartX*src[TL+7];
	out[index+3] = (PartY*Br1 + InvY*Br2)>>22;
#endif
}

static float __f32_add(float a,float b)
{
	return a+b;
}

static float __f32_sub(float a,float b)
{
	return a-b;
}

static float __f32_mul(float a,float b)
{
	return a*b;
}

static float __f32_mula(float a,float b,float c)
{
	return a*b+c;
}

static float __f32_div(float a,float b)
{
	return a/b;
}

//arm_mult_f32;
//代码2 原始的反向映射算法
BOOL X_RoateBitmapDC(HDC hdc,int cx,int cy,BYTE src[],int src_w,int src_h,int src_pitch,float theta,int *neww,int *newh,int format)
{
	float fsin, fcos;
	float x_fsin,x_fcos;
	float y_fsin,y_fcos;
	float fx,fy,c1,c2;
//	float scale;
	int x,y,idx,channels;
	int dst_w,dst_h,dst_pitch;

	switch(format)
	{
	case BM_ARGB8888:
		channels =4;
		break;
	case BM_RGB888:
		channels =3;
		break;
	case BM_AL8:
		channels =1;
		break;
	default: //No Suport.
		return FALSE;
	}

//	scale =1.5;
	//1. 计算基本参数
	fsin = sin(theta);
	fcos = cos(theta);

	dst_h = ceilf(abs(src_h*fcos) + abs(src_w*fsin));
	dst_w = ceilf(abs(src_w*fcos) + abs(src_h*fsin));

	dst_pitch =dst_w*channels;
	if(dst_pitch&0x03)
	{
		dst_pitch =(dst_pitch&0xFFFFFFFC) + 4;
	}

	*neww =dst_w;
	*newh =dst_h;

/*
	BYTE *dst = new BYTE[dst_h*dst_pitch];
	if (!dst)
	{
		return NULL;
	}
	memset(dst, 0, dst_h*dst_pitch);
*/

	c1 = (src_w - dst_w*fcos - dst_h*fsin) / 2;
	c2 = (src_h + dst_w*fsin - dst_h*fcos) / 2;

	//c1 =cx*fcos;
	//c2 =cy*fsin;

	y_fsin =c1;
	y_fcos =c2;

	//2. 计算反向坐标并计算插值
	for (y = 0; y < dst_h; y++)
	{
		int idx,xx,yy;

		idx =y*dst_pitch;
		x_fsin =0;
		x_fcos =0;

		for (x = 0; x < dst_w; x++)
		{
			//计算后向映射点的精确位置 每个点都使用原始公式计算
			/*
			fx = x*fcos + y*fsin + c1;
			fy = y*fcos - x*fsin + c2;
			*/

			fx = x_fcos + y_fsin;// + c1;
			fy = y_fcos - x_fsin;// + c2;

			xx =(int)fx;
			yy =(int)fy;
/*

			for(int i=0;i<nbytes;i++)
			{
				dst[i] =0;
			}
*/

			if(xx >= 0 && yy >= 0)
			if(xx < src_w && yy < src_h)
			{

				u8 out[4];
				idx =0;

				switch(format)
				{

				case BM_ARGB8888:
					MakeBilinearARGB8888(fx,fy,src,src_pitch,out,idx);
					gdSetPixelARGB(hdc,x+cx,y+cy,out[3],out[2],out[1],out[0]);
					break;

				case BM_AL8:
					MakeBilinearAL8(fx,fy,src,src_pitch,out,idx);
					break;
				}

			}
			else
			{
				//gdSetPixelARGB(hdc,xx,yy,0,0,0,0);
			}

			idx += channels;

			x_fsin += fsin;
			x_fcos += fcos;

//			x_fsin =__f32_add(x_fsin,fsin);
//			x_fcos =__f32_add(x_fcos,fcos);
		}

		y_fsin += fsin;
		y_fcos += fcos;
	}
	return TRUE;
}


//arm_mult_f32;
//代码2 原始的反向映射算法
static BYTE* X_RoateBitmap(BYTE src[],int src_w,int src_h,int src_pitch,float theta,int *neww,int *newh,int format)
{
	float fsin, fcos;
	float x_fsin,x_fcos;
	float y_fsin,y_fcos;
	float fx,fy,c1,c2;
//	float scale;
	int x,y,idx,channels;
	int dst_w,dst_h,dst_pitch;


	switch(format)
	{
	case BM_ARGB8888:
		channels =4;
		break;
	case BM_RGB888:
		channels =3;
		break;
	case BM_AL8:
		channels =1;
		break;
	default: //No Suport.
		return NULL;
	}

//	scale =1.5;
	//1. 计算基本参数
	fsin = sin(theta);
	fcos = cos(theta);

	dst_h = ceilf(abs(src_h*fcos) + abs(src_w*fsin));
	dst_w = ceilf(abs(src_w*fcos) + abs(src_h*fsin));

	dst_pitch =dst_w*channels;
	if(dst_pitch&0x03)
	{
		dst_pitch =(dst_pitch&0xFFFFFFFC) + 4;
	}

	*neww =dst_w;
	*newh =dst_h;

	BYTE *dst = new BYTE[dst_h*dst_pitch];
	if (!dst)
	{
		return NULL;
	}
	memset(dst, 0, dst_h*dst_pitch);

	c1 = (src_w - dst_w*fcos - dst_h*fsin) / 2;
	c2 = (src_h + dst_w*fsin - dst_h*fcos) / 2;

	y_fsin =c1;
	y_fcos =c2;

	//2. 计算反向坐标并计算插值
	for (y = 0; y < dst_h; y++)
	{
		int idx,xx,yy;

		idx =y*dst_pitch;
		x_fsin =0;
		x_fcos =0;

		for (x = 0; x < dst_w; x++)
		{
			//计算后向映射点的精确位置 每个点都使用原始公式计算
			/*
			fx = x*fcos + y*fsin + c1;
			fy = y*fcos - x*fsin + c2;
			*/

			fx = x_fcos + y_fsin;// + c1;
			fy = y_fcos - x_fsin;// + c2;

			xx =(int)fx;
			yy =(int)fy;

			if(xx >= 0 && yy >= 0)
			if(xx < src_w && yy < src_h)
			{
				switch(format)
				{
				case BM_ARGB8888:
					MakeBilinearARGB8888(fx,fy,src,src_pitch,dst,idx);
					break;
				case BM_AL8:
					MakeBilinearAL8(fx,fy,src,src_pitch,dst,idx);
					break;
				}

				//x_out(x,y,&dst[idx]);
				//I1[w1*y+x] = img[(int)fy*w+(int)fx];
				//I1[w1*y+x] = Interpolate(img, xx,yy,w,h, ex, ey, 2);//双线性插值


			}
			else
			{
				int i;
				for(i=0;i<channels;i++)
				{
					dst[idx+x+i] =0;
				}
			}

			idx += channels;

			x_fsin += fsin;
			x_fcos += fcos;

//			x_fsin =__f32_add(x_fsin,fsin);
//			x_fcos =__f32_add(x_fcos,fcos);
		}

		y_fsin += fsin;
		y_fcos += fcos;
	}
	return dst;
}



void*	X_RotateBitmapOut(void **bits,int *out_w,int *out_h,const BITMAP *src,float a)
{
	BYTE *dst;
	int w,h;

	// a =a*3.14/180;
	a =__f32_mul(a,3.14f);
	a =__f32_div(a,180);

	dst=(BYTE*)X_RoateBitmap((BYTE*)src->Bits,src->Width,src->Height,src->WidthBytes,a,&w,&h,src->Format);

	if(dst!=NULL)
	{
		*bits =dst;
		*out_w =w;
		*out_h =h;

	}
	return dst;
}

void	X_RotateBitmapDestroy(void *h)
{
	delete h;
}


/*============================================================================*/
/*
% INTERPOLATE 插值方法
% x 原图x整数坐标
% y 原图y整数坐标
% w,h 原图 sz图像大小
% ex x和亚像素坐标误差
% ey x和亚像素坐标误差
% way 1为最近邻插值，2为双线性插值方法
*/

static  BYTE Interpolate(BYTE f[], int x,int y,int w,int h, float ex, float ey, char way)
{
	BYTE gray=0; float fr1, fr2, fr3;

	//1. 误差统一到0到1之间
	if (ex < 0)
	{
		ex = 1 + ex; x--;
	}
	if (ey < 0)
	{
		ey = 1 + ey; y--;
	}
	if (y < 0 || x < 0)
	{
		return gray;
	}

	//2. 最邻近差值
	if (way == 1)
	{
		if (ex > 0.5)
		{
			x++;
		}
		if (ey > 0.5)
		{
			y++;
		}
		if (y > h || x > w)
		{
			return gray;
		}

		gray = f[w*y+x];
		return gray;
	}

	// 3.双线性插值
	if (((y + 1) > h) || ((x + 1) > w))
	{
		return gray;
	}

	if (way == 2)
	{
		fr1 = (1 - ex)*float(f[w * y + x]) + ex*float(f[w * y + x + 1]);
		fr2 = (1 - ex)*float(f[w * (y + 1) + x]) + ex*float(f[w * (y+1) + x + 1]);
		fr3 = (1 - ey)*fr1 + ey*fr2;
		gray = BYTE(fr3);
	}
	return gray;

}

/*============================================================================*/
//代码2 原始的反向映射算法

BYTE* normalRoate(BYTE img[],int w,int h,double theta,int *neww,int *newh)
{
	float fsin, fcos,c1,c2,fx,fy,ex,ey;
	int w1, h1,xx,yy;

	//1. 计算基本参数
	fsin = sin(theta);
	fcos = cos(theta);
	*newh=h1 = ceilf(abs(h*fcos) + abs(w*fsin));
	*neww=w1 = ceilf(abs(w*fcos) + abs(h*fsin));
	BYTE *I1 = new BYTE[w1*h1];
	if (!I1)
		return NULL;
	memset(I1, 0, w1*h1);
	c1 = (w - w1*fcos - h1*fsin) / 2;
	c2 = (h + w1*fsin - h1*fcos) / 2;

	//2. 计算反向坐标并计算插值
	for (int y = 0; y < h1; y++)
	{
		for (int x = 0; x < w1; x++)
		{
			//计算后向映射点的精确位置 每个点都使用原始公式计算
			fx = x*fcos + y*fsin + c1;
			//四次浮点乘法和四次浮点加法
			fy = y*fcos - x*fsin + c2;
			xx = roundf(fx);
			yy = roundf(fy);
			ex = fx - float(xx);
			ey = fy - float(yy);
			I1[w1*y+x] = Interpolate(img, xx,yy,w,h, ex, ey, 2);//双线性插值
		}
	}
	return I1;
}


/*============================================================================*/
#if 0
typedef	void FN_Output(void *argv,int x,int y,u8 val);

extern "C" BOOL GUI_RotateOutput(FN_Output *out,void *argv,int xoff,int yoff,BYTE img[],int w,int h,double theta,int *neww,int *newh);

static u8 xbuf[550];
static void output(void *argv,int x,int y,u8 val)
{
	static int yy=-1000;
	HDC hdc =(HDC)argv;
	if(val>0)
	{
		SetPixel(hdc,x,y,MapRGB(hdc,val,val,val));
	}
}
///////////////
typedef	void FN_LineOut(void *argv,int xoff,int yoff,int line_idx,int line_max,u8 *buf,u32 count);

extern "C" BOOL GUI_RotateOutputLine(FN_LineOut *out,void *argv,int xoff,int yoff,BYTE img[],int w,int h,double theta);


static void out_line(void *argv,int xoff,int yoff,int line_idx,int line_max,u8 *buf,u32 count)
{
	BITMAP bm;
	COLORREF c;
	HDC hdc =(HDC)argv;

	{

		c =MapRGB(hdc,180,10,10);

		bm.Format =BM_AL8;
		bm.Width =count;
		bm.Height =1;
		bm.WidthBytes =count;
		bm.Bits =buf;
		bm.LUT =&c;

		DrawBitmap(hdc,xoff,yoff+line_idx,&bm,NULL);

	}
}
#endif

#if 0
void PicRotary0(const BITMAP Dst,const BITMAP Src,float RotaryAngle,float ZoomX,float ZoomY,float move_x,float move_y)
{
    if ( (fabs(ZoomX*Src.Width)<1.0e-4) || (fabs(ZoomY*Src.Height)<1.0e-4) )
    {
    	return; //太小的缩放比例认为已经不可见
    }

    float rx0=Src.Width*0.5;  //(rx0,ry0)为旋转中心
    float ry0=Src.Height*0.5;

    for (long y=0;y<Dst.Height;++y)
    {
        for (long x=0;x<Dst.Width;++x)
        {
        	float fx=(float)((x- move_x-rx0)/ZoomX*cos(RotaryAngle) - (y- move_y-ry0)/ZoomY*sin(RotaryAngle) + rx0) ;
        	float fy=(float)((x- move_x-rx0)/ZoomX*sin(RotaryAngle) + (y- move_y-ry0)/ZoomY*cos(RotaryAngle) + ry0) ;
           // if (PixelsIsInPic(Src,srcx,srcy))
           //     Pixels(Dst,x,y)=Pixels(Src,srcx,srcy)

        	MakeBilinearARGB8888(fx,fy,Src.Bits,Src.WidthBytes,dst,idx);

        }
    }
}
#endif
///////////////

static SURFACE *pSurf;
static HDC hdc_mem;

#define	IMG_W	240
#define	IMG_H	320

static int a0=0;
static double d=0;
//static BYTE vbuf[IMG_W*IMG_H*4];
static BYTE *vbuf;

static void back_init(HDC hdc,int w,int h)
{
	RECT rc;
	int x,y,i,yy;

	rc.w =80;
	rc.h =80;

	yy=0;
	for(y=0;y<h;y+=rc.h)
	{
		i=yy&1;
		for(x=0;x<w;x+=rc.w)
		{
			if(i&1)
			{
				SetBrushColor(hdc,MapRGB(hdc,220,220,220));
			}
			else
			{
				SetBrushColor(hdc,MapRGB(hdc,160,160,175));
			}

			rc.x =x;
			rc.y =y;
			FillRect(hdc,&rc);
			i++;
		}
		yy++;
	}

}

static 	BITMAP bm_fish_0;
static 	BITMAP bm_src;
static  POINT pt[4];

static LRESULT	WinProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	RECT rc;

	switch(msg)
	{
		case WM_CREATE: //窗口创建时,会自动产生该消息,在这里做一些初始化的操作或创建子窗口.
		{
			a0=50;
			d=1.0;
			m0 =2;

			bm_fish_0.Format	= BM_ARGB8888;
			bm_fish_0.Width = 92;
			bm_fish_0.Height = 184;
			bm_fish_0.WidthBytes =bm_fish_0.Width*4;
			bm_fish_0.LUT =NULL;
			bm_fish_0.Bits =(void*)gImage_0;


			bm_src.Format	= BM_ARGB8888;
			bm_src.Width = IMG_W;
			bm_src.Height = IMG_H;
			bm_src.WidthBytes =IMG_W*4;
			bm_src.LUT =NULL;
			bm_src.Bits =(void*)GUI_GRAM_Alloc(bm_src.Height*bm_src.WidthBytes);

			vbuf =(BYTE*)bm_src.Bits;

			//pSurf =CreateSurface(SURF_RGB332,IMG_W,IMG_H,IMG_W,(void*)vbuf);
			pSurf =CreateSurface(SURF_ARGB8888,bm_src.Width,bm_src.Height,bm_src.WidthBytes,(void*)bm_src.Bits);
			hdc_mem =CreateDC(pSurf,NULL);
			ClrDisplay(hdc_mem,NULL,0x10);
			//ClrDisplay(hdc_mem,NULL,MapRGB(hdc_mem,210,210,210));

			rc.x =20;
			rc.y =40;
			rc.w =150;
			rc.h =60;


			SetPenSize(hdc_mem,3);
			SetPenColor(hdc_mem,0xFF);
	//		DrawRect(hdc_mem,&rc);

			SetPenColor(hdc_mem,0xC0);
			SetPenColor(hdc_mem,MapARGB(hdc_mem,250,240,10,10));
			rc.w =IMG_W;
			rc.h =20;
			rc.x =0;
			rc.y =0;
			DrawRect(hdc_mem,&rc);

			SetPenColor(hdc_mem,MapARGB(hdc_mem,240,0,50,150));
			rc.w =20;
			rc.h =IMG_H-20*2;
			rc.x =((IMG_W-rc.w)/2)+0.5;
			rc.y =((IMG_H-rc.h)/2)+0.5;
			DrawRect(hdc_mem,&rc);

			SetPenSize(hdc_mem,1);
			SetPenColor(hdc_mem,MapARGB(hdc_mem,240,20,20,20));
			rc.w =IMG_W;
			rc.h =IMG_H;
			rc.x =((IMG_W-rc.w)/2);
			rc.y =((IMG_H-rc.h)/2);
			DrawRect(hdc_mem,&rc);


			EnableAntiAlias(hdc_mem,TRUE);

			EnableAlpha(hdc_mem,TRUE);
			SetAlpha(hdc_mem,160);
			SetBrushColor(hdc_mem,MapARGB(hdc_mem,250,10,100,10));
			FillCircle(hdc_mem,180,80,45);
			EnableAlpha(hdc_mem,FALSE);

			if(1)
			{


				pt[0].x =0;
				pt[0].y =0;

				pt[1].x =pt[0].x+20;
				pt[1].y =pt[0].y+150;

				pt[2].x =pt[0].x+0;
				pt[2].y =pt[0].y+130;

				pt[3].x =pt[0].x-20;
				pt[3].y =pt[0].y+150;


				SetBrushColor(hdc_mem,MapARGB(hdc_mem,100,60,100,200));
				FillPolygon(hdc_mem,160,150,pt,4);

				SetPenSize(hdc_mem,3);
				SetPenColor(hdc_mem,MapARGB(hdc_mem,150,20,30,200));
				DrawPolygon(hdc_mem,160,150,pt,4);

			}
			EnableAntiAlias(hdc_mem,FALSE);

			DrawBitmap(hdc_mem,0,(IMG_H-bm_fish_0.Height)>>1,&bm_fish_0,NULL);

			GetClientRect(hwnd,&rc); //获得窗口的客户区矩形.

			CreateWindow(BUTTON,L"OK",WS_VISIBLE,rc.w-80,8,68,32,hwnd,ID_OK,NULL,NULL); //创建一个按钮(示例).
			SetTimer(hwnd,1,50,TMR_START,NULL);
		}
		return TRUE;
		////

		case WM_NOTIFY: //WM_NOTIFY消息:wParam低16位为发送该消息的控件ID,高16位为通知码;lParam指向了一个NMHDR结构体.
		{
			u16 code,id;

			code =HIWORD(wParam); //获得通知码类型.
			id   =LOWORD(wParam); //获得产生该消息的控件ID.

			if(id==ID_OK && code==BN_CLICKED) // 按钮“单击”了.
			{
				PostCloseMessage(hwnd); //使产生WM_CLOSE消息关闭窗口.
			}
		}
		break;
		////

		case WM_TIMER:
		{
			InvalidateRect(hwnd,NULL,FALSE);
		}
		break;
		////

		case WM_PAINT: //窗口需要绘制时，会自动产生该消息.
		{
			PAINTSTRUCT ps;
			HDC hdc;
			int x,y,w,h;
			BITMAP bm;
			COLORREF c;
			BYTE *dst=NULL;
			WCHAR wbuf[128];


			hdc =BeginPaint(hwnd,&ps); //开始绘图
			//ClrDisplay(hdc,NULL,MapRGB(hdc,255,255,255));

			GetClientRect(hwnd,&rc);
			back_init(hdc,rc.w,rc.h);

			////
			EnableAntiAlias(hdc,TRUE);

			SetPenColor(hdc,MapARGB(hdc,200,50,50,200));
			DrawPolygon(hdc,30,150,pt,4);

			SetBrushColor(hdc,MapARGB(hdc,200,50,50,100));
			FillPolygon(hdc,100,150,pt,4);

			EnableAntiAlias(hdc,FALSE);
			////

			c =MapRGB(hdc,100,10,150);

			d += 1.0f;
			if(d>360) d=0;

			t0 =GUI_GetTickCount();
			if(m0==0)
			{
				dst=normalRoate(vbuf,IMG_W,IMG_H,d*3.14/180,&w,&h);
			}

			if(m0==1)
			{
				//dst=X_RoateBitmap(vbuf,IMG_W,IMG_H,IMG_W*1,d*3.14f/180,&w,&h,BM_AL8);
			//	dst=X_RoateBitmap(vbuf,IMG_W,IMG_H,IMG_W*4,d*3.14f/180,&w,&h,BM_ARGB8888);

				BITMAP src;
				void *p;

				src.Format =BM_ARGB8888;
				src.Width =IMG_W;
				src.Height =IMG_H;
				src.WidthBytes =IMG_W*4;
				src.Bits =(void*)vbuf;
#if 1
				p=X_RotateBitmapOut((void**)&dst,&w,&h,&src,d);
				if(dst!=NULL)
				{
					//bm.Format =BM_AL8;
					bm.Format =BM_ARGB8888;
					bm.Width =w;
					bm.Height =h;
					//bm.WidthBytes =w*1;
					bm.WidthBytes =w*4;
					bm.Bits =(void*)dst;
					bm.LUT =&c;


					GetClientRect(hwnd,&rc);
					x = ((rc.w-w)/2)+0.5;
					y = ((rc.h-h)/2)+0.5;
					DrawBitmap(hdc,x,y,&bm,NULL);
				}
				X_RotateBitmapDestroy(p);
				dst=NULL;

#endif
#if 0
				X_RoateBitmapDC(hdc,0,0,(BYTE*)vbuf,IMG_W,IMG_H,IMG_W*4,d*3.14f/180,&w,&h,BM_ARGB8888);
#endif
			}

			if(m0==2)
			{
				BITMAP bm;

				EnableAntiAlias(hdc,TRUE);

				x =rc.w>>1;
				y =rc.h>>1;
				RotateBitmap(hdc,x,y,&bm_src,(int)d);
				dst=NULL;

				EnableAntiAlias(hdc,FALSE);
			}

			////
			if(dst!=NULL)
			{
				//bm.Format =BM_AL8;
				bm.Format =BM_ARGB8888;
				bm.Width =w;
				bm.Height =h;
				//bm.WidthBytes =w*1;
				bm.WidthBytes =w*4;
				bm.Bits =(void*)dst;
				bm.LUT =&c;


				GetClientRect(hwnd,&rc);
				x = ((rc.w-w)/2)+0.5;
				y = ((rc.h-h)/2)+0.5;
				DrawBitmap(hdc,x,y,&bm,NULL);
			}

			t1 =GUI_GetTickCount();

		//	GUI_RotateOutput(output,hdc,x,y,vbuf,IMG_W,IMG_H,d*3.14/180,&w,&h);
		//	GUI_RotateOutputLine(out_line,hdc,x,y,vbuf,IMG_W,IMG_H,d*3.14/180);
			////用户的绘制内容...

		//	EnableAntiAlias(hdc,TRUE);
			EnableAlpha(hdc,TRUE);
			SetAlpha(hdc,160);
			SetBrushColor(hdc,MapARGB(hdc_mem,250,10,100,10));
			FillCircle(hdc,100,60,45);
			EnableAlpha(hdc,FALSE);

			x_wsprintf(wbuf,L"Time: %dms",t1-t0);
			TextOut(hdc,10,10,wbuf,-1);

			EndPaint(hwnd,&ps); //结束绘图

			delete dst;
		}
		break;
		////

		case WM_CLOSE: //窗口关闭时，会自动产生该消息.
		{

			GUI_GRAM_Free(vbuf);
			DeleteSurface(pSurf);

			return DestroyWindow(hwnd); //调用DestroyWindow函数销毁窗口，该函数会使主窗口结束并退出消息循环;否则窗口将继续运行.
		}
		break;
		////

		default: //用户不关心的消息,由系统处理.
		{
			return DefWindowProc(hwnd,msg,wParam,lParam);
		}

	}

	return WM_NULL;
}

/*============================================================================*/

//static void	AppMain(void)
extern "C" void	GUI_DEMO_Rotate(void)
{
	HWND	hwnd;
	WNDCLASS	wcex;
	MSG msg;

	/////

	wcex.Tag 		    = WNDCLASS_TAG;

	wcex.Style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WinProc; //设置主窗口消息处理的回调函数.
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= NULL;
	wcex.hIcon			= NULL;
	wcex.hCursor		= NULL;

	//创建主窗口
	hwnd	=CreateWindowEx(	WS_EX_FRAMEBUFFER,
								&wcex,
								_T("emXGUI Window"), //窗口名称
								WS_CAPTION|WS_DLGFRAME|WS_BORDER|WS_CLIPCHILDREN,
								10,20,480,400,    //窗口位置和大小
								NULL,NULL,NULL,NULL);

	//显示主窗口
	ShowWindow(hwnd,SW_SHOW);

	//开始窗口消息循环(窗口关闭并销毁时,GetMessage将返回FALSE,退出本消息循环)。
	while(GetMessage(&msg,hwnd))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

/*============================================================================*/
