


#include "emXGUI.h"
#include "x_file.h"
#include "x_commdlg.h"
#include "libsdk_png.h"

#include "Include.h"

#define	x_malloc	GUI_MEM_Alloc
#define	x_free		GUI_MEM_Free


/*============================================================================*/
//定义控件ID
#define	ID_EXIT		0x1000
#define	ID_OPEN		0x1001

/*============================================================================*/
//static char file_path[256];
static char *file_path=NULL;

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
	RECT rc,rc0;

	switch(msg)
	{
		case WM_CREATE: //窗口创建时,会自动产生该消息,在这里做一些初始化的操作或创建子窗口.
		{

			GetClientRect(hwnd,&rc0); //获得窗口的客户区矩形.

			rc.w =80;
			rc.h =30;
			rc.x =rc0.w-rc.w-4;
			rc.y =8;
			CreateWindow(BUTTON,L"Exit",WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,ID_EXIT,NULL,NULL);

			OffsetRect(&rc,0,rc.h+4);
			CreateWindow(BUTTON,L"Open",WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,ID_OPEN,NULL,NULL);
		}
		return TRUE;
		////

		case WM_NOTIFY: //WM_NOTIFY消息:wParam低16位为发送该消息的控件ID,高16位为通知码;lParam指向了一个NMHDR结构体.
		{
			u16 code,id;

			code =HIWORD(wParam); //获得通知码类型.
			id   =LOWORD(wParam); //获得产生该消息的控件ID.

			if(id==ID_OPEN && code==BN_CLICKED) // 按钮“单击”了.
			{
				if(CDlg_FileExpoler(hwnd,20,30,300,300,L"Open PNG File","B:",file_path))
				{

					InvalidateRect(hwnd,NULL,FALSE);
				}
			}

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
			RECT rc;
			WCHAR wbuf[128];

			GetClientRect(hwnd,&rc);
			hdc =BeginPaint(hwnd,&ps); //开始绘图

			////用户的绘制内容...
			back_init(hdc,rc.w,rc.h,60);

			if(1)
			{

				X_FILE *fp;
				int fsize,t0,t1;
				char *buf=NULL;

				fp =x_fopen(file_path,"rb");
				if(fp!=NULL)
				{

					fsize =x_fsize(fp);
					buf =vmalloc(fsize);
					x_fread(buf,fsize,1,fp);
					x_fclose(fp);
				}

				if(buf!=NULL)
				{

					U16 w,h;

					t0 =GUI_GetTickCount();

#if 1
				{ //直接PNG解码显示.
					BITMAPINFO bm;

					if(PNG_GetInfo(&bm,buf))
					{
						PNG_Draw(hdc,(rc.w-(int)bm.Width)/2,(rc.h-(int)bm.Height)/2,buf);
					}
				}

#endif

#if 0
				{//PNG解码成BITMAP再显示.
					PNG_DEC *dec;

					BITMAP bm;
					dec =PNG_DecodeOpen(buf);
					if(dec)
					{

						PNG_DecodeGetBitmap(dec,&bm);
						DrawBitmap(hdc,(rc.w-(int)bm.Width)/2,(rc.h-(int)bm.Height)/2,&bm,NULL);
						PNG_DecodeClose(dec);
					}
				}

#endif

					t1 =GUI_GetTickCount();

					vfree(buf);

					////
					x_wsprintf(wbuf,L"PNG Time: %dms",t1-t0);
					SetTextColor(hdc,MapRGB(hdc,200,10,10));
					TextOut(hdc,10,20,wbuf,-1);
				}
			}

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

void	GUI_DEMO_PNG(void)
{
	HWND	hwnd;
	WNDCLASS	wcex;
	MSG msg;

	/////

	file_path =x_malloc(256);
	if(file_path==NULL)
	{
		return;
	}
	memset(file_path,0,256);


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
								_T("emXGUI DEMO: PNG Decode"), //窗口名称
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

	x_free(file_path);
}

/*============================================================================*/
