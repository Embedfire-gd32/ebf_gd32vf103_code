
/*
 *GUI_DEMO_OwnerDC.c
 *2018年10月22日下午6:05:46
 *
 */

#include "emXGUI.h"

#define	OWNERDC_WINDOW	1
#define	OWNERDC_CLIENT	2

#define	WM_CREATE_OWNERDC	WM_USER+1000
#define	WM_DELETE_OWNERDC	WM_USER+1001

/*============================================================================*/
//定义控件ID

enum eID{
	ID_OK =0x1000,
	ID_BTN1,
	ID_BTN2,
	ID_BTN3,
	ID_BTN4,


};

/*============================================================================*/

static SURFACE *pSurf;

static LRESULT	DlgProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	RECT rc,rc0,m_rc[4];

	switch(msg)
	{
		case WM_CREATE: //窗口创建时,会自动产生该消息,在这里做一些初始化的操作或创建子窗口.
		{

			GetClientRect(hwnd,&rc0); //获得窗口的客户区矩形.

			MakeMatrixRect(m_rc,&rc0,2,2,4,1);
			//rc.w =64;
			//rc.h =40;
			//rc.x =8;
			//rc.y =20;

			//OffsetRect(&rc,rc.w+4,0);
			rc =m_rc[0];
			CreateWindow(BUTTON,L">>",WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,ID_BTN1,NULL,NULL);

			//OffsetRect(&rc,rc.w+4,0);
			rc =m_rc[1];
			CreateWindow(BUTTON,L"...",WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,ID_BTN2,NULL,NULL);

			//OffsetRect(&rc,rc.w+4,0);
			rc =m_rc[2];
			CreateWindow(BUTTON,L"-",WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,ID_BTN3,NULL,NULL);

			//OffsetRect(&rc,rc.w+4,0);
			rc =m_rc[3];
			CreateWindow(BUTTON,L"+",WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,ID_BTN4,NULL,NULL);

		}
		return TRUE;
		////

		case WM_NOTIFY: //WM_NOTIFY消息:wParam低16位为发送该消息的控件ID,高16位为通知码;lParam指向了一个NMHDR结构体.
		{
			u16 code,id;

			code =HIWORD(wParam); //获得通知码类型.
			id   =LOWORD(wParam); //获得产生该消息的控件ID.

			if(id==ID_BTN1 && code==BN_CLICKED) // 按钮“单击”了.
			{
				//PostCloseMessage(hwnd); //使产生WM_CLOSE消息关闭窗口.
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
void _InsertWindow_Zbefore(HWND hNode, HWND hNew);
void _InsertWindow_Zafter(HWND hNode, HWND hNew);
void HWND_Remove(HWND hNode);

static WNDCLASS	wcex_dlg;

static LRESULT	WinProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	RECT rc;
	HWND wnd1,wnd2,wnd3;
	switch(msg)
	{
		case WM_CREATE: //窗口创建时,会自动产生该消息,在这里做一些初始化的操作或创建子窗口.
		{
			GetWindowRect(hwnd,&rc);
			pSurf =CreateSurface(SURF_SCREEN,rc.w,rc.h,NULL,0);
#if 1
			GetClientRect(hwnd,&rc); //获得窗口的客户区矩形.
			CreateWindow(BUTTON,L"OK",WS_VISIBLE,rc.w-80,8,68,32,hwnd,ID_OK,NULL,NULL); //创建一个按钮(示例).

			rc.w=80;
			rc.h=40;
			rc.x=-10;
			rc.y=-20;
			CreateWindow(BUTTON,L"Button1",WS_DLGFRAME|WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,ID_BTN1,NULL,NULL);

			OffsetRect(&rc,0,rc.h+20);
			CreateWindow(BUTTON,L"Button2",WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,ID_BTN2,NULL,NULL);

			OffsetRect(&rc,0,rc.h+20);
			CreateWindow(BUTTON,L"Button3",WS_DLGFRAME|WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,ID_BTN3,NULL,NULL);
#endif
			rc.w=120;
			rc.h=80;
			rc.x=20;
			rc.y=150;
			wnd1=CreateWindow(BUTTON,L"BTN1",WS_CLIPSIBLINGS|WS_BORDER|WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,5001,NULL,NULL);
			OffsetRect(&rc,30,45);
			wnd2=CreateWindow(BUTTON,L"BTN2",WS_CLIPSIBLINGS|WS_BORDER|WS_DLGFRAME|WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,5002,NULL,NULL);
			OffsetRect(&rc,30,45);
			wnd3=CreateWindow(BUTTON,L"BTN3",WS_CLIPSIBLINGS|WS_BORDER|WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,5003,NULL,NULL);
			OffsetRect(&rc,30,45);


			DestroyWindow(wnd2);
			//_InsertWindow_Zafter(wnd2,wnd3);
			//_InsertWindow_Zafter(wnd1,wnd3);

			//HWND_Remove(wnd2);

			//_InsertWindow_Zbefore(wnd2,wnd1);

#if 1
			/////
			wcex_dlg.Tag 		    = WNDCLASS_TAG;

			wcex_dlg.Style			= CS_HREDRAW | CS_VREDRAW;
			wcex_dlg.lpfnWndProc	= DlgProc; //设置主窗口消息处理的回调函数.
			wcex_dlg.cbClsExtra		= 0;
			wcex_dlg.cbWndExtra		= 0;
			wcex_dlg.hInstance		= NULL;
			wcex_dlg.hIcon			= NULL;
			wcex_dlg.hCursor		= NULL;

			rc.w =200;
			rc.h =64;
			rc.x =300;
			rc.y =120;
			CreateWindow(&wcex_dlg,L"Win1",WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,0x2000,NULL,NULL);

			OffsetRect(&rc,+70,30);
			CreateWindow(&wcex_dlg,L"Win1",WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,0x2001,NULL,NULL);

			OffsetRect(&rc,+70,30);
			CreateWindow(&wcex_dlg,L"Win1",WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,0x2002,NULL,NULL);
#endif
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
		case	WM_CREATE_OWNERDC:
		{
			HDC hdc=NULL;
			HWND wnd;

			wnd =(HWND)lParam;

			if(wnd==hwnd)
			{
				//return NULL;
			}

			if(wParam==OWNERDC_WINDOW)
			{
				return NULL;
				//GetWindowRect(wnd,&rc);
				//ScreenToWindow(hwnd,(POINT*)&rc.x,1);
				//hdc =CreateDC(pSurf,&rc);
			}

			if(wParam==OWNERDC_CLIENT)
			{
				GetClientRect(wnd,&rc);
				ClientToScreen(wnd,(POINT*)&rc.x,1);
				ScreenToWindow(hwnd,(POINT*)&rc.x,1);
				hdc =CreateDC(pSurf,&rc);
			}

			return (LRESULT)hdc;
		}
		break;
		////

		case	WM_DELETE_OWNERDC:
		{
			HDC hdc;
			HWND wnd;
			RECT rc;

			hdc =(HDC)wParam;
			wnd =(HWND)lParam;

			GetClientRect(wnd,&rc);
			ClientToScreen(wnd,(POINT*)&rc.x,1);
#if 1
			//if(wnd==hwnd)
			{
				HDC hdc0;

				hdc0 =CreateDC(NULL,NULL);
				BitBlt(hdc0,rc.x,rc.y,rc.w,rc.h,hdc,0,0,SRCCOPY);
				DeleteDC(hdc0);
			}
		//	else
			{
				//ScreenToClient(hwnd,(POINT*)&rc.x,1);
				//InvalidateRect(hwnd,&rc,FALSE);
				//printf("0");

				//InvalidateRect(hwnd,NULL,FALSE);
			}
#endif

			DeleteDC(hdc);

		}
		break;

		case WM_ERASEBKGND:
		{
			HDC hdc =(HDC)wParam;
			RECT rc =*(RECT*)lParam;

			SetBrushColor(hdc,MapRGB(hdc,200,200,240));
			FillRect(hdc,&rc);

			rc.x =-4;
			rc.y =-8;
			rc.w =40;
			rc.h =20;
			SetBrushColor(hdc,MapRGB(hdc,0,255,0));
			FillRect(hdc,&rc);

			return TRUE;
		}
		break;
		///

		case WM_PAINT: //窗口需要绘制时，会自动产生该消息.
		{
			PAINTSTRUCT ps;
			HDC hdc,hdc0;

			GetClientRect(hwnd,&rc);
			hdc =BeginPaint(hwnd,&ps); //开始绘图

		//	hdc0 =CreateDC(pSurf,NULL);
			////用户的绘制内容...
			TextOut(hdc,-4,-8,L"Hello",-1);
		//	BitBlt(hdc,0,0,rc.w,rc.h,hdc0,0,0,SRCCOPY);

		//	DeleteDC(hdc0);

			EndPaint(hwnd,&ps); //结束绘图
		}
		break;
		////

		case WM_CLOSE: //窗口关闭时，会自动产生该消息.
		{
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

void	GUI_DEMO_OwnerDC(void)
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
								_T("GUI_DEMO: OwnerDC"), //窗口名称
								WS_OVERLAPPEDWINDOW,
								//WS_CLIPCHILDREN|WS_WINSURFACE,
								10,20,600,400,    //窗口位置和大小
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
