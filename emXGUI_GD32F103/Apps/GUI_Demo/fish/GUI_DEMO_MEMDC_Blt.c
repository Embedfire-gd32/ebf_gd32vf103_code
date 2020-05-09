
/*
 *GUI_DEMO_MEMDC_Blt.c
 *2018年9月29日下午4:23:09
 *
 */

#include "emXGUI.h"
//#include "GUI_Font_XFT.h"
#include "x_libc.h"
/*============================================================================*/
extern const unsigned char gImage_0[];
extern const unsigned char gImage_1[];
extern const unsigned char gImage_5[];

extern const char ASCII_24_4BPP[];
extern const char ASCII_32_4BPP[];
extern const char ASCII_40_4BPP[];

/*============================================================================*/
#define	MEMDC_W	GUI_XSIZE  //MEMDC宽度.
#define	MEMDC_H GUI_YSIZE  //MEMDC高度.
#define	BGCOLOR	RGB888(10,50,70)  //背景色(RGB888).


#define OBJNUM     10 //显示的对象数量.
#define	SPEED_MIN	1 //移动速度最小值.
#define	SPEED_MAX	4 //移动速度最大值.
#define SIZE_MIN 80
#define SIZE_MAX 150

//定义控件ID
enum	eID{
	ID_EXIT =0x1000,
	ID_NEXT,
	ID_TITLE,
	ID_FPS,

	ID_RANDOM,
	ID_BITMAP,
	ID_TEXT,
	ID_RECT,

};

/*============================================================================*/


static 	RECT	ObjRect[OBJNUM];
static 	u32		ObjColor[OBJNUM];
static  HFONT   ObjFont[OBJNUM];
static  BITMAP *ObjBitmap[OBJNUM];
static 	u16		ObjType[OBJNUM];
static 	s8		ObjActive[OBJNUM];
static 	s8		ObjSpeedX[OBJNUM];
static 	s8		ObjSpeedY[OBJNUM];

static HDC hdc_mem=NULL;
static BITMAP bm1,bm2,bm3;
static HFONT hFont24=NULL;
static HFONT hFont32=NULL;
static HFONT hFont40=NULL;
static int t0,frame,fps;

static int type_id=0;

static  HFONT *FontTbl[4]={

		&hFont32,
		&hFont24,
		&hFont32,
		&hFont40,

};

static  BITMAP *BitmapTbl[4]={

		&bm1,
		&bm2,
		&bm3,
		&bm1,
};

/*============================================================================*/

static void BitmapInit(void)
{
	bm1.Format	= BM_ARGB8888;
	bm1.Width = 92;
	bm1.Height = 184;
	bm1.WidthBytes =bm1.Width*4;
	bm1.LUT =NULL;
	bm1.Bits =(void*)gImage_0;

	bm2.Format	= BM_ARGB8888;
	bm2.Width = 130;
	bm2.Height = 260;
	bm2.WidthBytes =bm2.Width*4;
	bm2.LUT =NULL;
	bm2.Bits =(void*)gImage_1;

	bm3.Format = BM_ARGB8888;
	bm3.Width = 92;
	bm3.Height = 92;
	bm3.WidthBytes =bm3.Width*4;
	bm3.LUT =NULL;
	bm3.Bits =(void*)gImage_5;

}

static void DrawHandler(HDC hdc,int Width,int Height)
{
	int i;


	ClrDisplay(hdc,NULL,MapRGB888(hdc,BGCOLOR));

    for(i=0; i<OBJNUM; i++)
    {
	      if(   ObjRect[i].x <= -40
	    	 || ObjRect[i].x >=Width
		     || ObjRect[i].y <= 0
			 || ObjRect[i].y >=Height )
	      {
		        ObjActive[i] = 0;
	      }

          if(ObjActive[i] != 0)
	      {

        	  int x,y;

        	  x =ObjRect[i].x;
        	  y =ObjRect[i].y;

		       switch (ObjType[i])
	  	       {

						case 0:
						{
							WCHAR wbuf[20];
							//SetFont(hdc,hFont32);
							SetFont(hdc,ObjFont[i]);
							SetTextColor(hdc,MapXRGB8888(hdc,ObjColor[i]));
							x_wsprintf(wbuf,L"FPS:%d",fps);
							TextOut(hdc,x,y,wbuf,-1);
						}
						break;
						////

						case 1:
						{
							WCHAR wbuf[20];
							//SetFont(hdc,hFont32);
							SetFont(hdc,ObjFont[i]);
							SetTextColor(hdc,MapXRGB8888(hdc,ObjColor[i]));
							x_wsprintf(wbuf,L"X:%04d:Y:%04d",x,y);
							TextOut(hdc,x,y,wbuf,-1);
						}
						break;
						////
						case 2:
						{
							WCHAR wbuf[20];
							//SetFont(hdc,hFont40);
							SetFont(hdc,ObjFont[i]);
							SetTextColor(hdc,MapXRGB8888(hdc,ObjColor[i]));
							TextOut(hdc,x,y,L"emXGUI",-1);
						}
						break;
						////

						case 3:
						{
							SetBrushColor(hdc,MapXRGB8888(hdc,ObjColor[i]));
							FillRect(hdc,&ObjRect[i]);
						}
						break;
						////
						case 4:
						default:
							DrawBitmap(hdc,x,y,ObjBitmap[i],NULL);
							//DrawBitmap(hdc,x,y,&bm3,NULL);
							break;

	  	         }

		       ObjRect[i].x += ObjSpeedX[i];
		       ObjRect[i].y += ObjSpeedY[i];


	      }

          if (ObjActive[i]==0 )
          {
        	  	u32 rand_val;

        	  	rand_val =x_rand();

        	    ObjRect[i].x =rand_val%Width;
        	    ObjRect[i].y =1;
           	    ObjRect[i].w =SIZE_MIN + ((rand_val>>4)%SIZE_MAX);
           	    ObjRect[i].h =SIZE_MIN + ((rand_val>>8)%SIZE_MAX);


				ObjSpeedX[i] =(int)((int)x_rand()%SPEED_MAX)-(int)(x_rand()%SPEED_MAX);
				ObjSpeedY[i] =SPEED_MIN + ((rand_val>>6)%SPEED_MAX);

				switch(type_id)
				{
					case ID_TEXT:
						ObjType[i] =2;
						break;

					case ID_RECT:
						ObjType[i] =3;
						break;

					case ID_BITMAP:
						ObjType[i] =4;
						break;

					default:
						ObjType[i] = (rand_val>>2)%8;
						break;
				}

				rand_val =x_rand();
				ObjColor[i]   = rand_val;
				ObjFont[i]    = *FontTbl[(rand_val>>4)%ARR_SIZE(FontTbl)];
				ObjBitmap[i]  = BitmapTbl[(rand_val>>8)%ARR_SIZE(BitmapTbl)];
				ObjActive[i]  = 1;

          }
    }

}


static LRESULT	WinProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	RECT rc0,rc;
	HWND wnd;

	switch(msg)
	{
		case WM_CREATE: //窗口创建时,会自动产生该消息,在这里做一些初始化的操作或创建子窗口.
		{
			GetClientRect(hwnd,&rc0); //获得窗口的客户区矩形.

			BitmapInit(); //初始化位图数据

			hFont24 =XFT_CreateFont(ASCII_24_4BPP);
			hFont32 =XFT_CreateFont(ASCII_32_4BPP);
			hFont40 =XFT_CreateFont(ASCII_40_4BPP);

			type_id =ID_RANDOM;

			frame=0;
			fps =0;
			t0 =GUI_GetTickCount();
			hdc_mem =CreateMemoryDC(SURF_SCREEN,MEMDC_W,MEMDC_H);

			rc.w =130;
			rc.h =50;
			rc.x =rc0.w-rc.w-8;
			rc.y =8;
			wnd=CreateWindow(BUTTON,L"Exit",	BS_FLAT|WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,ID_EXIT,NULL,NULL); //创建一个按钮.
			SetWindowFont(wnd,hFont32); //设置控件窗口字体.

			OffsetRect(&rc,0,rc.h+20);
			wnd=CreateWindow(TEXTBOX,L"---FPS",TBS_FLAT|WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,ID_FPS,NULL,NULL); //创建一个文字框.
			SetWindowFont(wnd,hFont40); //设置控件窗口字体.

			OffsetRect(&rc,0,rc.h+20);
			wnd=CreateWindow(BUTTON,L"Random",BS_FLAT|BS_RADIOBOX|BS_PUSHLIKE|BS_NOTIFY|WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,ID_RANDOM,NULL,NULL);
			SetWindowFont(wnd,hFont24); //设置控件窗口字体.

			OffsetRect(&rc,0,rc.h+8);
			wnd=CreateWindow(BUTTON,L"Bitmap",BS_FLAT|BS_RADIOBOX|BS_PUSHLIKE|BS_NOTIFY|WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,ID_BITMAP,NULL,NULL);
			SetWindowFont(wnd,hFont24); //设置控件窗口字体.

			OffsetRect(&rc,0,rc.h+8);
			wnd=CreateWindow(BUTTON,L"Text",BS_FLAT|BS_RADIOBOX|BS_PUSHLIKE|BS_NOTIFY|WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,ID_TEXT,NULL,NULL);
			SetWindowFont(wnd,hFont24); //设置控件窗口字体.

			OffsetRect(&rc,0,rc.h+8);
			wnd=CreateWindow(BUTTON,L"Rectangle",BS_FLAT|BS_RADIOBOX|BS_PUSHLIKE|BS_NOTIFY|WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,ID_RECT,NULL,NULL);
			SetWindowFont(wnd,hFont24); //设置控件窗口字体.


			wnd=CreateWindow(TEXTBOX,L"MEMDC Blt",TBS_FLAT|WS_VISIBLE,0,0,200,40,hwnd,ID_TITLE,NULL,NULL); //创建一个文字框.
			SetWindowFont(wnd,hFont32); //设置控件窗口字体.


			SetTimer(hwnd,1,1,TMR_START,NULL);
		}
		return TRUE;
		////

		case WM_TIMER:
		{
			InvalidateRect(hwnd,NULL,FALSE); //触发下一个WM_PAINT消息.
		}
		break;
		////

		case WM_NOTIFY: //WM_NOTIFY消息:wParam低16位为发送该消息的控件ID,高16位为通知码;lParam指向了一个NMHDR结构体.
		{
			u16 code,id;

			code =HIWORD(wParam); //获得通知码类型.
			id   =LOWORD(wParam); //获得产生该消息的控件ID.

			if(id>=ID_RANDOM && id<=ID_RECT)
			{
				if(code == BN_CHECKED)
				{
					type_id =id;
				}
			}


			if(id==ID_EXIT && code==BN_CLICKED) // 按钮“单击”了.
			{
				PostCloseMessage(hwnd); //使产生WM_CLOSE消息关闭窗口.
			}
		}
		break;
		////

		case	WM_ERASEBKGND:
		{
			HDC hdc=(HDC)wParam;
			RECT rc;

			GetClientRect(hwnd,&rc);
			SetBrushColor(hdc,MapRGB888(hdc,BGCOLOR));
			FillRect(hdc,&rc);
			return TRUE;
		}
		break;
		////
		case WM_CTLCOLOR:
		{
			u16 id;

			id =LOWORD(wParam);

			if(id== ID_TITLE) //ID_TEXT控件的颜色.
			{
				CTLCOLOR *cr;
				cr =(CTLCOLOR*)lParam;

				cr->TextColor =RGB888(250,250,250);
				cr->BackColor =BGCOLOR;
				cr->BorderColor =RGB888(50,50,50);
				return TRUE;
			}
			else if(id== ID_FPS) //ID_FPS控件的颜色.
			{
				CTLCOLOR *cr;
				cr =(CTLCOLOR*)lParam;

				cr->TextColor =RGB888(255,255,0);
				cr->BackColor =RGB888(0,50,70);
				cr->BorderColor =RGB888(50,50,50);
				return TRUE;
			}
			else
			{
				//其它按钮使用系统默认的颜色进行绘制，所以直接返回FALSE.
				return FALSE;
			}
		}
		break;
		////

		case WM_PAINT: //窗口需要绘制时，会自动产生该消息.
		{
			PAINTSTRUCT ps;
			HDC hdc;
			int t;
			WCHAR wbuf[20];

			t =GUI_GetTickCount();
			if((t-t0) >= 1000)
			{
				fps =frame;

				t0 =t;
				frame=0;

				wnd =GetDlgItem(hwnd,ID_FPS);
				x_wsprintf(wbuf,L"%dFPS",fps);
				SetWindowText(wnd,wbuf);
			}
			else
			{
				frame++;
			}


			hdc =BeginPaint(hwnd,&ps); //开始绘图

			////用户的绘制内容...
			DrawHandler(hdc_mem,MEMDC_W,MEMDC_H);

			BitBlt(hdc,0,40,MEMDC_W,MEMDC_H,hdc_mem,0,0,SRCCOPY);

			EndPaint(hwnd,&ps); //结束绘图

			////
//			InvalidateRect(hwnd,NULL,FALSE); //触发下一个WM_PAINT消息.
//			GUI_msleep(5); //释放一下CPU,否则将全占CPU进行绘制.
		}
		break;
		////

		case WM_CLOSE: //窗口关闭时，会自动产生该消息.
		{

			DeleteDC(hdc_mem);
			DeleteFont(hFont24);
			DeleteFont(hFont32);
			DeleteFont(hFont40);
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

void	GUI_DEMO_MEMDC_Blt(void)
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
	hwnd	=CreateWindowEx(	WS_EX_LOCKPOS,
								&wcex,
								L"GUI_DEMO: MEMDC Blt", //窗口名称
								WS_CLIPCHILDREN,
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
