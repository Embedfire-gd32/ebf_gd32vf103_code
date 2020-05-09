

/*============================================================================*/
#include <math.h>


#include "emXGUI.h"
//#include "GUI_Font_XFT.h"
#include "x_libc.h"
#include "x_obj.h"

/*============================================================================*/

extern const char ASCII_20_4BPP[];
extern const char ASCII_24_4BPP[];

/*============================================================================*/
#define	BK_W	rc_main.w
#define	BK_H	rc_main.h

#define	LEFT_OFFSET	 80
#define	TOP_OFFSET	 64
#define	RIGHT_OFFSET  (28+100+4)
#define BOTTOM_OFFSET (64)

#define	CUR_BOX_SIZE 20

static POINT *pt;
static int x_cur,y_cur;

static SURFACE *pSurfTop=NULL;
static HDC hdcTop=NULL;

static HFONT hFont20,hFont24;

static RECT rc_main,rc_wav,rc_button,rc_label,rc_x1_cur,rc_x2_cur,rc_y1_cur,rc_y2_cur;

static int x_step;

#define	Y_STEP_NUM	9

static int y_step_cur=0;

static const float y_step[Y_STEP_NUM]={0.1,0.2,0.5,1.0,2.0,5.0,10.0,20.0,50.0};

static const WCHAR *y_step_str[Y_STEP_NUM]={
	L"0.1mV",
	L"0.2mV",
	L"0.5mV",
	L"1mV",
	L"2mV",
	L"5mV",
	L"10mV",
	L"20mV",
	L"50mV",

};


#define	CUR_X1	1
#define	CUR_X2	2
#define	CUR_Y1	3
#define	CUR_Y2	4
static int focus_cur=0;

static int x1_cur,x2_cur,y1_cur,y2_cur;


/*============================================================================*/

struct	__alco_cfg{
	char tag[8]; //"ALCO.CFG"
	u32 B2_ref;
	u32 B10_ref;
	u32 B2_max;
	u32 B10_max;
};


/*============================================================================*/

enum eID{

	ID_BTN_BEGIN,////////////

	ID_EXIT =1,
	ID_START,
	ID_SENSOR_ON,
	ID_SENSOR_OFF,
	ID_Y_STEP,
	ID_PUMP,

	ID_BTN_END,//////////////

	ID_LISTBOX,

	ID_X1_CUR,
	ID_X2_CUR,
	ID_Y1_CUR,
	ID_Y2_CUR,

	ID_Y_STEP_SUB,
	ID_Y_STEP_ADD,

	ID_TEST,

	ID_SCROLL1,
	ID_SCROLL2,

};



static struct __x_obj_item *focus_obj =NULL;
static struct __x_obj_item *button_item =NULL;
static struct __x_obj_item *text_item =NULL;

static struct __x_obj_item *x1_cur_obj =NULL;
static struct __x_obj_item *x2_cur_obj =NULL;
static struct __x_obj_item *y1_cur_obj =NULL;
static struct __x_obj_item *y2_cur_obj =NULL;

/*============================================================================*/

static	void btn_draw(HDC hdc,struct __x_obj_item * obj)
{
	HFONT hFont;

	hFont =GetFont(hdc);

	if(obj->rc.w > 60 && obj->rc.h > 20)
	{
		SetFont(hdc,hFont20);
	}


	if(obj == focus_obj)
	{
		SetTextColor(hdc,MapRGB(hdc,255,0,255));
		SetPenColor(hdc,MapRGB(hdc,200,0,200));
		SetBrushColor(hdc,MapRGB(hdc,100,0,100));
		DrawText(hdc,obj->pszText,-1,&obj->rc,DT_SINGLELINE|DT_VCENTER|DT_CENTER|DT_BKGND|DT_BORDER);

	}
	else
	{
		RECT rc;
		CopyRect(&rc,&obj->rc);
		OffsetRect(&rc,2,2);
		SetBrushColor(hdc,MapRGB(hdc,20,20,20));
		FillRect(hdc,&rc);

		SetTextColor(hdc,MapRGB(hdc,0,200,255));
		SetPenColor(hdc,MapRGB(hdc,0,150,200));
		SetBrushColor(hdc,MapRGB(hdc,0,60,80));
		DrawText(hdc,obj->pszText,-1,&obj->rc,DT_SINGLELINE|DT_VCENTER|DT_CENTER|DT_BKGND|DT_BORDER);
	}

	SetFont(hdc,hFont);

}

static	void text_draw(HDC hdc,struct __x_obj_item * obj)
{
	HDC hdcTop;

	RECT rc;


	hdcTop =CreateMemoryDC(SURF_ARGB8888,obj->rc.w,obj->rc.h);
	rc =obj->rc;

	SetFont(hdcTop,hFont20);
	SetPenColor(hdcTop,MapARGB(hdcTop,200,200,255,200));
	SetBrushColor(hdcTop,MapARGB(hdcTop,80,50,255,50));
	SetTextColor(hdcTop,MapARGB(hdcTop,250,200,255,200));

	rc.x =0;
	rc.y =0;
	DrawText(hdcTop,obj->pszText,-1,&rc,DT_VCENTER|DT_CENTER|DT_BKGND|DT_BORDER);
	BitBlt(hdc,obj->rc.x,obj->rc.y,obj->rc.w,obj->rc.h,hdcTop,0,0,SRCCOPY);
	DeleteDC(hdcTop);
}

static void button_owner_draw(DRAWITEM_HDR *ds) //绘制一个按钮外观
{
	HWND hwnd;
	HDC hdc;
	RECT rc;
	WCHAR wbuf[128];

	hwnd =ds->hwnd; //button的窗口句柄.
	hdc =ds->hDC;   //button的绘图上下文句柄.
	rc =ds->rc;     //button的绘制矩形区.

	SetFont(hdc,hFont24);

	if(ds->State & BST_PUSHED)
	{ //按钮是按下状态
		SetTextColor(hdc,MapARGB(hdc,255,255,0,255));
		SetPenColor(hdc,MapARGB(hdc,255,200,0,200));
		SetBrushColor(hdc,MapARGB(hdc,150,100,0,100));
	}
	else
	{ //按钮是弹起状态
		SetBrushColor(hdc,MapARGB(hdc,80,0,150,150));
		SetPenColor(hdc,MapARGB(hdc,200,0,200,250));
		SetTextColor(hdc,MapARGB(hdc,250,200,255,255));
	}

	FillRect(hdc,&rc); //用矩形填充背景
	DrawRect(hdc,&rc); //画矩形外框

	GetWindowText(hwnd,wbuf,128); //获得按钮控件的文字
	DrawText(hdc,wbuf,-1,&rc,DT_VCENTER|DT_CENTER);//绘制文字(居中对齐方式)

}

static void listbox_owner_draw_1(DRAWITEM_HDR *ds)
{
	HWND hwnd;
	HDC hdc;
	RECT rc;
	int i,count,cursel;
	WCHAR wbuf[128];

	hwnd =ds->hwnd;
	hdc =ds->hDC;

	SetBrushColor(hdc,MapARGB(hdc,90,50,100,200)); //用单色填充整个listbox背景.
	FillRect(hdc,&ds->rc);

	i=SendMessage(hwnd,LB_GETTOPINDEX,0,0);    //获得第一个可见Item项索引。
	count=SendMessage(hwnd,LB_GETCOUNT,0,0);   //获得总的Item项总数。
	cursel=SendMessage(hwnd,LB_GETCURSEL,0,0); //获得当前选中的Item项索引。

	while(i < count) //循环绘制.
	{
		SendMessage(hwnd,LB_GETITEMRECT,i,(LPARAM)&rc); //获得指定Item项的矩形位置参数.
		if(rc.y > ds->rc.h) //如果是超出listbox客户区的Item项不绘制了，直接完成退出.
		{
			break;
		}

		if(i==cursel)
		{ // 绘制当前选中的Item项。

			SetBrushColor(hdc,MapARGB(hdc,200,240,100,0));
			FillRect(hdc,&rc);

			SetTextColor(hdc,MapRGB(hdc,255,255,255));
			SetFont(hdc,hFont24);
		}
		else
		{// 绘制未选中的Item项。

			SetTextColor(hdc,MapRGB(hdc,100,200,255));
			SetFont(hdc,hFont20);
		}

		SendMessage(hwnd,LB_GETTEXT,i,(LPARAM)wbuf);
		DrawText(hdc,wbuf,-1,&rc,DT_SINGLELINE|DT_LEFT|DT_VCENTER);

		i++; //下一个Item项索引值
	}
}

static void DrawFrame(HDC hdc,const POINT *pt,int count,COLORREF color)
{
	int i,x,y;
	WCHAR wbuf[128];
	RECT rc,m_rc[8];

	ClrDisplay(hdc,NULL,MapRGB(hdc,0,40,40));


	rc.x =0;
	rc.y =0;
	rc.w =rc_main.w;
	rc.h =30;
	SetBrushColor(hdc,MapRGB(hdc,0,128,0));
	FillRect(hdc,&rc);
	SetTextColor(hdc,MapRGB(hdc,255,255,255));
	DrawText(hdc,L"Show Wave",-1,&rc,DT_SINGLELINE|DT_VCENTER|DT_CENTER);


	SetPenColor(hdc,MapRGB(hdc,255,255,0));
	DrawRect(hdc,&rc_wav);

	////Draw Y
	SetPenColor(hdc,MapRGB(hdc,0,60,80));
	SetTextColor(hdc,MapRGB(hdc,0,255,255));
	i=0;

	x =rc_wav.x;
	y =rc_wav.y+rc_wav.h-1;

	rc.x =0;
	rc.y =y-8;
	rc.w =LEFT_OFFSET-2;
	rc.h =20;

	while(y >= rc_wav.y)
	{
		HLine(hdc,x,y,x+rc_wav.w);
		if(i>0)
		{
			x_wsprintf(wbuf,L"%.1f",i*y_step[y_step_cur]*20);
			DrawText(hdc,wbuf,-1,&rc,DT_SINGLELINE|DT_VCENTER|DT_RIGHT);
		}


		i++;
		y -= 20;
		rc.y -= 20;

	}

	////Draw X
	SetPenColor(hdc,MapRGB(hdc,0,60,80));
	SetTextColor(hdc,MapRGB(hdc,0,255,255));
	i=0;
	x=rc_wav.x;
	y=rc_wav.y;

	rc.x =x-8;
	rc.y =rc_wav.y+rc_wav.h+1;
	rc.w =20;
	rc.h =20;

	while(x < (rc_wav.x+rc_wav.w))
	{
		VLine(hdc,x,y,y+rc_wav.h);
		if(i>0)
		{
			x_wsprintf(wbuf,L"%d",i*x_step*20/1000);
			DrawText(hdc,wbuf,-1,&rc,DT_SINGLELINE|DT_VCENTER|DT_CENTER);
		}

		i++;
		x+=20;
		rc.x+=20;
	}

	/////DrawFrame
	if(count>0)
	{
		int x1,y1,x2,y2;
		SetPenColor(hdc,color);

		for(i=1;i<count;i++)
		{
			x1 = pt[i-1].x + rc_wav.x;
			y1 =-pt[i-1].y + rc_wav.y+rc_wav.h-1;
			x1 =MAX(x1,rc_wav.x);
			x1 =MIN(x1,rc_wav.x+rc_wav.w-1);
			y1 =MAX(y1,rc_wav.y);
			y1 =MIN(y1,rc_wav.y+rc_wav.h-1);

			x2 = pt[i].x + rc_wav.x;
			y2 =-pt[i].y + rc_wav.y+rc_wav.h-1;
			x2 =MAX(x2,rc_wav.x);
			x2 =MIN(x2,rc_wav.x+rc_wav.w-1);
			y2 =MAX(y2,rc_wav.y);
			y2 =MIN(y2,rc_wav.y+rc_wav.h-1);

			Line(hdc,x1,y1,x2,y2);
			//AA_DrawLine(hdc,x1,y1,x2,y2);
			//AA_DrawBoldLine(hdc,x1,y1,x2,y2);
		}
	}

	////Cursor X1
	SetPenColor(hdc,MapRGB(hdc,0,200,0));

	x=x1_cur+rc_wav.x;
	VLine(hdc,x,rc_wav.y-2,rc_wav.y+rc_wav.h-1);

	////Cursor X2
	x=x2_cur+rc_wav.x;
	VLine(hdc,x,rc_wav.y-2,rc_wav.y+rc_wav.h-1);

	////Cursor Y1
	y=rc_wav.y+rc_wav.h-y1_cur;
	HLine(hdc,rc_wav.x,y,rc_wav.x+rc_wav.w-1+2);

	////Cursor Y2
	y=rc_wav.y+rc_wav.h-y2_cur;
	HLine(hdc,rc_wav.x,y,rc_wav.x+rc_wav.w-1+2);


	MakeMatrixRect(m_rc,&rc_label,2,2,3,2);

	////X Text
	SetTextColor(hdc,MapRGB(hdc,255,255,255));
	SetPenColor(hdc,MapRGB(hdc,160,0,0));
	SetBrushColor(hdc,MapRGB(hdc,80,20,20));

	x_wsprintf(wbuf,L"X1:%dmS",x1_cur*x_step);
	DrawText(hdc,wbuf,-1,&m_rc[0],DT_SINGLELINE|DT_VCENTER|DT_LEFT|DT_BKGND|DT_BORDER);

	x_wsprintf(wbuf,L"X2:%dmS",x2_cur*x_step);
	DrawText(hdc,wbuf,-1,&m_rc[1],DT_SINGLELINE|DT_VCENTER|DT_LEFT|DT_BKGND|DT_BORDER);

	x_wsprintf(wbuf,L"X2-X1:%dmS",(x2_cur-x1_cur)*x_step);
	DrawText(hdc,wbuf,-1,&m_rc[2],DT_SINGLELINE|DT_VCENTER|DT_LEFT|DT_BKGND|DT_BORDER);

	////Y Text
	SetTextColor(hdc,MapRGB(hdc,255,255,255));
	SetPenColor(hdc,MapRGB(hdc,160,0,160));
	SetBrushColor(hdc,MapRGB(hdc,100,0,100));

	x_wsprintf(wbuf,L"Y1:%.1fmV",y1_cur*y_step[y_step_cur]);
	DrawText(hdc,wbuf,-1,&m_rc[3],DT_SINGLELINE|DT_VCENTER|DT_LEFT|DT_BKGND|DT_BORDER);

	x_wsprintf(wbuf,L"Y2:%.1fmV",y2_cur*y_step[y_step_cur]);
	DrawText(hdc,wbuf,-1,&m_rc[4],DT_SINGLELINE|DT_VCENTER|DT_LEFT|DT_BKGND|DT_BORDER);

	x_wsprintf(wbuf,L"Y2-Y1:%.1fmV",(y2_cur-y1_cur)*y_step[y_step_cur]);
	DrawText(hdc,wbuf,-1,&m_rc[5],DT_SINGLELINE|DT_VCENTER|DT_LEFT|DT_BKGND|DT_BORDER);

	////button obj
	x_obj_draw(hdc,button_item);
	x_obj_draw(hdc,text_item);

}

/*============================================================================*/
static float ADS1120_GetVoltage_mV(u16 addata);

static int move_id[5]=
{
		ID_EXIT ,
		ID_START,
		ID_SENSOR_ON,
		ID_SENSOR_OFF,
		ID_PUMP,

};


static int move_speed[5]=
{
		1*10 ,
		2*10,
		3*10,
		4*10,
		5*10,

};


static int bMove=0;

static	LRESULT	WinProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	RECT rc;

	switch(msg)
	{
	case WM_CREATE:
			x_cur=0;
			y_cur=0;

			x1_cur=8;
			x2_cur=25;
			y1_cur=15;
			y2_cur=35;
			x_step=50; //50ms
			y_step_cur=2;

			bMove=0;

			focus_obj =NULL;

			hFont20 =XFT_CreateFont(ASCII_20_4BPP);
			hFont24 =XFT_CreateFont(ASCII_24_4BPP);

			GetClientRect(hwnd,&rc);

			pt =new POINT[rc.w];

			pSurfTop =CreateSurface(SURF_ARGB4444,rc.w,rc.h,NULL,0);
			hdcTop =CreateDC(pSurfTop,NULL);
			ClrDisplay(hdcTop,NULL,MapARGB(hdcTop,0,0,0,0));


			GetClientRect(hwnd,&rc_main);
			CopyRect(&rc_wav,&rc_main);
			InflateRectEx(&rc_wav,-LEFT_OFFSET,-TOP_OFFSET,-RIGHT_OFFSET,-BOTTOM_OFFSET);

			rc_x1_cur.x =rc_wav.x+x1_cur-10;
			rc_x1_cur.y =rc_wav.y-CUR_BOX_SIZE;
			rc_x1_cur.w =CUR_BOX_SIZE;
			rc_x1_cur.h =CUR_BOX_SIZE;

			rc_x2_cur.x =rc_wav.x+x2_cur-10;
			rc_x2_cur.y =rc_wav.y-CUR_BOX_SIZE;
			rc_x2_cur.w =CUR_BOX_SIZE;
			rc_x2_cur.h =CUR_BOX_SIZE;

			rc_y1_cur.x =rc_wav.x+rc_wav.w;
			rc_y1_cur.y =rc_wav.y+rc_wav.h-y1_cur-10;
			rc_y1_cur.w =CUR_BOX_SIZE;
			rc_y1_cur.h =CUR_BOX_SIZE;

			rc_y2_cur.x =rc_wav.x+rc_wav.w;
			rc_y2_cur.y =rc_wav.y+rc_wav.h-y2_cur-10;
			rc_y2_cur.w =CUR_BOX_SIZE;
			rc_y2_cur.h =CUR_BOX_SIZE;

			rc_label.x =8;
			rc_label.y =rc_wav.y+rc_wav.h+24;
			rc_label.w =rc_main.w-20-100;
			rc_label.h =40;

			rc_button.w =100;
			rc_button.h =rc_main.h-40-64-4;
			rc_button.x =rc_main.w-rc_button.w-4;
			rc_button.y =40;

			if(1)
			{
				int i=0;
				RECT rc,m_rc[8];
				HWND wnd;

				MakeMatrixRect(m_rc,&rc_button,2,4,1,5);

				button_item =x_obj_create_class(L"ButtonItem",	0xFFFFFFFF,	&rc_button,X_OBJ_VISIBLE,0,btn_draw);
				text_item =x_obj_create_class(L"TextItem",	0xFFFFFFFF,	&rc_button,X_OBJ_VISIBLE,0,text_draw);


				rc =m_rc[0];
				//OffsetRect(&rc,-100,0);
				rc.x =rc_main.w-50;
				CreateWindow(BUTTON,L"Exit",WS_TRANSPARENT|WS_OWNERDRAW|WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,ID_EXIT,NULL,NULL);

				OffsetRect(&rc,0,rc.h+4);
				CreateWindow(BUTTON,L"ON",WS_TRANSPARENT|WS_OWNERDRAW|WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,ID_SENSOR_ON,NULL,NULL);

				OffsetRect(&rc,0,rc.h+4);
				CreateWindow(BUTTON,L"OFF",WS_TRANSPARENT|WS_OWNERDRAW|WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,ID_SENSOR_OFF,NULL,NULL);

				OffsetRect(&rc,0,rc.h+4);
				CreateWindow(BUTTON,L"Pump",WS_TRANSPARENT|WS_OWNERDRAW|WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,ID_PUMP,NULL,NULL);

				OffsetRect(&rc,0,rc.h+4);
				CreateWindow(BUTTON,L"Restart",WS_TRANSPARENT|WS_OWNERDRAW|WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,ID_START,NULL,NULL);

				rc.x =100;
				rc.y =20;
				rc.w =100;
				rc.h =32;

				CreateWindow(BUTTON,L"Button",WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,ID_START+1000,NULL,NULL);


				rc.x =500;
				rc.y =100;
				rc.w =150;
				rc.h =200;
				wnd=CreateWindow(LISTBOX,_T("Listbox2"),WS_TRANSPARENT|WS_OWNERDRAW|LBS_LINE|WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,ID_LISTBOX,NULL,NULL);
				SendMessage(wnd,LB_ADDSTRING,-1,(LPARAM)L"10HZ");
				SendMessage(wnd,LB_ADDSTRING,-1,(LPARAM)L"15HZ");
				SendMessage(wnd,LB_ADDSTRING,-1,(LPARAM)L"20HZ");
				SendMessage(wnd,LB_ADDSTRING,-1,(LPARAM)L"25HZ");
				SendMessage(wnd,LB_ADDSTRING,-1,(LPARAM)L"30HZ");
				SendMessage(wnd,LB_ADDSTRING,-1,(LPARAM)L"35HZ");
				SendMessage(wnd,LB_ADDSTRING,-1,(LPARAM)L"40HZ");
				SendMessage(wnd,LB_ADDSTRING,-1,(LPARAM)L"45HZ");
				SendMessage(wnd,LB_ADDSTRING,-1,(LPARAM)L"50HZ");

				x1_cur_obj	=x_obj_create(L"X1",	ID_X1_CUR,	&rc_x1_cur,	X_OBJ_VISIBLE,0,button_item);
				x2_cur_obj	=x_obj_create(L"X2",	ID_X2_CUR,	&rc_x2_cur,	X_OBJ_VISIBLE,0,button_item);
				y1_cur_obj	=x_obj_create(L"Y1",	ID_Y1_CUR,	&rc_y1_cur,	X_OBJ_VISIBLE,0,button_item);
				y2_cur_obj	=x_obj_create(L"Y2",	ID_Y2_CUR,	&rc_y2_cur,	X_OBJ_VISIBLE,0,button_item);


				rc.w =26;
				rc.h =26;
				rc.x =rc_main.w-rc.w-4;
				rc.y =rc_main.h-rc.h-32;
				x_obj_create(L"+",	ID_Y_STEP_ADD,	&rc,	X_OBJ_VISIBLE,0,button_item);

				rc.w =48;
				OffsetRect(&rc,-(rc.w+2),0);
				//x_obj_create(L"mV",	ID_Y_STEP,		&rc,	X_OBJ_VISIBLE,0,button_item);
				x_obj_create(y_step_str[y_step_cur],ID_Y_STEP,	&rc,	X_OBJ_VISIBLE,0,button_item);

				rc.w =26;
				OffsetRect(&rc,-(rc.w+2),0);
				x_obj_create(L"-",	ID_Y_STEP_SUB,	&rc,	X_OBJ_VISIBLE,0,button_item);
				OffsetRect(&rc,-(rc.w+2),0);

				/////
				rc.x =100;
				rc.y =100;
				rc.w =150;
				rc.h =80;
				x_obj_create(L"Text-1",	ID_TEST+1,	&rc,	X_OBJ_VISIBLE,0,text_item);

				OffsetRect(&rc,0,rc.h+8);
				x_obj_create(L"Text-2",	ID_TEST+2,	&rc,	X_OBJ_VISIBLE,0,text_item);

			}


			SetTimer(hwnd,1,10,TMR_START,NULL);
			return TRUE;
			////

	case	WM_TIMER:
	{
		HWND wnd;
		RECT rc;
		int i;

		if(bMove == 1)
		{
			for(i=0;i<5;i++)
			{
				wnd =GetDlgItem(hwnd,move_id[i]);
				if(wnd!=NULL)
				{

					GetWindowRect(wnd,&rc);

					if(rc.x > 250)
					{
						ScreenToClient(hwnd,(POINT*)&rc,1);
						SetBrushColor(hdcTop,MapARGB(hdcTop,0,0,0,0));
						FillRect(hdcTop,&rc);

						OffsetRect(&rc,-move_speed[i],0);
						MoveWindow(wnd,rc.x,rc.y,rc.w,rc.h,TRUE);
						UpdateWindow(wnd);

					}
					else if(rc.x > 200)
					{
						ScreenToClient(hwnd,(POINT*)&rc,1);
						SetBrushColor(hdcTop,MapARGB(hdcTop,0,0,0,0));
						FillRect(hdcTop,&rc);

						OffsetRect(&rc,-1,0);
						MoveWindow(wnd,rc.x,rc.y,rc.w,rc.h,TRUE);
						UpdateWindow(wnd);
					}

				}
				////
			}
			//InvalidateRect(hwnd,NULL,FALSE);
		}

		if(bMove == -1)
		{
			for(i=0;i<5;i++)
			{
				wnd =GetDlgItem(hwnd,move_id[i]);
				if(wnd!=NULL)
				{

					GetWindowRect(wnd,&rc);
					if(rc.x < (rc_main.w-100))
					{
						ScreenToClient(hwnd,(POINT*)&rc,1);
						SetBrushColor(hdcTop,MapARGB(hdcTop,0,0,0,0));
						FillRect(hdcTop,&rc);

						OffsetRect(&rc,move_speed[i],0);
						MoveWindow(wnd,rc.x,rc.y,rc.w,rc.h,TRUE);
						UpdateWindow(wnd);
					}
					else if(rc.x < (rc_main.w-50))
					{
						ScreenToClient(hwnd,(POINT*)&rc,1);
						SetBrushColor(hdcTop,MapARGB(hdcTop,0,0,0,0));
						FillRect(hdcTop,&rc);

						OffsetRect(&rc,1,0);
						MoveWindow(wnd,rc.x,rc.y,rc.w,rc.h,TRUE);
						UpdateWindow(wnd);
					}


				}
				////
			}
			//InvalidateRect(hwnd,NULL,FALSE);
		}


/*
		if(x_cur < rc_wav.w)
		{
			float mV;
			mV=ADS1120_GetVoltage_mV(0);

			//mV =sin(2*3.14*(x_cur/rc_wave.w))*10+50;


			mV = (1-sin( 2*3.14*(x_cur*1.0/(rc_wav.w/10)) ))*((rc_wav.h)/2);
			y_cur =(int)(mV/y_step[y_step_cur]);

			//y_cur =x_rand()%200;

			pt[x_cur].x =x_cur;
			pt[x_cur].y =y_cur;
			x_cur++;
			InvalidateRect(hwnd,NULL,FALSE);
		}
		*/

	}
	break;
	////

	case	WM_NOTIFY:
	{
		U16 code,id;
		code =HIWORD(wParam);
		id   =LOWORD(wParam);
		if(id==ID_EXIT && code==BN_CLICKED)
		{
			PostCloseMessage(hwnd);
		}

		if(id==ID_SENSOR_ON && code==BN_CLICKED)
		{
			bMove = 1;
		}

		if(id==ID_SENSOR_OFF && code==BN_CLICKED)
		{
			bMove = -1;
		}

	}
	break;
	/////

	case    WM_LBUTTONDOWN: //
			{
				int x,y;


				x=GET_LPARAM_X(lParam);
				y=GET_LPARAM_Y(lParam);

				focus_obj =x_obj_get_from_pos(button_item,x,y); //从x,y坐标值获得 x_obj对象.
				if(focus_obj==NULL)
				{ //没有
					return DefWindowProc(hwnd,msg,wParam,lParam);
				}

				if(focus_obj->id == ID_X1_CUR)
				{
					focus_cur =CUR_X1;
				}

				if(focus_obj->id == ID_X2_CUR)
				{
					focus_cur =CUR_X2;
				}

				if(focus_obj->id == ID_Y1_CUR)
				{
					focus_cur =CUR_Y1;
				}

				if(focus_obj->id == ID_Y2_CUR)
				{
					focus_cur =CUR_Y2;
				}

				if(focus_obj->id == ID_START)
				{////重新开始
					x_cur =0;
				}

				if(focus_obj->id == ID_Y_STEP)
				{
					y_step_cur++;
					if(y_step_cur>=Y_STEP_NUM)
					{
						y_step_cur=0;
					}
					x_obj_set_text(x_obj_get_from_id(button_item,ID_Y_STEP),y_step_str[y_step_cur]);
					////重新开始
					x_cur =0;

				}

				if(focus_obj->id == ID_SENSOR_ON)
				{

				}

				if(focus_obj->id == ID_SENSOR_OFF)
				{

				}

				if(focus_obj->id == ID_Y_STEP_SUB)
				{
					y_step_cur = MAX(y_step_cur-1,0);
					x_obj_set_text(x_obj_get_from_id(button_item,ID_Y_STEP),y_step_str[y_step_cur]);
					////重新开始
					x_cur =0;
				}

				if(focus_obj->id == ID_Y_STEP_ADD)
				{
					y_step_cur = MIN(y_step_cur+1,Y_STEP_NUM-1);
					x_obj_set_text(x_obj_get_from_id(button_item,ID_Y_STEP),y_step_str[y_step_cur]);
					////重新开始
					x_cur =0;
				}

				InvalidateRect(hwnd,NULL,TRUE);
			}
			return DefWindowProc(hwnd,msg,wParam,lParam);
			////

	case WM_LBUTTONUP:
			{
				int x,y;

				struct __x_obj_item *obj;

				x=GET_LPARAM_X(lParam);
				y=GET_LPARAM_Y(lParam);

				obj =x_obj_get_from_pos(button_item,x,y);
				if(obj!=NULL)
				{
					if(obj->id == focus_obj->id)
					{
						if(obj->id == ID_PUMP)
						{
							//sensor_pump_on();
						}

						if(obj->id == ID_EXIT)
						{
							SendMessage(hwnd,WM_CLOSE,0,0);
						}
					}

				}

				/////
				focus_cur =0;
				focus_obj =NULL;
				InvalidateRect(hwnd,NULL,TRUE);

			}
			return DefWindowProc(hwnd,msg,wParam,lParam);
			////

	case	WM_MOUSEMOVE:
			{
				int x,y;

				x=GET_LPARAM_X(lParam);
				y=GET_LPARAM_Y(lParam);


				x =x-rc_wav.x;
				x=MAX(x,0);
				x=MIN(x,rc_wav.w-1);

				y =y-rc_wav.y;
				y=MAX(y,0);
				y=MIN(y,rc_wav.h-1);

				if(focus_cur == CUR_X1)
				{

					x1_cur =x;

					x_obj_move_to(x1_cur_obj,(x1_cur+rc_wav.x)-(x1_cur_obj->rc.w/2),x1_cur_obj->rc.y);

				}

				if(focus_cur == CUR_X2)
				{

					x2_cur =x;

					x_obj_move_to(x2_cur_obj,(x+rc_wav.x)-(x2_cur_obj->rc.w/2),x2_cur_obj->rc.y);

				}

				if(focus_cur == CUR_Y1)
				{

					y1_cur =rc_wav.h-y;

					x_obj_move_to(y1_cur_obj,y1_cur_obj->rc.x,(y+rc_wav.y)-(y1_cur_obj->rc.h/2));

				}

				if(focus_cur == CUR_Y2)
				{

					y2_cur =rc_wav.h-y;
					x_obj_move_to(y2_cur_obj,y2_cur_obj->rc.x,(y+rc_wav.y)-(y2_cur_obj->rc.h/2));

				}

				InvalidateRect(hwnd,NULL,TRUE);

			}
			return DefWindowProc(hwnd,msg,wParam,lParam);
			////

	case WM_ERASEBKGND:
	{


	}
	return TRUE;
	////

	case	WM_DRAWITEM:
	{
		/*　当控件指定了WS_OWNERDRAW风格，则每次在绘制前都会给父窗口发送WM_DRAWITEM消息。
		 *  wParam参数指明了发送该消息的控件ID;lParam参数指向一个DRAWITEM_HDR的结构体指针，
		 *  该指针成员包含了一些控件绘制相关的参数.
	     */

		DRAWITEM_HDR *ds;

		ds =(DRAWITEM_HDR*)lParam;


			HDC hdc;
			RECT rc;

			rc =ds->rc;

			//把控件的客户区坐标转换成主窗口的客户区坐标。
			ClientToScreen(ds->hwnd,(POINT*)&rc,1);
			ScreenToClient(hwnd,(POINT*)&rc,1);

			hdc =CreateDC(pSurfTop,&rc); //在“顶层”Surface中创建一个DC。
			ds->hDC =hdc;

			if(ds->ID> ID_BTN_BEGIN && ds->ID<ID_BTN_END)
			{

				button_owner_draw(ds); //执行自绘制按钮
			}

			if(ds->ID == ID_LISTBOX)
			{
				listbox_owner_draw_1(ds);
			}

			DeleteDC(hdc);

			InvalidateRect(hwnd,&rc,FALSE); //使主窗口重绘.

		return TRUE;
	}
	break;
	////

	case WM_PAINT:
	{
		HDC hdc,hdc_bk;
		PAINTSTRUCT ps;
		RECT rc;
		WCHAR wbuf[128];

		hdc =BeginPaint(hwnd,&ps);

		GetClientRect(hwnd,&rc);
		hdc_bk =CreateMemoryDC(SURF_SCREEN,rc.w,rc.h);

		///////
		x_cur=0;
		while(x_cur < rc_wav.w)
		{
				float mV;
				//mV=ADS1120_GetVoltage_mV(0);

				//mV =sin(2*3.14*(x_cur/rc_wave.w))*10+50;

				mV = (1-sin( 2*3.14*(x_cur*1.0/(rc_wav.w/10)) ))*((rc_wav.h)/2);
				y_cur =(int)(mV/y_step[y_step_cur]);

				//y_cur =x_rand()%200;

				pt[x_cur].x =x_cur;
				pt[x_cur].y =y_cur;
				x_cur++;
								//InvalidateRect(hwnd,NULL,FALSE);
		}

		if(1)
		{

			x_wsprintf(wbuf,L"Y[abs]:%.1fmV",(y2_cur-y1_cur)*y_step[y_step_cur]);
			x_obj_set_text(x_obj_get_from_id(text_item,ID_TEST+1),wbuf);
		}

		rc.x=200;
		rc.y=200;
		rc.w=200;
		rc.h=40;
		SetTextColor(hdcTop,MapARGB(hdcTop,250,10,10,10));
		SetPenColor(hdcTop,MapARGB(hdcTop,150,100,200,255));
		SetBrushColor(hdcTop,MapARGB(hdcTop,100,250,250,250));
		DrawText(hdcTop,wbuf,-1,&rc,DT_VCENTER|DT_CENTER|DT_BKGND|DT_BORDER);

	/*
		rc.x =700;
		rc.y =100;
		rc.w=200;
		rc.h=50;
		SetBrushColor(hdcTop,MapARGB(hdcTop,200,255,100,100));
		FillRect(hdcTop,&rc);

		OffsetRect(&rc,30,30);
		SetBrushColor(hdcTop,MapARGB(hdcTop,200,100,255,100));
		FillRect(hdcTop,&rc);
*/
		DrawFrame(hdc_bk,pt,x_cur,MapRGB(hdc_bk,255,0,0));


		////////////////
		BitBlt(hdc_bk,0,0,BK_W,BK_H,hdcTop,0,0,SRCCOPY);
		BitBlt(hdc,0,0,BK_W,BK_H,hdc_bk,0,0,SRCCOPY);

		DeleteDC(hdc_bk);
		EndPaint(hwnd,&ps);
	}
	break;
	////

	case WM_DESTROY:
	{
		x_obj_del(button_item);
		x_obj_del(text_item);
		DeleteDC(hdcTop);
		DeleteSurface(pSurfTop);
		DeleteFont(hFont20);
		DeleteFont(hFont24);

		delete pt;

		return DefWindowProc(hwnd,msg,wParam,lParam);
	}
	////

		default:
			return DefWindowProc(hwnd,msg,wParam,lParam);
	}

	return WM_NULL;
}

/*============================================================================*/

static void ADS1120_Init(void)
{
//	sensor_enable();
}

static u16 ADS1120_GetData(void)
{
	//return get_alco_data();
	return x_rand()%0x7FFF;
}

static float ADS1120_GetVoltage_mV(u16 addata)
{
	float val;

	addata=ADS1120_GetData();
	val=(float)addata*(float)2048/32768;
	return val;
}


extern "C" void	GUI_DEMO_ShowWave(void)
{
	HWND hwnd;
	MSG	msg;
	WNDCLASS wcex;
	RECT rc;
	////

	ShowCursor(FALSE);
	ADS1120_Init();

	wcex.Tag	 		= WNDCLASS_TAG;

	wcex.Style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)WinProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= NULL;
	wcex.hIcon			= NULL;
	wcex.hCursor		= NULL;


	rc.x =0;
	rc.y =0;
	rc.w =GUI_XSIZE;
	rc.h =GUI_YSIZE;


	hwnd	=CreateWindowEx(	WS_EX_NODRAG|WS_EX_FRAMEBUFFER,&wcex,
							L"Show Wave",
							WS_CLIPCHILDREN,
							rc.x,rc.y,rc.w,rc.h,
							NULL,0,NULL,NULL);


	ShowWindow(hwnd,SW_SHOW);
	UpdateWindow(hwnd);


	while(GetMessage(&msg,hwnd))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	ShowCursor(TRUE);


}
