
#ifndef	__CLISTMENU_20190222_1920__
#define	__CLISTMENU_20190222_1920__

/*============================================================================*/

#include <emXGUI.h>

/*============================================================================*/
////ListMenu ��Ϣ
enum	eMSG_LISTMENU
{

	MSG_LISTMENU_BEGIN =WM_WIDGET+0,

	MSG_MOVE_PREV,            // [��ǰ�ƶ�]: <wParam>TRUE:�ƶ����Զ��ػ�;FALSE���ƶ����ػ�; <lParam>����; <����>����.
	MSG_MOVE_NEXT,            // [����ƶ�]: <wParam>TRUE:�ƶ����Զ��ػ�;FALSE���ƶ����ػ�; <lParam>����; <����>����.
	MSG_SET_SEL,              // [����ѡ�еĶ���]: <wParam>���ñ�ѡ�е�����ֵ; <lParam>����; <����>����.
	MSG_LIST_DRAG_ENABLE,     // [�б���ʹ��]: <wParam>TRUE:ʹ��;FALSE����ֹ; <lParam>����; <����>����.
	MSG_LISTMENU_END,

};


////ListMenu ˽�з���־(ֻ���õ�16λ)
#define	LMS_HOR			(1<<0) //ˮƽ������.
#define	LMS_VER			(1<<1) //��ֱ������.
#define	LMS_ICONFRAME	(1<<2) //ͼ�����.
#define	LMS_PAGEMOVE	(1<<3) //����ҳ�ƶ�.

//#define	LMS_ICONTITLE	(1<<4) //ͼ�������

////ListMenu ֪ͨ��
#define	LMN_CLICKED   0 //����.
#define	LMN_SELCHANGE 1 //ѡ��ı�.

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
