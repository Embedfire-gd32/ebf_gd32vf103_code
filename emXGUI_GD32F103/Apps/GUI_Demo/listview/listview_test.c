
#include <emXGUI.h>
#include "listview.h"

/*============================================================================*/
/*============================================================================*/

static HANDLE	hInst=NULL;


#define	ID_LV1	0x2000

/*============================================================================*/
/*============================================================================*/
/*============================================================================*/

static	LRESULT	WinProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{

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
					col.Width =68;
					col.pszText =L"电压(V)";
					SendMessage(hwndLV,LVM_INSERTCOLUMN,0,(LPARAM)&col);

					col.Width =80;
					col.pszText =L"电流(mA)";
					SendMessage(hwndLV,LVM_INSERTCOLUMN,1,(LPARAM)&col);

					col.Width =80;
					col.pszText =L"温度(℃)";
					SendMessage(hwndLV,LVM_INSERTCOLUMN,2,(LPARAM)&col);

					col.Width =100;
					col.pszText =L"备注";
					SendMessage(hwndLV,LVM_INSERTCOLUMN,3,(LPARAM)&col);

					////
					row.Height =24;
					row.pszText =L"项目1";
					SendMessage(hwndLV,LVM_INSERTROW,0,(LPARAM)&row);

					row.Height =20;
					row.pszText =L"项目2";
					SendMessage(hwndLV,LVM_INSERTROW,1,(LPARAM)&row);

					row.Height =30;
					row.pszText =L"项目3";
					SendMessage(hwndLV,LVM_INSERTROW,2,(LPARAM)&row);

					row.Height =30;
					row.pszText =L"项目4";
					SendMessage(hwndLV,LVM_INSERTROW,3,(LPARAM)&row);

					row.Height =44;
					row.pszText =L"项目5";
					SendMessage(hwndLV,LVM_INSERTROW,4,(LPARAM)&row);

					row.Height =24;
					row.pszText =L"项目6";
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

				}
				break;
				////

		case	WM_NOTIFY:
				{
					U16 id;

					id =LOWORD(wParam);

					if(id==ID_LV1)
					{
						NMLISTVIEW *pNM =(NMLISTVIEW*)lParam;
						GUI_Printf("ListView Notify: %d,%d\r\n",pNM->RowIdx,pNM->ColumnIdx);
						if(pNM->RowIdx==3 && pNM->hdr.code ==LVN_CLICKED)
						{
							WCHAR wbuf[20];
							number_input_box(10,20,200,240,L"输入数字",wbuf,16,hwnd);
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

					rc.x =32;
					rc.y =208;
					rc.w =128;
					rc.h =40;

					DrawText(hdc,L"ABCD\r\nEFG",-1,&rc,DT_VCENTER|DT_CENTER|DT_BKGND|DT_BORDER);

					OffsetRect(&rc,140,0);
					DrawText(hdc,L"123456",-1,&rc,DT_VCENTER|DT_CENTER|DT_BKGND|DT_BORDER);

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



int	Listview_Test(void)
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
	return	TRUE;
}

/*============================================================================*/
