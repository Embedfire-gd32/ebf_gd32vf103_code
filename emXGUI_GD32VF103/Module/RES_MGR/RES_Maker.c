

#include "Include.h"
#include "x_file.h"

#include "RES_MGR.h"

/*============================================================================*/
/*============================================================================*/
/*============================================================================*/
/*============================================================================*/

BOOL RES_DevRead(u8 *buf,u32 addr,u32 size);


typedef	struct __res_obj
{
	u32 size;
	u32 pos;

	u32 dir_off;
	u32 dat_off;
}res_obj;

#define	RES_SEEK_SET 0
#define	RES_SEEK_CUR 1

res_obj* x_res_open(const char *res_name)
{
	int i,len;
	struct _file_dir dir;
	res_obj *obj;

	len =x_strlen(res_name);
	for(i=0;i<4096;i+=32)
	{
		RES_DevRead((u8*)&dir,i,32);
		if(x_strncasecmp(dir.name,res_name,len)==0)
		{
			obj =kmalloc(sizeof(res_obj));
			if(obj!=NULL)
			{
				obj->dir_off =i;
				obj->dat_off =dir.offset;
				obj->size =dir.size;
				obj->pos =0;
			}
			return obj;

		}
	}

	return NULL;

}

u32 x_res_size(res_obj *res)
{
	return res->size;
}

u32 x_res_seek(res_obj *res,int pos,int mode)
{
	switch(mode)
	{
		case RES_SEEK_SET:
		res->pos =pos;
		break;
		////

		case RES_SEEK_CUR:
		res->pos += pos;
		break;
		////
	}

	res->pos =MIN(res->pos,res->size);
	return res->pos;
}

u32 x_res_read(res_obj *res,u8 *buf,u32 size)
{
	u32 addr;

	addr =res->pos;

	if((addr+size) > res->size)
	{
		size =res->size-addr;
	}

	if(size > 0)
	{
		addr += res->dat_off;
		RES_DevRead(buf,addr,size);
		res->pos += size;
	}

	return size;
}

void x_res_close(res_obj *res)
{
	kfree(res);
}

/*=========================================================================================*/

BOOL RES_DevRead(u8 *buf,u32 addr,u32 size);

static HINSTANCE hInst=NULL;
static HFONT hFont=NULL;
/*============================================================================*/


static int res_file_count=0;
static int res_file_size=0;

#if 0
s32 GetResOffset(u32 res_base,const char *res_name)
{
	int i,len;
	struct _file_dir dir;

	len =x_strlen(res_name);
	for(i=0;i<4096;i+=32)
	{
		RES_DevRead((u8*)&dir,res_base+i,32);

		if(x_strncasecmp(dir.name,res_name,len)==0)
		{
			return dir.offset;
		}
	}

	return -1;
}
#endif
/*============================================================================*/

static void GetResDirInfo(const char *res_dir,int *count,int *size)
{
	dir_obj *dir;
	struct dir_info fo;
	char buf[128];
	int res_count,res_size;

	res_count =0;
	res_size  =0;

	dir =SYS_opendir(res_dir);
	if(dir!=NULL)
	{
		fo.name_buf_size =128;
		fo.name_buf =buf;

		while(SYS_readdir(dir,&fo)==SYS_NO_ERR)
		{

			if(fo.name_buf[0] == '\0')
			{
				break;
			}

			if(fo.attr & FA_DIR)
			{
				continue;
			}
			res_count ++;
			res_size  += fo.fsize;
		}

		SYS_closedir(dir);
	}

	*count =res_count;
	*size  =res_size;
}


void MakeRes(const char *out_file,const char *res_dir,void (*cb)(int count,int size))
{
	dir_obj *dir;
	struct dir_info fo;
	X_FILE *fp_out,*fp0;
	char buf[128],*file_buf,*hdr_buf;
	int i,size,offset,file_count_cur,file_size_cur;

	file_count_cur =0;
	file_size_cur  =0;

	offset =4096; //跳过4096字节文件头.

	dir =SYS_opendir(res_dir);
	if(dir!=NULL)
	{
		file_buf =(char*)vmalloc(4096);
		hdr_buf  =(char*)vmalloc(4096); //res文件头为4096字节大小.

		if(file_buf == NULL || hdr_buf == NULL)
		{
			goto exit;
		}

		fp_out =x_fopen(out_file,"wb+");

		x_fseek(fp_out,0,SEEK_SET);
		x_fwrite(hdr_buf,4096,1,fp_out);

		fo.name_buf_size =128;
		fo.name_buf =buf;

		while(SYS_readdir(dir,&fo)==SYS_NO_ERR)
		{
			char path[256];

			if(fo.name_buf[0] == '\0')
			{
				break;
			}

			if(fo.attr & FA_DIR)
			{
				continue;
			}


			if(1)
			{
				size =0;

				x_sprintf(path,"%s/%s",res_dir,fo.name_buf);
				fp0 =x_fopen(path,"rb");
				if(fp0!=NULL)
				{
					while(!x_feof(fp0))
					{
						i=x_fread(file_buf,4096,1,fp0);
						if(i > 0)
						{
							file_size_cur += i;

							cb(file_count_cur,file_size_cur);

							if(i&0xF)
							{
								i += 16-(i&0xF);
							}
							size += x_fwrite(file_buf,i,1,fp_out);
						}
						else
						{
							SYS_msleep(20);
						}



					}
					x_fclose(fp0);
				}

			}

			if(size > 0)
			{
				memcpy(&hdr_buf[file_count_cur*32 + 0],fo.name_buf,28);
				memcpy(&hdr_buf[file_count_cur*32 + 24],&fo.fsize,4);
				memcpy(&hdr_buf[file_count_cur*32 + 28],&offset,4);

				file_count_cur++;
				offset += size;

				x_fseek(fp_out,offset,SEEK_SET);
			}
		}
		cb(file_count_cur,file_size_cur);

		//写文件头内容
		x_fseek(fp_out,0,SEEK_SET);
		x_fwrite(hdr_buf,4096,1,fp_out);
		x_fclose(fp_out);
exit:
		SYS_closedir(dir);

		if(file_buf!=NULL) 	vfree(file_buf);
		if(hdr_buf!=NULL)	vfree(hdr_buf);
	}

}


/*============================================================================*/

enum eID
{
	eID_EXIT =0x1000,
	eID_OK,
	eID_START,

	eID_PROG_FILE,
	eID_PROG_SIZE,
	eID_TEXT_SRC_DIR,
	eID_TEXT_DST_OUT,

	eID_TIMER_100ms
};

enum eMSG
{
	eMSG_MAKE_START=WM_USER+1,
	eMSG_MAKE_DONE,
};

/*============================================================================*/
static u16 rc_main_w,rc_main_h;
static HWND hwndMain=NULL;

static volatile int file_count_cur,file_size_cur;

static const char *ps_out_file=NULL;
static const char *ps_res_dir=NULL;

static mutex_obj *mutex_make=NULL;

static void res_callback(int file_count,int size)
{
	file_count_cur =file_count;
	file_size_cur  =size;
}

static int thread_maker(void *argv)
{
	SYS_thread_set_priority(NULL,-4);

	if(SYS_mutex_lock(mutex_make,100)==SYS_NO_ERR)
	{
		PostAsyncMessage(hwndMain,eMSG_MAKE_START,0,0);

		MakeRes(ps_out_file,ps_res_dir,res_callback);
		InvalidateRect(hwndMain,NULL,FALSE);
		PostAsyncMessage(hwndMain,eMSG_MAKE_DONE,0,0);
	}
	SYS_mutex_unlock(mutex_make);
	return TRUE;
}

static WNDPROC def_pb_proc=NULL;
//static WNDCLASSEX my_pb;

static	LRESULT	my_pb_proc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	if(msg==WM_ERASEBKGND)
	{
		return TRUE;
	}

	if(msg==WM_PAINT)
	{
		HDC hdc,hdc_mem;
		PAINTSTRUCT ps;
		RECT m_rc[2],rc;
		int rangle,val;
		WCHAR wbuf[128];

		GetClientRect(hwnd,&rc);

		BeginPaint(hwnd,&ps);

		hdc =CreateMemoryDC(SURF_SCREEN,rc.w,rc.h);
		if(hdc!=NULL)
		{

			GetWindowText(hwnd,wbuf,128);

			rangle =SendMessage(hwnd,PBM_GET_RANGLE,0,0);
			val  =SendMessage(hwnd,PBM_GET_VALUE,0,0);

			MakeProgressRect(m_rc,&rc,rangle,val,PB_ORG_LEFT);

			//SetBrushColor(hdc,MapRGB(hdc,0,220,0));
			//FillRect(hdc,&m_rc[0]);
			GradientFillRect(hdc,&m_rc[0],MapRGB(hdc,100,255,100),MapRGB(hdc,0,170,0),TRUE);

			SetBrushColor(hdc,MapRGB(hdc,255,255,255));
			FillRect(hdc,&m_rc[1]);

			SetPenColor(hdc,MapRGB(hdc,128,0,128));
			VLine(hdc,m_rc[1].x,m_rc[1].y,m_rc[1].h);

			SetTextColor(hdc,MapRGB(hdc,0,0,0));
			DrawText(hdc,wbuf,-1,&rc,DT_SINGLELINE|DT_LEFT|DT_VCENTER);

			BitBlt(ps.hdc,0,0,rc.w,rc.h,hdc,0,0,SRCCOPY);
			DeleteDC(hdc);
		}

		EndPaint(hwnd,&ps);
		return TRUE;
	}

	return def_pb_proc(hwnd,msg,wParam,lParam);

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
			//GetClassInfoEx(PROGRESSBAR,&my_pb);
			//def_pb_proc			= my_pb.lpfnWndProc;
			//my_pb.lpfnWndProc	= my_pb_proc;
			//my_pb.hInstance		= hInst;
			mutex_make =SYS_mutex_create(NULL);

			file_count_cur =0;
			file_size_cur  =0;

			GetResDirInfo(ps_res_dir,&res_file_count,&res_file_size);

			hwndMain =hwnd;

			GetClientRect(hwnd,&rc);
			rc_main_w =rc.w;
			rc_main_h =rc.h;

			rc.x =10;
			rc.y =48;
			rc.w =220;
			rc.h =24;
			wnd=CreateWindow(PROGRESSBAR,	L" ",	PBS_TEXT|WS_BORDER|WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,eID_PROG_FILE,hInst,NULL);
			def_pb_proc=SetWindowProc(wnd,my_pb_proc);
			SendMessage(wnd,PBM_SET_RANGLE,FALSE,res_file_count);
			SendMessage(wnd,PBM_SET_VALUE,FALSE,0);



			OffsetRect(&rc,0,rc.h+4);
			wnd=CreateWindow(PROGRESSBAR,	L" ",	PBS_TEXT|WS_BORDER|WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,eID_PROG_SIZE,hInst,NULL);
			SetWindowProc(wnd,my_pb_proc);
			SendMessage(wnd,PBM_SET_RANGLE,FALSE,res_file_size);
			SendMessage(wnd,PBM_SET_VALUE,FALSE,0);
			//SendMessage(wnd,PBM_SET_MODE,FALSE,PBM_L_R);

			OffsetRect(&rc,0,rc.h+4);
			x_mbstowcs(wbuf,ps_res_dir,128);
			CreateWindow(TEXTBOX,	wbuf,	TBS_SUNK|WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,eID_TEXT_SRC_DIR,hInst,NULL);

			OffsetRect(&rc,0,rc.h+4);
			x_mbstowcs(wbuf,ps_out_file,128);
			CreateWindow(TEXTBOX,	wbuf,	TBS_SUNK|WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,eID_TEXT_DST_OUT,hInst,NULL);

			OffsetRect(&rc,0,rc.h+4);
			rc.w =100;
			rc.h =40;
			CreateWindow(BUTTON,	L"Exit",	WS_BORDER|WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,eID_EXIT,hInst,NULL);

			OffsetRect(&rc,rc.w+4,0);
			CreateWindow(BUTTON,	L"Start",	WS_BORDER|WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,eID_START,hInst,NULL);


			//OffsetRect(&rc,0,rc.h+4);
			//CreateWindow(BUTTON,	L"OK",		BS_3D|WS_BORDER|WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,eID_OK,hInst,NULL);

			SetTimer(hwnd,eID_TIMER_100ms,150,TMR_START,NULL);

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
				file_count_cur =0;
				file_size_cur =0;
				SYS_thread_create(thread_maker,NULL,4096,NULL,0);

			}


		}
		break;
		////
		case WM_TIMER:
		{
			u16 id =LOWORD(wParam);
			if(id==eID_TIMER_100ms)
			{

				wnd =GetDlgItem(hwnd,eID_PROG_FILE);
				SendMessage(wnd,PBM_SET_VALUE,TRUE,file_count_cur);

				x_wsprintf(wbuf,L"FILE:%d/%d",file_count_cur,res_file_count);
				SetWindowText(wnd,wbuf);

				wnd =GetDlgItem(hwnd,eID_PROG_SIZE);
				SendMessage(wnd,PBM_SET_VALUE,TRUE,file_size_cur);
				x_wsprintf(wbuf,L"SIZE:%d/%d",file_size_cur,res_file_size);
				SetWindowText(wnd,wbuf);

				//InvalidateRect(hwnd,NULL,FALSE);
			}
		}
		break;
		////


		//case	WM_ERASEBKGND:
		//return TRUE;
		////

		case	WM_PAINT:
		{
			HDC hdc;
			PAINTSTRUCT ps;

			hdc =BeginPaint(hwnd,&ps);
			if(hdc!=NULL)
			{
				rc.x =0;
				rc.y =0;
				rc.w =rc_main_w;
				rc.h =32;
				SetTextColor(hdc,MapRGB(hdc,0,0,0));
				SetBrushColor(hdc,MapRGB(hdc,130,180,200));

				//GradientFillRect(hdc,&rc,MapRGB(hdc,0,0,200),MapRGB(hdc,255,255,255),TRUE);
				DrawText(hdc,L"RES Maker",-1,&rc,DT_SINGLELINE|DT_VCENTER|DT_CENTER|DT_BKGND);

				EndPaint(hwnd,&ps);
			}

		}
		break;
		////

		case	WM_CLOSE:
		if(SYS_mutex_lock(mutex_make,100)==SYS_NO_ERR)
		{
			SYS_mutex_destroy(mutex_make);
			DestroyWindow(hwnd);
		}
		break;
		////

		case	eMSG_MAKE_START:
		EnableWindow(GetDlgItem(hwnd,eID_START),FALSE);
		EnableWindow(GetDlgItem(hwnd,eID_EXIT),FALSE);
		break;

		case	eMSG_MAKE_DONE:
		EnableWindow(GetDlgItem(hwnd,eID_START),TRUE);
		EnableWindow(GetDlgItem(hwnd,eID_EXIT),TRUE);
		break;
		////


		default:

			return	DefWindowProc(hwnd,msg,wParam,lParam);
	}

	return	WM_NULL;
}

int	Dlg_RES_Maker(int x,int y,int w,int h,const char *out_file,const char *res_dir)
{
	HWND hwnd;
	MSG	msg;
	WNDCLASS wcex;

	////
	ps_out_file =out_file;
	ps_res_dir  =res_dir;
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

	hwnd	=CreateWindow(	&wcex,L"RES Maker",WS_CLIPCHILDREN,
							x,y,w,h,
							NULL,0,NULL,NULL);

	ShowWindow(hwnd,SW_SHOW);
	UpdateWindow(hwnd);

	while(GetMessage(&msg,hwnd))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return	TRUE;
}

