
/*
 *GUI_DEMO_DrawAA.c
 *2018年10月30日下午4:42:09
 *
 */

#include "emXGUI.h"
#include "x_libc.h"
#include "emXGUI_Arch.h"
/*============================================================================*/
//定义控件ID
#define	ID_OK		0x1000

/*============================================================================*/

static int b_aa=FALSE;
static int r=5;
/*============================================================================*/

static LRESULT	WinProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	RECT rc;

	switch(msg)
	{
		case WM_CREATE: //窗口创建时,会自动产生该消息,在这里做一些初始化的操作或创建子窗口.
		{
			b_aa=TRUE;
			r=60;
			GetClientRect(hwnd,&rc); //获得窗口的客户区矩形.

			CreateWindow(BUTTON,L"OK",WS_VISIBLE,rc.w-80,8,68,32,hwnd,ID_OK,NULL,NULL); //创建一个按钮(示例).
			//SetTimer(hwnd,1,500,TMR_START,NULL);

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
			InvalidateRect(hwnd,NULL,TRUE);
		}
		break;


		case WM_PAINT: //窗口需要绘制时，会自动产生该消息.
		{
			PAINTSTRUCT ps;
			HDC hdc;
			int i, t0,t1;
			WCHAR wbuf[128];
			POINT pt;
			SURFACE *pSurf;


			GetClientRect(hwnd,&rc);
			pSurf =CreateSurface(SURF_SCREEN,rc.w,rc.h,0,NULL);
			hdc =CreateDC(pSurf,NULL);

			BeginPaint(hwnd,&ps); //开始绘图
//			EnableAntiAlias(hdc,TRUE);

			////用户的绘制内容...
			SetBrushColor(hdc,MapRGB(hdc,200,200,220));
			FillRect(hdc,&rc);
			TextOut(hdc,10,10,L"Draw AA",-1);

			SetPenColor(hdc,MapRGB(hdc,10,10,10));
			pt.x =200;
			pt.y =50;

			ShowCursor(FALSE);
			t0 =GUI_GetTickCount();

			for(i=0;i<360;i+=10)
			{
				RotatePoint(&pt,1,300,200,10*3.14/180);
				Line(hdc,300,200,pt.x,pt.y);
			}


			//////
			EnableAlpha(hdc,TRUE);

			rc.x =100;
			rc.y =150;
			rc.w =160;
			rc.h =100;

			SetAlpha(hdc,128);
			SetBrushColor(hdc,MapRGB(hdc,200,0,0));
			FillRect(hdc,&rc);


			OffsetRect(&rc,50,30);
			SetBrushColor(hdc,MapRGB(hdc,0,0,200));
			FillRect(hdc,&rc);

			OffsetRect(&rc,50,30);
			SetBrushColor(hdc,MapRGB(hdc,200,0,200));
			FillRoundRect(hdc,&rc,20);


			OffsetRect(&rc,50,30);
			SetBrushColor(hdc,MapRGB(hdc,200,200,0));
			FillCircle(hdc,rc.x,rc.y,50);

			EnableAlpha(hdc,FALSE);

			OffsetRect(&rc,50,30);
			SetBrushColor(hdc,MapRGB(hdc,0,200,0));
			FillRect(hdc,&rc);

			t1 =GUI_GetTickCount();
			ShowCursor(TRUE);

			x_wsprintf(wbuf,L"Time:%dms",t1-t0);
			SetTextColor(hdc,MapRGB(hdc,0,0,0));
			TextOut(hdc,100,280,wbuf,-1);

			GetClientRect(hwnd,&rc);
			BitBlt(ps.hdc,0,0,rc.w,rc.h,hdc,0,0,SRCCOPY);
			EndPaint(hwnd,&ps); //结束绘图

//			EnableAntiAlias(hdc,FALSE);

			DeleteSurface(pSurf);
			DeleteDC(hdc);
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

void	GUI_DEMO_DrawAlpha(void)
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
								_T("GUI_DEMO: Draw Alpha"), //窗口名称
								WS_OVERLAPPEDWINDOW,
								10,20,650,450,    //窗口位置和大小
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
