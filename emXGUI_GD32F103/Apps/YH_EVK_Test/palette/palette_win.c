
/*
 *palette_win.c
 *2018年5月15日下午3:57:43
 *
 */

#include "emXGUI.h"
#include "palette.h"

/*============================================================================*/
//定义控件ID
#define	ID_OK		0x1000

/*============================================================================*/
void Touch_Button_Down(uint16_t x,uint16_t y);
void Draw_Trail(int16_t pre_x,int16_t pre_y,int16_t x,int16_t y,Brush_Style* brush);
void Touch_Button_Up(uint16_t x,uint16_t y);

HDC hdc_pal=NULL;

#define GTP_MAX_TOUCH         5
static s16 pre_x[GTP_MAX_TOUCH] ={-1,-1,-1,-1,-1};
static s16 pre_y[GTP_MAX_TOUCH] ={-1,-1,-1,-1,-1};


/*============================================================================*/

static LRESULT	WinProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	RECT rc;

	switch(msg)
	{
		case WM_CREATE: //窗口创建时,会自动产生该消息,在这里做一些初始化的操作或创建子窗口.
		{
			GetClientRect(hwnd,&rc); //获得窗口的客户区矩形.

			hdc_pal =CreateMemoryDC(BM_RGB565,rc.w,rc.h); //创建一个与主窗口客户区大小的内存型绘图设备,给"画板"使用.
			Palette_Init();

			CreateWindow(BUTTON,L"退出",WS_VISIBLE,rc.w-80,8,68,32,hwnd,ID_OK,NULL,NULL); //创建一个按钮(示例).
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
				PostCloseMessage(hwnd); //产生WM_CLOSE消息关闭窗口
			}
		}
		break;
		////

		case WM_MOUSEMOVE:
		case WM_LBUTTONDOWN:
		{
			u32 key;
			s16 x,y;
			int id=0;

			key =wParam;
			x=LOWORD(lParam);
			y=HIWORD(lParam);

			if(key&MK_LBUTTON)
			{
				Touch_Button_Down(x,y);

				Draw_Trail(pre_x[id],pre_y[id],x,y,&brush);

				pre_x[id] = x; pre_y[id] =y;

				InvalidateRect(hwnd,NULL,FALSE);

			}
		}
		return DefWindowProc(hwnd,msg,wParam,lParam);
		////

		case WM_LBUTTONUP:
		{
			u32 key;
			s16 x,y;
			int id=0;

			key =wParam;
			x=LOWORD(lParam);
			y=HIWORD(lParam);

			Touch_Button_Up(x,y);
			pre_x[id] =-1;
			pre_y[id] =-1;

		}
		return DefWindowProc(hwnd,msg,wParam,lParam);
		////
		case WM_ERASEBKGND:
		{
			return TRUE;
		}
		////


		case WM_PAINT: //窗口需要绘制时，会自动产生该消息.
		{
			PAINTSTRUCT ps;
			HDC hdc;

			hdc =BeginPaint(hwnd,&ps);

			////用户的绘制内容...
			//TextOut(hdc,10,10,L"Hello",-1);
			GetClientRect(hwnd,&rc);
			BitBlt(hdc,rc.x,rc.y,rc.w,rc.h,hdc_pal,0,0,SRCCOPY); //将"画板"所在的DC内容输出到窗口上.

			EndPaint(hwnd,&ps);
		}
		break;
		////

		case WM_CLOSE: //窗口关闭时，会自动产生该消息.
		{
			///用户在这里做一些资源释放的操作.

			DeleteDC(hdc_pal); //删除“画板"的DC
			return DestroyWindow(hwnd); //调用DestroyWindow函数销毁窗口，该函数会使主窗口结束并退出消息循环.
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

void	palette_win(void)
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
								_T("Palette"), //窗口名称
								WS_CLIPCHILDREN,
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
