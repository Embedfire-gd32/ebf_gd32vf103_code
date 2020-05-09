
/*
 *GUI_DEMO_DrawText.c
 *2019��8��8������9:23:56
 *
 */

#include "emXGUI.h"
/*============================================================================*/
//����ؼ�ID
#define	ID_EXIT		0x1000

/*============================================================================*/

static LRESULT	WinProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	RECT rc;

	switch(msg)
	{
		case WM_CREATE: //���ڴ���ʱ,���Զ���������Ϣ,��������һЩ��ʼ���Ĳ����򴴽��Ӵ���.
		{
			GetClientRect(hwnd,&rc); //��ô��ڵĿͻ�������.

			CreateWindow(BUTTON,L"Exit",WS_VISIBLE,rc.w-80,8,68,32,hwnd,ID_EXIT,NULL,NULL); //����һ����ť(ʾ��).
		}
		return TRUE;
		////

		case WM_NOTIFY: //WM_NOTIFY��Ϣ:wParam��16λΪ���͸���Ϣ�Ŀؼ�ID,��16λΪ֪ͨ��;lParamָ����һ��NMHDR�ṹ��.
		{
			u16 code,id;

			code =HIWORD(wParam); //���֪ͨ������.
			id   =LOWORD(wParam); //��ò�������Ϣ�Ŀؼ�ID.

			if(id==ID_EXIT && code==BN_CLICKED) // ��ť����������.
			{
				PostCloseMessage(hwnd); //ʹ����WM_CLOSE��Ϣ�رմ���.
			}
		}
		break;
		////

		case WM_PAINT: //������Ҫ����ʱ�����Զ���������Ϣ.
		{
			PAINTSTRUCT ps;
			HDC hdc;

			hdc =BeginPaint(hwnd,&ps); //��ʼ��ͼ

			////�û��Ļ�������...

			SetTextColor(hdc,MapRGB(hdc,0,0,0));
			SetBrushColor(hdc,MapRGB(hdc,150,250,250));
			SetPenColor(hdc,MapRGB(hdc,10,10,10));

			rc.x =2;
			rc.y =2;
			rc.w =160;
			rc.h =40;

			DrawText(hdc,L"123456789",5,&rc,DT_SINGLELINE|DT_LEFT|DT_VCENTER|DT_BORDER|DT_BKGND);

			OffsetRect(&rc,0,rc.h+2);
			DrawText(hdc,L"123456789",-1,&rc,DT_SINGLELINE|DT_LEFT|DT_VCENTER|DT_BORDER|DT_BKGND);

			OffsetRect(&rc,0,rc.h+2);
			DrawText(hdc,L"Align-Center",-1,&rc,DT_SINGLELINE|DT_CENTER|DT_VCENTER|DT_BORDER|DT_BKGND);

			SetTextColor(hdc,MapRGB(hdc,255,0,0));
			SetBrushColor(hdc,MapRGB(hdc,0,0,0));
			SetPenColor(hdc,MapRGB(hdc,0,255,0));
			OffsetRect(&rc,0,rc.h+2);
			rc.h=100;
			DrawText(hdc,L"Multi Line\r\n"
					 	 L"1234\r\n"
						 L"5678\r\n"
						 L"ABCEFGHIJKLMNOPQRSTUVWXYZ\r\n"
						 ,-1,&rc,DT_LEFT|DT_TOP|DT_BORDER|DT_BKGND);

			SetTextColor(hdc,MapRGB(hdc,255,255,255));
			SetBrushColor(hdc,MapRGB(hdc,100,0,200));
			SetPenColor(hdc,MapRGB(hdc,255,0,0));
			OffsetRect(&rc,0,rc.h+2);
			rc.h=120;
			DrawText(hdc,L"Multi Line\r\n"
						 L"Word Break\r\n"
						 L"ABCEFGHIJKLMNOPQRSTUVWXYZ"
						 ,-1,&rc,DT_LEFT|DT_TOP|DT_BORDER|DT_BKGND|DT_WORDBREAK);



			EndPaint(hwnd,&ps); //������ͼ
		}
		break;
		////

		case WM_CLOSE: //���ڹر�ʱ�����Զ���������Ϣ.
		{

			return DestroyWindow(hwnd); //����DestroyWindow�������ٴ��ڣ��ú�����ʹ�����ڽ������˳���Ϣѭ��;���򴰿ڽ���������.
		}
		break;
		////

		default: //�û������ĵ���Ϣ,��ϵͳ����.
		{
			return DefWindowProc(hwnd,msg,wParam,lParam);
		}

	}

	return WM_NULL;
}

/*============================================================================*/

//static void	AppMain(void)
void	GUI_DEMO_DrawText(void)
{
	HWND	hwnd;
	WNDCLASS	wcex;
	MSG msg;

	/////

	wcex.Tag 		    = WNDCLASS_TAG;

	wcex.Style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WinProc; //������������Ϣ����Ļص�����.
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= NULL;
	wcex.hIcon			= NULL;
	wcex.hCursor		= NULL;

	//����������
	hwnd	=CreateWindowEx(	NULL,
								&wcex,
								_T("GUI_DEMO:DrawText"), //��������
								WS_CAPTION|WS_DLGFRAME|WS_BORDER|WS_CLIPCHILDREN,
								0,0,300,400,    //����λ�úʹ�С
								NULL,NULL,NULL,NULL);

	//��ʾ������
	ShowWindow(hwnd,SW_SHOW);

	//��ʼ������Ϣѭ��(���ڹرղ�����ʱ,GetMessage������FALSE,�˳�����Ϣѭ��)��
	while(GetMessage(&msg,hwnd))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

/*============================================================================*/
