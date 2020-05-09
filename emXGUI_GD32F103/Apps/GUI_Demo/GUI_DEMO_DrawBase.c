
/*
 *GUI_DEMO_DrawBase.c
 *2019年7月30日下午7:51:39
 *
 */

#include "emXGUI.h"
/*============================================================================*/
//定义控件ID
#define	ID_OK		0x1000

/*============================================================================*/

static void draw_1(HDC hdc)
{
	RECT rc;
	COLORREF c1,c2;

	rc.x =10;
	rc.y =20;
	rc.w =200;
	rc.h =30;

	////
	EnableAntiAlias(hdc,FALSE);
	SetBrushColor(hdc,MapRGB(hdc,150,10,10));
	FillRoundRect(hdc,&rc,20);

	OffsetRect(&rc,0,rc.h+20);
	EnableAntiAlias(hdc,TRUE);
	SetBrushColor(hdc,MapRGB(hdc,10,10,150));
	FillRoundRect(hdc,&rc,20);
	EnableAntiAlias(hdc,FALSE);

	////
	OffsetRect(&rc,0,rc.h+20);

	SetBrushColor(hdc,MapRGB(hdc,250,250,250));
	//FillRect(hdc,&rc);

	c1=MapRGB(hdc,250,250,250);
	c2=MapRGB(hdc,0,0,0);
	GradientFillRoundRect(hdc,&rc,20,c1,c2,TRUE);


	OffsetRect(&rc,0,rc.h+20);

	EnableAntiAlias(hdc,TRUE);
	SetBrushColor(hdc,MapRGB(hdc,250,250,250));
	//FillRect(hdc,&rc);

	c1=MapRGB(hdc,150,150,250);
	c2=MapRGB(hdc,50,10,10);
	GradientFillRoundRect(hdc,&rc,20,c1,c2,TRUE);
	EnableAntiAlias(hdc,FALSE);

	////
	OffsetRect(&rc,0,rc.h+20);
	rc.w =80;
	rc.h =180;
	SetBrushColor(hdc,MapRGB(hdc,250,250,250));
	FillRect(hdc,&rc);

	c1=MapRGB(hdc,250,0,0);
	c2=MapRGB(hdc,0,0,200);
	GradientFillRoundRect(hdc,&rc,20,c1,c2,FALSE);


	OffsetRect(&rc,rc.w+20,0);

	EnableAntiAlias(hdc,TRUE);
	SetBrushColor(hdc,MapRGB(hdc,250,250,250));
	FillRect(hdc,&rc);

	c1=MapRGB(hdc,250,0,0);
	c2=MapRGB(hdc,0,0,200);
	GradientFillRoundRect(hdc,&rc,20,c1,c2,FALSE);
	EnableAntiAlias(hdc,FALSE);
}

static volatile int aa,alpha;

static void draw_2(HDC hdc)
{
	volatile int x,y,r,a1,a2;

	EnableAntiAlias(hdc,aa);
	EnableAlpha(hdc,alpha);
	SetAlpha(hdc,128);

	SetBrushColor(hdc,MapRGB(hdc,150,0,0));

	x =110;
	y =100;
	r =70;
	a1 =0;
	a2 =360;
	FillArc(hdc,x,y,r,r+12,a1,a2);


	y =270;
	r =80;
	FillCircle(hdc,x,y,r);

}

/*============================================================================*/

static void back_init(HDC hdc,int w,int h,int cell_size)
{
	RECT rc;
	int x,y,i,yy;

	rc.w =cell_size;
	rc.h =cell_size;

	yy=0;
	for(y=0;y<h;y+=rc.h)
	{
		i=yy&1;
		for(x=0;x<w;x+=rc.w)
		{
			if(i&1)
			{
				SetBrushColor(hdc,MapRGB(hdc,150,150,180));
			}
			else
			{
				SetBrushColor(hdc,MapRGB(hdc,100,100,150));
			}

			rc.x =x;
			rc.y =y;
			FillRect(hdc,&rc);
			i++;
		}
		yy++;
	}

}

static LRESULT	WinProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	RECT rc;

	switch(msg)
	{
		case WM_CREATE: //窗口创建时,会自动产生该消息,在这里做一些初始化的操作或创建子窗口.
		{
			aa=FALSE;
			alpha=TRUE;

			GetClientRect(hwnd,&rc); //获得窗口的客户区矩形.

			CreateWindow(BUTTON,L"OK",WS_VISIBLE,rc.w-80,8,68,32,hwnd,ID_OK,NULL,NULL); //创建一个按钮(示例).
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

		case WM_PAINT: //窗口需要绘制时，会自动产生该消息.
		{
			PAINTSTRUCT ps;
			HDC hdc;
			GetClientRect(hwnd,&rc);

			hdc =BeginPaint(hwnd,&ps); //开始绘图

			////用户的绘制内容...
			back_init(hdc,rc.w,rc.h,60);

			//draw_1(hdc);
			draw_2(hdc);
			////

			//TextOut(hdc,10,10,L"Hello",-1);

			EndPaint(hwnd,&ps); //结束绘图
		}
		break;
		////

		case WM_CLOSE: //窗口关闭时，会自动产生该消息.
		{

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
void	GUI_DEMO_DrawBase(void)
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
	hwnd	=CreateWindowEx(	NULL,
								&wcex,
								_T("GUI_DEMO:DrawBase"), //窗口名称
								WS_CAPTION|WS_DLGFRAME|WS_BORDER|WS_CLIPCHILDREN,
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
