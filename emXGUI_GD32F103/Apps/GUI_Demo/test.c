
#include	"emXGUI.h"

#include	"x_libc.h"

#ifndef	_T
#define	_T(x) L##x
#endif

/*===================================================================================*/

#define	BK_COLOR	RGB888(250,250,250)

/*===================================================================================*/
#if 0
static void button_owner_draw(DRAWITEM_HDR *ds) //����һ����ť���
{
	HWND hwnd;
	HDC hdc;
	RECT rc;
	WCHAR wbuf[128];

	hwnd = ds->hwnd; //button�Ĵ��ھ��.
	hdc = ds->hDC;   //button�Ļ�ͼ�����ľ��.
	rc = ds->rc;     //button�Ļ��ƾ�����.
	HDC hdc_mem;
	hdc_mem = CreateMemoryDC(SURF_ARGB4444, rc.w, rc.h);

	SetBrushColor(hdc_mem, MapRGB(hdc_mem, 0,0, 0, 0));
	FillRect(hdc_mem, &rc);
	SetBrushColor(hdc_mem, MapRGB(hdc_mem,  255, 0,0,0));
	FillCircle(hdc_mem, rc.x + rc.w, rc.y, rc.w);

	//��ť����״̬
	if (ds->State & BST_PUSHED)
	{
		SetTextColor(hdc_mem, MapRGB(hdc_mem, 255,105, 105, 105));      //��������ɫ
	}
	else//��ť����״̬
	{
		SetTextColor(hdc_mem, MapRGB(hdc_mem, 255,255, 255, 255));
	}
	/* ʹ�ÿ���ͼ������ */
	//SetFont(hdc, controlFont_64);
	GetWindowText(ds->hwnd, wbuf, 128); //��ð�ť�ؼ�������

	DrawText(hdc_mem, wbuf, -1, &rc, NULL);//��������(���ж��뷽ʽ)

	BitBlt(hdc, rc.x, rc.y, rc.w, rc.h, hdc_mem, 0, 0, SRCCOPY);
	DeleteDC(hdc_mem);
  /* �ָ�Ĭ������ */
	SetFont(hdc, defaultFont);

}
#endif
static void button_owner_draw(DRAWITEM_HDR *ds) //����һ����ť���
{
	HWND hwnd;
	HDC hdc;
	RECT rc;
	WCHAR wbuf[128];

	hwnd = ds->hwnd; //button�Ĵ��ھ��.
	hdc = ds->hDC;   //button�Ļ�ͼ�����ľ��.
	rc = ds->rc;     //button�Ļ��ƾ�����.
	//SetFont(hdc, hFont24);


	if (ds->State & BST_PUSHED)
	{ //��ť�ǰ���״̬
		SetTextColor(hdc, MapARGB(hdc, 255, 255, 0, 255));
		SetPenColor(hdc, MapARGB(hdc, 255, 200, 0, 200));
		SetBrushColor(hdc, MapARGB(hdc, 150, 100, 0, 100));
	}
	else
	{ //��ť�ǵ���״̬
		SetBrushColor(hdc, MapARGB(hdc, 80, 0, 150, 150));
		SetPenColor(hdc, MapARGB(hdc, 200, 0, 200, 250));
		SetTextColor(hdc, MapARGB(hdc, 250, 200, 255, 255));
	}

	EnableAlpha(hdc,TRUE);
	SetAlpha(hdc,100);
	FillRoundRect(hdc, &rc, 25); //�þ�����䱳��

	SetAlpha(hdc,255);
	DrawRoundRect(hdc, &rc, 25); //���������

	GetWindowText(hwnd, wbuf, 128); //��ð�ť�ؼ�������
	DrawText(hdc, wbuf, -1, &rc, DT_VCENTER | DT_CENTER);//��������(���ж��뷽ʽ)

}

static int cnt=0;

//static SURFACE *pSurfTop = NULL;
static	LRESULT	win_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	RECT rc,rc0;

	switch (msg)
	{
		case WM_CREATE:
		{
			int i;
			WCHAR wbuf[24];
			GetClientRect(hwnd, &rc0);
		//	pSurfTop = CreateSurface(SURF_ARGB4444, rc.w, rc.h, NULL, 0);
			rc.w =100;
			rc.h =80;
			rc.x =rc0.w-120;
			rc.y =10;

			for(i=0;i<6;i++)
			{
				x_wsprintf(wbuf,L"BTN%02d",i);
				CreateWindow(BUTTON, wbuf, WS_VISIBLE |WS_TRANSPARENT| WS_OWNERDRAW,rc.x,rc.y,rc.w,rc.h, hwnd, 0x1000+i, NULL, NULL);
				OffsetRect(&rc,0,rc.h+8);
			}

			SetTimer(hwnd,1,200,TMR_START,NULL);
			break;
		}

		case WM_TIMER:
		{
			cnt++;
			InvalidateRect(hwnd,NULL,FALSE);
		}
		break;

		case WM_DRAWITEM:
		{
			DRAWITEM_HDR *ds;
			ds = (DRAWITEM_HDR*)lParam;
			HDC hdc_widget;
			RECT rc = ds->rc;



		//	hdc_widget = CreateDC(pSurfTop, &rc);
			//ds->hDC = hdc_widget;
			button_owner_draw(ds);

		//	DeleteDC(hdc_widget);

		//	InvalidateRect(hwnd, &rc, FALSE);
			return TRUE;
		}
#if 1
		case WM_ERASEBKGND:
		{
			HDC hdc =(HDC)wParam;
			RECT rc =*(RECT*)lParam;

			SetBrushColor(hdc,MapXRGB8888(hdc,BK_COLOR));
			FillRect(hdc,&rc);
		}
		return TRUE;
#endif

		case WM_PAINT:
		{
			HDC hdc_bk;
			HDC hdc;
			RECT rc;
			PAINTSTRUCT ps;
			WCHAR wbuf[128];
			hdc = BeginPaint(hwnd, &ps);
#if 0
			GetClientRect(hwnd, &rc);
			hdc_bk = CreateDC(pSurfTop, &rc);
			SetBrushColor(hdc_bk, MapRGB888(hdc_bk, BK_COLOR));
			FillRect(hdc_bk, &rc);



			BitBlt(hdc, rc.x, rc.y, rc.w, rc.h, hdc_bk, rc.x, rc.y, SRCCOPY);
			DeleteDC(hdc_bk);
#endif

			GetClientRect(hwnd, &rc);
			SetBrushColor(hdc, MapRGB888(hdc, BK_COLOR));
			FillRect(hdc, &rc);

			SetTextColor(hdc,MapRGB(hdc,250,0,0));
			SetBrushColor(hdc,MapRGB(hdc,10,200,100));

			rc.x =680;
			rc.y =30;
			rc.w =100;
			rc.h =24;
			x_wsprintf(wbuf,L"Count:%04d",cnt);
			DrawText(hdc,wbuf,-1,&rc,DT_VCENTER|DT_CENTER|DT_BKGND);
			//TextOut(hdc,710,40,wbuf,-1);

			EndPaint(hwnd, &ps);
			break;
		}
		default:
			return	DefWindowProc(hwnd, msg, wParam, lParam);
	}

	return	WM_NULL;
}

void	xGUI_DEMO_Test(void)
{
	HWND	hwnd;
	WNDCLASS	wcex;
	MSG msg;
	//��1���֣�����wcex������
	wcex.Tag = WNDCLASS_TAG;
	wcex.Style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = win_proc; //������������Ϣ����Ļص�����.
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = NULL;//hInst;
	wcex.hIcon = NULL;//LoadIcon(hInstance, (LPCTSTR)IDI_WIN32_APP_TEST);
	wcex.hCursor = NULL;//LoadCursor(NULL, IDC_ARROW);
	//��2���֣�����������
	hwnd = CreateWindowEx(WS_EX_FRAMEBUFFER,
		&wcex,
		_T("GUI Demo - Button"),
		WS_CAPTION|WS_CLIPCHILDREN|WS_CLOSEBOX,
		0, 0, GUI_XSIZE, GUI_YSIZE,
		NULL, NULL, NULL, NULL);

	//��3���֣���ʾ������
	ShowWindow(hwnd, SW_SHOW);
	//��ʼ������Ϣѭ��(���ڹرղ�����ʱ,GetMessage������FALSE,�˳�����Ϣѭ��)��
	while (GetMessage(&msg, hwnd))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}
/*===================================================================================*/
/*===================================================================================*/
/*===================================================================================*/
/*===================================================================================*/
