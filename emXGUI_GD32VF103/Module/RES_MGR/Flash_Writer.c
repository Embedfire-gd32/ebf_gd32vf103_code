

#include "Include.h"

#include "x_file.h"
#include "RES_MGR.h"


/*============================================================================*/
/*============================================================================*/

U32  RES_DevGetID(void);
BOOL RES_DevWrite(u8 *buf,u32 addr,u32 size);
BOOL RES_DevRead(u8 *buf,u32 addr,u32 size);
int  RES_DevEraseSector(u32 addr);

/*============================================================================*/
static u32 dev_id=0;

static HWND hwndMain=NULL;

static u32 write_flash_bps=0;
static u8  write_flash_finshed=FALSE;

/*============================================================================*/

void CopyFileToSPIFlash(u32 addr,const char *file_path,void (*cb)(u32 cur_size,u32 total_size))
{
	char *wt_buf;
	char *rd_buf;
	u32 t1,t2,wt_n;

	wt_buf =dma_mem_alloc(4096);
	rd_buf =dma_mem_alloc(4096);

	//sf_ReadBuffer(buf,0,32);
	//if(buf[0]!='X')
	{
		X_FILE *fp;

		int i,n,fsize;

		//fp =x_fopen("B:kernel/fonts/GB2312_20_4BPP.xft","rb");
		fp =x_fopen(file_path,"rb");
		if(fp!=NULL)
		{
			fsize =x_fsize(fp);
#if 0
			if(1)
			{
				while(i < fsize)
				{
					i += RES_DevEraseSector(addr);
					//i += 0x40000;
				}
			}
#endif
			wt_n=0;
			t1 =GUI_GetTickCount();
			for(i=0;i<fsize;i+=n)
			{
				n =x_fread(wt_buf,4096,1,fp);
				RES_DevWrite(wt_buf,addr+i,n);
				if(1)
				{
					memset(rd_buf,0,4096);
					RES_DevRead(rd_buf,addr+i,4096);

					if(memcmp(wt_buf,rd_buf,n)!=0)
					{
						SYS_msleep(50);
						MessageBox(hwndMain,10,20,240,160,L"Flash Write Error!",L"Error",NULL);
					}
				}


				t2 =GUI_GetTickCount();
				if((t2-t1)>1000)
				{
					write_flash_bps=wt_n;

					wt_n=0;
					t1=t2;
				}
				else
				{
					wt_n += n;
				}

				if(cb!=NULL)
				{
					cb(i,fsize);
				}
			}
			x_fclose(fp);
			if(cb!=NULL)
			{
				cb(i,fsize);
			}
		}


	}
	write_flash_finshed=TRUE;
	dma_mem_free(wt_buf);
	dma_mem_free(rd_buf);
}

/*============================================================================*/


/*============================================================================*/
/*============================================================================*/

enum eID
{
	eID_EXIT =0x1000,
	eID_OK,
	eID_START,
	eID_TIMER_100ms
};

enum eMSG
{
	eMSG_PROG_START  =WM_USER+1,
	eMSG_PROG_DONE,
};

#define	BKCOLOR	RGB888(120,180,190)

/*============================================================================*/
extern const char ASCII_16_4BPP[];

static HFONT hFont=NULL;
//static HINSTANCE hInst=NULL;

static u16 rc_main_w,rc_main_h;

static const char *ps_file=NULL;
static u32 write_addr=0;


static WCHAR *wszFilePath=NULL;
static u32     FileSize=0;

static u32 write_flash_cur=0;
static u32 write_flash_size=0;

static mutex_obj *mutex_thread=NULL;

static void cb_write_flash(u32 cur_size,u32 total_size)
{
	static int t0=0;
	int t1;


	write_flash_cur =cur_size;
	write_flash_size =total_size;

	t1 =GUI_GetTickCount();
	if((t1-t0) > 100)
	{
		t0=t1;
		InvalidateRect(hwndMain,NULL,FALSE);
		SYS_msleep(5);

	}


}

static int thread_write_flash(void *argv)
{
	if(SYS_mutex_lock(mutex_thread,100)==SYS_NO_ERR)
	{
		PostAsyncMessage(hwndMain,eMSG_PROG_START,0,0);
		CopyFileToSPIFlash(write_addr,ps_file,cb_write_flash);
		InvalidateRect(hwndMain,NULL,FALSE);
		PostAsyncMessage(hwndMain,eMSG_PROG_DONE,0,0);
	}
	SYS_mutex_unlock(mutex_thread);
	return TRUE;
}

static	LRESULT	WinProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	RECT m_rc[2],rc;
	WCHAR wbuf[128];
	HWND wnd;

	switch(msg)
	{

		case	WM_CREATE:
		{
			dev_id =RES_DevGetID();

			mutex_thread =SYS_mutex_create(NULL);

			write_flash_cur =0;
			write_flash_size =0;
			write_flash_finshed=FALSE;

			hwndMain =hwnd;
			hFont =XFT_CreateFont(ASCII_16_4BPP);

			GetClientRect(hwnd,&rc);
			rc_main_w =rc.w;
			rc_main_h =rc.h;

			rc.w =100;
			rc.h =32;
			rc.x =8;
			rc.y =rc_main_h-rc.h-2;

			wnd=CreateWindow(BUTTON,	L"Exit",	WS_BORDER|WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,eID_EXIT,NULL,NULL);
			SetWindowFont(wnd,hFont);

			OffsetRect(&rc,rc.w+4,0);
			wnd=CreateWindow(BUTTON,	L"Start",	WS_BORDER|WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,eID_START,NULL,NULL);
			SetWindowFont(wnd,hFont);

			//OffsetRect(&rc,0,rc.h+8);
			//CreateWindow(BUTTON,	L"OK",		BS_3D|WS_BORDER|WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,eID_OK,hInst,NULL);


			SetTimer(hwnd,eID_TIMER_100ms,200,TMR_START,NULL);

		}
		break;
		////

		case	WM_NOTIFY:
		{
			u16 event,id;

			event =HIWORD(wParam);
			id =LOWORD(wParam);

			if(id==eID_EXIT && event==BN_CLICKED)
			{
				PostCloseMessage(hwnd);
			}


			if(id==eID_START && event==BN_CLICKED)
			{
				write_flash_cur =0;
				write_flash_size =0;
				SYS_thread_create(thread_write_flash,NULL,8192,NULL,0);

			}

		}
		break;
		////
		case WM_TIMER:
		{
			u16 id =LOWORD(wParam);
			if(id==eID_TIMER_100ms)
			{
				//InvalidateRect(hwnd,NULL,FALSE);
			}
		}
		break;
		////

		case eMSG_PROG_START:
		{
			EnableWindow(GetDlgItem(hwnd,eID_START),FALSE);
			EnableWindow(GetDlgItem(hwnd,eID_EXIT),FALSE);
		}
		break;
		////

		case eMSG_PROG_DONE:
		{
			//EnableWindow(GetDlgItem(hwnd,eID_START),TRUE);
			EnableWindow(GetDlgItem(hwnd,eID_EXIT),TRUE);
		}
		break;
		////


		case	WM_ERASEBKGND:
		{
			HDC hdc =(HDC)wParam;
			RECT rc =*(RECT*)lParam;
			SetBrushColor(hdc,MapRGB888(hdc,BKCOLOR));
			FillRect(hdc,&rc);
		}
		return TRUE;
		////

		case	WM_PAINT:
		{
			HDC hdc;
			PAINTSTRUCT ps;

			hdc =BeginPaint(hwnd,&ps);
			if(hdc!=NULL)
			{
				SetFont(hdc,hFont);

				rc.x =0;
				rc.y =0;
				rc.w =rc_main_w;
				rc.h =28;
				SetTextColor(hdc,MapRGB(hdc,250,240,240));
				SetBrushColor(hdc,MapRGB(hdc,200,10,10));
				x_wsprintf(wbuf,L"Flash Writer(%08X)",dev_id);
				DrawText(hdc,wbuf,-1,&rc,DT_SINGLELINE|DT_VCENTER|DT_CENTER|DT_BKGND);

				SetTextColor(hdc,MapRGB(hdc,0,0,0));
				SetBrushColor(hdc,MapRGB888(hdc,BKCOLOR));

				OffsetRect(&rc,0,rc.h+8);
				x_wsprintf(wbuf,L"File Path: %s",wszFilePath);
				DrawText(hdc,wbuf,-1,&rc,DT_SINGLELINE|DT_VCENTER|DT_LEFT|DT_BKGND);

				OffsetRect(&rc,0,rc.h);
				x_wsprintf(wbuf,L"File Size: %d Bytes",FileSize);
				DrawText(hdc,wbuf,-1,&rc,DT_SINGLELINE|DT_VCENTER|DT_LEFT|DT_BKGND);

				OffsetRect(&rc,0,rc.h);
				x_wsprintf(wbuf,L"Flash ID: %08X",dev_id);
				DrawText(hdc,wbuf,-1,&rc,DT_SINGLELINE|DT_VCENTER|DT_LEFT|DT_BKGND);

				if(write_flash_cur > 0)
				{
				SetTextColor(hdc,MapRGB(hdc,0,0,0));

				OffsetRect(&rc,0,rc.h+8);
				rc.h =24;
				MakeProgressRect(m_rc,&rc,write_flash_size,write_flash_cur,PB_ORG_LEFT);
				SetBrushColor(hdc,MapRGB(hdc,10,230,10));
				FillRect(hdc,&m_rc[0]);
				SetBrushColor(hdc,MapRGB(hdc,255,255,255));
				FillRect(hdc,&m_rc[1]);
				x_wsprintf(wbuf,L"%d,  %dKB/S  ",write_flash_cur,write_flash_bps>>10);
				DrawText(hdc,wbuf,-1,&rc,DT_SINGLELINE|DT_VCENTER|DT_RIGHT);
				}

				if(write_flash_finshed)
				{
					SetBrushColor(hdc,MapRGB888(hdc,BKCOLOR));
					SetTextColor(hdc,MapRGB(hdc,180,10,10));
					OffsetRect(&rc,0,rc.h+8);
					x_wsprintf(wbuf,L"Finshed!");
					DrawText(hdc,wbuf,-1,&rc,DT_SINGLELINE|DT_VCENTER|DT_CENTER|DT_BKGND);

				}

				EndPaint(hwnd,&ps);
			}

		}
		break;
		////

		case	WM_CLOSE:
		if(SYS_mutex_lock(mutex_thread,100)==SYS_NO_ERR)
		{
			SYS_mutex_destroy(mutex_thread);

			DeleteFont(hFont);
			DestroyWindow(hwnd);
		}
		break;
		////


		default:
			return	DefWindowProc(hwnd,msg,wParam,lParam);
	}

	return	WM_NULL;
}

int	Dlg_Flash_Writer(int x,int y,int w,int h,const char *file_path,u32 addr)
{
	HWND hwnd;
	MSG	msg;
	WNDCLASS wcex;

	////
	ps_file 	=file_path;
	write_addr  =addr;

	if(1)
	{
		X_FILE *fp;

		fp =x_fopen(file_path,"rb");
		if(fp!=NULL)
		{
			FileSize =x_fsize(fp);
			x_fclose(fp);
		}
		else
		{
			return FALSE;
		}
	}

	wszFilePath =(WCHAR*)vmalloc(256*sizeof(WCHAR));

	x_mbstowcs(wszFilePath,file_path,256);

	//Sleep(200);
	//SYS_thread_create(thread,(void*)0x1234,4096,stack,0);

	wcex.Tag	 		= WNDCLASS_TAG;
	wcex.Style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)WinProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= NULL;
	wcex.hIcon			= NULL;
	wcex.hCursor		= NULL;


	hwnd	=CreateWindow(	&wcex,L"Flash Writer",WS_CLIPCHILDREN,
							x,y,w,h,
							NULL,0,NULL,NULL);

	ShowWindow(hwnd,SW_SHOW);
	UpdateWindow(hwnd);

	while(GetMessage(&msg,hwnd))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	vfree(wszFilePath);
	return	TRUE;
}

