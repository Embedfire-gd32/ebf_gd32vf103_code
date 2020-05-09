
/*
 *GUI_DEMO_Rotate.cpp
 *2019年3月14日下午10:07:54
 *
 */


#include <string.h>
#include <stdlib.h>
#include <math.h>

#include "emXGUI.h"
#include "Kernel.h"
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

BOOL ScaleBitmap(HDC hdc, int dst_x, int dst_y, int dst_w,int dst_h,const BITMAP *bitmap);


}

/*============================================================================*/

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

///////////////

static SURFACE *pSurf;
static HDC hdc_mem;

#define	IMG_W	100
#define	IMG_H	200

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
static  int scale_w,scale_h;

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
			scale_w =200;
			scale_h =290;

			bm_fish_0.Format	= BM_ARGB8888;
			bm_fish_0.Width = 92;
			bm_fish_0.Height = 184;
			bm_fish_0.WidthBytes =bm_fish_0.Width*4;
			bm_fish_0.LUT =NULL;
			bm_fish_0.Bits =(void*)gImage_0;


			bm_src.Format	= BM_ARGB4444;
			bm_src.Width = IMG_W;
			bm_src.Height = IMG_H;
			bm_src.WidthBytes =IMG_W*2;
			bm_src.LUT =NULL;
			bm_src.Bits =(void*)vmalloc(bm_src.Height*bm_src.WidthBytes);

			vbuf =(BYTE*)bm_src.Bits;

			//pSurf =CreateSurface(SURF_RGB332,IMG_W,IMG_H,IMG_W,(void*)vbuf);
			pSurf =CreateSurface(SURF_ARGB4444,bm_src.Width,bm_src.Height,bm_src.WidthBytes,(void*)bm_src.Bits);
			hdc_mem =CreateDC(pSurf,NULL);
			ClrDisplay(hdc_mem,NULL,0x10);

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
			SetBrushColor(hdc_mem,MapARGB(hdc_mem,250,10,100,10));
			FillCircle(hdc_mem,180,100,30);
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
			WCHAR wbuf[128];


			hdc =BeginPaint(hwnd,&ps); //开始绘图
			//ClrDisplay(hdc,NULL,MapRGB(hdc,255,255,255));

			GetClientRect(hwnd,&rc);
			back_init(hdc,rc.w,rc.h);

			c =MapRGB(hdc,100,10,150);

			d += 1.0f;
			if(d>360) d=0;

			t0 =GUI_GetTickCount();
			if(m0==0)
			{

			}

			if(m0==1)
			{

				ScaleBitmap(hdc,10,20,scale_w,scale_h,&bm_src);

			}

			if(m0==2)
			{


				EnableAntiAlias(hdc,TRUE);
				ScaleBitmap(hdc,10,20,scale_w,scale_h,&bm_src);
				EnableAntiAlias(hdc,FALSE);
			}

			////

			t1 =GUI_GetTickCount();

		//	GUI_RotateOutput(output,hdc,x,y,vbuf,IMG_W,IMG_H,d*3.14/180,&w,&h);
		//	GUI_RotateOutputLine(out_line,hdc,x,y,vbuf,IMG_W,IMG_H,d*3.14/180);
			////用户的绘制内容...

			x_wsprintf(wbuf,L"Time: %dms",t1-t0);
			TextOut(hdc,10,10,wbuf,-1);

			EndPaint(hwnd,&ps); //结束绘图


		}
		break;
		////

		case WM_CLOSE: //窗口关闭时，会自动产生该消息.
		{

			vfree(vbuf);
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
extern "C" void	GUI_DEMO_ScaleBitmap2(void)
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
								_T("emXGUI ScaleBitmap2"), //窗口名称
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
