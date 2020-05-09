
#ifndef	__CLISTMENU_20190222_1920__
#define	__CLISTMENU_20190222_1920__

/*============================================================================*/

#include <emXGUI.h>

/*============================================================================*/
////ListMenu 消息
enum	eMSG_LISTMENU
{

	MSG_LISTMENU_BEGIN =WM_WIDGET+0,

	MSG_MOVE_PREV,            // [向前移动]: <wParam>TRUE:移动后自动重绘;FALSE：移动后不重绘; <lParam>忽略; <返回>忽略.
	MSG_MOVE_NEXT,            // [向后移动]: <wParam>TRUE:移动后自动重绘;FALSE：移动后不重绘; <lParam>忽略; <返回>忽略.
	MSG_SET_SEL,              // [设置选中的对象]: <wParam>设置被选中的索引值; <lParam>忽略; <返回>忽略.
	MSG_LIST_DRAG_ENABLE,     // [列表滑动使能]: <wParam>TRUE:使能;FALSE：禁止; <lParam>忽略; <返回>忽略.
	MSG_LISTMENU_END,

};


////ListMenu 私有风格标志(只能用低16位)
#define	LMS_HOR			(1<<0) //水平滑动的.
#define	LMS_VER			(1<<1) //垂直滑动的.
#define	LMS_ICONFRAME	(1<<2) //图标带框.
#define	LMS_PAGEMOVE	(1<<3) //按整页移动.

//#define	LMS_ICONTITLE	(1<<4) //图标带标题

////ListMenu 通知码
#define	LMN_CLICKED   0 //单击.
#define	LMN_SELCHANGE 1 //选项被改变.

typedef struct {
	NMHDR hdr;
	int idx;
}LM_NMHDR;


struct	__obj_list{

	const WCHAR *pName;
	const void **bmp;
	void (*cbStartup)(HWND hwnd);
};

typedef struct
{

	const struct __obj_list *list_objs;
	int x_num;
	int y_num;

}list_menu_cfg_t;



extern const WNDCLASS wcex_ListMenu;

/*============================================================================*/
/*============================================================================*/
/*============================================================================*/
/*============================================================================*/

#endif /*__CLISTMENU__*/
