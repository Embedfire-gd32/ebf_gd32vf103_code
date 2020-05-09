
#include <emXGUI.h>

#ifndef	_T
#define	_T(x) L##x
#endif

/*===================================================================================*/
#define	ID_EXIT		0x1000
#define	ID_LISTBOX1	0x1101
#define	ID_LISTBOX2	0x1102
#define	ID_LISTBOX3	0x1103

/*===================================================================================*/


static int old_sel=0;

static	LRESULT	win_proc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	HDC hdc;
	PAINTSTRUCT ps;
	RECT rc;
	WCHAR wbuf[128];
	int i,x,y;
	HWND wnd;
	
	switch(msg)
	{
		case	WM_CREATE:
				

				GetClientRect(hwnd,&rc);
				
				//CreateWindow(BUTTON,_T("Exit"),WS_BORDER|WS_VISIBLE|WS_CHILDWINDOW,8,8,80,30,hwnd,ID_EXIT,NULL,NULL);
				//wnd=CreateWindow(LISTBOX,_T("Listbox1"),WS_BORDER|WS_VISIBLE|WS_CHILDWINDOW,8,8,160,80,hwnd,ID_LISTBOX1,NULL,NULL);
				wnd=CreateWindow(LISTBOX,_T("Listbox"),LBS_LINE|LBS_NOTIFY|WS_BORDER|WS_VISIBLE,8,8,160,100,hwnd,ID_LISTBOX1,NULL,NULL);
				SendMessage(wnd,LB_ADDSTRING,-1,(LPARAM)L"Item-1-0");
				SendMessage(wnd,LB_ADDSTRING,-1,(LPARAM)L"Item-1-1");
				SendMessage(wnd,LB_ADDSTRING,-1,(LPARAM)L"Item-1-2");
				SendMessage(wnd,LB_ADDSTRING,-1,(LPARAM)L"Item-1-3");
				SendMessage(wnd,LB_ADDSTRING,-1,(LPARAM)L"Item-1-4");
				SendMessage(wnd,LB_ADDSTRING,-1,(LPARAM)L"Item-1-5");
				SendMessage(wnd,LB_ADDSTRING,-1,(LPARAM)L"Item-1-6");
				SendMessage(wnd,LB_ADDSTRING,-1,(LPARAM)L"Item-1-7");
				SendMessage(wnd,LB_ADDSTRING,-1,(LPARAM)L"Item-1-8");
				SendMessage(wnd,LB_ADDSTRING,-1,(LPARAM)L"Item-1-9");
				SendMessage(wnd,LB_ADDSTRING,-1,(LPARAM)L"Item-1-10");

				SendMessage(wnd,LB_SETITEMHEIGHT,1,15);
				SendMessage(wnd,LB_SETITEMHEIGHT,3,50);

				////
				wnd=CreateWindow(LISTBOX,_T("Listbox2"),WS_VISIBLE,200,8,150,160,hwnd,ID_LISTBOX2,NULL,NULL);
				SendMessage(wnd,LB_ADDSTRING,-1,(LPARAM)L"Item-2-0");
				SendMessage(wnd,LB_ADDSTRING,-1,(LPARAM)L"Item-2-1");
				SendMessage(wnd,LB_ADDSTRING,-1,(LPARAM)L"Item-2-2");
				SendMessage(wnd,LB_ADDSTRING,-1,(LPARAM)L"Item-2-3");
				SendMessage(wnd,LB_ADDSTRING,-1,(LPARAM)L"Item-2-4");
				SendMessage(wnd,LB_ADDSTRING,-1,(LPARAM)L"Item-2-5");
				SendMessage(wnd,LB_ADDSTRING,-1,(LPARAM)L"Item-2-6");
				SendMessage(wnd,LB_ADDSTRING,-1,(LPARAM)L"Item-2-7");
				SendMessage(wnd,LB_ADDSTRING,-1,(LPARAM)L"Item-2-8");
				SendMessage(wnd,LB_ADDSTRING,-1,(LPARAM)L"Item-2-9");
				SendMessage(wnd,LB_ADDSTRING,-1,(LPARAM)L"Item-2-10");

				wnd=CreateWindow(LISTBOX,_T("Listbox3"),LBS_LINE|WS_BORDER|WS_VISIBLE,8,130,160,100,hwnd,ID_LISTBOX3,NULL,NULL);
				SendMessage(wnd,LB_ADDSTRING,0,(LPARAM)L"Item-3-0");
				SendMessage(wnd,LB_ADDSTRING,1,(LPARAM)L"Item-3-1");
				SendMessage(wnd,LB_ADDSTRING,2,(LPARAM)L"Item-3-2");
				SendMessage(wnd,LB_ADDSTRING,3,(LPARAM)L"Item-3-3");
				SendMessage(wnd,LB_ADDSTRING,4,(LPARAM)L"Item-3-4");
				SendMessage(wnd,LB_ADDSTRING,5,(LPARAM)L"Item-3-5");
				SendMessage(wnd,LB_ADDSTRING,6,(LPARAM)L"Item-3-6");
				SendMessage(wnd,LB_ADDSTRING,7,(LPARAM)L"Item-3-7");
				SendMessage(wnd,LB_ADDSTRING,8,(LPARAM)L"Item-3-8");
				SendMessage(wnd,LB_ADDSTRING,9,(LPARAM)L"Item-3-9");
				SendMessage(wnd,LB_ADDSTRING,10,(LPARAM)L"Item-3-10");


				old_sel =0;
				SetTimer(hwnd,1,1000,TMR_START,NULL);
				return TRUE;
				////////
				


				
		case	WM_TIMER:
				
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

					SendMessage(nr->hwndFrom,LB_SETITEMHEIGHT,old_sel,15);

					SendMessage(nr->hwndFrom,LB_SETITEMHEIGHT,i,30);

					old_sel =i;

					GUI_Printf("Listbox SEL_CHANGE:%d.\r\n",i);
				}

				if(nr->code==LBN_CLICKED)
				{
					i =SendMessage(nr->hwndFrom,LB_GETCURSEL,0,0);
					i =SendMessage(nr->hwndFrom,LB_GETITEMOFFSET,i,0);

					//SendMessage(nr->hwndFrom,LB_SETPOS,0,i);
				}

				if(nr->code == LBN_POSCHANGE)
				{
					GUI_Printf("Listbox POS_CHANGE\r\n");
				}
			}

		}
		break;
		////
		case	WM_CTLCOLOR:
		{
			u16 id;

			id =LOWORD(wParam);

			if(id== ID_LISTBOX3)
			{
				CTLCOLOR *cr;
				cr =(CTLCOLOR*)lParam;

					cr->TextColor =RGB888(255,255,255);
					cr->BackColor =RGB888(0,130,0);
					cr->BorderColor =RGB888(50,150,50);
					cr->ForeColor =RGB888(0,50,0);

				return TRUE;
			}
			else
			{
				return FALSE;
			}

		}
		break;
		////

		case	WM_PAINT:
				hdc	=BeginPaint(hwnd,&ps);
				
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
					PostQuitMessage(hwnd);
				}
				return	TRUE;
				////
				
		default:
				return	DefWindowProc(hwnd,msg,wParam,lParam);
	}
	
	return	WM_NULL;
}

void	GUI_DEMO_Listbox(void)
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
		
		
		hwnd	=CreateWindowEx(	NULL,
									&wcex,
									_T("GUI Demo - Listbox"),
									WS_OVERLAPPEDWINDOW,
									20,30,400,300,
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
