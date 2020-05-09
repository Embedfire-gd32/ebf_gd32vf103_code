
/*
 *GUI_DEMO_MEMDC.c
 *2018年8月23日上午11:55:43
 *
 */

#include <math.h>
#include "emXGUI.h"

/*============================================================================*/
//定义控件ID
#define	ID_OK		0x1000
#define	ID_NEXT		0x1001

#define	ID_TMR_50	0x1100
#define ID_TMR_200  0x1101


#define MEMDC_W 320
#define MEMDC_H 240


/*============================================================================*/
static HDC hdc_mem=NULL;
static int v1=0;
static int v1_inc=0;

static int v2=0;
static int v2_inc=0;
static int point_type=0;

//画表盘的指针
static void	X_MeterPointer(HDC hdc,int cx,int cy,int r,u32 color,int st_angle,int angle_size,int dat_size,int dat_val,int style)
{
	int angle,i,j;
	POINT pt[8];

	////

	if(angle_size>360)	angle_size=360;

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
	SetBrushColor(hdc,color);

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


	FillPolygon(hdc,0,0,pt,5);

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


	FillPolygon(hdc,0,0,pt,5); //抗锯齿的多边形填充

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

static LRESULT	WinProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	RECT rc;

	switch(msg)
	{
		case WM_CREATE: //窗口创建时,会自动产生该消息,在这里做一些初始化的操作或创建子窗口.
		{
			v1 =0;
			v1_inc=1;

			v2 =0;
			v2_inc=1;

			point_type =0;

			GetClientRect(hwnd,&rc); //获得窗口的客户区矩形.

			hdc_mem =CreateMemoryDC(SURF_ARGB4444,MEMDC_W,MEMDC_H); //创建一个与屏幕颜色格式相同的内存DC.
			ClrDisplay(hdc_mem,NULL,ARGB4444(0,0,0,0));

			CreateWindow(BUTTON,L"OK",WS_VISIBLE,rc.w-80,8,68,32,hwnd,ID_OK,NULL,NULL); //创建一个按钮(示例).

			CreateWindow(BUTTON,L"->",WS_VISIBLE,rc.w-80,8+40,68,32,hwnd,ID_NEXT,NULL,NULL);

			SetTimer(hwnd,ID_TMR_50,50,TMR_START,NULL); //创建一个50ms的窗口定时器。
			SetTimer(hwnd,ID_TMR_200,200,TMR_START,NULL); //创建一个200ms的窗口定时器。
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

			if(id==ID_NEXT && code == BN_CLICKED)
			{
				point_type ++;
				if(point_type > 6)
				{
					point_type =0;
				}
				InvalidateRect(hwnd,NULL,FALSE);
			}
		}
		break;
		////

		case WM_TIMER:
		{
			u16 id;
			id =LOWORD(wParam);
			if(id==ID_TMR_50)
			{

				if(v1 > 100)
				{
					v1_inc =-1;
				}

				if(v1 < 0)
				{
					v1_inc =1;
				}
				v1 += v1_inc;

				InvalidateRect(hwnd,NULL,FALSE);
			}

			if(id==ID_TMR_200)
			{

				if(v2 > 100)
				{
					v2_inc =-1;
				}

				if(v2 < 0)
				{
					v2_inc =1;
				}
				v2 += v2_inc;

				InvalidateRect(hwnd,NULL,FALSE);
			}
		}
		break;
		////

		case WM_PAINT: //窗口需要绘制时，会自动产生该消息.
		{
			PAINTSTRUCT ps;
			HDC hdc;
			int y,cx,cy,r;
			hdc =BeginPaint(hwnd,&ps); //开始绘图

			GetClientRect(hwnd,&rc);
			SetBrushColor(hdc,MapRGB(hdc,0xB0,0xB0,0xB0));
			FillRect(hdc,&rc);

			////用户的绘制内容...
			cx =MEMDC_W>>1;
			cy =MEMDC_H>>1;
			r  =MIN(MEMDC_W>>1,MEMDC_H>>1);
			//TextOut(hdc,10,10,L"Hello",-1);

			y =20;
			ClrDisplay(hdc_mem,NULL,MapRGB(hdc_mem,200,200,200));

			EnableAntiAlias(hdc_mem,TRUE);

			X_MeterPointer(hdc_mem,cx,cy,r,MapRGB(hdc_mem,20,20,200),30,150,100,v1,0); //在MEMDC里绘制。
			BitBlt(hdc,10,y,MEMDC_W,MEMDC_H,hdc_mem,0,0,SRCCOPY); //将MEMDC输出到窗口中。

			y += 30;//MEMDC_H+20;
			ClrDisplay(hdc_mem,NULL,MapRGB(hdc_mem,150,150,200));
			X_MeterPointer(hdc_mem,cx,cy,r,MapRGB(hdc_mem,200,20,20),30,150,100,v2,point_type);


			//////
			rc.x =0;
			rc.y =0;
			rc.w =100;
			rc.h =60;
			SetBrushColor(hdc_mem,MapARGB(hdc_mem,0x10,0xA0,0xC0,0xE0));
			FillRect(hdc_mem,&rc);

			SetBrushColor(hdc_mem,MapARGB(hdc_mem,0x00,200,200,200));
			FillCircle(hdc_mem,100,100,50);

			SetPenColor(hdc_mem,MapARGB(hdc_mem,0,10,200,10));
			DrawCircle(hdc_mem,150,150,50);

			EnableAntiAlias(hdc_mem,FALSE);

			///////
			BitBlt(hdc,10,y,MEMDC_W-50,MEMDC_H-50,hdc_mem,10,10,SRCCOPY);

			EndPaint(hwnd,&ps); //结束绘图
		}
		break;
		////

		case WM_CLOSE: //窗口关闭时，会自动产生该消息.
		{

			DeleteDC(hdc_mem);
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

void	GUI_DEMO_MEMDC(void)
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
								_T("emXGUI DEMO:MEMDC"), //窗口名称
								WS_OVERLAPPEDWINDOW,
								0,0,GUI_XSIZE,GUI_YSIZE,    //窗口位置和大小
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
