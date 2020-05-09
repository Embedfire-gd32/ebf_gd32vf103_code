
#include <stdlib.h>

#include "emXGUI.h"
//#include "GUI_Font_XFT.h"
#include "x_libc.h"

#ifndef	_T
#define	_T(x) L##x
#endif

/*===================================================================================*/
#define	ID_EXIT		0x1000
#define	ID_LISTBOX1	0x1101
#define	ID_LISTBOX2	0x1102
#define	ID_LISTBOX3	0x1103
#define	ID_LISTBOX4	0x1104

#define TEXTCOLOR	RGB888(255,0,0)

extern const char ASCII_20_4BPP[];
extern const char ASCII_24_4BPP[];

static HFONT hFont20;
static HFONT hFont24;
//static HDC hdc_bk=NULL;

/*===================================================================================*/

static void listbox_owner_draw_1(DRAWITEM_HDR *ds)
{
	HWND hwnd;
	HDC hdc;
	RECT rc;
	int i,count,cursel;
	WCHAR wbuf[128];

	hwnd =ds->hwnd;
	hdc =ds->hDC;

	SetFont(hdc,hFont24);

	SetBrushColor(hdc,MapRGB(hdc,50,60,100)); //用单色填充整个listbox背景.
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

			GradientFillRect(hdc,&rc,MapRGB(hdc,220,220,220),MapRGB(hdc,220,20,20),TRUE); //画渐变矩形。

			SetTextColor(hdc,MapRGB(hdc,250,250,250));
			SetFont(hdc,hFont24);
		}
		else
		{// 绘制未选中的Item项。

			SetTextColor(hdc,MapRGB(hdc,10,255,0));
			SetFont(hdc,hFont20);
		}

		SendMessage(hwnd,LB_GETTEXT,i,(LPARAM)wbuf);
		DrawText(hdc,wbuf,-1,&rc,DT_SINGLELINE|DT_LEFT|DT_VCENTER);

		i++; //下一个Item项索引值
	}
}

static void listbox_owner_draw_2(DRAWITEM_HDR *ds)
{
	HWND hwnd;
	HDC hdc;
	RECT rc;
	int i,count,cursel;
	WCHAR wbuf[128];

	hwnd =ds->hwnd;
//	hdc =ds->hDC;

	hdc =CreateMemoryDC(SURF_SCREEN,ds->rc.w,ds->rc.h); //创建一个内存DC来绘图.
	//SetBrushColor(hdc,MapRGB(hdc,0,0,0));
	//FillRect(hdc,&ds->rc);
	GradientFillRect(hdc,&ds->rc,MapRGB(hdc,230,230,230),MapRGB(hdc,0,0,50),TRUE);

	i=SendMessage(hwnd,LB_GETTOPINDEX,0,0);
	count=SendMessage(hwnd,LB_GETCOUNT,0,0);
	cursel=SendMessage(hwnd,LB_GETCURSEL,0,0);
	SetFont(hdc,hFont24);
	while(i<count)
	{
		SendMessage(hwnd,LB_GETITEMRECT,i,(LPARAM)&rc);
		if(rc.y > ds->rc.h)
		{
			break;
		}
		int a;

		//a=MAX(rc.y*255/ds->rc.h,0);


		a=abs(rc.y+(rc.h/2)-(ds->rc.h/2))*255/(ds->rc.h/2);
		a=MAX(a,0);
		a=MIN(a,255);

		SetTextColor(hdc,MapRGB888(hdc,AlphaBlendColor(RGB888(255,10,10),RGB888(100,100,100),a)));

		if(i==cursel)
		{
			//GradientFillRect(hdc,&rc,MapRGB(hdc,220,220,220),MapRGB(hdc,220,20,20),TRUE);
			//SetTextColor(hdc,MapRGB(hdc,250,250,250));


			SetBrushColor(hdc,MapRGB(hdc,255,255,255));
			FillRect(hdc,&rc);

		}
		else
		{
			//SetTextColor(hdc,MapRGB(hdc,10,255,0));

		}

		SendMessage(hwnd,LB_GETTEXT,i,(LPARAM)wbuf);
		DrawText(hdc,wbuf,-1,&rc,DT_SINGLELINE|DT_CENTER|DT_VCENTER);

		i++;
	}

	BitBlt(ds->hDC,0,0,ds->rc.w,ds->rc.h,hdc,0,0,SRCCOPY); //将内存DC的内容输出到窗口(DC)中.
	DeleteDC(hdc);
}


static void _draw_listbox(HDC hdc,HWND hwnd,COLOR_RGB32 text_c,COLOR_RGB32 back_c,COLOR_RGB32 sel_c)
{

	RECT rc,rc_cli;
	int i,count,cursel;
	WCHAR wbuf[128];


	GetClientRect(hwnd,&rc_cli);

	SetBrushColor(hdc,MapRGB888(hdc,back_c));
	FillRect(hdc,&rc_cli);

	SetTextColor(hdc,MapRGB888(hdc,text_c));

	i=SendMessage(hwnd,LB_GETTOPINDEX,0,0);
	count=SendMessage(hwnd,LB_GETCOUNT,0,0);
	cursel=SendMessage(hwnd,LB_GETCURSEL,0,0);

	SetFont(hdc,hFont24);

	while(i<count)
	{
		SendMessage(hwnd,LB_GETITEMRECT,i,(LPARAM)&rc);
		if(rc.y > rc_cli.h)
		{
			break;
		}


		if(i==cursel)
		{
			SetBrushColor(hdc,MapRGB888(hdc,sel_c));
			FillRect(hdc,&rc);
		}
		else
		{
			//SetTextColor(hdc,MapRGB(hdc,10,255,0));
		}

		SendMessage(hwnd,LB_GETTEXT,i,(LPARAM)wbuf);
		DrawText(hdc,wbuf,-1,&rc,DT_SINGLELINE|DT_CENTER|DT_VCENTER);

		i++;
	}



}

static void listbox_owner_draw_3(DRAWITEM_HDR *ds)
{
	HWND hwnd;
	HDC hdc,hdc0,hdc1;
	RECT rc_m;
	int x,y,w,h;

	hwnd =ds->hwnd;
	hdc =ds->hDC;

	//定义一个中间的矩形．
	rc_m.w =ds->rc.w;
	rc_m.h =40;
	rc_m.x =0;
	rc_m.y =(ds->rc.h-rc_m.h)>>1;

	//创建两个一样大小的DC,把listbox分别绘制进去，但颜色参数不同的.
	hdc0 =CreateMemoryDC(SURF_SCREEN,ds->rc.w,ds->rc.h);
	hdc1 =CreateMemoryDC(SURF_SCREEN,ds->rc.w,ds->rc.h);

	//一个listbox绘到hdc0中．
	_draw_listbox(hdc0,hwnd,RGB888(100,230,160),RGB888(0,50,50),RGB888(10,100,100));

	//一个listbox绘到hdc1中．
	_draw_listbox(hdc1,hwnd,RGB888(250,10,10),RGB888(10,20,30),RGB888(150,200,220));

	//中间框绘制到hdc1中．
	SetPenColor(hdc1,MapRGB(hdc1,110,130,150));
	DrawRect(hdc1,&rc_m);

	//上面的矩形部分丛hdc0里复制出来.
	x =0;
	y =0;
	w =rc_m.w;
	h =rc_m.y-ds->rc.y;
	BitBlt(hdc,x,y,w,h,hdc0,x,y,SRCCOPY);

	//中间矩形部分丛hdc1里复制出来.
	BitBlt(hdc,rc_m.x,rc_m.y,rc_m.w,rc_m.h,hdc1,rc_m.x,rc_m.y,SRCCOPY);

	//下面的矩形部分丛hdc0里复制出来.
	x =0;
	y =rc_m.y+rc_m.h;
	w =rc_m.w;
	h =ds->rc.h-(rc_m.y+rc_m.h);
	BitBlt(hdc,x,y,w,h,hdc0,x,y,SRCCOPY);


	DeleteDC(hdc0);
	DeleteDC(hdc1);

}

static void listbox_owner_draw_4(DRAWITEM_HDR *ds)
{
	HWND hwnd;
	HDC hdc;
	RECT rc;
	int i,count,cursel;
	WCHAR wbuf[128];
	POINT pt;

	hwnd =ds->hwnd;
	hdc =ds->hDC;

//	hdc =CreateMemoryDC(SURF_ARGB4444,ds->rc.w,ds->rc.h); //创建一个内存DC来绘图.


	rc =ds->rc;

//	pt.x =rc.x;
//	pt.y =rc.y;
//	ClientToScreen(hwnd,&pt,1);
//	ScreenToClient(GetParent(hwnd),&pt,1);
//	BitBlt(hdc,0,0,rc.w,rc.h,hdc_bk,pt.x,pt.y,SRCCOPY);

	EnableAlpha(hdc,TRUE);
	SetAlpha(hdc,100);

	SetBrushColor(hdc,MapRGB(hdc,220,220,240));
	FillRect(hdc,&ds->rc);
	//GradientFillRect(hdc,&ds->rc,MapRGB(hdc,230,230,230),MapRGB(hdc,0,0,50),TRUE);

	i=SendMessage(hwnd,LB_GETTOPINDEX,0,0);
	count=SendMessage(hwnd,LB_GETCOUNT,0,0);
	cursel=SendMessage(hwnd,LB_GETCURSEL,0,0);
	//SetFont(hdc,hFont20);

	while(i<count)
	{
		SendMessage(hwnd,LB_GETITEMRECT,i,(LPARAM)&rc);
		if(rc.y > ds->rc.h)
		{
			break;
		}
		int a;

		//a=MAX(rc.y*255/ds->rc.h,0);


		a=abs(rc.y+(rc.h/2)-(ds->rc.h/2))*255/(ds->rc.h/2);
		a=MAX(a,0);
		a=MIN(a,255);

		//SetTextColor(hdc,MapRGB888(hdc,AlphaBlendColor(RGB888(255,10,10),RGB888(100,100,100),a)));
		SetAlpha(hdc,255);
		SetTextColor(hdc,MapRGB(hdc,50,10,10));

		if(i==cursel)
		{
			//GradientFillRect(hdc,&rc,MapRGB(hdc,220,220,220),MapRGB(hdc,220,20,20),TRUE);
			SetTextColor(hdc,MapRGB(hdc,250,10,10));

			SetAlpha(hdc,200);
			SetBrushColor(hdc,MapRGB(hdc,250,250,250));
			FillRect(hdc,&rc);

		}
		else
		{
			SetAlpha(hdc,255);
			SetTextColor(hdc,MapRGB(hdc,10,10,100));
		}

		SendMessage(hwnd,LB_GETTEXT,i,(LPARAM)wbuf);
		DrawText(hdc,wbuf,-1,&rc,DT_SINGLELINE|DT_LEFT|DT_VCENTER);

		i++;
	}

//	BitBlt(ds->hDC,0,0,ds->rc.w,ds->rc.h,hdc,0,0,SRCCOPY); //将内存DC的内容输出到窗口(DC)中.
//	DeleteDC(hdc);
}


static int old_sel=0;
#if 0
static void make_bkgnd(HDC hdc,const RECT *prc)
{
	RECT rc;

	rc =*prc;

	SetBrushColor(hdc,MapRGB(hdc,10,50,100));
	//FillRect(hdc,&rc);
	GradientFillRect(hdc,&rc,MapRGB(hdc,230,230,230),MapRGB(hdc,0,0,200),TRUE);

	SetTextColor(hdc,MapRGB(hdc,250,50,50));
	SetFont(hdc,hFont24);
	TextOut(hdc,20,200,L"Backgroud-1234567890",-1);

	SetPenColor(hdc,MapRGB(hdc,0,250,0));

	rc.x =150;
	rc.y =150;
	rc.w =150;
	rc.h =100;
	DrawRect(hdc,&rc);

}
#endif

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

static	LRESULT	win_proc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	PAINTSTRUCT ps;
	RECT rc;
	int i;
	HWND wnd;
	HDC hdc;
	WCHAR wbuf[128];
	switch(msg)
	{
		case	WM_CREATE:
				
				hFont20 =XFT_CreateFont(ASCII_20_4BPP);
				hFont24 =XFT_CreateFont(ASCII_24_4BPP);


				GetClientRect(hwnd,&rc);
				
				//hdc_bk  =CreateMemoryDC(SURF_SCREEN,rc.w,rc.h);
				//make_bkgnd(hdc_bk,&rc);

				//CreateWindow(BUTTON,_T("Exit"),WS_BORDER|WS_VISIBLE|WS_CHILDWINDOW,8,8,80,30,hwnd,ID_EXIT,NULL,NULL);
				//wnd=CreateWindow(LISTBOX,_T("Listbox1"),WS_BORDER|WS_VISIBLE|WS_CHILDWINDOW,8,8,160,80,hwnd,ID_LISTBOX1,NULL,NULL);
				wnd=CreateWindow(LISTBOX,_T("Listbox"),WS_OWNERDRAW|LBS_LINE|LBS_NOTIFY|WS_BORDER|WS_VISIBLE,8,8,160,140,hwnd,ID_LISTBOX1,NULL,NULL);
				for(i=0;i<100;i++)
				{
					x_wsprintf(wbuf,L"Item-1-%03d",i);
					SendMessage(wnd,LB_ADDSTRING,-1,(LPARAM)wbuf);
				}


				SendMessage(wnd,LB_SETITEMHEIGHT,1,15);
				SendMessage(wnd,LB_SETITEMHEIGHT,3,50);
				SendMessage(wnd,LB_SETTOPINDEX,3,0);

				////
				wnd=CreateWindow(LISTBOX,_T("Listbox2"),WS_OWNERDRAW|LBS_LINE|WS_BORDER|WS_VISIBLE,200,8,150,120,hwnd,ID_LISTBOX2,NULL,NULL);
				for(i=0;i<100;i++)
				{
					x_wsprintf(wbuf,L"Item-2-%03d",i);
					SendMessage(wnd,LB_ADDSTRING,-1,(LPARAM)wbuf);
				}

				wnd=CreateWindow(LISTBOX,_T("Listbox3"),WS_OWNERDRAW|LBS_LINE|WS_BORDER|WS_VISIBLE,8,160,160,120,hwnd,ID_LISTBOX3,NULL,NULL);
				for(i=0;i<100;i++)
				{
					x_wsprintf(wbuf,L"Item-3-%03d",i);
					SendMessage(wnd,LB_ADDSTRING,-1,(LPARAM)wbuf);
				}

				wnd=CreateWindow(LISTBOX,_T("Listbox4"),WS_TRANSPARENT|WS_OWNERDRAW|LBS_LINE|WS_BORDER|WS_VISIBLE,200,160,160,120,hwnd,ID_LISTBOX4,NULL,NULL);
				for(i=0;i<100;i++)
				{
					x_wsprintf(wbuf,L"Item-4-%03d",i);
					SendMessage(wnd,LB_ADDSTRING,0,(LPARAM)wbuf);
				}


				old_sel =0;
				SetTimer(hwnd,1,50,TMR_START,NULL);
				return TRUE;
				////////

		case	WM_TIMER:
		{
				int cur_sel,i,y;
				RECT rc0;

				if(GetKeyState(VK_LBUTTON)==0) //鼠标左键／触摸屏　没有按下时.
				{
					////

					wnd =GetDlgItem(hwnd,ID_LISTBOX3); //获得LISTBOX3的HWND.

					cur_sel=SendMessage(wnd,LB_GETCURSEL,0,0); //获得LISTBOX当前选中的项.

					GetClientRect(wnd,&rc0);
					SendMessage(wnd,LB_GETITEMRECT,cur_sel,(LPARAM)&rc); //获得选中项的矩形位置.

					y =rc.y+(rc.h/2);

					if(y < (rc0.h/2)) //如果选中的项在Listbox　中线的上面,则往下移动.
					{
						if(abs(y-(rc0.h/2)) > (rc.h>>2))
						{//与中线隔得远，就每次多移一点距离(看上去移动速度快)．
							//i =rc.h>>2;
							i =4;
						}
						else
						{//与中线隔得很近了，就只移1个offset(缓慢移动)．
							i =1;
						}

						SendMessage(wnd,LB_OFFSETPOS,TRUE,i);

					}

					if(y > (rc0.h/2)) //如果选中的项在Listbox　中线的下面,则往上移动.
					{
						if(abs(y-(rc0.h/2)) > (rc.h>>2))
						{//与中线隔得远，就每次多移一点距离(看上去移动速度快)．
							//i =rc.h>>2;
							i =4;
						}
						else
						{//与中线隔得很近了，就只移1个offset(缓慢移动)．
							i =1;
						}

						SendMessage(wnd,LB_OFFSETPOS,TRUE,-i);

					}

				}
				

		}
				GUI_Printf("WM_TIMER: id:%d\r\n",wParam);
				break;
				
		
		case	WM_LBUTTONDOWN:
		/*
				i=MessageBox(hwnd,_T("Hello"),_T("MessageBox"),MB_YESNO|MB_ICONQUESTION);
				
				if(i==IDYES)
				{
					SendMessage(hwnd,WM_CLOSE,0,0);
				}  */
				
				//SendMessage(hwnd,WM_CLOSE,0,0);
				GUI_Printf("window0:WM_LBUTTONDOWN:%08XH,%d,%d\r\n",wParam,HIWORD(lParam),LOWORD(lParam));
				break;
				////
				
		case	WM_LBUTTONUP:
				GUI_Printf("window0:WM_LBUTTONUP:%08XH,%d,%d\r\n",wParam,HIWORD(lParam),LOWORD(lParam));
				//SendMessage(hwnd,WM_CLOSE,0,0);
				break;
				////
		case	WM_MOUSEMOVE:
				GUI_Printf("window0:WM_MOUSEMOVE:%08XH,%d,%d\r\n",wParam,HIWORD(lParam),LOWORD(lParam));
				break;
				////

		case	WM_NOTIFY:
		{
			NMHDR *nr;
			nr =(NMHDR*)lParam;

			if(nr->idFrom == ID_LISTBOX1)
			{
				if(nr->code == LBN_SELCHANGE)
				{
					int i;

					i =SendMessage(nr->hwndFrom,LB_GETCURSEL,0,0);

					SendMessage(nr->hwndFrom,LB_SETITEMHEIGHT,old_sel,26);

					SendMessage(nr->hwndFrom,LB_SETITEMHEIGHT,i,36);

					old_sel =i;

					GUI_Printf("Listbox SEL_CHANGE:%d.\r\n",i);
				}

				if(nr->code==LBN_CLICKED)
				{
					i =SendMessage(nr->hwndFrom,LB_GETCURSEL,0,0);
					i =SendMessage(nr->hwndFrom,LB_GETITEMOFFSET,i,0);

					//SendMessage(nr->hwndFrom,LB_SETPOS,0,i);
				}
			}

		}
		break;
		////
		case	WM_CTLCOLOR:


		case	WM_DRAWITEM:
		{
			DRAWITEM_HDR *ds;

			ds =(DRAWITEM_HDR*)lParam;

			if(wParam==ID_LISTBOX1)
			{
				listbox_owner_draw_1(ds);
				return TRUE;
			}

			if(wParam==ID_LISTBOX2)
			{
				listbox_owner_draw_2(ds);
				return TRUE;
			}

			if(wParam==ID_LISTBOX3)
			{
				listbox_owner_draw_3(ds);
				return TRUE;
			}

			if(wParam==ID_LISTBOX4)
			{
			//	HDC hdc_old,hdc_mem;
				RECT rc;
				POINT pt;

				rc =ds->rc;

			//	hdc_mem =CreateMemoryDC(SURF_SCREEN,rc.w,rc.h);

				//hdc_old =ds->hDC;
				//ds->hDC =hdc_mem;

				pt.x =rc.x;
				pt.y =rc.y;
				ClientToScreen(ds->hwnd,&pt,1);
				ScreenToClient(hwnd,&pt,1);
		//		BitBlt(ds->hDC,0,0,rc.w,rc.h,hdc_bk,pt.x,pt.y,SRCCOPY);

				listbox_owner_draw_4(ds);

				////



			//	BitBlt(hdc_old,0,0,rc.w,rc.h,hdc_mem,0,0,SRCCOPY);

			//	DeleteDC(hdc_mem);

				return TRUE;
			}

			return FALSE;
		}
		break;
		////

		case	WM_ERASEBKGND:
				return TRUE;
						
		case	WM_PAINT:
				hdc	=BeginPaint(hwnd,&ps);
				GetClientRect(hwnd,&rc);
				back_init(hdc,rc.w,rc.h,50);
				//SetBrushColor(hdc,MapRGB(hdc,10,20,200));
				//back_init(hdc_bk,rc.w,rc.h,50);
				//BitBlt(hdc,0,0,rc.w,rc.h,hdc_bk,0,0,SRCCOPY);

				EndPaint(hwnd,&ps);
				return	TRUE;
				////
			
		case	WM_CLOSE:
				{
					
					GUI_Printf("win_close\r\n");
					DestroyWindow(hwnd);
				}
				return	TRUE;
				////
				
		case	WM_DESTROY:
				{

					GUI_Printf("win_destroy:\r\n");

					DeleteFont(hFont20);
					DeleteFont(hFont24);
					//DeleteDC(hdc_bk);
					PostQuitMessage(hwnd);
				}
				return	TRUE;
				////
				
		default:
				return	DefWindowProc(hwnd,msg,wParam,lParam);
	}
	
	return	WM_NULL;
}

void	GUI_DEMO_Listbox_OwnerDraw(void)
{
		HWND	hwnd;
		WNDCLASS	wcex;
		MSG msg;
	
		//(��һ����.)
		InvalidateRect(GetDesktopWindow(),NULL,TRUE);
		
		/////
		wcex.Tag 		    = WNDCLASS_TAG;

		wcex.Style			= CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc	= win_proc;
		wcex.cbClsExtra		= 0;
		wcex.cbWndExtra		= 0;
		wcex.hInstance		= 0;//hInst;
		wcex.hIcon			= 0;//LoadIcon(hInstance, (LPCTSTR)IDI_WIN32_APP_TEST);
		wcex.hCursor		= 0;//LoadCursor(NULL, IDC_ARROW);
		
		
		hwnd	=CreateWindowEx(	WS_EX_FRAMEBUFFER,
									&wcex,
									_T("GUI Demo - Listbox-OwnerDraw"),
									WS_OVERLAPPEDWINDOW,
									20,30,400,320,
									NULL,NULL,NULL,NULL);
		
			
		ShowWindow(hwnd,SW_SHOW);	
		while(GetMessage(&msg,hwnd))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	
}
/*===================================================================================*/
/*===================================================================================*/
/*===================================================================================*/
/*===================================================================================*/
