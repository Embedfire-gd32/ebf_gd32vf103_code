
/*
 *GUI_DEMO_RadiaMenu.c
 *2018年12月26日下午8:16:38
 *
 */

#include <math.h>
#include "emXGUI.h"

extern const unsigned char gImage_0[];
extern const unsigned char gImage_1[];
extern const unsigned char gImage_5[];

/*============================================================================*/
//定义控件ID
#define	ID_OK		0x1000

enum eMSG{
	MSG_MENU_CLICKED =WM_USER+1,

};
/*============================================================================*/

typedef struct {

  const int    id;
  const BITMAP *pBitmap;
  const WCHAR  *pText;
} BITMAP_ITEM;

typedef struct {
  int xPos;
  int yPos;
  int Index;
} ITEM_INFO;

typedef struct {
  int                 Pos;
  int                 NumItems;
  int                 xSizeItem;
  int                 ySizeItem;
  int                 xSizeWin;
  int                 ySizeWin;
  int                 rx;
  int                 ry;
  int                 mx;
  int                 my;
  int                 FinalMove;
  const BITMAP_ITEM *pBitmapItem;
  ITEM_INFO         *pItemInfo;

} PARA;

/*============================================================================*/

static 	BITMAP bm_fish_0;
static 	BITMAP bm_fish_1;
static 	BITMAP bm_fish_5;

static int _x0,_y0,move_n;

#define	ITEM_W	100
#define	ITEM_H	100
#define	MOVE_STEP	100

#define	BK_COLOR	RGB888(10,10,100)
#define SHIFT_RIGHT_16(x)    ((x) / 1000)

static PARA  Para;
static PARA  *pPara;
static int _xPosRect = 0;
static int _yPosRect = 0;

static const BITMAP_ITEM _aBitmapItem[] = {

		{0x1001, &bm_fish_0,  L"Menu1" },
		{0x1002, &bm_fish_0,  L"Menu2" },
		{0x1003, &bm_fish_0,  L"Menu3" },
		{0x1004, &bm_fish_0,  L"Menu4" },
		{0x1005, &bm_fish_0,  L"Menu5" },
		{0x1006, &bm_fish_0,  L"Menu6" },
		{0x1007, &bm_fish_0,  L"Menu7" },
		{0x1008, &bm_fish_1,  L"Menu8" },
		{0x1008, &bm_fish_5,  L"Menu9" },

};

static ITEM_INFO   aItemInfo[ARR_SIZE(_aBitmapItem)];


int  GUI__CosHQ(int Ang1000)
{

	double f=Ang1000;
	double v;

	v= cos((2*3.1416*f)/360000);
	v *=1000;
	return (int)(v);
}

int  GUI__SinHQ(int Ang1000)
{
	double f=Ang1000;
	double v;

	v= sin((2*3.1416*f)/360000);
	v *=1000;
	return (int)(v);
}

static void _Init(int x,int y,int w,int h)
{

	move_n =0;

	bm_fish_0.Format	= BM_ARGB8888;
	bm_fish_0.Width = 92;
	bm_fish_0.Height = 184;
	bm_fish_0.WidthBytes =bm_fish_0.Width*4;
	bm_fish_0.LUT =NULL;
	bm_fish_0.Bits =(void*)gImage_0;

	bm_fish_1.Format	= BM_ARGB8888;
	bm_fish_1.Width = 130;
	bm_fish_1.Height = 260;
	bm_fish_1.WidthBytes =bm_fish_1.Width*4;
	bm_fish_1.LUT =NULL;
	bm_fish_1.Bits =(void*)gImage_1;

	bm_fish_5.Format = BM_ARGB8888;
	bm_fish_5.Width = 92;
	bm_fish_5.Height = 92;
	bm_fish_5.WidthBytes =bm_fish_5.Width*4;
	bm_fish_5.LUT =NULL;
	bm_fish_5.Bits =(void*)gImage_5;


	pPara =&Para;
	pPara->NumItems    = ARR_SIZE(_aBitmapItem);
	pPara->pBitmapItem = _aBitmapItem;
	pPara->pItemInfo   = aItemInfo;
	pPara->Pos         = 0;

	pPara->xSizeWin  = w;
	pPara->ySizeWin  = h;

	pPara->xSizeItem = ITEM_W;//pPara->pBitmapItem[0].pBitmap->Width;
	pPara->ySizeItem = ITEM_H;//pPara->pBitmapItem[0].pBitmap->Height;

	pPara->rx        = x+(pPara->xSizeWin - pPara->xSizeItem) / 2;
	pPara->ry        = y+(pPara->ySizeWin - pPara->ySizeItem) / 2;
	//pPara->mx        = pPara->rx;//x+pPara->xSizeWin / 2;
	//pPara->my        = pPara->ry;// + pPara->ySizeItem / 2;
	pPara->mx        = pPara->rx;;
	pPara->my        = pPara->ry;;


}




static void OnClick(HWND hwnd,int x,int y)
{
	POINT pt;
	RECT rc;
	int i;

	pt.x =x;
	pt.y =y;

	i = pPara->NumItems-1;

	rc.x =pPara->pItemInfo[i].xPos;
	rc.y =pPara->pItemInfo[i].yPos;
	rc.w =ITEM_W;
	rc.h =ITEM_H;

	if(move_n < 3)
	{
		if(PtInRect(&rc,&pt))
		{
			i=_aBitmapItem[pPara->pItemInfo[i].Index].id;
			PostMessage(hwnd,MSG_MENU_CLICKED,i,0);
		}
	}

}

static void _Draw(HDC hdc,HWND hwnd)
{
	ITEM_INFO   ItemInfo;
	int         a1000;
	int         Swap;
	int         i;
	int         SinHQ;
	int         CosHQ;
	int         a;
	int x,y;
	RECT rc;

	GetClientRect(hwnd,&rc);

	pPara =&Para;

	    //
	    // Calculate current positions of items
	    //
	    a1000 = (pPara->Pos * 3600) / pPara->NumItems;
	    for (i = 0; i < pPara->NumItems; i++)
	    {
	      a                         = (90000) + a1000 + (i * 360000) / pPara->NumItems;
	      SinHQ                     = GUI__SinHQ(a);
	      CosHQ                     = GUI__CosHQ(a);
	      pPara->pItemInfo[i].Index = i;
	      pPara->pItemInfo[i].xPos  = pPara->mx - (SHIFT_RIGHT_16(CosHQ * pPara->rx));
	      pPara->pItemInfo[i].yPos  = pPara->my + (SHIFT_RIGHT_16(SinHQ * pPara->ry));
	    }

	    //
	    // Bubble sort items to be able to draw background items first
	    //
	    do
	    {
	      Swap = 0;
	      for (i = 0; i < (pPara->NumItems - 1); i++)
	      {
	        if (pPara->pItemInfo[i].yPos > pPara->pItemInfo[i + 1].yPos)
	        {
	          ItemInfo                = pPara->pItemInfo[i];
	          pPara->pItemInfo[i]     = pPara->pItemInfo[i + 1];
	          pPara->pItemInfo[i + 1] = ItemInfo;
	          Swap                    = 1;
	        }
	      }
	    } while (Swap);

	    //
	    // Draw items
	    //
	    for (i = 0; i < pPara->NumItems; i++)
	    {
	    	RECT rc;
	    	const BITMAP *bm;

	    	bm =pPara->pBitmapItem[pPara->pItemInfo[i].Index].pBitmap;

	    	x =pPara->pItemInfo[i].xPos;
	    	y =pPara->pItemInfo[i].yPos;

	    	x += ((int)ITEM_W-(int)bm->Width)/2;
	    	y += ((int)ITEM_H-(int)bm->Height)/2;
	    	DrawBitmap(hdc,x,y,(pPara->pBitmapItem + pPara->pItemInfo[i].Index)->pBitmap,NULL);

		    rc.x =pPara->pItemInfo[i].xPos;
		    rc.y =pPara->pItemInfo[i].yPos;
		    rc.w =ITEM_W;
		    rc.h =ITEM_H;

		    SetPenColor(hdc,MapRGB(hdc,50,50,100));
		    DrawRect(hdc,&rc);


	    }

	    //
	    // Determine the position to draw the frame and the explanation at
	    //
	    if ((_xPosRect == 0) || (_yPosRect == 0))
	    {
	      for (i = 0; i < pPara->NumItems; i++)
	      {
	        if (pPara->pItemInfo[i].yPos > _yPosRect)
	        {
	          _xPosRect = pPara->pItemInfo[i].xPos;
	          _yPosRect = pPara->pItemInfo[i].yPos;
	        }
	      }
	    }

	    //
	    // Draw item text only after final move
	    //
	   // if (pPara->FinalMove)
	    {

	    	const WCHAR *pText;
	    	RECT rc;


	    	int i;

	    	i =pPara->NumItems - 1;

	    	pText =(pPara->pBitmapItem + pPara->pItemInfo[i].Index)->pText;

	    	rc.x =pPara->pItemInfo[i].xPos;
	    	rc.y =pPara->pItemInfo[i].yPos;
	    	rc.w =ITEM_W;
	    	rc.h =ITEM_H;



	    	SetPenColor(hdc,MapRGB(hdc,250,0,0));
	    	DrawRect(hdc,&rc);


	    	GetClientRect(hwnd,&rc);
	    	SetTextColor(hdc,MapRGB(hdc,250,20,20));

	    	//rc.h =30;
	    	//rc.y -=rc.h;
	    	//DrawText(hdc,pText,-1,&rc,DT_VCENTER|DT_CENTER);
			TextOut(hdc,_xPosRect,_yPosRect,pText,-1);


	      //GUI_DispStringHCenterAt((pPara->pBitmapItem + pPara->pItemInfo[pPara->NumItems - 1].Index)->pExplanation, _xPosRect, _yPosRect + _bmRectRed_60x60.YSize * 2 / 3);
	    }
	    //
	    // Draw frame surround the current item
	    //

	    //GUI_SetColor(GUI_RED);
	    //GUI_DrawBitmap(&_bmRectRed_60x60, _xPosRect - _bmRectRed_60x60.XSize / 2, _yPosRect - _bmRectRed_60x60.YSize / 2);



}



static LRESULT	WinProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	RECT rc;

	switch(msg)
	{
		case WM_CREATE: //窗口创建时,会自动产生该消息,在这里做一些初始化的操作或创建子窗口.
		{
			GetClientRect(hwnd,&rc); //获得窗口的客户区矩形.
			CreateWindow(BUTTON,L"OK",WS_VISIBLE,rc.w-80,8,68,32,hwnd,ID_OK,NULL,NULL);

			InflateRectEx(&rc,-ITEM_W,-ITEM_H,-ITEM_W,-(ITEM_H+30));
			_Init(rc.x,rc.y,rc.w,rc.h);

		}
		return TRUE;
		////

		case WM_LBUTTONDOWN:
		{
			move_n=0;
			_x0 =GET_LPARAM_X(lParam);
			_y0 =GET_LPARAM_Y(lParam);
		}
		return DefWindowProc(hwnd,msg,wParam,lParam);

		case WM_LBUTTONUP:
		{
			int x,y;

			x =GET_LPARAM_X(lParam);
			y =GET_LPARAM_Y(lParam);

			OnClick(hwnd,x,y);
			InvalidateRect(hwnd,NULL,FALSE);

			move_n=0;
		}
		return DefWindowProc(hwnd,msg,wParam,lParam);


		case WM_MOUSEMOVE:
		{
			int x,y;

			x =GET_LPARAM_X(lParam);
			y =GET_LPARAM_Y(lParam);

			if(move_n++ >= 3)
			{
				if(wParam&MK_LBUTTON)
				{
					pPara->FinalMove  = TRUE;
					pPara->Pos += x-_x0;

					if (pPara->Pos > pPara->NumItems * MOVE_STEP)
					{
						pPara->Pos -= pPara->NumItems * MOVE_STEP;
					}

					if (pPara->Pos < 0)
					{
						pPara->Pos += pPara->NumItems * MOVE_STEP;
					}
					InvalidateRect(hwnd,NULL,FALSE);
				}
				else
				{
					pPara->FinalMove  = FALSE;
				}

			}

			_x0 =x;
			_y0 =y;

		}
		return DefWindowProc(hwnd,msg,wParam,lParam);
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

		case MSG_MENU_CLICKED: //
		{
			int menu_id;

			menu_id =wParam;
			printf("Menu Clicked: %08XH\r\n",menu_id);
		}
		break;
		/////

		case WM_ERASEBKGND:
		return TRUE;

		case WM_PAINT: //窗口需要绘制时，会自动产生该消息.
		{
			PAINTSTRUCT ps;
			HDC hdc;
			HDC hdc_mem;

			GetClientRect(hwnd,&rc);

			hdc =BeginPaint(hwnd,&ps); //开始绘图

			////用户的绘制内容...
			//TextOut(hdc,10,10,L"Hello",-1);
			hdc_mem =CreateMemoryDC(SURF_SCREEN,rc.w,rc.h);

			ClrDisplay(hdc_mem,NULL,MapRGB888(hdc_mem,BK_COLOR));
			_Draw(hdc_mem,hwnd);
			BitBlt(hdc,0,0,rc.w,rc.h,hdc_mem,0,0,SRCCOPY);
			DeleteDC(hdc_mem);
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
void	GUI_DEMO_RadiaMenu(void)
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
								_T("GUI_Demo: RadiaMenu"), //窗口名称
								WS_OVERLAPPEDWINDOW,
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
