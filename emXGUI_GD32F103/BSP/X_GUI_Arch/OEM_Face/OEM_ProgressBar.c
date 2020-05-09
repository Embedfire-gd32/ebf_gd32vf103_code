
#include "emXGUI.h"
#include "emXGUI_Arch.h"

/*=========================================================================================*/
#define	PROGBAR_TEXTCOLOR		XRGB8888(0,0,0)
#define	PROGBAR_BACKCOLOR		XRGB8888(220,220,220)
#define	PROGBAR_BODERCOLOR		XRGB8888(30,30,50)
#define	PROGBAR_FORECOLOR		XRGB8888(0,210,0)


static void _SetColor(HWND hwnd,CTLCOLOR *cr,u32 style,u32 state)
{

	cr->TextColor =PROGBAR_TEXTCOLOR;
	cr->BackColor =PROGBAR_BACKCOLOR;
	cr->BorderColor =PROGBAR_BODERCOLOR;
	cr->ForeColor  =PROGBAR_FORECOLOR;

	if(style&WS_DISABLED)
	{
		cr->TextColor =RGB32_Offset(cr->TextColor,-10,-10,-10);
		cr->BackColor =RGB32_Offset(cr->BackColor,-10,-10,-10);
		cr->BorderColor =RGB32_Offset(cr->BorderColor,-10,-10,-10);
		cr->ForeColor =RGB32_Offset(cr->ForeColor,-10,-10,-10);

		cr->TextColor =RGB32_AVG(cr->TextColor,RGB888(120,120,128));
		cr->BackColor =RGB32_AVG(cr->BackColor,RGB888(120,120,128));
		cr->BorderColor =RGB32_AVG(cr->BorderColor,RGB888(120,120,128));
		cr->ForeColor =RGB32_AVG(cr->ForeColor,RGB888(120,120,128));

	}

}

static void fill_pb_rect(HDC hdc,RECT *prc,COLOR_RGB32 c)
{
	RECT rc;
	COLOR_RGB32 c1,c2;

	rc = *prc;

	rc.h >>= 1;
	c1 =RGB32_Offset(c,+35,+35,+35);
	c2 =RGB32_Offset(c,+10,+10,+10);
	GradientFillRect(hdc,&rc,MapRGB888(hdc,c1),MapRGB888(hdc,c2),TRUE);

	OffsetRect(&rc,0,rc.h);
	rc.h =(prc->y+prc->h) - rc.y;
	c1 =RGB32_Offset(c,-5,-5,-5);
	SetBrushColor(hdc,MapRGB888(hdc,c1));
	FillRect(hdc,&rc);

}

/*============================================================================*/

static void draw_flat_prog_bar(HDC hdc,HWND hwnd,PROGRESSBAR_CFG *pPB,ePB_ORG align,CTLCOLOR *cr)
{
	RECT m_rc[2],rc;

	GetClientRect(hwnd,&rc);

	SetPenColor(hdc,MapRGB888(hdc,cr->BorderColor));
	DrawRect(hdc,&rc);

	InflateRect(&rc,-1,-1);
	MakeProgressRect(m_rc,&rc,pPB->Rangle,pPB->Value,align);

	SetBrushColor(hdc,MapRGB888(hdc,cr->ForeColor));
	FillRect(hdc,&m_rc[0]);
	SetBrushColor(hdc,MapRGB888(hdc,cr->BackColor));
	FillRect(hdc,&m_rc[1]);

}

static void draw_3D_prog_bar(HDC hdc,HWND hwnd,PROGRESSBAR_CFG *pPB,ePB_ORG align,CTLCOLOR *cr)
{
	RECT m_rc[2],rc;
	COLORREF c1,c2;


	GetClientRect(hwnd,&rc);

	SetPenColor(hdc,MapRGB888(hdc,cr->BorderColor));
	DrawRect(hdc,&rc);

	InflateRect(&rc,-1,-1);
	MakeProgressRect(m_rc,&rc,pPB->Rangle,pPB->Value,align);

	c1 =RGB32_AVG(cr->ForeColor,RGB888(240,240,240));
	c2 =RGB32_AVG(cr->ForeColor,RGB888(20,20,20));

	Fill3DRect(hdc,&m_rc[0],MapRGB888(hdc,c1),MapRGB888(hdc,c2));
	SetBrushColor(hdc,MapRGB888(hdc,cr->BackColor));
	FillRect(hdc,&m_rc[1]);

}

static void draw_def_prog_bar(HDC hdc,HWND hwnd,PROGRESSBAR_CFG *pPB,ePB_ORG align,CTLCOLOR *cr)
{
	RECT m_rc[2],rc;
	COLORREF c1;


	GetClientRect(hwnd,&rc);

	c1 =RGB32_AVG(cr->BorderColor,cr->BackColor);
	SetPenColor(hdc,MapRGB888(hdc,c1));
	DrawRect(hdc,&rc);

	InflateRect(&rc,-1,-1);
	c1 =RGB32_Offset(cr->BackColor,+25,+25,+25);
	SetPenColor(hdc,MapRGB888(hdc,c1));
	DrawRect(hdc,&rc);


	//SetPenColor(hdc,MapRGB888(hdc,cr->BorderColor));
	//DrawRect(hdc,&rc);

	InflateRect(&rc,-1,-1);
	MakeProgressRect(m_rc,&rc,pPB->Rangle,pPB->Value,align);

	fill_pb_rect(hdc,&m_rc[0],cr->ForeColor);
	fill_pb_rect(hdc,&m_rc[1],cr->BackColor);
/*
	c1 =RGB32_AVG(cr->ForeColor,RGB888(240,240,240));
	c2 =RGB32_AVG(cr->ForeColor,RGB888(20,20,20));


	GradientFillRect(hdc,&m_rc[0],MapRGB888(hdc,c1),MapRGB888(hdc,c2),TRUE);
	SetBrushColor(hdc,MapRGB888(hdc,cr->BackColor));
	FillRect(hdc,&m_rc[1]);
*/
}

static void _Paint(DRAWITEM_HDR *di,CTLCOLOR *cr,const WCHAR *pText)
{
	HWND hwnd;
	HDC hdc;
	RECT rc;
	ePB_ORG align;
	PROGRESSBAR_CFG cfg;
	////

	hwnd =di->hwnd;
	hdc =di->hDC;
	rc =di->rc;

	cfg.cbSize =sizeof(PROGRESSBAR_CFG);
	cfg.fMask  =PB_CFG_ALL;

	SendMessage(hwnd,PBM_GET_CFG,0,(LPARAM)&cfg);
	switch(di->Style&PBS_ALIGN_MASK)
	{
		case PBS_ALIGN_LEFT:
			align =PB_ORG_LEFT;
			break;
		case PBS_ALIGN_RIGHT:
			align =PB_ORG_RIGHT;
			break;
		case PBS_ALIGN_TOP:
			align =PB_ORG_TOP;
			break;
		case PBS_ALIGN_BOTTOM:
			align =PB_ORG_BOTTOM;
			break;

		default:
			return;
	}

	switch(di->Style & PBS_FACE_MASK)
	{
		case PBS_FLAT:
			draw_flat_prog_bar(hdc,hwnd,&cfg,align,cr);
			break;

		case PBS_3D:
			draw_3D_prog_bar(hdc,hwnd,&cfg,align,cr);
			break;

		default:
			draw_def_prog_bar(hdc,hwnd,&cfg,align,cr);
			break;
	}

	if(di->Style & PBS_TEXT)
	{
		GetClientRect(hwnd,&rc);
		SetTextColor(hdc,MapRGB888(hdc,cr->TextColor));
		DrawText(hdc,pText,-1,&rc,cfg.TextFlag);
	}

}


static win_oem_t oem=
{
	_SetColor,
	_Paint,
};

win_oem_t* GetProgressBarOEM(void)
{
	return &oem;
}
