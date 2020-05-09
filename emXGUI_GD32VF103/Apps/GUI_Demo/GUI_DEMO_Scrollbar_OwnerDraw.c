
#include <emXGUI.h>
#include <x_libc.h>

#include <libsdk_png.h>

#ifndef	_T
#define	_T(x) L##x
#endif

extern const char ASCII_40_4BPP[];
extern const unsigned char gImage_5[];

extern const u8 fr_img[];
extern const u8 bk_img[];

/*===================================================================================*/
#define	ID_EXIT			0x1000
#define	ID_SCROLLBAR1	0x1101
#define	ID_SCROLLBAR2	0x1102
#define	ID_TEXTBOX1		0x1200


/*===================================================================================*/
static HFONT hFont40=NULL;
static BITMAP bm3;
static PNG_DEC *fr_png,*bk_png;

static void textbox_owner_draw(DRAWITEM_HDR *ds)
{
	HDC hdc;
	RECT rc;
	WCHAR wbuf[40];

	rc =ds->rc;
	hdc =ds->hDC;

	GetWindowText(ds->hwnd,wbuf,40);

	EnableAntiAlias(hdc,FALSE);
	EnableAlpha(hdc,TRUE);
	SetAlpha(hdc,100);
	SetBrushColor(hdc,MapRGB(hdc,100,100,100));
	FillRect(hdc,&rc);

	EnableAlpha(hdc,FALSE);
	SetPenColor(hdc,MapRGB(hdc,200,200,200));
	SetPenSize(hdc,3);
	DrawRect(hdc,&rc);

	SetTextColor(hdc,MapRGB(hdc,250,0,0));
	DrawText(hdc,wbuf,-1,&rc,DT_SINGLELINE|DT_VCENTER|DT_CENTER);

}

static void scrollbar_owner_draw(DRAWITEM_HDR *ds)
{
	HWND hwnd;
	HDC hdc;
	RECT rc,rc0;
	int i;
	WCHAR wbuf[128];
	SCROLLINFO sif;
	u32 br_c;
	BITMAP bm;
	hwnd =ds->hwnd;
	hdc =ds->hDC;

	GetClientRect(hwnd,&rc);


	sif.fMask =SIF_ALL;

	SendMessage(hwnd,SBM_GETSCROLLINFO,0,(LPARAM)&sif);


	////画背景.
	SetPenColor(hdc,MapRGB(hdc,10,10,100));
	DrawRect(hdc,&rc);

	PNG_DecodeGetBitmap(bk_png,&bm);
	DrawBitmap(hdc,rc.x,rc.y+(rc.h-bm.Height)/2,&bm,NULL);


	////画背景框.
	if(1)
	{
		int x,y,r;
		RECT rc0;
		i =SendMessage(hwnd,SBM_GETTRACKRECT,0,(LPARAM)&rc);
		DrawRect(hdc,&rc);

		rc0.x=ds->rc.x;
		rc0.y =ds->rc.y;
		rc0.w =rc.x-rc0.x;
		rc0.h =ds->rc.h;

		////画滑块.
		PNG_DecodeGetBitmap(fr_png,&bm);
		DrawBitmap(hdc,rc.x+(int)(rc.w-(int)bm.Width)/2,rc.y+(int)(rc.h-(int)bm.Height)/2,&bm,NULL);

	}



}

static void scrollbar_owner_drawxxx(DRAWITEM_HDR *ds)
{
	HWND hwnd;
	HDC hdc;
	RECT rc,rc0;
	int i;
	WCHAR wbuf[128];
	SCROLLINFO sif;
	u32 br_c;

	hwnd =ds->hwnd;
	hdc =ds->hDC;

	GetClientRect(hwnd,&rc);


	sif.fMask =SIF_ALL;

	SendMessage(hwnd,SBM_GETSCROLLINFO,0,(LPARAM)&sif);

	////画背景.
	EnableAntiAlias(hdc,FALSE);
	EnableAlpha(hdc,TRUE);
	SetAlpha(hdc,100);
	SetBrushColor(hdc,MapRGB(hdc,150,150,150));
	rc0 =rc;
	InflateRectEx(&rc0,0,-(rc0.h>>2),0,-(rc0.h>>2));
	FillRoundRect(hdc,&rc0,rc0.h/2);

	////画背景框.
	EnableAntiAlias(hdc,TRUE);
	EnableAlpha(hdc,TRUE);
	SetAlpha(hdc,200);
	SetPenSize(hdc,2);
	SetPenColor(hdc,MapRGB(hdc,240,240,240));
	DrawRoundRect(hdc,&rc0,rc0.h/2);

	if(1)
	{
		int x,y,r;
		RECT rc0;
		i =SendMessage(hwnd,SBM_GETTRACKRECT,0,(LPARAM)&rc);

		rc0.x=ds->rc.x;
		rc0.y =ds->rc.y;
		rc0.w =rc.x-rc0.x;
		rc0.h =ds->rc.h;

		EnableAntiAlias(hdc,TRUE);
		EnableAlpha(hdc,FALSE);
		InflateRectEx(&rc0,-4,-((rc0.h-10)/2),-(rc.w/2),-((rc0.h-10)/2));

		SetBrushColor(hdc,MapRGB(hdc,0,100,250));
		SetPenColor(hdc,MapRGB(hdc,150,200,250));
		FillRoundRect(hdc,&rc0,rc0.h/2);
		DrawRoundRect(hdc,&rc0,rc0.h/2);

		////画滑块.
		x =rc.x+(rc.w>>1);
		y =rc.y+(rc.h>>1);
		r =MIN(ds->rc.w,ds->rc.h)/2;
		EnableAntiAlias(hdc,FALSE);
		EnableAlpha(hdc,TRUE);
		SetAlpha(hdc,150);
		if(ds->State & SST_THUMBTRACK)
		{
			SetAlpha(hdc,200);
			SetBrushColor(hdc,MapRGB(hdc,50,100,250));
		}
		else
		{
			SetAlpha(hdc,150);
			SetBrushColor(hdc,MapRGB(hdc,240,240,240));
		}
		FillCircle(hdc,x,y,r-4);

		EnableAntiAlias(hdc,TRUE);
		EnableAlpha(hdc,TRUE);
		SetAlpha(hdc,200);
		SetPenSize(hdc,2);
		SetPenColor(hdc,MapRGB(hdc,240,240,240));
		DrawCircle(hdc,x,y,r-4);

	}

	//DrawRect(hdc,&rc);
	//x_wsprintf(wbuf,L"%d",sif.nValue);

	//InflateRect(&rc,40,0);
	//DrawText(hdc,wbuf,-1,&rc,DT_VCENTER|DT_CENTER);

}



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

static	LRESULT	win_proc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	HDC hdc;
	PAINTSTRUCT ps;
	RECT rc,rc0;
	WCHAR wbuf[128];
	int i,x,y;
	HWND wnd;

	switch(msg)
	{
		case	WM_CREATE:
				
				GetClientRect(hwnd,&rc0);
				
				if(1)
				{
					SCROLLINFO sif;
					sif.cbSize		=sizeof(sif);
					sif.fMask		=SIF_ALL;
					sif.nMin		=-500;
					sif.nMax		=+500;
					sif.nValue		=0;
					sif.TrackSize	=80;
					sif.ArrowSize	=20;


					rc.w =600;
					rc.h =90;
					rc.x =(rc0.w-rc.w)/2;
					rc.y =rc0.h>>1;
					wnd = CreateWindow(SCROLLBAR,L"HScroll",WS_OWNERDRAW|WS_VISIBLE|WS_TRANSPARENT,rc.x,rc.y,rc.w,rc.h,hwnd,ID_SCROLLBAR1,NULL,NULL);
					SendMessage(wnd,SBM_SETSCROLLINFO,TRUE,(LPARAM)&sif);

					rc.w =200;
					rc.h =100;
					rc.x =(rc0.w-rc.w)/2;
					rc.y =40;
					wnd = CreateWindow(TEXTBOX,L"---",WS_OWNERDRAW|WS_VISIBLE|WS_TRANSPARENT,rc.x,rc.y,rc.w,rc.h,hwnd,ID_TEXTBOX1,NULL,NULL);
					SetWindowFont(wnd,hFont40);
				}


				return TRUE;
				////////

		case	WM_NOTIFY: //通知消息
		{
			NMHDR *nr;
			u16 ctr_id;

			ctr_id =LOWORD(wParam); //wParam低16位是发送该消息的控件ID.
			nr =(NMHDR*)lParam; //lParam参数，是以NMHDR结构体开头.

			if(ctr_id == ID_SCROLLBAR1 || ctr_id == ID_SCROLLBAR2 )
			{
				NM_SCROLLBAR *sb_nr;
				int i;

				sb_nr =(NM_SCROLLBAR*)nr; //Scrollbar的通知消息实际为 NM_SCROLLBAR扩展结构,里面附带了更多的信息.
				switch(nr->code)
				{
					case SBN_CLICKED: //单击
					{

						if(sb_nr->cmd==SB_TRACK) //NM_SCROLLBAR结构体成员cmd指明了单击发生的位置
						{//在滑块内单击.
							//GUI_Printf("SCROLLBAR CLICK In Track.\r\n");
						}
						else
						{
							//GUI_Printf("SCROLLBAR CLICK :%d.\r\n",sb_nr->cmd);
						}
					}
					break;
					////

					case SBN_THUMBTRACK: //滑块移动
					{

						i =sb_nr->nTrackValue; //获得滑块当前位置值

						SendMessage(nr->hwndFrom,SBM_SETVALUE,TRUE,i); //设置位置值


						x_wsprintf(wbuf,L"%d",i);
						wnd =GetDlgItem(hwnd,ID_TEXTBOX1);
						if(wnd!=NULL)
						{
							SetWindowText(wnd,wbuf);
							InvalidateRect(wnd,NULL,FALSE);
						}
						//GUI_Printf("SCROLLBAR TRACK :%d.\r\n",i);
					}
					break;
					////

					default:
						break;

				}

			}

		}
		break;
		////
#if 0
		case	WM_CTLCOLOR:
		{
			u16 id;

			id =LOWORD(wParam);

			if(id== ID_SCROLLBAR1)
			{
				CTLCOLOR *cr;
				cr =(CTLCOLOR*)lParam;


				cr->TextColor =RGB888(100,255,255);
				cr->BackColor =RGB888(100,100,150);
				cr->BorderColor =RGB888(50,50,150);
				cr->ForeColor =RGB888(50,150,250);

				return TRUE;
			}
			else
			{
				return FALSE;
			}

		}
		break;
		////
#endif

		case	WM_DRAWITEM:
		{
			DRAWITEM_HDR *ds;

			ds =(DRAWITEM_HDR*)lParam;
#if 1
			if(wParam==ID_SCROLLBAR1)
			{
				scrollbar_owner_draw(ds);
				return TRUE;
			}
			if(wParam==ID_TEXTBOX1)
			{
				textbox_owner_draw(ds);
				return TRUE;
			}
#endif
			return FALSE;
		}
		break;
						
		case	WM_PAINT:
				hdc	=BeginPaint(hwnd,&ps);
				GetClientRect(hwnd,&rc);
				back_init(hdc,rc.w,rc.h,80);
				
				EndPaint(hwnd,&ps);
				return	TRUE;
				////
			
				
		default:
				return	DefWindowProc(hwnd,msg,wParam,lParam);
	}
	
	return	WM_NULL;
}

void	GUI_DEMO_ScrollbarOwnerDraw(void)
{
		HWND	hwnd;
		WNDCLASS	wcex;
		MSG msg;

		/////
		bm3.Format = BM_ARGB8888;
		bm3.Width = 92;
		bm3.Height = 92;
		bm3.WidthBytes =bm3.Width*4;
		bm3.LUT =NULL;
		bm3.Bits =(void*)gImage_5;

		fr_png =PNG_DecodeOpen(fr_img);
		bk_png =PNG_DecodeOpen(bk_img);

		hFont40 =XFT_CreateFont(ASCII_40_4BPP);

		wcex.Tag 		    = WNDCLASS_TAG;

		wcex.Style			= CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc	= win_proc;
		wcex.cbClsExtra		= 0;
		wcex.cbWndExtra		= 0;
		wcex.hInstance		= 0;
		wcex.hIcon			= 0;//
		wcex.hCursor		= 0;
		
		
		
		hwnd	=CreateWindowEx(	WS_EX_FRAMEBUFFER,
									&wcex,
									_T("GUI Demo - Scrollbar-OwnerDraw"),
									WS_OVERLAPPEDWINDOW,
									0,0,GUI_XSIZE,GUI_YSIZE,
									NULL,NULL,NULL,NULL);
		
			
		ShowWindow(hwnd,SW_SHOW);	
		while(GetMessage(&msg,hwnd))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	
		DeleteFont(hFont40);
		PNG_DecodeClose(fr_png);
		PNG_DecodeClose(bk_png);
}

/*===================================================================================*/
/*===================================================================================*/
/*===================================================================================*/
/*===================================================================================*/
