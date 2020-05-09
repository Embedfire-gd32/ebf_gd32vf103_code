
/*
 *GUI_DEMO_MEMDC_ARGB.c
 *2019年8月5日下午8:55:21
 *
 */

#include "emXGUI.h"
/*============================================================================*/
//定义控件ID
#define	ID_OK		0x1000

/*============================================================================*/

static HDC hdc_mem;
static HDC hdc_ARGB;

static LRESULT	WinProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	RECT rc;
	int i,cx,cy,ri,ro;

	switch(msg)
	{
		case WM_CREATE: //窗口创建时,会自动产生该消息,在这里做一些初始化的操作或创建子窗口.
		{
			GetClientRect(hwnd,&rc); //获得窗口的客户区矩形.

			hdc_mem =CreateMemoryDC(SURF_RGB565,rc.w,rc.h);
			GradientFillRect(hdc_mem,&rc,MapRGB(hdc_mem,255,0,0),MapRGB(hdc_mem,0,128,255),TRUE);



			hdc_ARGB =CreateMemoryDC(SURF_ARGB4444,rc.w,rc.h);
			ClrDisplay(hdc_ARGB,NULL,MapARGB(hdc_ARGB,200,200,200,200));

			EnableAntiAlias(hdc_ARGB,TRUE);
		//	EnableAlpha(hdc_ARGB,TRUE);
		//	SetAlpha(hdc_ARGB,128);

			cx =rc.w/2;
			cy =rc.h/2;
			ro =MIN(cx,cy)-10;
			ri =ro-15;


			SetBrushColor(hdc_ARGB,MapARGB(hdc_ARGB,0,100,100,100));
			FillArc(hdc_ARGB,cx,cy,ri,ro,30,250);

			FillCircle(hdc_ARGB,cx,cy,ri/4);


			SetBrushColor(hdc_ARGB,MapARGB(hdc_ARGB,0,0,0,50));
			FillCircle(hdc_ARGB,cx,cy-50,ri/4);

			RECT rc0;
			rc0.x =4;
			rc0.y =8;
			rc0.w =20;
			rc0.h =10;
			FillRect(hdc_ARGB,&rc0);

			BitBlt(hdc_mem,0,0,rc.w,rc.h,hdc_ARGB,0,0,SRCCOPY);



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

			hdc =BeginPaint(hwnd,&ps); //开始绘图

			////用户的绘制内容...
			GetClientRect(hwnd,&rc);
			BitBlt(hdc,0,0,rc.w,rc.h,hdc_mem,0,0,SRCCOPY);
			//TextOut(hdc,10,10,L"Hello",-1);

			EndPaint(hwnd,&ps); //结束绘图
		}
		break;
		////

		case WM_CLOSE: //窗口关闭时，会自动产生该消息.
		{

			DeleteDC(hdc_mem);
			DeleteDC(hdc_ARGB);
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
void	GUI_DEMO_MEMDC_ARGB(void)
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
								_T("GUI_Demo: MEMDC-ARGB"), //窗口名称
								WS_CAPTION|WS_DLGFRAME|WS_BORDER|WS_CLIPCHILDREN,
								10,20,240,240,    //窗口位置和大小
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
