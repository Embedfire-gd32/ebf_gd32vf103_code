
/*
 *GUI_DEMO_DrawMeter.c
 *2018年10月4日下午2:36:29
 *
 */

#include <math.h>
#include <stdlib.h>
#include "emXGUI.h"
#include "x_libc.h"
/*============================================================================*/
extern const unsigned char gImage_0[];

#define	MEMDC_W	300
#define	MEMDC_H	300

/*============================================================================*/
//定义控件ID
#define	ID_OK		0x1000

static SURFACE *pSurf=NULL;
static HDC hdc_mem=NULL;
static HDC hdc_bk=NULL;

static BITMAP bm_0;

static float dat1,dat2;
static float dat1_speed,dat2_speed;

static int fps=0;
static int frame,t0;

static int pt_type=0;
static int a0=0;

static int b_stretch;
static int b_aa_blt;

static int b_draw_meter;
static int b_draw_2d;
static int b_draw_bitmap;

/*============================================================================*/
#if 0
static	 __inline U8	__alpha_blend_value(U8 dst,U8 src,U8 src_a)
{
//	return	( src * src_a + dst * (255-src_a))>>8;
	return	(((src-dst)*src_a)>>8) + dst;
}

static void __SetPixelARGB(HDC hdc,int x,int y,U8 r ,U8 g,U8 b,U8 a)
{
	U32 c;
	U8 r0,g0,b0;

	c=GetPixel(hdc,x,y);
	GetRGB(hdc,c,&r0,&g0,&b0);
	r0 =__alpha_blend_value(r0,r,a);
	g0 =__alpha_blend_value(g0,g,a);
	b0 =__alpha_blend_value(b0,b,a);
	SetPixel(hdc,x,y,MapRGB(hdc,r0,g0,b0));

}
/////////

#endif

//画表指针
static void	DrawMeterPointer(HDC hdc,int cx,int cy,int r,u32 color,int st_angle,int angle_size,int dat_size,int dat_val,int style)
{
	int angle,i,j;
	POINT pt[8];

	////

	if(angle_size>360)
	{
		angle_size=360;
	}

	angle = st_angle+(dat_val*angle_size)/dat_size;
	angle = MIN(angle,st_angle+angle_size);

	angle -= 90;


	////
#if 0	//画表盘背景
	SetBrushColor(hdc,bk_color);
	FillRect(hdc,&rc);

	SetPenColor(hdc,bd_color);
	DrawCircle(hdc,cx,cy,r);
	r-=8;
	DrawArc(hdc,cx,cy,r,st_angle,st_angle+angle_size);

	j=angle_size/10;
	for(i=(st_angle-90);i<=(st_angle-90)+angle_size;i+=j)
	{
		int x0,y0,x1,y1;

		x0	= (int)(cx + sin(i*3.14/180)*r);
		y0	= (int)(cy - cos(i*3.14/180)*r);

		x1	= (int)(cx + sin(i*3.14/180)*(r-8));
		y1	= (int)(cy - cos(i*3.14/180)*(r-8));

		DrawLine(hdc,x0,y0,x1,y1);
	}
#endif
	////
	//????r =r-(r>>5);

	if(style==0)//表针风格-1
	{
		pt[0].x	 = (int)(cx + sin(angle*3.14/180)*r);
		pt[0].y	 = (int)(cy - cos(angle*3.14/180)*r);

		pt[1].x = (int)(cx + sin((angle+90)*3.14/180)*(r>>5));
		pt[1].y = (int)(cy - cos((angle+90)*3.14/180)*(r>>5));

		pt[2].x = (int)(cx + sin((angle+180)*3.14/180)*(r>>5));
		pt[2].y = (int)(cy - cos((angle+180)*3.14/180)*(r>>5));

		pt[3].x = (int)(cx + sin((angle+270)*3.14/180)*(r>>5));
		pt[3].y = (int)(cy - cos((angle+270)*3.14/180)*(r>>5));

		pt[4].x = pt[0].x;
		pt[4].y = pt[0].y;


		SetBrushColor(hdc,color);
		FillPolygon(hdc,0,0,pt,5);

		SetPenColor(hdc,color);
		//AA_DrawPolygon(hdc,0,0,pt,5);


		//SetPenColor(hdc,color);
		//AA_DrawPolygon(hdc,0,0,pt,5); //抗锯齿的多边形绘制
	}


	if(style==1) //表针风格-2
	{
		pt[0].x	 = (int)(cx + sin(angle*3.14/180)*r);
		pt[0].y	 = (int)(cy - cos(angle*3.14/180)*r);

		pt[1].x = (int)(cx + sin((angle+4)*3.14/180)*(r-(r>>2)));
		pt[1].y = (int)(cy - cos((angle+4)*3.14/180)*(r-(r>>2)));

		pt[2].x = cx;
		pt[2].y = cy;

		pt[3].x = (int)(cx + sin((angle-4)*3.14/180)*(r-(r>>2)));
		pt[3].y = (int)(cy - cos((angle-4)*3.14/180)*(r-(r>>2)));

		pt[4].x = pt[0].x;
		pt[4].y = pt[0].y;

		SetBrushColor(hdc,color);
		FillPolygon(hdc,0,0,pt,5); //抗锯齿的多边形填充

		SetPenColor(hdc,color);
		//AA_DrawPolygon(hdc,0,0,pt,5);

	}


	if(style==2) //表针风格-3:箭头样式
	{
		POINT pt[7];

		pt[0].x	 = (int)(cx + sin(angle*3.14/180)*r);
		pt[0].y	 = (int)(cy - cos(angle*3.14/180)*r);

		pt[1].x = (int)(cx + sin((angle+6)*3.14/180)*(r-(r>>2)));
		pt[1].y = (int)(cy - cos((angle+6)*3.14/180)*(r-(r>>2)));

		pt[2].x = (int)(cx + sin((angle+1)*3.14/180)*(r-(r>>2)+(r>>4)));
		pt[2].y = (int)(cy - cos((angle+1)*3.14/180)*(r-(r>>2)+(r>>4)));

		pt[3].x = (int)cx;
		pt[3].y = (int)cy;

		pt[4].x = (int)(cx + sin((angle-1)*3.14/180)*(r-(r>>2)+(r>>4)));
		pt[4].y = (int)(cy - cos((angle-1)*3.14/180)*(r-(r>>2)+(r>>4)));

		pt[5].x = (int)(cx + sin((angle-6)*3.14/180)*(r-(r>>2)));
		pt[5].y = (int)(cy - cos((angle-6)*3.14/180)*(r-(r>>2)));

		pt[6].x = pt[0].x;
		pt[6].y = pt[0].y;

		FillPolygon(hdc,0,0,pt,7);

	}

	if(style==3) //表针风格-4:矩形样式
	{
		POINT pt[7];
		int x1,y1;

		x1 = (int)(cx + sin((angle-0)*3.14/180)*(r-(r>>5)));
		y1 = (int)(cy - cos((angle-0)*3.14/180)*(r-(r>>5)));

		pt[0].x	 = (int)(x1 + sin((angle-90)*3.14/180)*(2));
		pt[0].y	 = (int)(y1 - cos((angle-90)*3.14/180)*(2));

		pt[1].x	 = x1;
		pt[1].y	 = y1;

		pt[2].x	 = (int)(x1 + sin((angle+90)*3.14/180)*(2));
		pt[2].y	 = (int)(y1 - cos((angle+90)*3.14/180)*(2));


		pt[3].x	 = (int)(cx + sin((angle+90)*3.14/180)*(2));
		pt[3].y	 = (int)(cy - cos((angle+90)*3.14/180)*(2));

		pt[4].x	 = cx;
		pt[4].y	 = cy;

		pt[5].x	 = (int)(cx + sin((angle-90)*3.14/180)*(2));
		pt[5].y	 = (int)(cy - cos((angle-90)*3.14/180)*(2));

		pt[6].x = pt[0].x;
		pt[6].y = pt[0].y;

		FillPolygon(hdc,0,0,pt,7);

	}

	if(style==4) //表针风格-5:条形+尖针样式
	{
		POINT pt[7];
		int x1,y1;

		x1 = (int)(cx + sin((angle-0)*3.14/180)*(r-(r>>5)));
		y1 = (int)(cy - cos((angle-0)*3.14/180)*(r-(r>>5)));

		pt[0].x	 = (int)(cx + sin((angle-60)*3.14/180)*(5));
		pt[0].y	 = (int)(cy - cos((angle-60)*3.14/180)*(5));

		pt[1].x	 = cx;
		pt[1].y	 = cy;

		pt[2].x	 = (int)(cx + sin((angle+60)*3.14/180)*(5));
		pt[2].y	 = (int)(cy - cos((angle+60)*3.14/180)*(5));


		pt[3].x	 = (int)(pt[2].x + sin((angle+0)*3.14/180)*(r-(r>>2)));
		pt[3].y	 = (int)(pt[2].y - cos((angle+0)*3.14/180)*(r-(r>>2)));

		pt[4].x	 = x1;
		pt[4].y	 = y1;

		pt[5].x	 = (int)(pt[0].x + sin((angle+0)*3.14/180)*(r-(r>>2)));
		pt[5].y	 = (int)(pt[0].y - cos((angle+0)*3.14/180)*(r-(r>>2)));

		pt[6].x = pt[0].x;
		pt[6].y = pt[0].y;

		FillPolygon(hdc,0,0,pt,7);

	}

	if(style==5) //表针风格-6:菱形箭头样式1
	{
		POINT pt[5];
		int x1,y1;

		x1 = (int)(cx + sin((angle-0)*3.14/180)*(r-(r>>5)));
		y1 = (int)(cy - cos((angle-0)*3.14/180)*(r-(r>>5)));

		pt[0].x	 = x1;
		pt[0].y	 = y1;

		pt[1].x = (int)(cx + sin((angle+12)*3.14/180)*(r-(r>>1)));
		pt[1].y = (int)(cy - cos((angle+12)*3.14/180)*(r-(r>>1)));

		pt[2].x = (int)cx;
		pt[2].y = (int)cy;

		pt[3].x = (int)(cx + sin((angle-12)*3.14/180)*(r-(r>>1)));
		pt[3].y = (int)(cy - cos((angle-12)*3.14/180)*(r-(r>>1)));

		pt[4].x = pt[0].x;
		pt[4].y = pt[0].y;

		FillPolygon(hdc,0,0,pt,5);

	}

	if(style==6) //表针风格-7:菱形箭头样式2
	{
		POINT pt[5];
		int x1,y1;

		x1 = (int)(cx + sin((angle-0)*3.14/180)*((r*15)>>4));
		y1 = (int)(cy - cos((angle-0)*3.14/180)*((r*15)>>4));

		pt[0].x	 = x1;
		pt[0].y	 = y1;

		pt[1].x = (int)(cx + sin((angle+15)*3.14/180)*((r*4)>>4));
		pt[1].y = (int)(cy - cos((angle+15)*3.14/180)*((r*4)>>4));

		pt[2].x = (int)cx;
		pt[2].y = (int)cy;

		pt[3].x = (int)(cx + sin((angle-15)*3.14/180)*((r*4)>>4));
		pt[3].y = (int)(cy - cos((angle-15)*3.14/180)*((r*4)>>4));

		pt[4].x = pt[0].x;
		pt[4].y = pt[0].y;

		FillPolygon(hdc,0,0,pt,5);

	}

	////
#if 0	//画表针中心装饰
	SetBrushColor(hdc,fr_color);
	FillCircle(hdc,cx,cy,r>>3);
#endif

}

static void AA_Blt(HDC hdc,int x,int y,int w,int h)
{
	int x0,y0;

	int r,g,b;
	U32 c;
	U8 r0,g0,b0;

	for(y0=0;y0<h;y0++)
	{
		for(x0=0;x0<w;x0++)
		{

			c=GetPixel(hdc,x+x0+0,y+y0+0);
			GetRGB(hdc,c,&r0,&g0,&b0);
			r=r0;
			g=g0;
			b=b0;

			c=GetPixel(hdc,x+x0+1,y+y0+0);
			GetRGB(hdc,c,&r0,&g0,&b0);
			r+=r0;
			g+=g0;
			b+=b0;

			c=GetPixel(hdc,x+x0+0,y+y0+1);
			GetRGB(hdc,c,&r0,&g0,&b0);
			r+=r0;
			g+=g0;
			b+=b0;

			c=GetPixel(hdc,x+x0+1,y+y0+1);
			GetRGB(hdc,c,&r0,&g0,&b0);
			r+=r0;
			g+=g0;
			b+=b0;


			SetPixel(hdc,x+x0,y+y0,MapRGB(hdc,r>>2,g>>2,b>>2));
		}
	}
}

void GL16_AA_Blt(const SURFACE *pSurf,int x,int y,int w,int h);

/*============================================================================*/
//画表盘背景
static void MakeMeterBackgnd(HDC hdc)
{
	ClrDisplay(hdc,NULL,MapRGB(hdc,0,40,50));
}

static LRESULT	WinProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	RECT rc;

	switch(msg)
	{
		case WM_CREATE: //窗口创建时,会自动产生该消息,在这里做一些初始化的操作或创建子窗口.
		{
			GetClientRect(hwnd,&rc); //获得窗口的客户区矩形.
			a0 =30;
			bm_0.Format	= BM_ARGB8888;     //位图格式
			//bm_0.Format	= BM_RGB565;     //位图格式
			bm_0.Width  = 92;              //宽度
			bm_0.Height = 184;             //高度
			bm_0.WidthBytes =bm_0.Width*4; //每行字节数
			bm_0.LUT =NULL;                //查找表(RGB/ARGB格式不使用该参数)
			bm_0.Bits =(void*)gImage_0;    //位图数据

			b_draw_meter=1;
			b_draw_2d=0;
			b_draw_bitmap=0;

			pt_type=1;
			b_stretch=0;
			b_aa_blt=0;


			dat1 =0;
			dat2 =0;
			dat1_speed =1;
			dat2_speed =2;
			pSurf =CreateSurface(SURF_SCREEN,MEMDC_W,MEMDC_H,0,NULL);
			hdc_mem =CreateDC(pSurf,NULL); //用于前景的MEMDC.
			hdc_bk  =CreateMemoryDC(SURF_SCREEN,MEMDC_W,MEMDC_H); //用于背景的MEMDC.
			MakeMeterBackgnd(hdc_bk);

			CreateWindow(BUTTON,L"OK",WS_VISIBLE,rc.w-80,8,68,32,hwnd,ID_OK,NULL,NULL); //创建一个按钮(示例).

		//	SetTimer(hwnd,1,10,TMR_START,NULL);
			t0 =GUI_GetTickCount();
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
		/////

		case WM_PAINT: //窗口需要绘制时，会自动产生该消息.
		{
			PAINTSTRUCT ps;
			HDC hdc;
			int t,y;
			WCHAR wbuf[24];

//while(1){
			hdc =BeginPaint(hwnd,&ps); //开始绘图


			//////////
			if(dat1 >= 100)
			{
				dat1_speed =-fabs(dat1_speed);
			}

			if(dat1 <= 0)
			{
				dat1 = 0;
				dat1_speed =fabs(dat1_speed);
			}
			dat1 += dat1_speed;
			/////


			t =GUI_GetTickCount();
			if((t-t0) >= 1000)
			{
				fps =frame;

				t0 =t;
				frame=0;
			}
			else
			{
				frame++;
			}

			a0 += 3;
			a0 =a0%360;


			////用户的绘制内容...
			//TextOut(hdc,10,10,L"Hello",-1);
			BitBlt(hdc_mem,0,0,MEMDC_W,MEMDC_H,hdc_bk,0,0,SRCCOPY);


			t =GUI_GetTickCount();
			if(b_draw_meter)
			{
			DrawMeterPointer(hdc_mem,MEMDC_W/2,MEMDC_H/2,100,MapRGB(hdc_mem,250,0,0),30,180,100,(int)dat1,pt_type);
		//	DrawMeterPointer(hdc_mem,MEMDC_W/2,MEMDC_H/2,100,MapRGB(hdc_mem,0,0,250),30,180,100,(int)dat2,1);
			}

			if(b_draw_bitmap)
			{
			RotateBitmap(hdc_mem,MEMDC_W/2,MEMDC_H/2,&bm_0,a0);
			}
			t =GUI_GetTickCount()-t;

			if(b_draw_2d)
			{
			SetPenColor(hdc_mem,MapRGB(hdc_mem,250,250,250));
			HLine(hdc_mem,10,20,150);
			VLine(hdc_mem,30,50,150);
			Line(hdc_mem,50,50,200,100);

			SetPenColor(hdc_mem,MapRGB(hdc_mem,50,250,0));
			DrawCircle(hdc_mem,200,100,40);

			SetBrushColor(hdc_mem,MapRGB(hdc_mem,250,250,0));
			FillCircle(hdc_mem,250,200,40);
			}
			y=100;
			//TextOut(hdc_mem,10,y,L"ABCDEFG12345",-1);

			y+=20;
			x_wsprintf(wbuf,L"FPS:%d;DrawTime:%dms",fps,t);
			TextOut(hdc_mem,10,y,wbuf,-1);

			if(b_stretch)
			{
				StretchBlt(hdc,400,20,MEMDC_W,MEMDC_H,hdc_mem,1,1,MEMDC_W-1,MEMDC_H-1,SRCCOPY);
			}
			else
			{
				BitBlt(hdc,400,20,MEMDC_W,MEMDC_H,hdc_mem,0,0,SRCCOPY);
			}

			t =GUI_GetTickCount();
			if(b_aa_blt)
			{
				//GL16_AA_Blt(pSurf,0,0,MEMDC_W,MEMDC_H);
				//GL16_AA_Blt(pSurf,0,0,MEMDC_W,MEMDC_H);
				//GL16_AA_Blt(pSurf,0,0,MEMDC_W,MEMDC_H);
				//GL16_AA_Blt(pSurf,0,0,MEMDC_W,MEMDC_H);
			}
			//AA_Blt(hdc_mem,50,50,200,200);

			t =GUI_GetTickCount()-t;

			y+=100;
			x_wsprintf(wbuf,L"AA_Blt Time:%dms",t);
			TextOut(hdc_mem,10,y,wbuf,-1);

			BitBlt(hdc,10,20,MEMDC_W,MEMDC_H,hdc_mem,0,0,SRCCOPY);


			//StretchBlt(hdc,10,20,400,400,hdc_mem,0,0,MEMDC_W,MEMDC_H,SRCCOPY);
			EndPaint(hwnd,&ps); //结束绘图

			InvalidateRect(hwnd,NULL,FALSE);
//}
		}
		break;
		////

		case WM_CLOSE: //窗口关闭时，会自动产生该消息.
		{
			DeleteSurface(pSurf);
			DeleteDC(hdc_mem);
			DeleteDC(hdc_bk);
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

void	GUI_DEMO_DrawMeter(void)
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
	hwnd	=CreateWindowEx(	0,
								&wcex,
								L"emXGUI DEMO: DrawMeter", //窗口名称
								WS_OVERLAPPEDWINDOW,
								0,0,GUI_XSIZE,GUI_YSIZE,    //窗口位置和大小
								NULL,0,NULL,NULL);

	//显示主窗口
	ShowWindow(hwnd,SW_SHOW);

	//开始窗口消息循环(窗口关闭并销毁时,GetMessage将返回FALSE,退出本消息循环)。
	while(GetMessage(&msg,hwnd))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

extern volatile int s_frameDone;
void* LCD_SwapBuffers(void);

void	xxGUI_DEMO_DrawMeter(void)
{
	HDC hdc;
	int t,y;
	WCHAR wbuf[128];
	SURFACE *pSurf;
	a0 =30;
	bm_0.Format	= BM_ARGB8888;     //位图格式
	//bm_0.Format	= BM_RGB565;     //位图格式
	bm_0.Width  = 92;              //宽度
	bm_0.Height = 184;             //高度
	bm_0.WidthBytes =bm_0.Width*4; //每行字节数
	bm_0.LUT =NULL;                //查找表(RGB/ARGB格式不使用该参数)
	bm_0.Bits =(void*)gImage_0;    //位图数据

	b_draw_meter=1;
	b_draw_2d=1;
	b_draw_bitmap=0;

	pt_type=0;
	b_stretch=0;
	b_aa_blt=0;


	dat1 =0;
	dat2 =0;
	dat1_speed =1;
	dat2_speed =2;

	hdc_bk  =CreateMemoryDC(SURF_SCREEN,MEMDC_W,MEMDC_H); //用于背景的MEMDC.
	MakeMeterBackgnd(hdc_bk);
	//hdc_mem =CreateMemoryDC(SURF_SCREEN,MEMDC_W,MEMDC_H);
	pSurf =CreateSurface(SURF_SCREEN,320,480,320*2,NULL);
	hdc_mem =CreateDC(pSurf,NULL);

	hdc =GetDC(NULL);
	//////////
	//while(1)
	{

		//void *fb;

		//while(s_frameDone==FALSE);
		//s_frameDone=FALSE;
		//fb =LCD_SwapBuffers();


	if(dat1 >= 100)
	{
		dat1_speed =-fabs(dat1_speed);
	}

		if(dat1 <= 0)
		{
			dat1 = 0;
			dat1_speed =fabs(dat1_speed);
		}
		dat1 += dat1_speed;
		/////


		t =GUI_GetTickCount();
		if((t-t0) >= 1000)
		{
			fps =frame;

			t0 =t;
			frame=0;
		}
		else
		{
			frame++;
		}

		a0 += 5;
		a0 =a0%360;


		////用户的绘制内容...
		//TextOut(hdc,10,10,L"Hello",-1);
		BitBlt(hdc_mem,0,0,MEMDC_W,MEMDC_H,hdc_bk,0,0,SRCCOPY);


		t =GUI_GetTickCount();
		if(b_draw_meter)
		{
		DrawMeterPointer(hdc_mem,MEMDC_W/2,MEMDC_H/2,100,MapRGB(hdc_mem,250,0,0),30,180,100,(int)dat1,pt_type);
	//	DrawMeterPointer(hdc_mem,MEMDC_W/2,MEMDC_H/2,100,MapRGB(hdc_mem,0,0,250),30,180,100,(int)dat2,1);
		}

		if(b_draw_bitmap)
		{
		RotateBitmap(hdc_mem,MEMDC_W/2,MEMDC_H/2,&bm_0,a0);
		}
		t =GUI_GetTickCount()-t;

		if(b_draw_2d)
		{
		SetPenColor(hdc_mem,MapRGB(hdc_mem,250,250,250));
		HLine(hdc_mem,10,20,150);
		VLine(hdc_mem,30,50,150);
		Line(hdc_mem,50,50,200,100);

		SetPenColor(hdc_mem,MapRGB(hdc_mem,50,250,0));
		DrawCircle(hdc_mem,200,100,40);

		SetBrushColor(hdc_mem,MapRGB(hdc_mem,250,250,0));
		FillCircle(hdc_mem,250,200,40);
		}
		y=100;
		//TextOut(hdc_mem,10,y,L"ABCDEFG12345",-1);

		y+=20;
		x_wsprintf(wbuf,L"FPS:%d;DrawTime:%dms",fps,t);
		TextOut(hdc_mem,10,y,wbuf,-1);

		if(b_stretch)
		{
			StretchBlt(hdc,400,20,MEMDC_W,MEMDC_H,hdc_mem,1,1,MEMDC_W-1,MEMDC_H-1,SRCCOPY);
		}
		else
		{
			BitBlt(hdc,400,20,MEMDC_W,MEMDC_H,hdc_mem,0,0,SRCCOPY);
		}

		t =GUI_GetTickCount();
		if(b_aa_blt)
		{
			//GL16_AA_Blt(pSurf,0,0,MEMDC_W,MEMDC_H);
			//GL16_AA_Blt(pSurf,0,0,MEMDC_W,MEMDC_H);
			//GL16_AA_Blt(pSurf,0,0,MEMDC_W,MEMDC_H);
			//GL16_AA_Blt(pSurf,0,0,MEMDC_W,MEMDC_H);
		}
		//AA_Blt(hdc_mem,50,50,200,200);

		t =GUI_GetTickCount()-t;

		y+=100;
		x_wsprintf(wbuf,L"AA_Blt Time:%dms",t);
		TextOut(hdc_mem,10,y,wbuf,-1);

		BitBlt(hdc,10,20,MEMDC_W,MEMDC_H,hdc_mem,0,0,SRCCOPY);

		//DeleteDC(hdc_mem);
		//DeleteSurface(pSurf);

	}


}

/*============================================================================*/
