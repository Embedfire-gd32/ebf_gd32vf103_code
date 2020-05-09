

#include 	<emXGUI.h>

#include	"listview.h"
#include	"list.h"
/*==================================================================*/

void*	GUI_MEM_Alloc(U32 size);
void	GUI_MEM_Free(void *p);

/*==================================================================*/

#define	LV_DEF_TEXTCOLOR	RGB888(0,0,0)
#define	LV_DEF_BACKCOLOR	RGB888(240,240,240)
#define	LV_DEF_BORDERCOLOR	RGB888(100,100,100)


typedef struct
{
	U32 Format;
	COLOR_RGB32 TextColor,BackColor,BorderColor;
	LONG Data;
	WCHAR *pszText;
	U32 MaxTextLen;

}lv_cell_t;

typedef struct
{
	list_t	node;
	WCHAR *Text;
	U32 Width;

}lv_col_t;

typedef struct
{
	list_t	node;
	lv_cell_t *cell_item;
	U32 Height;
}lv_row_t;


typedef struct	{

	U16 col_num;	//列数量
	U16 row_num;	//行数量

	U16 col_height;	//列表头高度
	U16 rsv;
	//U16 row_width;	//行表头宽度


	U16 first_col_idx;	//第一个列索引
	U16	first_row_idx;	//第一个行索引

	U16 cur_sel_col; //当前选中的列
	U16 cur_sel_row; //当前选中的行

	lv_cell_t *sel_cell;	//当前选口中的单元


	list_t list_col_hdr;	//列表头链表
	list_t list_row_hdr;	//行表头链表


}listview_t;


////????
typedef	struct	__LVINFO{

	U16 col_count;
	U16 row_count;
}LVINFO;

/*==================================================================*/

static WCHAR*	GUI_wstr_alloc(const WCHAR *str)
{
	int len;
	WCHAR *new_str;

	len	=GUI_wstrlen(str);
	len += 1;			//'\0'

	new_str	=(WCHAR*)GUI_MEM_Alloc(len*2);	//sizeof(WCHAR)

	if(new_str != NULL)
	{
		GUI_wstrncpy(new_str,str,len);
	}
	return	new_str;
}

static void	GUI_wstr_free(WCHAR *str)
{
	GUI_MEM_Free(str);
}
/*==================================================================*/

static list_t* find_index_node(list_t *list,int idx)
{
	int i;
	list_t *n=NULL;

	i=0;
	n=list->next;
	while(1)
	{
		if(i==idx)
		{
			break;
		}

		if(n==list)
		{
			break;
		}

		n=n->next;
		i++;
	}

	return n;
}


static	void listview_reset(listview_t *pLV)
{

	pLV->col_num =0;
	pLV->row_num =0;
	pLV->col_height =24;
//	pLV->row_width  =56;
	pLV->first_col_idx =0;
	pLV->first_row_idx =0;

	pLV->cur_sel_col =0;
	pLV->cur_sel_row =0;

	pLV->sel_cell =NULL;

	list_init(&pLV->list_col_hdr);
	list_init(&pLV->list_row_hdr);

}

/*==================================================================*/

static BOOL listview_add_col(listview_t *pLV,U16 idx,LVCOLUM *lv_col)
{
	list_t *n;
	lv_col_t *col;

	col =(lv_col_t*)GUI_MEM_Alloc(sizeof(lv_col_t));

	if(col!=NULL)
	{
		list_init(&col->node);
		col->Text =lv_col->pszText;
		col->Width =lv_col->Width;

		n =find_index_node(&pLV->list_col_hdr,idx);
		list_add_tail(&col->node,n);

		pLV->col_num++;
		return TRUE;
	}
	return FALSE;
}


static BOOL listview_add_row(listview_t *pLV,U16 idx,LVROW *lv_row)
{
	list_t *n;
	lv_row_t *row;
	int i;

	row =(lv_row_t*)GUI_MEM_Alloc(sizeof(lv_row_t));

	if(row!=NULL)
	{
		list_init(&row->node);
		row->Height =lv_row->Height;

		n =find_index_node(&pLV->list_row_hdr,idx);
		list_add_tail(&row->node,n);
		pLV->row_num++;
		////

		row->cell_item =(lv_cell_t*)GUI_MEM_Alloc(sizeof(lv_cell_t)*pLV->col_num);

		for(i=0;i<pLV->col_num;i++)
		{
			row->cell_item[i].Format =LVF_DEFAULT;

			row->cell_item[i].TextColor =LV_DEF_TEXTCOLOR;
			row->cell_item[i].BackColor =LV_DEF_BACKCOLOR;
			row->cell_item[i].BorderColor =LV_DEF_BORDERCOLOR;
			row->cell_item[i].Data       =0;
			row->cell_item[i].pszText    =NULL;
			row->cell_item[i].MaxTextLen =0;

		}

		return TRUE;
	}
	return FALSE;
}

static void listview_set_first_col_idx(listview_t *pLV,U16 idx)
{
	if(idx < 0)
	{
		return;
	}

	if(idx>(pLV->col_num-1))
	{
		return;
	}

	pLV->first_col_idx =idx;

}

static void listview_set_first_row_idx(listview_t *pLV,U16 idx)
{
	if(idx<0)
	{
		return;
	}

	if(idx>pLV->row_num)
	{
		idx =pLV->row_num-1;
	}
	pLV->first_row_idx =idx;

}


static void listview_clean_row(listview_t *pLV,lv_row_t *row)
{
	int i ;

	list_del_init(&row->node);
	for(i=0;i<pLV->col_num;i++)
	{
		GUI_wstr_free(row->cell_item[i].pszText);
		row->cell_item[i].pszText =NULL;
	}

	GUI_MEM_Free(row->cell_item);
	GUI_MEM_Free(row);
}

static BOOL listview_delete_row(listview_t *pLV,U16 idx)
{
	list_t *n;
	lv_row_t *row;

	if(pLV->row_num <= 0)
	{
		return FALSE;
	}

	n =find_index_node(&pLV->list_row_hdr,idx);
	if(n!=&pLV->list_row_hdr)
	{
		row =(lv_row_t*)list_entry(n,lv_row_t,node);

		listview_clean_row(pLV,row);

		if(pLV->cur_sel_row == idx)
		{
			pLV->cur_sel_row =0;
			pLV->sel_cell =NULL;
		}

		pLV->row_num --;
		return TRUE;
	}
	return FALSE;
}

static void listview_delete_all_rows(listview_t *pLV)
{
	list_t *lst,*n;

	lst =&pLV->list_row_hdr;
	n = lst->next;

	while(n!=lst)
	{
		lv_row_t *row;

		row =(lv_row_t*)list_entry(n,lv_row_t,node);
		n =n->next;

		listview_clean_row(pLV,row);
	}

	pLV->first_row_idx =0;
	pLV->row_num =0;
}

static	void listview_destroy(listview_t *pLV)
{
	list_t *lst,*n;

	//destroy row
	listview_delete_all_rows(pLV);

	//destroy col
	lst =&pLV->list_col_hdr;
	n =lst->next;
	while(n!=lst)
	{
		lv_col_t *col;

		col =(lv_col_t*)list_entry(n,lv_col_t,node);
		n =n->next;

		GUI_MEM_Free(col);

	}
}


static BOOL listview_set_cell(listview_t *pLV,U32 idx,LVCELL *new)
{
	list_t *lst,*n;
	lv_row_t *row;
	lv_cell_t *cell;
	U16 col_idx,row_idx;

	row_idx =(idx>>16)&0xFFFF;
	col_idx =idx&0xFFFF;

	n =find_index_node(&pLV->list_row_hdr,row_idx);
	if(n==&pLV->list_row_hdr)
	{
		return FALSE;
	}

	row =(lv_row_t*)list_entry(n,lv_row_t,node);

	cell = &row->cell_item[col_idx];

	if(new->Mask&LVCF_FORMAT)
	{
		cell->Format =new->Format;
	}

	if(new->Mask&LVCF_TEXT)
	{
		if(cell->pszText!=NULL)
		{
			GUI_wstr_free(cell->pszText);
		}
		cell->pszText =GUI_wstr_alloc(new->pszText);
	}

	if(new->Mask&LVCF_TEXTCOLOR)
	{
		cell->TextColor =new->TextColor;
	}

	if(new->Mask&LVCF_BACKCOLOR)
	{
		cell->BackColor =new->BackColor;
	}

	if(new->Mask&LVCF_BORDERCOLOR)
	{
		cell->BorderColor =new->BorderColor;
	}

	if(new->Mask&LVCF_DATA)
	{
		cell->Data =new->Data;
	}

	return TRUE;
}

static BOOL listview_get_cell(listview_t *pLV,U32 idx,LVCELL *out)
{
	list_t *lst,*n;
	lv_row_t *row;
	lv_cell_t *cell;
	U16 col_idx,row_idx;

	row_idx =(idx>>16)&0xFFFF;
	col_idx =idx&0xFFFF;

	n =find_index_node(&pLV->list_row_hdr,row_idx);

	row =(lv_row_t*)list_entry(n,lv_row_t,node);

	cell = &row->cell_item[col_idx];

	if(out->Mask&LVCF_FORMAT)
	{
		out->Format =cell->Format;
	}

	if(out->Mask&LVCF_TEXT)
	{
		out->pszText =cell->pszText;
	}

	if(out->Mask&LVCF_TEXTCOLOR)
	{
		out->TextColor =cell->TextColor;
	}

	if(out->Mask&LVCF_BACKCOLOR)
	{
		out->BackColor =cell->BackColor;
	}

	if(out->Mask&LVCF_BORDERCOLOR)
	{
		out->BorderColor =cell->BorderColor;
	}

	if(out->Mask&LVCF_DATA)
	{
		out->Data =cell->Data;
	}

	return TRUE;
}


static BOOL  listview_lbutton_down(HWND hwnd,listview_t *pLV,int pos_x,int pos_y)
{
	POINT pt;
	RECT rc;
	int i,x,y,idx_row;

	list_t *lst_row,*n_row;
	list_t *lst_col,*n_col;
	lv_col_t *col;
	lv_row_t *row;

	pt.x =pos_x;
	pt.y =pos_y;

	idx_row = pLV->first_row_idx;
	y =pLV->col_height;

	lst_row =&pLV->list_row_hdr;
	//n_row   =lst_row->next;
	n_row =find_index_node(lst_row,idx_row);
	while(n_row!=lst_row)
	{

		row =(lv_row_t*)list_entry(n_row,lv_row_t,node);

		i =pLV->first_col_idx;

		rc.x =0;//pLV->row_width;
		rc.y =y;
		rc.h =row->Height;

		lst_col =&pLV->list_col_hdr;
		//n_col =lst_col->next;
		n_col =find_index_node(lst_col,i);
		while(n_col!=lst_col)
		{
			col =(lv_col_t*)list_entry(n_col,lv_col_t,node);

			rc.w =col->Width;

			if(PtInRect(&rc,&pt))
			{
				lv_cell_t *cell,*old_sel;

				cell =&row->cell_item[i];

				old_sel =pLV->sel_cell;

				pLV->sel_cell =cell;

				pLV->cur_sel_col =i;
				pLV->cur_sel_row =idx_row;

				if(cell != old_sel)
				{
					if(GetWindowLong(hwnd,GWL_STYLE)&LVS_NOTIFY)
					{
						NMLISTVIEW nm;

						nm.hdr.hwndFrom =hwnd;
						nm.hdr.idFrom =GetWindowLong(hwnd,GWL_ID);
						nm.hdr.code =0;

						nm.ColumnIdx =i;
						nm.RowIdx =idx_row;
						nm.lParam =cell->Data;

						NotifyParentEx(hwnd,LVN_SELCHANGE,(NMHDR*)&nm);

						//SendMessage(GetParent(hwnd),WM_NOTIFY,nm.hdr.idFrom,(LPARAM)&nm);
					}
				}
				return TRUE;

			}

			//下一个cell
			n_col =n_col->next;
			rc.x += rc.w;
			i++;

		}

		//下一个Row.
		n_row =n_row->next;
		y += row->Height;
		idx_row++;
	}
	return FALSE;
}

static BOOL  listview_lbutton_up(HWND hwnd,listview_t *pLV,int pos_x,int pos_y)
{
	NMLISTVIEW nm;

	if(pLV->sel_cell!=NULL)
	{
		nm.hdr.hwndFrom =hwnd;
		nm.hdr.idFrom =GetWindowLong(hwnd,GWL_ID);
		nm.hdr.code =0;

		nm.ColumnIdx =pLV->cur_sel_col;
		nm.RowIdx    =pLV->cur_sel_row;
		nm.lParam    =(LPARAM)pLV->sel_cell->Data;
		NotifyParentEx(hwnd,LVN_CLICKED,(NMHDR*)&nm);
	}
	return TRUE;
}

static void listview_paint(HDC hdc,HWND hwnd,listview_t *pLV)
{
	RECT rc;
	list_t *lst,*n;
	lv_col_t *col;
	lv_row_t *row;
	int y;
	////

	SetTextColor(hdc,MapRGB(hdc,0,0,0));
	SetBrushColor(hdc,MapRGB(hdc,220,220,220));
	SetPenColor(hdc,MapRGB(hdc,160,160,200));

	//// 绘制列标签
	rc.x =0;
	rc.y =0;
	rc.h =pLV->col_height;

	lst =&pLV->list_col_hdr;
	n =find_index_node(lst,pLV->first_col_idx);
	while(n!=lst)
	{
		col =(lv_col_t*)list_entry(n,lv_col_t,node);
		rc.w =col->Width;

		//Fill3DRect(hdc,&rc,MapRGB(hdc,200,200,200),MapRGB(hdc,130,130,130));
		GradientFillRect(hdc,&rc,MapRGB(hdc,210,210,210),MapRGB(hdc,170,170,170),TRUE);
		DrawText(hdc,col->Text,-1,&rc,DT_SINGLELINE|DT_VCENTER|DT_CENTER|DT_BORDER);
		OffsetRect(&rc,rc.w-1,0);

		n =n->next;
	}

	//// 绘制列表行
	y =pLV->col_height-1;

	lst =&pLV->list_row_hdr;
	n =find_index_node(lst,pLV->first_row_idx);
	while(n!=lst)
	{
		row =(lv_row_t*)list_entry(n,lv_row_t,node);

		//绘制一行所有的成员
		if(1)
		{
			int i;
			U32 dt;
			list_t *lst,*n;
			lv_col_t *col;
			lv_cell_t *cell;

			rc.x =0;;
			rc.y =y;
			rc.h =row->Height;

			i =pLV->first_col_idx;
			lst =&pLV->list_col_hdr;
			n =find_index_node(lst,i);
			while(n!=lst)
			{
				col =(lv_col_t*)list_entry(n,lv_col_t,node);

				rc.w =col->Width;

				cell =&row->cell_item[i];

				SetTextColor(hdc,MapXRGB8888(hdc,cell->TextColor));
				SetBrushColor(hdc,MapXRGB8888(hdc,cell->BackColor));
				SetPenColor(hdc,MapRGB(hdc,160,160,200));

				dt =DT_SINGLELINE|DT_BKGND|DT_BORDER;

				switch(cell->Format&LVF_ALIGN_V_MASK)
				{
					case LVF_BOTTOM:
						dt |= DT_BOTTOM;
						break;

					case LVF_VCENTER:
						dt |= DT_VCENTER;
						break;

					case LVF_TOP:
						dt |= DT_TOP;
						break;
				}

				switch(cell->Format&LVF_ALIGN_H_MASK)
				{
					case LVF_LEFT:
						dt |= DT_LEFT;
						break;

					case LVF_CENTER:
						dt |= DT_CENTER;
						break;

					case LVF_RIGHT:
						dt |= DT_RIGHT;
						break;
				}

				if(cell->Format&LVF_MULTILINE)
				{
					dt &= ~DT_SINGLELINE;
				}

				if(cell->pszText!=NULL)
				{
					DrawText(hdc,cell->pszText,-1,&rc,dt);
				}
				else
				{
					DrawText(hdc,L" ",0,&rc,dt);
				}

				if(cell==pLV->sel_cell)
				{
					RECT rc0;
					CopyRect(&rc0,&rc);
					InflateRect(&rc0,-1,-1);
					SetPenColor(hdc,MapRGB888(hdc,~pLV->sel_cell->BackColor));
					DrawRect(hdc,&rc0);
				}

				OffsetRect(&rc,rc.w-1,0);

				//Next row item;
				n =n->next;
				i++;

			}

		}

		//下一行
		y += row->Height-1;
		n =n->next;

	}



}
/*==================================================================*/

static	LRESULT	listview_proc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	listview_t *pLV;

	pLV =(listview_t*)GetWindowLong(hwnd,GWL_EXDATA_ADDR);

	switch(msg)
	{
		case WM_CREATE:
			listview_reset(pLV);
			return TRUE;
			////

		case LVM_INSERTCOLUMN:
			return listview_add_col(pLV,(U16)wParam,(LVCOLUM*)lParam);
			////

		case LVM_INSERTROW:
			return listview_add_row(pLV,(U16)wParam,(LVROW*)lParam);
			////

		case LVM_SETCELL:
			return listview_set_cell(pLV,(U32)wParam,(LVCELL*)lParam);
			////

		case LVM_GETCELL:
			return listview_get_cell(pLV,(U32)wParam,(LVCELL*)lParam);
			////

		case LVM_DELETEROW:
			listview_delete_row(pLV,LOWORD(wParam));
			if(lParam)
			{
				InvalidateRect(hwnd,NULL,FALSE);
			}
			return TRUE;
			////

		case LVM_DELETEALLROWS:
			listview_delete_all_rows(pLV);
			return TRUE;
			////

		case LVM_SETFIRSTCOLUMN:
			listview_set_first_col_idx(pLV,LOWORD(wParam));
			if(lParam)
			{
				InvalidateRect(hwnd,NULL,FALSE);
			}
			return TRUE;
			////

		case LVM_SETFIRSTROW:
			listview_set_first_row_idx(pLV,LOWORD(wParam));
			if(lParam)
			{
				InvalidateRect(hwnd,NULL,FALSE);
			}
			return TRUE;
			////

		case WM_LBUTTONDOWN:
			if(listview_lbutton_down(hwnd,pLV,LOWORD(lParam),HIWORD(lParam)))
			{
				InvalidateRect(hwnd,NULL,FALSE);
			}
			break;
			////
		case WM_LBUTTONUP:
			if(listview_lbutton_up(hwnd,pLV,LOWORD(lParam),HIWORD(lParam)))
			{
				InvalidateRect(hwnd,NULL,FALSE);
			}
			break;
			////

		case WM_PAINT:
			{
				HDC hdc;
				PAINTSTRUCT ps;

				hdc =BeginPaint(hwnd,&ps);
				if(hdc!=NULL)
				{
					listview_paint(hdc,hwnd,pLV);
				}
				EndPaint(hwnd,&ps);
			}
			break;
			////

		case WM_DESTROY:
			listview_destroy(pLV);
			return DefWindowProc(hwnd,msg,wParam,lParam);
			////

		default:
			return DefWindowProc(hwnd,msg,wParam,lParam);
	}

	return 0;
}

const WNDCLASS wcexLISTVIEW={

	WNDCLASS_TAG,
	CS_HREDRAW | CS_VREDRAW,
	listview_proc,
	0,
	sizeof(listview_t), //cbWndExtra
	NULL,
	NULL,
	NULL,
	NULL

};

/*==================================================================*/
