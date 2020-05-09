#ifndef		__LISTVIEW_H___
#define		__LISTVIEW_H___

#ifdef	__cplusplus
extern "C"{
#endif

/*=========================================================================================*/
#if 0
typedef struct _LVCOLUMN {

	UINT mask;          //说明此结构中哪些成员是有效的
	int fmt;            //列的对齐方式
	int cx;             //列的初始宽度
	WCHAR *pszText; 	//列的标题
	int cchTextMax;   	//pszText所指向的缓冲区的大小
	int iSubItem;      	//与列关联的子项的索引值，从0开始
	int iImage;         //与列关联的图像列表中指定图像的索引值
	int iOrder;         //第几列，0代表最左一列

} LVCOLUMN;

typedef struct _LVITEM {
	UINT   mask;        //说明LVITEM结构中哪些成员有效
	int    iItem;       //项目的索引值(可以视为行号)从0开始
	int    iSubItem;    //子项的索引值(可以视为列号)从0开始
	UINT   state;       //子项的状态
	UINT   stateMask;   //状态有效的屏蔽位
	WCHAR *pszText;   	//主项或子项的名称
	int    cchTextMax; 	//pszText所指向的缓冲区大小
	int    iImage;      //关联图像列表中指定图像的索引值
	LPARAM lParam;    	//程序定义的32位参数
	int iIndent;        //表示图像位置缩进的单位
} LVITEM;
#endif

typedef	struct	__LVCOLUM
{
	WCHAR *pszText;
	U32 Width;
}LVCOLUM;

typedef	struct	__LVROW
{
	WCHAR *pszText;
	U32 Height;
}LVROW;

typedef	struct	tagNMLISTVIEW
{
	NMHDR hdr;
	U16 ColumnIdx;
	U16 RowIdx;
	LPARAM lParam;
}NMLISTVIEW;

//ListView Style(16Bit)
#define	LVS_NOTIFY			(1<<0)


//LVITEM Mask
#define	LVCF_FORMAT			(1<<0)
#define	LVCF_TEXT			(1<<1)
#define	LVCF_TEXTCOLOR		(1<<2)
#define	LVCF_BACKCOLOR		(1<<3)
#define	LVCF_BORDERCOLOR	(1<<4)
#define	LVCF_DATA			(1<<5)
#define	LVCF_ALL			(LVCF_FORMAT|LVCF_TEXT|LVCF_TEXTCOLOR|LVCF_BACKCOLOR|LVCF_BORDERCOLOR|LVCF_DATA)

//LVITEM Format
#define	LVF_TEXT			(1<<0)	//显示文字
#define	LVF_IMAGE			(1<<1)	//显示图像

#define	LVF_ALIGN_V_MASK	(3<<2)
#define	LVF_BOTTOM			(0<<2)	//垂直底部对齐
#define	LVF_VCENTER			(1<<2)	//垂直居中对齐
#define	LVF_TOP				(2<<2)	//垂直顶部对齐

#define	LVF_ALIGN_H_MASK	(3<<4)
#define	LVF_LEFT			(0<<4)	//水平居左对齐
#define	LVF_CENTER			(1<<4)	//水平居中对齐
#define	LVF_RIGHT			(2<<4)	//水平居右对齐

#define	LVF_MULTILINE		(1<<6)	//多行文字模式

#define	LVF_DEFAULT			(LVF_TEXT|LVF_VCENTER|LVF_CENTER)


typedef	struct	__LVCELL
{
	U16 Mask;
	U16 Format;

	S32   MaxTextLen;
	WCHAR *pszText;
	COLOR_RGB32 TextColor,BackColor,BorderColor;
	LONG Data;
}LVCELL;

#define	MAKE_LVCELL_IDX(row,col) ((row<<16)|col)

/*=========================================================================================*/

#define	LVN_CLICKED		0x00
#define	LVN_SELCHANGE	0x82

#define	LVM_FIRST	0x7000

#if 0
#define	LVM_INSERTCOLUMN  	(LVM_FIRST+0)	//加入列，wParam 为整型，指定列号，lParam 为指向LV_COLUMN结构的指针
#define	LVM_SETCOLUMN  		(LVM_FIRST+1)	//设置列，参数同上
#define	LVM_INSERTITEM  	(LVM_FIRST+2)	//加入项目或子项目，wParam 为0，lParam 为指向LV_ITEM结构的指针
#define	LVM_SETITEM  		(LVM_FIRST+3)	//设置项目或子项目，参数同上
#define	LVM_GETITEM  		(LVM_FIRST+4)	//取得项目或子项目，参数同上
#define	LVM_GETNEXTITEM  	(LVM_FIRST+5)	//取得下一个项目或子项目，可以用来取得光标选择的项目
#define	LVM_DELETEITEM  	(LVM_FIRST+6)	//删除项目或子项目，wParam 为整型，指定项目索引号，lParam 为0
#define	LVM_DELETEALLITEMS  (LVM_FIRST+7)	//删除所有项目，wParam 和 lParam 均为0
#define	LVM_SETTEXTCOLOR  	(LVM_FIRST+8)	//设置文字颜色，wParam 为0，lParam 为颜色的RGB值
#define	LVM_SETTEXTBKCOLOR  (LVM_FIRST+9)	//设置文字背景色，参数同上
#define	LVM_SETBKCOLOR  	(LVM_FIRST+10)	//设置背景色，参数同上
#endif

#define	LVM_INSERTCOLUMN  	(LVM_FIRST+0)	//加入列，wParam 为整型，指定列号，lParam 为指向LV_COLUMN结构的指针
#define	LVM_INSERTROW  		(LVM_FIRST+1)	//加入行。
#define	LVM_SETCELL			(LVM_FIRST+2)	//设置一个单元
#define	LVM_GETCELL			(LVM_FIRST+3)	//获得一个单元
#define	LVM_DELETEROW		(LVM_FIRST+4)	//删除一行:   wParam:行索引号;lParam:TRUE:重绘;FALSE:非重绘.
#define	LVM_DELETEALLROWS	(LVM_FIRST+5)	//删除所有行: wParam:忽略;lParam:TRUE:重绘;FALSE:非重绘.
#define	LVM_SETFIRSTCOLUMN	(LVM_FIRST+6)   //设置第一个显示的列: wParam:列索引号;lParam:TRUE:重绘,FALSE:非重绘;
#define	LVM_SETFIRSTROW		(LVM_FIRST+7)   //设置第一个显示的行: wParam:行索引号;lParam:TRUE:重绘,FALSE:非重绘.

/*=========================================================================================*/

extern const WNDCLASS wcexLISTVIEW;

/*=========================================================================================*/

#ifdef	__cplusplus
}
#endif

#endif

