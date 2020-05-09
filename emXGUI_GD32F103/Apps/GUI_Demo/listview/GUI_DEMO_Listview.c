
#include <emXGUI.h>

#include	"listview.h"

/*============================================================================*/
/*============================================================================*/

static HANDLE	hInst=NULL;


#define	ID_LV1	0x1000

#define	ID_BTN1	0x2000
#define	ID_BTN2	0x2001
#define	ID_BTN3	0x2002
#define	ID_BTN4	0x2003

/*============================================================================*/
/*============================================================================*/
/*============================================================================*/

static	LRESULT	WinProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	RECT rc0,rc;

	switch(msg)
	{
		case	WM_CREATE:
				{
					HWND hwndLV;
					LVCOLUM col;
					LVROW row;
					LVCELL cell;

					hwndLV=CreateWindow(&wcexLISTVIEW,L"Listview",WS_BORDER|WS_VISIBLE|LVS_NOTIFY,8,4,400,200,hwnd,ID_LV1,hInst,NULL);

					////
					col.Width =88;
					col.pszText =L"电压(V)";
					SendMessage(hwndLV,LVM_INSERTCOLUMN,0,(LPARAM)&col);

					col.Width =88;
					col.pszText =L"电流(mA)";
					SendMessage(hwndLV,LVM_INSERTCOLUMN,1,(LPARAM)&col);

					col.Width =88;
					col.pszText =L"温度(℃)";
					SendMessage(hwndLV,LVM_INSERTCOLUMN,2,(LPARAM)&col);

					col.Width =100;
					col.pszText =L"备注";
					SendMessage(hwndLV,LVM_INSERTCOLUMN,3,(LPARAM)&col);

					////
					row.Height =24;
					SendMessage(hwndLV,LVM_INSERTROW,0,(LPARAM)&row);

					row.Height =20;
					SendMessage(hwndLV,LVM_INSERTROW,1,(LPARAM)&row);

					row.Height =30;
					SendMessage(hwndLV,LVM_INSERTROW,2,(LPARAM)&row);

					row.Height =30;
					SendMessage(hwndLV,LVM_INSERTROW,3,(LPARAM)&row);

					row.Height =44;
					SendMessage(hwndLV,LVM_INSERTROW,4,(LPARAM)&row);

					row.Height =24;
					SendMessage(hwndLV,LVM_INSERTROW,5,(LPARAM)&row);

					////
					cell.Mask =LVCF_TEXT|LVCF_TEXTCOLOR;

					cell.TextColor =XRGB8888(0,0,0);
					cell.pszText =L"A0";
					SendMessage(hwndLV,LVM_SETCELL,MAKE_LVCELL_IDX(0,0),(LPARAM)&cell);
					cell.pszText =L"A1";
					SendMessage(hwndLV,LVM_SETCELL,MAKE_LVCELL_IDX(0,1),(LPARAM)&cell);
					cell.pszText =L"A2";
					SendMessage(hwndLV,LVM_SETCELL,MAKE_LVCELL_IDX(0,2),(LPARAM)&cell);
					cell.pszText =L"A3";
					SendMessage(hwndLV,LVM_SETCELL,MAKE_LVCELL_IDX(0,3),(LPARAM)&cell);

					cell.TextColor =XRGB8888(0,0,200);
					cell.pszText =L"B0";
					SendMessage(hwndLV,LVM_SETCELL,MAKE_LVCELL_IDX(1,0),(LPARAM)&cell);
					cell.pszText =L"B1";
					SendMessage(hwndLV,LVM_SETCELL,MAKE_LVCELL_IDX(1,1),(LPARAM)&cell);
					cell.pszText =L"B2";
					SendMessage(hwndLV,LVM_SETCELL,MAKE_LVCELL_IDX(1,2),(LPARAM)&cell);
					cell.pszText =L"B3";
					SendMessage(hwndLV,LVM_SETCELL,MAKE_LVCELL_IDX(1,3),(LPARAM)&cell);

					cell.TextColor =XRGB8888(255,0,0);
					cell.pszText =L"D0";
					SendMessage(hwndLV,LVM_SETCELL,MAKE_LVCELL_IDX(3,0),(LPARAM)&cell);
					cell.pszText =L"D1";
					SendMessage(hwndLV,LVM_SETCELL,MAKE_LVCELL_IDX(3,1),(LPARAM)&cell);
					cell.pszText =L"D2";
					SendMessage(hwndLV,LVM_SETCELL,MAKE_LVCELL_IDX(3,2),(LPARAM)&cell);
					cell.pszText =L"D3";
					SendMessage(hwndLV,LVM_SETCELL,MAKE_LVCELL_IDX(3,3),(LPARAM)&cell);


					cell.Mask = LVCF_FORMAT|LVCF_TEXT|LVCF_TEXTCOLOR|LVCF_BACKCOLOR;
					cell.Format =LVF_TEXT|LVF_TOP|LVF_LEFT;
					cell.TextColor =XRGB8888(255,255,255);
					cell.BackColor =XRGB8888(0,200,50);
					cell.pszText =L"E1左上";
					SendMessage(hwndLV,LVM_SETCELL,MAKE_LVCELL_IDX(4,1),(LPARAM)&cell);

					cell.Mask = LVCF_FORMAT|LVCF_TEXT|LVCF_TEXTCOLOR|LVCF_BACKCOLOR;
					cell.Format =LVF_TEXT|LVF_BOTTOM|LVF_RIGHT;
					cell.TextColor =XRGB8888(255,255,0);
					cell.BackColor =XRGB8888(0,100,150);
					cell.pszText =L"E2右下";
					SendMessage(hwndLV,LVM_SETCELL,MAKE_LVCELL_IDX(4,2),(LPARAM)&cell);

					cell.Mask = LVCF_FORMAT|LVCF_TEXT|LVCF_TEXTCOLOR|LVCF_BACKCOLOR;
					cell.Format =LVF_TEXT|LVF_VCENTER|LVF_CENTER|LVF_MULTILINE;
					cell.TextColor =XRGB8888(0,0,0);
					cell.BackColor =XRGB8888(240,100,50);
					cell.pszText =L"E3\r\n多行文字";
					SendMessage(hwndLV,LVM_SETCELL,MAKE_LVCELL_IDX(4,3),(LPARAM)&cell);

					//SendMessage(hwndLV,LVM_DELETEROW,2,TRUE);
					//SendMessage(hwndLV,LVM_SETFIRSTCOLUMN,1,TRUE);
					//SendMessage(hwndLV,LVM_SETFIRSTROW,2,TRUE);

					GetClientRect(hwnd,&rc0);
					rc.x =8;
					rc.y =rc0.h-40;
					rc.w =80;
					rc.h =32;
					CreateWindow(BUTTON,L"<-",WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,ID_BTN1,hInst,NULL);

					OffsetRect(&rc,rc.w+2,0);
					CreateWindow(BUTTON,L"->",WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,ID_BTN2,hInst,NULL);

					OffsetRect(&rc,rc.w+2,0);
					CreateWindow(BUTTON,L"UP",WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,ID_BTN3,hInst,NULL);

					OffsetRect(&rc,rc.w+2,0);
					CreateWindow(BUTTON,L"DOWN",WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,ID_BTN4,hInst,NULL);


				}
				break;
				////

		case	WM_NOTIFY:
				{
					U16 code,id;
					static int col=0;
					static int row=0;

					code =HIWORD(wParam);
					id =LOWORD(wParam);

					if(id==ID_BTN1 && code==BN_CLICKED)
					{

						HWND wnd;

						if(col>0)
						{
						col--;
						}
						wnd =GetDlgItem(hwnd,ID_LV1);

						SendMessage(wnd,LVM_SETFIRSTCOLUMN,col,TRUE);
						InvalidateRect(wnd,NULL,TRUE);

					}

					if(id==ID_BTN2 && code==BN_CLICKED)
					{

						HWND wnd;

						col++;

						wnd =GetDlgItem(hwnd,ID_LV1);
						SendMessage(wnd,LVM_SETFIRSTCOLUMN,col,TRUE);
						InvalidateRect(wnd,NULL,TRUE);

					}

					if(id==ID_BTN3 && code==BN_CLICKED)
					{

						HWND wnd;

						if(row>0)
						{
							row--;
						}
						wnd =GetDlgItem(hwnd,ID_LV1);

						SendMessage(wnd,LVM_SETFIRSTROW,row,TRUE);
						InvalidateRect(wnd,NULL,TRUE);

					}

					if(id==ID_BTN4 && code==BN_CLICKED)
					{

						HWND wnd;

						row++;

						wnd =GetDlgItem(hwnd,ID_LV1);
						SendMessage(wnd,LVM_SETFIRSTROW,row,TRUE);
						InvalidateRect(wnd,NULL,TRUE);

					}


					if(id==ID_LV1)
					{
						NMLISTVIEW *pNM =(NMLISTVIEW*)lParam;
						GUI_Printf("ListView Notify: %d,%d\r\n",pNM->RowIdx,pNM->ColumnIdx);
						if(pNM->RowIdx==2 && pNM->hdr.code ==LVN_CLICKED)
						{
							WCHAR wbuf[20];

							if(number_input_box(10,20,200,240,L"输入数字",wbuf,16,hwnd))
							{
								LVCELL cell;

								cell.Mask = LVCF_FORMAT|LVCF_TEXT|LVCF_TEXTCOLOR;
								cell.Format =LVF_TEXT|LVF_VCENTER|LVF_CENTER;
								cell.TextColor =XRGB8888(0,64,128);
								//cell.BackColor =XRGB8888(0,50,200);
								cell.pszText =wbuf;
								SendMessage(pNM->hdr.hwndFrom,LVM_SETCELL,MAKE_LVCELL_IDX(pNM->RowIdx,pNM->ColumnIdx),(LPARAM)&cell);

							}
						}

						//SendMessage(pNM->hdr.hwndFrom,LVM_DELETEROW,pNM->RowIdx,TRUE);
					}
				}
				break;
				////

		case	WM_PAINT:
				{
					PAINTSTRUCT ps;
					HDC hdc;
					RECT rc;

					hdc =BeginPaint(hwnd,&ps);



					EndPaint(hwnd,&ps);
				}
				break;
				////

		default:
		return	DefWindowProc(hwnd,msg,wParam,lParam);
	}

	return	WM_NULL;
}


/*============================================================================*/


void	GUI_DEMO_Listview(void)
{
	HWND hwnd;
	MSG	msg;
	WNDCLASS wcex;

	////

	wcex.Tag	 		= WNDCLASS_TAG;

	wcex.Style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)WinProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= NULL; //hInst
	wcex.hIcon			= NULL;
	wcex.hCursor		= NULL;


	hwnd	=CreateWindow(	&wcex,L"Listview Test",WS_OVERLAPPEDWINDOW,
							10,20,460,288,
							NULL,NULL,hInst,NULL);


	ShowWindow(hwnd,SW_SHOW);
	UpdateWindow(hwnd);

	while(GetMessage(&msg,hwnd))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

}

/*============================================================================*/
