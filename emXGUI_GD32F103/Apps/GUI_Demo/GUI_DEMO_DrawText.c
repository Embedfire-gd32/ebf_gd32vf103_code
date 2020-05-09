
/*
 *GUI_DEMO_DrawText.c
 *2019年8月8日下午9:23:56
 *
 */

#include "emXGUI.h"
/*============================================================================*/
//定义控件ID
#define	ID_EXIT		0x1000

/*============================================================================*/

static LRESULT	WinProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	RECT rc;

	switch(msg)
	{
		case WM_CREATE: //窗口创建时,会自动产生该消息,在这里做一些初始化的操作或创建子窗口.
		{
			GetClientRect(hwnd,&rc); //获得窗口的客户区矩形.

			CreateWindow(BUTTON,L"Exit",WS_VISIBLE,rc.w-80,8,68,32,hwnd,ID_EXIT,NULL,NULL); //创建一个按钮(示例).
		}
		return TRUE;
		////

		case WM_NOTIFY: //WM_NOTIFY消息:wParam低16位为发送该消息的控件ID,高16位为通知码;lParam指向了一个NMHDR结构体.
		{
			u16 code,id;

			code =HIWORD(wParam); //获得通知码类型.
			id   =LOWORD(wParam); //获得产生该消息的控件ID.

			if(id==ID_EXIT && code==BN_CLICKED) // 按钮“单击”了.
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

			SetTextColor(hdc,MapRGB(hdc,0,0,0));
			SetBrushColor(hdc,MapRGB(hdc,150,250,250));
			SetPenColor(hdc,MapRGB(hdc,10,10,10));

			rc.x =2;
			rc.y =2;
			rc.w =160;
			rc.h =40;

			DrawText(hdc,L"123456789",5,&rc,DT_SINGLELINE|DT_LEFT|DT_VCENTER|DT_BORDER|DT_BKGND);

			OffsetRect(&rc,0,rc.h+2);
			DrawText(hdc,L"123456789",-1,&rc,DT_SINGLELINE|DT_LEFT|DT_VCENTER|DT_BORDER|DT_BKGND);

			OffsetRect(&rc,0,rc.h+2);
			DrawText(hdc,L"Align-Center",-1,&rc,DT_SINGLELINE|DT_CENTER|DT_VCENTER|DT_BORDER|DT_BKGND);

			SetTextColor(hdc,MapRGB(hdc,255,0,0));
			SetBrushColor(hdc,MapRGB(hdc,0,0,0));
			SetPenColor(hdc,MapRGB(hdc,0,255,0));
			OffsetRect(&rc,0,rc.h+2);
			rc.h=100;
			DrawText(hdc,L"Multi Line\r\n"
					 	 L"1234\r\n"
						 L"5678\r\n"
						 L"ABCEFGHIJKLMNOPQRSTUVWXYZ\r\n"
						 ,-1,&rc,DT_LEFT|DT_TOP|DT_BORDER|DT_BKGND);

			SetTextColor(hdc,MapRGB(hdc,255,255,255));
			SetBrushColor(hdc,MapRGB(hdc,100,0,200));
			SetPenColor(hdc,MapRGB(hdc,255,0,0));
			OffsetRect(&rc,0,rc.h+2);
			rc.h=120;
			DrawText(hdc,L"Multi Line\r\n"
						 L"Word Break\r\n"
						 L"ABCEFGHIJKLMNOPQRSTUVWXYZ"
						 ,-1,&rc,DT_LEFT|DT_TOP|DT_BORDER|DT_BKGND|DT_WORDBREAK);



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
void	GUI_DEMO_DrawText(void)
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
								_T("GUI_DEMO:DrawText"), //窗口名称
								WS_CAPTION|WS_DLGFRAME|WS_BORDER|WS_CLIPCHILDREN,
								0,0,300,400,    //窗口位置和大小
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
