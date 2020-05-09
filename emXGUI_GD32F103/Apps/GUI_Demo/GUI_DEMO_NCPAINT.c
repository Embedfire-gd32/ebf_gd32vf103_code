
/*
 *GUI_DEMO_NCPAINT.c
 *2018年11月15日下午5:04:44
 *
 */

#include "emXGUI.h"
/*============================================================================*/
//定义控件ID
#define	ID_OK		0x1000

/*============================================================================*/

static LRESULT	WinProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	RECT rc;

	switch(msg)
	{
		case WM_CREATE: //窗口创建时,会自动产生该消息,在这里做一些初始化的操作或创建子窗口.
		{
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

		case WM_NCPAINT:
		{
			HDC hdc =(HDC)wParam;
			NCPAINT_STRUCT *nc =(NCPAINT_STRUCT*)lParam;

			RECT rc;

			rc.x=0;
			rc.y=0;
			rc.w=nc->Width;
			rc.h=nc->Height;
			if(nc->Hit==HTCAPTION)
			{
				SetBrushColor(hdc,MapRGB(hdc,50,10,10));
			}
			else
			{
				SetBrushColor(hdc,MapRGB(hdc,50,100,200));
			}
			FillRect(hdc,&rc);

			rc =nc->rcCaption;

			if(nc->Hit==HTCAPTION)
			{
				SetTextColor(hdc,MapRGB(hdc,250,10,10));
				SetBrushColor(hdc,MapRGB(hdc,100,10,10));
			}
			else
			{
				SetTextColor(hdc,MapRGB(hdc,230,240,250));
				SetBrushColor(hdc,MapRGB(hdc,50,100,200));
			}
			DrawText(hdc,nc->pText,-1,&rc,DT_VCENTER|DT_CENTER|DT_BKGND);

			rc =nc->rcCloseBox;
			if(nc->Hit==HTCLOSE)
			{
				SetTextColor(hdc,MapRGB(hdc,250,10,10));
				SetPenColor(hdc,MapRGB(hdc,250,10,10));
				SetBrushColor(hdc,MapRGB(hdc,10,50,200));
			}
			else
			{
				SetTextColor(hdc,MapRGB(hdc,250,250,250));
				SetPenColor(hdc,MapRGB(hdc,10,100,100));
				SetBrushColor(hdc,MapRGB(hdc,10,50,100));

			}
			DrawText(hdc,L"X",-1,&rc,DT_VCENTER|DT_CENTER|DT_BKGND);

		}
		return TRUE;
		/////

		case WM_PAINT: //窗口需要绘制时，会自动产生该消息.
		{
			PAINTSTRUCT ps;
			HDC hdc;

			hdc =BeginPaint(hwnd,&ps); //开始绘图

			////用户的绘制内容...
			TextOut(hdc,10,10,L"WM_NCPAINT Owner Draw.",-1);

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

void	GUI_DEMO_NCPAINT(void)
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
								_T("GUI_DEMO: WM_NCPAINT"), //窗口名称
								WS_OVERLAPPEDWINDOW,
								10,20,480,360,    //窗口位置和大小
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
