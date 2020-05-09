

#include <stdio.h>
#include <stdlib.h>

#include "x_libc.h"
#include "x_obj.h"
#include "CListMenu.h"

#include "emXGUI.h"


/*============================================================================*/


#define	BK_COLOR RGB888(10,40,50)

/*============================================================================*/

enum eID
{
	ID_EXIT=0x1000,
	ID_OK,
	ID_PREV_PAGE,
	ID_NEXT_PAGE,
	ID_TMR_100,
//	ID_TMR_500,
	ID_PREV_OBJ,
	ID_NEXT_OBJ,

};


/*============================================================================*/
#define	MSG_DRAG_ENABLE

class	CListMenu{

public:

	LRESULT	OnCreate(HWND hwnd,list_menu_cfg_t *cfg);
	LRESULT	OnDestroy(HWND hwnd);
	LRESULT	OnPaint(HWND hwnd);
	LRESULT	OnLButtonDown(HWND hwnd,int x,int y);
	LRESULT	OnLButtonUp(HWND hwnd,int x,int y);
	LRESULT	OnMouseMove(HWND hwnd,int x,int y);
	LRESULT	OnNotify(HWND hwnd,u16 code,u16 id);
	LRESULT	OnKeyUp(HWND hwnd,int key_val);
	LRESULT	OnKeyDown(HWND hwnd,int key_val);
	LRESULT	OnTimer(HWND hwnd,int tmr_id);

	void SetSelObj(int idx);

	void MoveTo(int dx,int dy);
	void MoveToPrev(int bXmove,int bYmove);
	void MoveToNext(int bXmove,int bYmove);
	void ListDragEnable(BOOL en);
private:


	void OffsetObjs(int dx,int dy);
	void draw_icon_obj(HDC hdc,struct __x_obj_item *obj,u32 flag,u32 style);
	LRESULT DrawFrame(HDC hdc,HWND hwnd);


	HWND hwndMain;

	int page_num,page_cur;
	int lbtn_down_x,lbtn_down_y;
	int x_move,x_moved,y_moved;
	int x_num,y_num;

	int	x_move_to,y_move_to;
	BOOL drag_en;
	BOOL execu_obj;


	RECT rc_main,rc_list;

	const struct __obj_list *obj_tbl;

	struct __x_obj_item *focus_list_obj;
	struct __x_obj_item *list_item;

};

/*============================================================================*/

static BOOL is_ver_list(HWND hwnd)
{
	if(GetWindowLong(hwnd,GWL_STYLE)&LMS_VER)
	{
		return TRUE;
	}
	return FALSE;
}

static BOOL is_page_move(HWND hwnd)
{
	if(GetWindowLong(hwnd,GWL_STYLE)&LMS_PAGEMOVE)
	{
		return TRUE;
	}
	return FALSE;

}


/*============================================================================*/

#define	OBJ_ACTIVE	(1<<0)

void CListMenu::draw_icon_obj(HDC hdc,struct __x_obj_item *obj,u32 flag,u32 style)
{

	WCHAR wstr[64],wbuf[64+12];
	RECT rc,rc0;
	const void *bmp;
	BITMAPINFO info;
	int x,y;

	rc =obj->rc;

	if(flag&OBJ_ACTIVE)
	{
		//draw_bmp(hdc,rc.x,rc.y,rc.w,rc.h,selected_bmp);
		InflateRect(&rc,-2,-2);

	#if 1 /* 矩形背景 */
		SetBrushColor(hdc,MapRGB888(hdc,RGB32_AVG(BK_COLOR,RGB888(200,200,250))));
		//FillRoundRect(hdc,&rc,20);
		FillRect(hdc,&rc);
	#endif
		////

 	 #if 1 /* 矩形外框 */
		/*
		EnableAntiAlias(hdc,TRUE);
		SetPenColor(hdc,MapRGB(hdc,250,50,50));
		SetPenSize(hdc,2);
		DrawRoundRect(hdc,&rc,20);
		EnableAntiAlias(hdc,FALSE);
		*/

		SetPenColor(hdc,MapRGB(hdc,250,20,20));
		DrawRect(hdc,&rc);
		InflateRect(&rc,-1,-1);
		SetPenColor(hdc,MapRGB(hdc,250,100,100));
		DrawRect(hdc,&rc);

    #endif
	}
	else
	{
		if(style& LMS_ICONFRAME)
		{ //外框
			InflateRect(&rc,-2,-2);
/*
			EnableAntiAlias(hdc,TRUE);
			SetPenColor(hdc,MapRGB(hdc,180,180,180));
			SetPenSize(hdc,1);
			DrawRoundRect(hdc,&rc,20);
			EnableAntiAlias(hdc,FALSE);
*/

			SetPenColor(hdc,MapRGB(hdc,180,180,180));
			DrawRect(hdc,&rc);
			InflateRect(&rc,-1,-1);
			SetPenColor(hdc,MapRGB(hdc,100,100,100));
			DrawRect(hdc,&rc);

		}

	}

#if 1
	bmp =(*obj_tbl[obj->id].bmp);
	if(bmp!=NULL)
	{
		BMP_GetInfo(&info,bmp);

		x = rc.x+(((int)rc.w-(int)info.Width)/2);
		y = rc.y+(((int)rc.h-(int)info.Height)/2);
		BMP_Draw(hdc,x,y,bmp,NULL);
	}
#endif
	/////
	SetTextColor(hdc,MapRGB(hdc,250,250,250));

	rc0.w =rc.w;
	rc0.h =32;
	rc0.x =rc.x;
	rc0.y =rc.y+rc.h-rc0.h-6;
	DrawText(hdc,obj->pszText,-1,&rc0,DT_SINGLELINE|DT_BOTTOM|DT_CENTER);

}


void CListMenu::SetSelObj(int idx)
{
	struct __x_obj_item *obj;
	int i;

	i=0;

	obj =x_obj_get_first(list_item);
	while(obj!=NULL)
	{
		if(i==idx)
		{
			focus_list_obj =obj;
			InvalidateRect(hwndMain,NULL,FALSE);
			break;
		}
		obj =x_obj_get_next(obj);
		i++;
	}
}

void CListMenu::MoveTo(int dx,int dy)
{
	x_move_to =dx;
	y_move_to =dy;
}

void CListMenu::MoveToPrev(int bXmove,int bYmove)
{
	struct __x_obj_item *obj=NULL;
	int bMovePage=is_page_move(hwndMain);

	obj =x_obj_get_first(list_item);


	if(bXmove)
	{
		if(bMovePage)
		{
			x_move_to += rc_list.w;
		}
		else
		{
			x_move_to = ((obj->rc.x+(obj->rc.w>>1))/obj->rc.w)*obj->rc.w;
		}

		if(x_move_to > 0)
		{
			x_move_to =0;
		}
	}

	if(bYmove)
	{
		if(bMovePage)
		{
			y_move_to += rc_list.h;
		}
		else
		{
			y_move_to = ((obj->rc.y+(obj->rc.h>>1))/obj->rc.h)*obj->rc.h;
		}

		if(y_move_to > 0)
		{
			y_move_to =0;
		}
	}

}

void CListMenu::MoveToNext(int bXmove,int bYmove)
{
	struct __x_obj_item *obj=NULL;
	int bMovePage=is_page_move(hwndMain);

	obj =x_obj_get_first(list_item);

	if(bXmove)
	{
		if(bMovePage)
		{
			x_move_to -= rc_list.w;
		}
		else
		{
			x_move_to = ((obj->rc.x-(obj->rc.w))/obj->rc.w)*obj->rc.w;
		}


		if(x_move_to < (-(page_num*rc_list.w)))
		{
			x_move_to =-(page_num*rc_list.w);
		}
	}

	if(bYmove)
	{
		if(bMovePage)
		{
			y_move_to -= rc_list.h;
		}
		else
		{
			y_move_to = ((obj->rc.y-(obj->rc.h))/obj->rc.h)*obj->rc.h;
		}

		if(y_move_to < (-(page_num*rc_list.h)))
		{
			y_move_to =-(page_num*rc_list.h);
		}
	}

}

void CListMenu::ListDragEnable(BOOL en)
{
	drag_en =en;
}

/*============================================================================*/

extern const char bkgnd_bmp[];

LRESULT CListMenu::DrawFrame(HDC hdc,HWND hwnd)
{
	int i,a,x,y,style;
	WCHAR wbuf[128],wstr[64];
	RECT rc;
	struct __x_obj_item *obj;
	////////

	style=GetWindowLong(hwnd,GWL_STYLE);


	////backgroup
	//StretchBlt(hdc,0,0,rc_main.w,rc_main.h,hdc_bkgnd,0,0,bkgnd_w,bkgnd_h,SRCCOPY);
	//BitBlt(hdc,0,0,rc_main.w,rc_main.h,hdc_bkgnd,0,0,SRCCOPY);

	ClrDisplay(hdc,NULL,MapRGB888(hdc,BK_COLOR));
	//BMP_Draw(hdc,0,0,bkgnd_bmp,NULL);

	////list_item
	SetFont(hdc,NULL);
	obj =x_obj_get_first(list_item);
	while(obj != NULL)
	{
		if(IsIntersectRect(&rc_list,&obj->rc))
		{	//只绘制在可视区内的对象.
			if(focus_list_obj == obj)
			{
				draw_icon_obj(hdc,obj,OBJ_ACTIVE,style);
			}
			else
			{
				draw_icon_obj(hdc,obj,0,style);
			}
		}

		obj = x_obj_get_next(obj);
	}

#if 0
	//// draw title
	//SetFont(hdc,hFont24);
	if(style&LMS_ICONTITLE)
	{
		if(focus_list_obj!=NULL)
		{

			//rc =rc_title;
			//rc.x =0;
			//rc.y =0;
			//ClrDisplay(hdc_argb,&rc,ARGB4444(8,0,0,0));
			//SetPenColor(hdc_argb,ARGB4444(13,0,0,0));
			//DrawRect(hdc_argb,&rc);

			rc =rc_title;
			//BitBlt(hdc,rc.x,rc.y,rc.w,rc.h,hdc_argb,0,0,SRCCOPY);
			SetTextColor(hdc,MapRGB(hdc,10,250,10));
			SetBrushColor(hdc,MapRGB(hdc,0,30,30));
			DrawText(hdc,focus_list_obj->pszText,-1,&rc,DT_SINGLELINE|DT_VCENTER|DT_CENTER|DT_BKGND);

			SetTextColor(hdc,MapRGB(hdc,200,250,250));
			x_wsprintf(wbuf,L" %d/%d",page_cur+1,page_num);
			DrawText(hdc,wbuf,-1,&rc,DT_SINGLELINE|DT_VCENTER|DT_LEFT);
		}
	}

#endif

#if 1

	//// draw page label
	SetPenSize(hdc,1);
	if(is_ver_list(hwnd))
	{
		int x,y,i;
		RECT m_rc[2],rc;


		obj =x_obj_get_first(list_item);

		if(obj->rc.y != y_move_to)
		{
			i=MIN(0-obj->rc.y,page_num*rc_list.h);

			rc.w =100;
			rc.h =16;
			rc.x =(rc_main.w-rc.w)>>1;
			rc.y =rc_main.h-rc.h-2;
			MakeProgressRect(m_rc,&rc,page_num*rc_list.h,i,PB_ORG_LEFT);

			SetPenColor(hdc,MapRGB(hdc,200,200,200));
			SetBrushColor(hdc,MapRGB(hdc,250,250,250));
			FillRect(hdc,&m_rc[0]);
			DrawRect(hdc,&rc);
		}


	}
	else
	{
		int x,y,i;
		RECT m_rc[2],rc;


		obj =x_obj_get_first(list_item);

		if(obj->rc.x != x_move_to)
		{
			i=MIN(0-obj->rc.x,page_num*rc_list.w);

			rc.w =100;
			rc.h =16;
			rc.x =(rc_main.w-rc.w)>>1;
			rc.y =rc_main.h-rc.h-2;
			MakeProgressRect(m_rc,&rc,page_num*rc_list.w,i,PB_ORG_LEFT);

			SetPenColor(hdc,MapRGB(hdc,200,200,200));
			SetBrushColor(hdc,MapRGB(hdc,250,250,250));
			FillRect(hdc,&m_rc[0]);
			DrawRect(hdc,&rc);
		}

	}
#endif

	////end

	return TRUE;
}


/*============================================================================*/

void CListMenu::OffsetObjs(int dx,int dy)
{
	struct __x_obj_item *obj;

	////list_item
	obj =x_obj_get_first(list_item);
	while(obj != NULL)
	{

		OffsetRect(&obj->rc,dx,dy);

		obj = x_obj_get_next(obj);
	}

}



/*============================================================================*/

LRESULT CListMenu::OnCreate(HWND hwnd,list_menu_cfg_t *cfg)
{
	int i,j,page,style;
	RECT rc,*m_rc;

	hwndMain =hwnd;
	focus_list_obj =NULL;
	execu_obj =FALSE;

	style =GetWindowLong(hwnd,GWL_STYLE);
	GetClientRect(hwnd,&rc_main);

	drag_en =TRUE;
	x_move_to =0;
	y_move_to =0;

	rc =rc_main;
	x_num =cfg->x_num;
	y_num =cfg->y_num;
	obj_tbl =cfg->list_objs;

	m_rc =new RECT[x_num*y_num];

	rc.x=0;
	rc.y=0;
	rc.w=rc_main.w;
	rc.h=0;

#if 0
	if(style&LMS_ICONTITLE)
	{
		rc.x =0;
		rc.y =0;
		rc.w =rc_main.w;
		rc.h =32;
	}
	else
	{
		rc.x=0;
		rc.y=0;
		rc.w=rc_main.w;
		rc.h=0;
	}
	rc_title =rc;
#endif

	OffsetRect(&rc,0,rc.h);
	rc.h =rc_main.h-rc.y;
	InflateRectEx(&rc,0,0,0,-28);
	rc_list =rc;


	list_item =x_obj_create(L"list_item", 0xFFFFFFFF, &rc_list,X_OBJ_VISIBLE,0,NULL);
	rc =rc_list;

	MakeMatrixRect(m_rc,&rc_list,0,0,x_num,y_num);
	if(is_ver_list(hwnd))
	{
		for(i=0;obj_tbl[i].pName!=NULL;i++)
		{
			const WCHAR *pText;
			page =i/(x_num*y_num);
			j    =i%(x_num*y_num);

			rc = m_rc[j];
			rc.y += page*rc_list.h;
			pText =obj_tbl[i].pName;

		//	InflateRect(&rc,-6,-8);
			x_obj_create(pText,	i,	&rc,	X_OBJ_VISIBLE,0,list_item);

		}
	}
	else
	{
		for(i=0;obj_tbl[i].pName!=NULL;i++)
		{
			const WCHAR *pText;
			page =i/(x_num*y_num);
			j    =i%(x_num*y_num);

			rc = m_rc[j];
			rc.x += page*rc_list.w;
			pText =obj_tbl[i].pName;

		//	InflateRect(&rc,-6,-8);
			x_obj_create(pText,	i,	&rc,	X_OBJ_VISIBLE,0,list_item);

		}
	}

	x_move =m_rc[0].x;
	page_num =page+1;
	page_cur =0;
	focus_list_obj =x_obj_get_from_id(list_item,0);
	////



//	hFontSEG_32 =XFT_CreateFont(SEG_NUM_32);


	SetTimer(hwnd,ID_TMR_100,20,TMR_START,NULL);
//	SetTimer(hwnd,ID_TMR_500,500,TMR_START,NULL);
	delete m_rc;

	return TRUE;
}

LRESULT CListMenu::OnDestroy(HWND hwnd)
{
//	int i;

	x_obj_del(list_item);
	//x_obj_del(button_item);
	//DeleteDC(hdc_mem);
	//DeleteDC(hdc_argb);
	//DeleteDC(hdc_ico);

//	DeleteFont(hFontSEG_32);



	PostQuitMessage(hwnd);
	return TRUE;
}

LRESULT	CListMenu::OnPaint(HWND hwnd)
{
	HDC hdc,hdc_mem;
	PAINTSTRUCT ps;
	RECT rc;

	GetClientRect(hwnd,&rc);
	hdc_mem =CreateMemoryDC(SURF_SCREEN,rc.w,rc.h);
	DrawFrame(hdc_mem,hwnd);


	hdc =BeginPaint(hwnd,&ps);
	BitBlt(hdc,0,0,rc_main.w,rc_main.h,hdc_mem,0,0,SRCCOPY);
	EndPaint(hwnd,&ps);

	DeleteDC(hdc_mem);
	return TRUE;
}

LRESULT CListMenu::OnNotify(HWND hwnd,u16 code,u16 id)
{
#if 0
//	int i;
	struct __x_obj_item *obj;

	if(code!=BN_CLICKED)
	{
		return FALSE;
	}

	switch(id)
	{
	case ID_FOCUS_PREV:
		obj =x_obj_get_prev(focus_list_obj); //find prev obj.
		if(obj != NULL)
		{
			focus_list_obj =obj;
			if(obj->rc.x < rc_list.x)
			{
				MoveToPrevPage();
			}

			if(obj->rc.x > (rc_list.x+rc_list.w-1))
			{
				MoveToNextPage();
			}
		}
		else
		{
			focus_list_obj =x_obj_get_prev(list_item);
			SetToPage(page_num-1);
		}
		break;
		////

	case ID_FOCUS_NEXT:
		obj =x_obj_get_next(focus_list_obj); //find next obj.
		if(obj!=NULL)
		{
			focus_list_obj =obj;
			if(obj->rc.x < rc_list.x)
			{
				MoveToPrevPage();
			}

			if(obj->rc.x > (rc_list.x+rc_list.w-1))
			{
				MoveToNextPage();
			}
		}
		else
		{
			focus_list_obj =x_obj_get_next(list_item);
			SetToPage(0);
		}
		break;
		////

	case ID_FOCUS_LEFT:
		obj =x_obj_get_left(focus_list_obj);
		if(obj != NULL)
		{
			focus_list_obj =obj;
			if(obj->rc.x < rc_list.x)
			{
				MoveToPrevPage();
			}

			if(obj->rc.x > (rc_list.x+rc_list.w-1))
			{
				MoveToNextPage();
			}
		}
		else
		{
			focus_list_obj =x_obj_get_prev(list_item);
			SetToPage(page_num-1);
		}
		break;
		////

//		break;
		////

	case ID_FOCUS_RIGHT:
		obj =x_obj_get_right(focus_list_obj);
		if(obj!=NULL)
		{
			focus_list_obj =obj;
			if(obj->rc.x < rc_list.x)
			{
				MoveToPrevPage();
			}

			if(obj->rc.x > (rc_list.x+rc_list.w-1))
			{
				MoveToNextPage();
			}
		}
		else
		{
			focus_list_obj =x_obj_get_next(list_item);
			SetToPage(0);
		}
		break;
	}
#endif

	return TRUE;
}

LRESULT	CListMenu::OnKeyUp(HWND hwnd,int key_val)
{
#if 0
	if(key_val == VK_LEFT)
	{
		PostMessage(hwnd,WM_NOTIFY,ID_FOCUS_LEFT|(BN_CLICKED<<16),0);
	}

	if(key_val == VK_RIGHT)
	{
		PostMessage(hwnd,WM_NOTIFY,ID_FOCUS_RIGHT|(BN_CLICKED<<16),0);

	}

	if(key_val == VK_UP)
	{
		PostMessage(hwnd,WM_NOTIFY,ID_FOCUS_UP|(BN_CLICKED<<16),0);
	}

	if(key_val == VK_DOWN)
	{
		PostMessage(hwnd,WM_NOTIFY,ID_FOCUS_DOWN|(BN_CLICKED<<16),0);
	}
#endif

/*
	if(key_val == VK_RETURN)
	{
		int id;

		id = focus_list_obj->id;
		if(id >=0 & id<= 256)
		{
			if(execu_obj!=FALSE)
			{
				obj_tbl[id].cbStartup(hwnd);
			}

		}
	}
*/
	return TRUE;
}

	

LRESULT	CListMenu::OnKeyDown(HWND hwnd,int key_val)
{
/*
	////
	if(key_val == VK_NUMPAD0)
	{
		PostMessage(hwnd,WM_COMMAND,ID_FOCUS_PREV,0);
		InvalidateRect(hwnd,NULL,FALSE);
	}

	if(key_val == VK_NUMPAD1)
	{
		PostMessage(hwnd,WM_COMMAND,ID_FOCUS_NEXT,0);
		InvalidateRect(hwnd,NULL,FALSE);
	}

	if(key_val == VK_NUMPAD2)
	{
		MoveToPrevPage();
		InvalidateRect(hwnd,NULL,FALSE);
	}

	if(key_val == VK_NUMPAD3)
	{
		MoveToNextPage();
		InvalidateRect(hwnd,NULL,FALSE);
	}
*/
	return TRUE;
}


LRESULT	CListMenu::OnLButtonDown(HWND hwnd,int x,int y)
{
//	int id;
	struct __x_obj_item *obj=NULL;
//	RECT rc;

	execu_obj =TRUE;

	x_moved =x;
	y_moved =y;

	lbtn_down_x =x;
	lbtn_down_y =y;

	obj =x_obj_get_from_pos(list_item,x,y);
	if(obj!=NULL)
	{
		focus_list_obj =obj;
	}
#if 0
	obj =x_obj_get_from_pos(button_item,x,y);
	{
		focus_btn_obj =obj;
	}

	if(focus_btn_obj!=NULL)
	{
		btn_down_count =0;
		id =focus_btn_obj->id;

		switch(id)
		{
			case ID_PREV_PAGE:
			{
				MoveToPrevPage();
			}
			break;

			case ID_NEXT_PAGE:
			{
				MoveToNextPage();
			}
			break;

			case ID_OK:
			case ID_EXIT:
			{
				break;
			}

			default:
				break;
		}
	}
#endif
	InvalidateRect(hwnd,NULL,FALSE);
	return TRUE;
}

LRESULT	CListMenu::OnLButtonUp(HWND hwnd,int x,int y)
{
	int id;
	POINT pt;
	struct __x_obj_item *obj=NULL;
	////

	pt.x =x;
	pt.y =y;

	if(PtInRect(&rc_list,&pt))
	{
		if(is_ver_list(hwnd))
		{
			if((y-lbtn_down_y) < -50)
			{
				//MoveToNextPage();
				MoveToNext(FALSE,TRUE);
			}

			if((y-lbtn_down_y) > 50)
			{

				//MoveToPrevPage();
				MoveToPrev(FALSE,TRUE);
			}

			if(abs(y-lbtn_down_y) > 30)
			{
				execu_obj =FALSE; //拖动时，不执行obj，防止误操作
			}
		}
		else
		{
			//obj =x_obj_get_first(list_item);
			if((x-lbtn_down_x) < -50)
			{ //GoLEFT
				MoveToNext(TRUE,FALSE);

			}

			if((x-lbtn_down_x) > 50)
			{//GoRIGHT

				MoveToPrev(TRUE,FALSE);
			}

			if(abs(x-lbtn_down_x) > 30)
			{
				execu_obj =FALSE; //拖动时，不执行obj，防止误操作
			}

		}
	}

	////////
	obj=NULL;
	//if(obj==NULL) obj =x_obj_get_from_pos(button_item,x,y);
	if(obj==NULL)
	{
		obj =x_obj_get_from_pos(list_item,x,y);
	}

	if(obj != NULL)
	{
		id =obj->id;
/*
		if(id==ID_EXIT)
		{
			SendMessage(hwnd,WM_CLOSE,0,0);
		}

		if(id==ID_OK)
		{
			SendMessage(hwnd,WM_CLOSE,0,0);
		}

		if(id == ID_PREV_OBJ)
		{
			PostMessage(hwnd,WM_NOTIFY,ID_FOCUS_PREV|(BN_CLICKED<<16),0);
		}
		if(id == ID_NEXT_OBJ)
		{
			PostMessage(hwnd,WM_NOTIFY,ID_FOCUS_NEXT|(BN_CLICKED<<16),0);
		}
*/
		if(id >=0 & id<= 256)
		{
			if(execu_obj!=FALSE)
			if(id == focus_list_obj->id)
			{
				LM_NMHDR lmn;
				lmn.idx =id;

				NotifyParentEx(hwnd,LMN_CLICKED,(NMHDR*)&lmn);

			}
		}

	}
	/////
	execu_obj =TRUE;

//	focus_btn_obj =NULL;
	InvalidateRect(hwnd,NULL,FALSE);
	return 0;
}

LRESULT	CListMenu::OnMouseMove(HWND hwnd,int x,int y)
{
	int x_off,y_off;
	POINT pt;

	if(drag_en==FALSE)
	{
		return TRUE;
	}

	pt.x =x;
	pt.y =y;

	if(GetKeyState(VK_LBUTTON))
	if(PtInRect(&rc_list,&pt))
	{
		struct __x_obj_item *obj=NULL;
		x_off = x-x_moved;
		y_off = y-y_moved;

		if(is_ver_list(hwnd))
		{
			//OffsetObjs(0,y_off);

			//限制Y方向移动范围.
			if(y_off > 0)
			{
				obj =x_obj_get_first(list_item);
				if(obj->rc.y > obj->rc.h)
				{
					y_off =0;
				}

			}
			else
			{
				obj =x_obj_get_last(list_item);
				if(obj->rc.y < 0)
				{
					y_off =0;
				}
			}
			OffsetObjs(0,y_off);
		}
		else
		{
			//限制X方向移动范围.
			if(x_off > 0)
			{
				obj =x_obj_get_first(list_item);
				if(obj->rc.x > obj->rc.w)
				{
					x_off =0;
				}

			}
			else
			{
				obj =x_obj_get_last(list_item);
				if(obj->rc.x < 0)
				{
					x_off =0;
				}
			}
			OffsetObjs(x_off,0);
		}

		InvalidateRect(hwnd,NULL,FALSE);

		x_moved =x;
		y_moved =y;
	}
	return TRUE;

}

LRESULT	CListMenu::OnTimer(HWND hwnd,int tmr_id)
{
	BOOL need_draw=FALSE;

	if(tmr_id==ID_TMR_100)
	{
		POINT pt;
		RECT rc;

		GetCursorPos(&pt);
		GetWindowRect(hwnd,&rc);
		if((GetKeyState(VK_LBUTTON)==0) || (PtInRect(&rc,&pt)==FALSE))
		{
			struct __x_obj_item *obj=NULL;

			obj =x_obj_get_first(list_item);
			if(obj!=NULL)
			{
				int x,y,x_end,y_end;

				////

				if(is_ver_list(hwnd))
				{

					y =obj->rc.y;
					if(y > y_move_to)
					{

						if((y-y_move_to) > 60)
						{
							y =MIN(obj->rc.h>>2,y-y_move_to);
						}
						else if((y-y_move_to) > 20)
						{
							y =MIN(obj->rc.h>>4,y-y_move_to);
						}
						else
						{
							y =MIN(1,y-y_move_to);
						}

						OffsetObjs(0,-y);
						need_draw =TRUE;
					}
					else if(y < y_move_to)
					{

						if((y_move_to-y) > 60)
						{
							y =MIN(obj->rc.h>>2,y_move_to-y);
						}
						else if((y_move_to-y) > 20)
						{
							y =MIN(obj->rc.h>>4,y_move_to-y);
						}
						else
						{
							y =MIN(1,y_move_to-y);
						}
						OffsetObjs(0,y);
						need_draw =TRUE;
					}
				}
				else
				{

					x =obj->rc.x;
					if(x > x_move_to)
					{

						//x =MIN(rc_list.w>>3,x-x_move_to);
						if((x-x_move_to) > 60)
						{
							x =MIN(obj->rc.w>>2,x-x_move_to);
						}
						else if((x-x_move_to) > 20)
						{
							x =MIN(obj->rc.w>>4,x-x_move_to);
						}
						else
						{
							x =MIN(1,x-x_move_to);
						}

						OffsetObjs(-x,0);
						need_draw =TRUE;
					}
					else if(x < x_move_to)
					{
						//x =MIN(rc_list.w>>3,x_move_to-x);
						if((x_move_to-x) > 60)
						{
							x =MIN(obj->rc.w>>2,x_move_to-x);
						}
						else if((x_move_to-x) > 20)
						{
							x =MIN(obj->rc.w>>4,x_move_to-x);
						}
						else
						{
							x =MIN(1,x_move_to-x);
						}
						OffsetObjs(x,0);
						need_draw =TRUE;
					}

				}


			}
		}
	}


	if(need_draw!=FALSE)
	{
		InvalidateRect(hwnd,NULL,FALSE);
	}

	return TRUE;
}

///////////////////////////////////////////////////////

static	LRESULT	WinProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	CListMenu *pApp =(CListMenu*)GetWindowLong(hwnd,GWL_USERDATA);

	switch(msg)
	{
		case WM_CREATE:
		{

			list_menu_cfg_t *cfg =(list_menu_cfg_t*)lParam;

			pApp =new CListMenu;
			SetWindowLong(hwnd,GWL_USERDATA,(LONG)pApp);

			return pApp->OnCreate(hwnd,cfg);
		}
		break;
		////

		case 	MSG_SET_SEL:
		{
			int idx =wParam;

			pApp->SetSelObj(idx);

		}
		break;

		case	MSG_MOVE_PREV:
		{
			if(is_ver_list(hwnd))
			{
				pApp->MoveToPrev(FALSE,TRUE);
			}
			else
			{
				pApp->MoveToPrev(TRUE,FALSE);
			}

			if(wParam)
			{
				InvalidateRect(hwnd,NULL,FALSE);
			}
		}
		break;
		////

		case	MSG_MOVE_NEXT:
		{
			if(is_ver_list(hwnd))
			{
				pApp->MoveToNext(FALSE,TRUE);
			}
			else
			{
				pApp->MoveToNext(TRUE,FALSE);
			}

			if(wParam)
			{
				InvalidateRect(hwnd,NULL,FALSE);
			}
		}
		break;
		////

		case MSG_LIST_DRAG_ENABLE:
		{
			pApp->ListDragEnable(wParam);
		}
		break;
		////

		case    WM_NOTIFY:
		{
			return pApp->OnNotify(hwnd,HIWORD(wParam),HIWORD(wParam));
		}
		return DefWindowProc(hwnd,msg,wParam,lParam);
		////

		case    WM_KEYUP:
		{
			int key;

			key=LOWORD(wParam);
			return pApp->OnKeyUp(hwnd,key);
		}
		return DefWindowProc(hwnd,msg,wParam,lParam);
		////
		case    WM_KEYDOWN:
		{
			int key;

			key=LOWORD(wParam);
			return pApp->OnKeyDown(hwnd,key);
		}
		return DefWindowProc(hwnd,msg,wParam,lParam);
		////

		case    WM_LBUTTONDOWN:
		{
			int x,y;

			x=LOWORD(lParam);
			y=HIWORD(lParam);
			return pApp->OnLButtonDown(hwnd,x,y);
		}
		return DefWindowProc(hwnd,msg,wParam,lParam);
		////

//		case WM_MOUSELEAVE:
		case WM_LBUTTONUP:
		{
			int x,y;

			x=LOWORD(lParam);
			y=HIWORD(lParam);
			pApp->OnLButtonUp(hwnd,x,y);
		}
		return DefWindowProc(hwnd,msg,wParam,lParam);
		////

		case WM_MOUSEMOVE:
		{
			int x,y;

			x=LOWORD(lParam);
			y=HIWORD(lParam);
			pApp->OnMouseMove(hwnd,x,y);
		}
		return DefWindowProc(hwnd,msg,wParam,lParam);
		////

		case WM_TIMER:
		{
			int tmr_id;

			tmr_id =wParam;
			return pApp->OnTimer(hwnd,tmr_id);
		}
		break;
		/////



		case WM_ERASEBKGND:
		{
			return TRUE;
		}
		////

		case WM_PAINT:
			return pApp->OnPaint(hwnd);
			//////

		case WM_DESTROY:
			{
				pApp->OnDestroy(hwnd);
				delete pApp;

			}
			return DefWindowProc(hwnd,msg,wParam,lParam);

		default:			return DefWindowProc(hwnd,msg,wParam,lParam);

	}

	return WM_NULL;
}
/*============================================================================*/

const WNDCLASS wcex_ListMenu={

		WNDCLASS_TAG,
		0,
		WinProc,
		0,		/* cbClsExtra */
		0,		/* cbWndExtra */
		NULL,	/* hInstance */
		NULL,	/* hIcon */
		NULL,	/* hCursor */

};

/*=========================================================================*/

/*=========================================================================*/

