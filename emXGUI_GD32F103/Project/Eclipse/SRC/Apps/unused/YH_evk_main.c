
#include	"Include.h"
#include	"Kernel.h"

#include	"emXGUI.h"
#include	"CListMenu.h"
#include 	"YH_evk_app.h"

/*=========================================================================*/


void 	GamePaperMain(int x,int y,int w,int h,const WCHAR *pName);
void 	ChineseChessMain(void);
void 	GUI_Demo(void);
void 	GUI_DEMO_RectSpeed(void);
int		InfoNES_WinMain(HANDLE hInstance,void *argv);
void 	AudioPlayer(const WCHAR* Name,int x,int y,int w,int h);
int		ImageView(int x,int y,int w,int h,const WCHAR *Name,const char *path);
void	palette_win(void);
void	CameraTest(void);
void	Camera_QRDecoder(void);
void	GUI_DEMO_TTF(void);
void	GUI_DEMO_WatchTime(void);
void	GUI_DEMO_WatchTime_Bitmap(void);


/*============================================================================*/

typedef int  FN_ThreadEntry(void *argv);

//获得当前时间
static void GetTime(U8 *hour,U8 *min,U8 *sec)
{
	local_time_t tm;
	SYS_get_local_time(&tm);

	if(hour!=NULL)
	{
		*hour =tm.hour;
	}
	if(min!=NULL)
	{
		*min =tm.min;
	}
	if(sec!=NULL)
	{
		*sec =tm.sec;
	}
}

//获得CPU占用百分
static U8 GetCPUUsage(void)
{
	return mk_cpu_usage(0);
}

//创建线程。
static void _CreateThread(FN_ThreadEntry *Entry,void *argv,int StackSize)
{
	SYS_thread_create ((pfn_thread_entry)Entry,argv,StackSize,NULL,0);
}


/*=========================================================================*/

static void MSGBOX(HWND hwnd,const WCHAR *Text,const WCHAR *Caption)
{
	MSGBOX_OPTIONS ops;
	const WCHAR *btn[]={L"确定"};

	ops.Flag =MB_BTN_WIDTH(60)|MB_ICONINFORMATION;
	ops.pButtonText =btn;
	ops.ButtonCount =1;

	MessageBox(hwnd,20,50,250,150,Text,Caption,&ops);
}

/*=========================================================================*/
/*=========================================================================*/
/*=========================================================================*/
/*=========================================================================*/
/*=========================================================================*/

static void app_gui_test(HWND hwnd)
{
	static int thread=0;
	static int app=0;

	if(thread==0)
	{  //创建一个独立线程来运行自已...
		_CreateThread((FN_ThreadEntry*)app_gui_test,hwnd,8192);
		thread =1;
		return;
	}

	if(thread==1) //线程已创建了?
	{
		if(app==0)
		{
			app=1;
			GUI_Demo();

			app=0;
			thread=0;
		}
		else
		{
			MSGBOX(hwnd,L"程序已在运行中...",L"消息提示!");
		}
	}
}

/*=========================================================================*/

static void app_ttf_test(HWND hwnd)
{
	static int thread=0;
	static int app=0;

	if(thread==0)
	{  //创建一个独立线程来运行自已...
		_CreateThread((FN_ThreadEntry*)app_ttf_test,hwnd,8192);
		thread =1;
		return;
	}

	if(thread==1) //线程已创建了?
	{
		if(app==0)
		{
			app=1;
			GUI_DEMO_TTF();

			app=0;
			thread=0;
		}
		else
		{
			MSGBOX(hwnd,L"程序已在运行中...",L"消息提示!");
		}
	}

}

/*=========================================================================*/

static void app_watchtime_test(HWND hwnd)
{
	static int thread=0;
	static int app=0;

	if(thread==0)
	{  //创建一个独立线程来运行自已...
		_CreateThread((FN_ThreadEntry*)app_watchtime_test,hwnd,8192);
		thread =1;
		return;
	}

	if(thread==1) //线程已创建了?
	{
		if(app==0)
		{
			app=1;
			GUI_DEMO_WatchTime_Bitmap();
			GUI_DEMO_WatchTime();


			app=0;
			thread=0;
		}
		else
		{
			MSGBOX(hwnd,L"程序已在运行中...",L"消息提示!");
		}
	}
}

/*=========================================================================*/

static void app_gamepager(HWND hwnd)
{
	static int thread=0;
	static int app=0;

	if(thread==0)
	{  //创建一个独立线程来运行自已...
		_CreateThread((FN_ThreadEntry*)app_gamepager,hwnd,8192);
		thread =1;
		return;
	}

	if(thread==1) //线程已创建了?
	{
		if(app==0)
		{
			app=1;
			GamePaperMain(0,0,GUI_XSIZE,GUI_YSIZE,L"纸牌");

			app=0;
			thread=0;
		}
		else
		{
			MSGBOX(hwnd,L"程序已在运行中...",L"消息提示!");
		}
	}
}

/*=========================================================================*/

static void app_chinesechess(HWND hwnd)
{
	static int thread=0;
	static int app=0;

	if(thread==0)
	{  //创建一个独立线程来运行自已...
		_CreateThread((FN_ThreadEntry*)app_chinesechess,hwnd,8192);
		thread =1;
		return;
	}

	if(thread==1) //线程已创建了?
	{
		if(app==0)
		{
			app=1;
			ChineseChessMain();

			app=0;
			thread=0;
		}
		else
		{
			MSGBOX(hwnd,L"程序已在运行中...",L"消息提示!");
		}
	}
}

/*=========================================================================*/

static void app_audio_player(HWND hwnd)
{
	static int thread=0;
	static int app=0;

	if(thread==0)
	{  //创建一个独立线程来运行自已...
		_CreateThread((FN_ThreadEntry*)app_audio_player,hwnd,8192);
		thread =1;
		return;
	}

	if(thread==1) //线程已创建了?
	{
		if(app==0)
		{
			app=1;
			AudioPlayer(L"音乐播放器",0,0,GUI_XSIZE,GUI_YSIZE);

			app=0;
			thread=0;
		}
		else
		{
			MSGBOX(hwnd,L"程序已在运行中...",L"消息提示!");
		}
	}
}

/*=========================================================================*/

static void app_palette(HWND hwnd)
{
	static int thread=0;
	static int app=0;

	if(thread==0)
	{  //创建一个独立线程来运行自已...
		_CreateThread((FN_ThreadEntry*)app_palette,hwnd,8192);
		thread =1;
		return;
	}

	if(thread==1) //线程已创建了?
	{
		if(app==0)
		{
			app=1;
			palette_win();

			app=0;
			thread=0;
		}
		else
		{
			MSGBOX(hwnd,L"程序已在运行中...",L"消息提示!");
		}
	}
}

/*=========================================================================*/

static void app_infones(HWND hwnd)
{
	static int thread=0;
	static int app=0;

	if(thread==0)
	{  //创建一个独立线程来运行自已...
		_CreateThread((FN_ThreadEntry*)app_infones,hwnd,8192);
		thread =1;
		return;
	}

	if(thread==1) //线程已创建了?
	{
		if(app==0)
		{
			app=1;
			InfoNES_WinMain(NULL,(void*)"B:");

			app=0;
			thread=0;
		}
		else
		{
			MSGBOX(hwnd,L"程序已在运行中...",L"消息提示!");
		}
	}
}

/*=========================================================================*/

static void app_image_play(HWND hwnd)
{
	static int thread=0;
	static int app=0;

	if(thread==0)
	{  //创建一个独立线程来运行自已...
		_CreateThread((FN_ThreadEntry*)app_image_play,hwnd,8192);
		thread =1;
		return;
	}

	if(thread==1) //线程已创建了?
	{
		if(app==0)
		{
			app=1;
			ImageView(10,20,480,400,L"图片播放","B:Image");

			app=0;
			thread=0;
		}
		else
		{
			MSGBOX(hwnd,L"程序已在运行中...",L"消息提示!");
		}
	}
}

/*=========================================================================*/

static void app_cam_test(HWND hwnd)
{
	static int thread=0;
	static int app=0;

	if(thread==0)
	{  //创建一个独立线程来运行自已...
		_CreateThread((FN_ThreadEntry*)app_cam_test,hwnd,8192);
		thread =1;
		return;
	}

	if(thread==1) //线程已创建了?
	{
		if(app==0)
		{
			app=1;
			CameraTest();

			app=0;
			thread=0;
		}
		else
		{
			MSGBOX(hwnd,L"程序已在运行中...",L"消息提示!");
		}
	}
}

/*=========================================================================*/

static void app_cam_qrdecoder(HWND hwnd)
{
	static int thread=0;
	static int app=0;

	if(thread==0)
	{  //创建一个独立线程来运行自已...
		_CreateThread((FN_ThreadEntry*)app_cam_qrdecoder,hwnd,8192);
		thread =1;
		return;
	}

	if(thread==1) //线程已创建了?
	{
		if(app==0)
		{
			app=1;
			Camera_QRDecoder();

			app=0;
			thread=0;
		}
		else
		{
			MSGBOX(hwnd,L"程序已在运行中...",L"消息提示!");
		}
	}
}

/*=========================================================================*/

/*=========================================================================*/

static void dummy(HWND hwnd)
{

}

/*=========================================================================*/

static const void *pDefIcon   =app_1;
static const void *pIcon_app1 =app_1;
static const void *pIcon_app2 =app_2;

static struct __obj_list menu_list_1[]={


#if 1
		L"音频播放器",		&pDefIcon, 		app_audio_player,
//		L"纸牌",			icon_card, 			app_gamepager,
//		L"图片播放",		&pIcon_app1, 	app_image_play,

//		L"中国象棋",		icon_chess, 		app_chinesechess,
//		L"摄像头测试",		icon_camera, 		app_cam_test,
//		L"摄像头识别二维码",	icon_qrdecoder, 	app_cam_qrdecoder,

		L"NES模拟器",		&pDefIcon, 			app_infones,
		L"画图板",			&pIcon_app2, 			app_palette,
//		L"矢量字体测试",	&pDefIcon, 		app_ttf_test,

		L"模拟时钟",		&pDefIcon, 		app_watchtime_test,
		L"GUI基本测试",	&pDefIcon, 		app_gui_test,
//		L"计算器",		icon_calc, 		dummy,

//		L"二维码生成",	icon_qrcode, 	dummy,
		L"Test1-1",		&pDefIcon, 				dummy,
		L"Test1-2",		&pDefIcon, 				dummy,
		L"Test1-3",		&pDefIcon, 				dummy,
		L"Test1-4",		&pDefIcon, 				dummy,
		L"Test1-5",		&pDefIcon, 				dummy,
		L"Test1-6",		&pDefIcon, 				dummy,



#endif

		////

		NULL,	NULL,	NULL, //结束标志!


};

static struct __obj_list menu_list_2[]={


#if 1
		L"音频播放器",	&pDefIcon, 		app_audio_player,
		L"GUI基本测试",	&pDefIcon, 		app_gui_test,
		L"Test2-1",		&pDefIcon, 		dummy,
		L"Test2-2",		&pDefIcon, 		dummy,
		L"Test2-3",		&pDefIcon, 		dummy,
		L"Test2-4",		&pDefIcon, 		dummy,


#endif

		////

		NULL,	NULL,	NULL, //结束标志!


};
/*=========================================================================*/
//
static int capture_screen(void *argv)
{
	const char szCurDir[]="B:";

	SYS_thread_set_priority(NULL,-4); //不需要太高的线程优先级,比默认优先级降低4级...

	while(1)
	{
		static int key_down=0;

		while(GetKeyState(VK_LBUTTON))
		{
			SYS_msleep(50);
		}

		while(1)
		{
			if(GetKeyState(VK_LBUTTON)) //在屏幕左上角0,0,32,32矩形范围内点击开始截屏
			{
				key_down++;
				if(key_down == 2)
				{
					RECT rc;
					POINT pt;
					local_time_t tm;
					char buf[128];

					rc.x =0;
					rc.y =0;
					rc.w =32;
					rc.h =32;

					GetCursorPos(&pt);
					if(PtInRect(&rc,&pt))
					{
						SYS_get_local_time(&tm);
						x_sprintf(buf,"%s\\%04d%02d%02d_%02d%02d%02d.bmp",szCurDir,tm.year,tm.mon,tm.day,tm.hour,tm.min,tm.sec);
						//buz_output(100,100,1);
						CaptureScreenToBMPFile((const char*)buf);

					}
					break;
				}

			}
			else
			{
				key_down=0;
			}
			SYS_msleep(50);
		}

	}
}

/*=========================================================================*/


static int win_pos=0;
static u8 hour,min,sec;
static const WCHAR *pOEMText=NULL;
static u8 List1_Col,List1_Row;
static u8 List2_Col,List2_Row;

#define	ID_LIST_1 0x1000
#define ID_LIST_2 0x1001
#define	ID_PREV		0x1002
#define	ID_NEXT		0x1003

static	LRESULT	WinProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	switch(msg)
	{
		case WM_CREATE:
		{
			list_menu_cfg_t cfg;
			RECT rc,rc0;

			win_pos =0;
			GetTime(&hour,&min,&sec);

			GetClientRect(hwnd,&rc0);

			rc.x =0;
			rc.y =32;
			rc.w =rc0.w;
			rc.h =rc0.h-rc.y-130;

			InflateRectEx(&rc,-32,0,-32,0);

			//ListMenu控件，需要在创建时传入一个 list_menu_cfg_t 的结构体参数.
			cfg.list_objs =menu_list_1; //指定list列表.
			cfg.x_num =List1_Col; //水平项数.
			cfg.y_num =List1_Row; //垂直项数.
			CreateWindow(&wcex_ListMenu,L"ListMenu1",WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,ID_LIST_1,NULL,&cfg);

			OffsetRect(&rc,0,rc.h);
			rc.h =rc0.h-rc.y;

			cfg.list_objs =menu_list_2;
			cfg.x_num =List2_Col;
			cfg.y_num =List2_Row;
			CreateWindow(&wcex_ListMenu,L"ListMenu2",WS_VISIBLE|WS_BORDER,rc.x,rc.y,rc.w,rc.h,hwnd,ID_LIST_2,NULL,&cfg);

			rc.w=32;
			rc.h=64;
			rc.x=0;
			rc.y=100;
			CreateWindow(BUTTON,L"<",BS_FLAT|BS_NOTIFY|WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,ID_PREV,NULL,&cfg);

			rc.w=32;
			rc.h=64;
			rc.x=(rc0.w-rc.w);
			rc.y=100;
			CreateWindow(BUTTON,L">",BS_FLAT|BS_NOTIFY|WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,ID_NEXT,NULL,&cfg);

			SetTimer(hwnd,1,50,TMR_START,NULL);
		}
		break;
		////

		case WM_NOTIFY:
		{
			u16 code,id;
			LM_NMHDR *nm;

			code =HIWORD(wParam);
			id   =LOWORD(wParam);

			nm   =(LM_NMHDR*)lParam;

			if(code==LMN_CLICKED)
			{
				switch(id)
				{
				case ID_LIST_1:
					menu_list_1[nm->idx].cbStartup(hwnd);
					break;
					////
				case ID_LIST_2:
					menu_list_2[nm->idx].cbStartup(hwnd);
					break;
					////


				}

			}

			////
			if(code == BN_PUSHED && id==ID_PREV)
			{
				SendMessage(GetDlgItem(hwnd,ID_LIST_1),MSG_MOVE_PREV,TRUE,0);
			}
			////
			if(code == BN_PUSHED && id==ID_NEXT)
			{
				SendMessage(GetDlgItem(hwnd,ID_LIST_1),MSG_MOVE_NEXT,TRUE,0);
			}

		}
		break;
		////

		case WM_ERASEBKGND:
		{
			HDC hdc;
			HDC hdc_mem;
			RECT rc;
			WCHAR wbuf[128];
#if 0
			hdc =GetDCEx(hwnd,NULL,0);

			hdc_mem =CreateMemoryDC(SURF_ARGB4444,200,100);

			ClrDisplay(hdc_mem,NULL,ARGB4444(8,15,15,15));

			rc.x =0;
			rc.y =0;
			rc.w =100;
			rc.h =50;
			SetPenColor(hdc_mem,ARGB4444(15,10,10,10));
			DrawRect(hdc_mem,&rc);
			SetTextColor(hdc_mem,ARGB4444(15,15,0,0));
			x_wsprintf(wbuf,L"%d:%02d:%02d",hour,min,sec);
			TextOut(hdc_mem,10,20,wbuf,-1);
			BitBlt(hdc,20,50,200,100,hdc_mem,0,0,SRCCOPY);

			ReleaseDC(hwnd,hdc);
			DeleteDC(hdc_mem);
#endif
			GetClientRect(hwnd,&rc);
			hdc =(HDC)wParam;
			SetBrushColor(hdc,MapRGB(hdc,10,30,50));
			FillRect(hdc,&rc);

			return TRUE;
		}

		case WM_PAINT:
		{
			HDC hdc;
			PAINTSTRUCT ps;
			RECT rc;
			WCHAR wbuf[128];




			hdc =BeginPaint(hwnd,&ps);

			GetClientRect(hwnd,&rc);
			rc.h =32;

			SetBrushColor(hdc,MapRGB(hdc,0,0,0));
			FillRect(hdc,&rc);

			SetTextColor(hdc,MapRGB(hdc,255,255,255));
			DrawText(hdc,pOEMText,-1,&rc,DT_LEFT|DT_VCENTER);

			x_wsprintf(wbuf,L"%d:%02d:%02d",hour,min,sec);
			DrawText(hdc,wbuf,-1,&rc,DT_CENTER|DT_VCENTER);

			x_wsprintf(wbuf,L"CPU:%d%% ",GetCPUUsage());
			DrawText(hdc,wbuf,-1,&rc,DT_RIGHT|DT_VCENTER);




			EndPaint(hwnd,&ps);
			////


		}
		break;
		////

		case WM_LBUTTONDOWN:
		{

			POINT pt;
			RECT rc;

			pt.x =GET_LPARAM_X(lParam);
			pt.y =GET_LPARAM_Y(lParam);

			GetClientRect(hwnd,&rc);
			rc.h =32;

			if(PtInRect(&rc,&pt))
			{
				if(win_pos==0)
				{
					win_pos =1;
				}
				else
				{
					win_pos =0;
				}
			}
		}
		break;
		////

		case WM_TIMER:
		{
			RECT rc;
			GetWindowRect(hwnd,&rc);
			if(win_pos==0)
			{
				if(rc.y > 0)
				{
					OffsetRect(&rc,0,-(rc.h>>3));
					rc.y =MAX(rc.y,0);
					MoveWindow(hwnd,rc.x,rc.y,rc.w,rc.h,TRUE);
				}
			}
			else
			{
				if(rc.y < (GUI_YSIZE-32))
				{
					OffsetRect(&rc,0,(rc.h>>3));
					rc.y =MIN(rc.y,(GUI_YSIZE-32));
					MoveWindow(hwnd,rc.x,rc.y,rc.w,rc.h,TRUE);
				}
			}

			////
			static int sec0=-1;
			GetTime(&hour,&min,&sec);
			if(sec0!=sec)
			{
				sec0 =sec;
				InvalidateRect(hwnd,NULL,FALSE);
			}
		}
		break;
		////

		default:
			return DefWindowProc(hwnd,msg,wParam,lParam);


	}
	return WM_NULL;

}

void YH_EVK_Main(int x,int y,int w,int h,const WCHAR *pText,u8 col1,u8 row1,u8 col2,u8 row2)
{

	pOEMText =pText;
	List1_Col =col1;
	List1_Row =row1;
	List2_Col =col2;
	List2_Row =row2;

	//创建一个独立线程来运行“截屏"程序.
	//SYS_thread_create(capture_screen,NULL,4*1024,NULL,0);

	HWND hwnd;
	MSG	msg;
	WNDCLASS wcex;

	wcex.Tag	 		= WNDCLASS_TAG;

	wcex.Style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)WinProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= NULL;
	wcex.hIcon			= NULL;
	wcex.hCursor		= NULL;

	hwnd	=CreateWindowEx(	WS_EX_LOCKPOS|WS_EX_LOCKZORDER,
								&wcex,
								pText,WS_OVERLAPPED|WS_CLIPCHILDREN,
								x,y,w,h,
								NULL,0,NULL,NULL);


	ShowWindow(hwnd,SW_SHOW);

	while(GetMessage(&msg,hwnd))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}


}


/*=========================================================================*/
