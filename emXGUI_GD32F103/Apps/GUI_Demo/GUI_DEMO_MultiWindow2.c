
/*
 *GUI_DEMO_MultiWindow2.c
 *2018年12月24日下午2:29:04
 *
 */

#include <stdlib.h>

#include "emXGUI.h"
#include "x_libc.h"

/*============================================================================*/
//定义控件ID
enum eID
{
	eID_OK =0x1000,
	eID_SET,
	eID_SET1,
	eID_SET2,
	eID_SET3,
	eID_SET4,

	eID_BTN1,
	eID_BTN2,

};

/*============================================================================*/

HDC BeginPaintEx(HWND hwnd, PAINTSTRUCT *lpPaint,SURFACE *pSurf,const RECT *prc);

/*============================================================================*/
//#define WS_TOPSURFACE       0x00040000UL

//static SURFACE *pSurfTop=NULL;
static SURFACE *pSurfDlg=NULL;

/*============================================================================*/

static LRESULT	dlg_set_xx_WinProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	switch(msg)
	{
		case WM_CREATE: //窗口创建时,会自动产生该消息,在这里做一些初始化的操作或创建子窗口.
		{

		}
		return TRUE;
		////
		case WM_PAINT: //窗口需要绘制时，会自动产生该消息.
		{
			PAINTSTRUCT ps;
			HDC hdc;
			RECT rc;
			hdc =BeginPaint(hwnd,&ps); //开始绘图

			////用户的绘制内容...

			SetTextColor(hdc,MapRGB(hdc,250,250,250));
			SetBrushColor(hdc,MapRGB(hdc,240,10,10));


			rc.x =10;
			rc.y =20;
			rc.w =200;
			rc.h =24;
			DrawText(hdc,L"Set xxx..",-1,&rc,DT_CENTER|DT_VCENTER|DT_BKGND);
			//TextOut(hdc,10,10,L"Hello",-1);

			EndPaint(hwnd,&ps); //结束绘图


		}
		break;

		default: //用户不关心的消息,由系统处理.
		{
			return DefWindowProc(hwnd,msg,wParam,lParam);
		}

	}
	return WM_NULL;

}

/*============================================================================*/

static RECT win_rc;
static int b_close=FALSE;

static LRESULT	dlg_set_WinProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	RECT rc,rc0;

	switch(msg)
	{
		case WM_CREATE: //窗口创建时,会自动产生该消息,在这里做一些初始化的操作或创建子窗口.
		{


			b_close =FALSE;

			GetClientRect(hwnd,&rc0); //获得窗口的客户区矩形.
			CreateWindow(BUTTON,L"OK",WS_VISIBLE,rc0.w-80,8,68,32,hwnd,eID_OK,NULL,NULL);


			rc.x =20;
			rc.y =80;
			rc.w =120;
			rc.h =30;
			CreateWindow(BUTTON,L"Set1",WS_VISIBLE|WS_TRANSPARENT,rc.x,rc.y,rc.w,rc.h,hwnd,eID_SET1,NULL,NULL);

			OffsetRect(&rc,0,rc.h+4);
			CreateWindow(BUTTON,L"Set2",WS_VISIBLE|WS_TRANSPARENT,rc.x,rc.y,rc.w,rc.h,hwnd,eID_SET2,NULL,NULL);

			OffsetRect(&rc,0,rc.h+4);
			CreateWindow(BUTTON,L"Set3",WS_VISIBLE|WS_TRANSPARENT,rc.x,rc.y,rc.w,rc.h,hwnd,eID_SET3,NULL,NULL);

			OffsetRect(&rc,0,rc.h+4);
			CreateWindow(BUTTON,L"Set4",WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,eID_SET4,NULL,NULL);

			//SetTimer(hwnd,1,500,TMR_START,NULL);
			SetTimer(hwnd,2,20,TMR_START,NULL);

		}
		return TRUE;

		case WM_TIMER:
		{
			switch(wParam)
			{

				case 1:
				{
					//tmr_500ms++;
					//InvalidateRect(hwnd,NULL,FALSE);

				}
				break;
				////

				case 2:
				{
					if(GetKeyState(VK_LBUTTON)!=0)
					{
						break;
					}

					////
					GetWindowRect(hwnd,&rc);

					if(b_close==FALSE)
					{
						if(rc.y < (win_rc.y+40) )
						{
							if((win_rc.y-rc.y)>50)
							{
								rc.y +=30;
							}
							if((win_rc.y-rc.y)>30)
							{
								rc.y +=20;
							}
							else
							{
								rc.y +=4;
							}
							ScreenToClient(GetParent(hwnd),(POINT*)&rc.x,1);
							MoveWindow(hwnd,rc.x,rc.y,rc.w,rc.h,TRUE);
						}
					}
					else
					{
						if(rc.y > -(rc.h))
						{
							rc.y -= 40;

							ScreenToClient(GetParent(hwnd),(POINT*)&rc.x,1);
							MoveWindow(hwnd,rc.x,rc.y,rc.w,rc.h,TRUE);
						}
						else
						{
							PostCloseMessage(hwnd);
						}
					}


				}
				break;
			}
		}
		break;
		////

		case WM_NOTIFY: //WM_NOTIFY消息:wParam低16位为发送该消息的控件ID,高16位为通知码;lParam指向了一个NMHDR结构体.
		{
			u16 code,id;

			code =HIWORD(wParam); //获得通知码类型.
			id   =LOWORD(wParam); //获得产生该消息的控件ID.

			if(id==eID_SET1 && code==BN_CLICKED)
			{
				WNDCLASS wcex;

				GetWindowRect(GetDlgItem(hwnd,eID_SET1),&rc);
				ScreenToClient(hwnd,(POINT*)&rc.x,1);
				rc.x += rc.w+1;
				rc.w =220;
				rc.h =100;

				wcex.Tag	 		= WNDCLASS_TAG;
				wcex.Style			= CS_HREDRAW | CS_VREDRAW;
				wcex.lpfnWndProc	= (WNDPROC)dlg_set_xx_WinProc;
				wcex.cbClsExtra		= 0;
				wcex.cbWndExtra		= 0;
				wcex.hInstance		= NULL;
				wcex.hIcon			= NULL;
				wcex.hCursor		= NULL;


				CreateWindowEx(
								WS_EX_FRAMEBUFFER,
								&wcex,L"Set_1_xxx",
								WS_OVERLAPPED|WS_CLIPCHILDREN|WS_CAPTION|WS_DLGFRAME|WS_BORDER|WS_VISIBLE|WS_CLOSEBOX,
								rc.x,rc.y,rc.w,rc.h,
								hwnd,0,NULL,NULL);

			}

			if(id==eID_SET2 && code==BN_CLICKED)
			{
				WNDCLASS wcex;

				GetWindowRect(GetDlgItem(hwnd,eID_SET2),&rc);
				ScreenToClient(hwnd,(POINT*)&rc.x,1);
				rc.x += rc.w+1;
				rc.w =220;
				rc.h =100;

				wcex.Tag	 		= WNDCLASS_TAG;
				wcex.Style			= CS_HREDRAW | CS_VREDRAW;
				wcex.lpfnWndProc	= (WNDPROC)dlg_set_xx_WinProc;
				wcex.cbClsExtra		= 0;
				wcex.cbWndExtra		= 0;
				wcex.hInstance		= NULL;
				wcex.hIcon			= NULL;
				wcex.hCursor		= NULL;


				CreateWindowEx(
								WS_EX_FRAMEBUFFER,
								&wcex,L"Set_2_xxx",
								WS_OVERLAPPED|WS_CLIPCHILDREN|WS_CAPTION|WS_DLGFRAME|WS_BORDER|WS_VISIBLE|WS_CLOSEBOX,
								rc.x,rc.y,rc.w,rc.h,
								hwnd,0,NULL,NULL);

			}

			if(id==eID_OK && code==BN_CLICKED) // 按钮“单击”了.
			{
				PostCloseMessage(hwnd); //使产生WM_CLOSE消息关闭窗口.
			}
		}
		break;
		////

		case WM_ERASEBKGND:
		{
			HDC hdc =(HDC)wParam;
			RECT rc =*(RECT*)lParam;
			EnableAlpha(hdc,TRUE);
			SetAlpha(hdc,128);
			SetBrushColor(hdc,MapARGB(hdc,100,0,200,0));
			FillRect(hdc,&rc);

		}
		return TRUE;

		case WM_PAINT: //窗口需要绘制时，会自动产生该消息.
		{
			PAINTSTRUCT ps;
			HDC hdc;
			RECT rc,rc0;

			////??
			GetClientRect(hwnd,&rc);
			rc0 =rc;
			ClientToScreen(hwnd,(POINT*)&rc0.x,1);

			hdc=BeginPaint(hwnd,&ps); //开始绘图

			//hdc =CreateDC(pSurfTop,&rc0);
			////???
			EnableAlpha(hdc,TRUE);

			SetAlpha(hdc,80);
			rc.x =0;
			rc.y =0;
			rc.w =300;
			rc.h =100;
			SetBrushColor(hdc,MapARGB(hdc,100,10,10,10));
			FillRect(hdc,&rc);

			////用户的绘制内容...
			SetAlpha(hdc,50);
			SetTextColor(hdc,MapRGB(hdc,240,10,10));
			SetBrushColor(hdc,MapARGB(hdc,100,250,250,250));

			rc.x =10;
			rc.y =20;
			rc.w =200;
			rc.h =24;
			DrawText(hdc,L"Set...",-1,&rc,DT_CENTER|DT_VCENTER|DT_BKGND);
			//TextOut(hdc,10,10,L"Hello",-1);

			EndPaint(hwnd,&ps); //结束绘图
/*
			if(1)
			{
				HWND wnd;
				wnd =GetParent(hwnd);

				GetClientRect(hwnd,&rc);
				ClientToScreen(hwnd,(POINT*)&rc.x,1);
				ScreenToClient(wnd,(POINT*)&rc.x,1);
				InvalidateRect(wnd,&rc,TRUE);
			}*/
			//RedrawWindow(hwnd,NULL,RDW_ALLCHILDREN);
		}
		break;
		////
		case WM_CLOSE:
		{
			b_close=TRUE;

			GetWindowRect(hwnd,&rc);

			if(rc.y <= -(rc.h))
			{
				DestroyWindow(hwnd);
				return TRUE;
			}
			return FALSE;
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

WNDPROC def_btn_proc;

static int paint_n=0;
static int win_main_count=0;
static int xx,yy;x_inc,y_inc;

static LRESULT	BTN_WinProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{

	RECT rc,rc0;
	switch(msg)
	{
		case WM_ERASEBKGND:
		return FALSE;

		case WM_PAINT:
		{
			HDC hdc;
			PAINTSTRUCT ps;
			WCHAR wbuf[128];
/*
			if(1)
			{
				HWND wnd;
				wnd =GetParent(hwnd);


				GetClientRect(hwnd,&rc);
				ClientToScreen(hwnd,(POINT*)&rc.x,1);
				ScreenToClient(wnd,(POINT*)&rc.x,1);

				InvalidateRect(wnd,&rc,TRUE);
				UpdateWindow(wnd);
			}
*/

			GetClientRect(hwnd,&rc);
			hdc=BeginPaint(hwnd,&ps);


			rc0 =rc;
			ClientToScreen(hwnd,(POINT*)&rc0.x,1);


			//hdc =CreateDC(pSurfTop,&rc0);
			EnableAlpha(hdc,TRUE);
			//SetAlpha(hdc,255);
			//SetBrushColor(hdc,MapARGB(hdc,0,0,0,0));
			//FillRect(hdc,&rc);

			SetAlpha(hdc,255);
			SetPenColor(hdc,MapRGB(hdc,0,0,100));
			DrawRoundRect(hdc,&rc,8);

			InflateRect(&rc,-1,-1);
			SetAlpha(hdc,100);
			if(SendMessage(hwnd,BM_GETSTATE,0,0)&BST_PUSHED)
			{
				SetBrushColor(hdc,MapRGB(hdc,100,100,100));
			}
			else
			{
				SetBrushColor(hdc,MapRGB(hdc,240,200,200));
			}
			FillRoundRect(hdc,&rc,8);

			SetAlpha(hdc,255);
			SetTextColor(hdc,MapRGB(hdc,240,0,0));
			GetWindowText(hwnd,wbuf,128);
			DrawText(hdc,wbuf,-1,&rc,DT_VCENTER|DT_CENTER);



			EndPaint(hwnd,&ps);
#if 0
			//DeleteDC(hdc);


			if(1)
			{
				HWND wnd;
				wnd =GetParent(hwnd);

				GetClientRect(hwnd,&rc);
				ClientToScreen(hwnd,(POINT*)&rc.x,1);
				ScreenToClient(wnd,(POINT*)&rc.x,1);
				InvalidateRect(wnd,&rc,TRUE);
			}
#endif


			return TRUE;
		}
		break;

		default:
			return def_btn_proc(hwnd,msg,wParam,lParam);
	}
}

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

static LRESULT	WinProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	RECT rc;
	HWND wnd;

	switch(msg)
	{
		case WM_CREATE: //窗口创建时,会自动产生该消息,在这里做一些初始化的操作或创建子窗口.
		{
			xx=0;
			yy=0;
			x_inc=4;
			y_inc=4;
			paint_n=0;
			win_main_count=0;

			GetClientRect(hwnd,&rc); //获得窗口的客户区矩形.

			CreateWindow(BUTTON,L"OK",WS_VISIBLE,rc.w-80,8,68,32,hwnd,eID_OK,NULL,NULL);
			CreateWindow(BUTTON,L"Set",WS_VISIBLE,rc.w-80,rc.h-32-2,68,32,hwnd,eID_SET,NULL,NULL);

			wnd=CreateWindow(BUTTON,L"Button1",WS_CAPTION|WS_VISIBLE|WS_TRANSPARENT,100,100,100,60,hwnd,eID_BTN1,NULL,NULL);
			def_btn_proc =SetWindowProc(wnd,BTN_WinProc);

			wnd=CreateWindow(BUTTON,L"Button2",WS_CAPTION|WS_VISIBLE|WS_TRANSPARENT,100,180,100,80,hwnd,eID_BTN2,NULL,NULL);
			def_btn_proc =SetWindowProc(wnd,BTN_WinProc);

			//SetTimer(hwnd,1,1000,TMR_START,NULL);

		}
		return TRUE;
		////

		case WM_TIMER:
		{
			u16 tmr_id=wParam;
			if(tmr_id==1)
			{
				win_main_count++;

				GetClientRect(hwnd,&rc);

				if(xx>rc.w)
				{
					x_inc = -abs(x_inc);
				}

				if(xx<rc.x)
				{
					x_inc = abs(x_inc);
				}

				if(yy>rc.h)
				{
					y_inc =-abs(y_inc);
				}

				if(yy<rc.y)
				{
					y_inc =abs(y_inc);
				}

				xx += x_inc;
				yy += y_inc;

				InvalidateRect(hwnd,NULL,FALSE);
			}
		}
		break;
		////

		case WM_NOTIFY: //WM_NOTIFY消息:wParam低16位为发送该消息的控件ID,高16位为通知码;lParam指向了一个NMHDR结构体.
		{
			u16 code,id;

			code =HIWORD(wParam); //获得通知码类型.
			id   =LOWORD(wParam); //获得产生该消息的控件ID.

			if(id==eID_SET && code==BN_CLICKED)
			{
				WNDCLASS wcex;


				wcex.Tag	 		= WNDCLASS_TAG;
				wcex.Style			= CS_HREDRAW | CS_VREDRAW;
				wcex.lpfnWndProc	= (WNDPROC)dlg_set_WinProc;
				wcex.cbClsExtra		= 0;
				wcex.cbWndExtra		= 0;
				wcex.hInstance		= NULL;
				wcex.hIcon			= NULL;
				wcex.hCursor		= NULL;

				if(1)
				{
					RECT rc;

					GetClientRect(hwnd,&rc);
					//ClientToScreen(hwnd,(POINT*)&rc.x,1);

					win_rc.w =400;
					win_rc.h =280;

					win_rc.x = rc.x+(rc.w-win_rc.w)/2;
					win_rc.y = rc.y;//rc.y+(rc.h>>2);

					CreateWindowEx(
									NULL,
									&wcex,L"Set",
									WS_VISIBLE
									|WS_CAPTION
									|WS_TRANSPARENT
									|WS_CLIPCHILDREN,

									win_rc.x,-win_rc.h,win_rc.w,win_rc.h,
									hwnd,0,NULL,NULL);
				}

			}


			if(id==eID_OK && code==BN_CLICKED) // 按钮“单击”了.
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
			RECT rc;
			WCHAR wbuf[128];


			hdc =BeginPaint(hwnd,&ps); //开始绘图

			GetClientRect(hwnd,&rc);
			back_init(hdc,rc.w,rc.h);

			//SetBrushColor(hdc,MapRGB(hdc,200,200,200));
			//FillRect(hdc,&rc);

			////用户的绘制内容...
			x_wsprintf(wbuf,L"Count: %d\r\nPaint:%d",win_main_count,paint_n++);
			SetTextColor(hdc,MapRGB(hdc,240,10,10));
			SetBrushColor(hdc,MapRGB(hdc,10,50,70));
			SetPenColor(hdc,MapRGB(hdc,200,200,250));




			rc.x =xx;
			rc.y =yy;
			rc.w =200;
			rc.h =50;
			DrawText(hdc,wbuf,-1,&rc,DT_CENTER|DT_VCENTER|DT_BKGND|DT_BORDER);
			//TextOut(hdc,10,10,L"Hello",-1);

			SetTextColor(hdc,MapRGB(hdc,240,240,10));
			SetBrushColor(hdc,MapRGB(hdc,0,100,100));
			SetPenColor(hdc,MapRGB(hdc,100,200,100));
			rc.x =200;
			rc.y =100;
			rc.w =150;
			rc.h =80;
			DrawText(hdc,wbuf,-1,&rc,DT_CENTER|DT_VCENTER|DT_BKGND|DT_BORDER);
			//TextOut(hdc,10,10,L"Hello",-1);

			//RedrawWindow(hwnd,NULL,RDW_ALLCHILDREN);
			//RedrawWindow(hwnd,NULL,RDW_ALLCHILDREN|RDW_UPDATENOW);
			//UpdateTop(hwnd,hdc);
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

void	GUI_DEMO_MultiWindow2(void)
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
								_T("GUI_DEMO: MultiWindow2"), //窗口名称
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

/*============================================================================*/
