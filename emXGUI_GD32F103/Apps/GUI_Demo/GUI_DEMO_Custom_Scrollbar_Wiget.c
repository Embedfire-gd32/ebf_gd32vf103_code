
/*
 *GUI_DEMO_CustomWidget.c
 *2018年9月7日上午10:53:01
 *
 */

#include "emXGUI.h"
#include "x_libc.h"


#ifndef	GUI_DEBUG
#define	GUI_DEBUG(x,...)
#endif

/*============================================================================*/
//定义控件内部ID
#define ID_OK		        0x1000
#define ID_PLUS         0x1001
#define ID_MINUS        0x1002
#define ID_SCROLLBAR1	  0x1003
#define ID_NUM_TEXT     0x1004      

/* 定义控件ID */
#define ID_SC_TEMP  0x1005
#define ID_SC_HUMI  0x1006
#define ID_SC_DWELL  0x1007
#define ID_SC_HEATING  0x1008
#define ID_SC_COOLING  0x1009

#define ID_TEMP_BUTTON        0x100A
#define ID_TEMP_INFO_BUTTON   0x100B
#define ID_HUMI_INFO_BUTTON   0x100C
#define ID_TEMP_HUMI_WIGET    0x100D

#define ID_DWELL_BUTTON        0x100E
#define ID_DWELL_INFO_BUTTON  0x100F
#define ID_DWELL_WIGET         0x1010

#define ID_HEATING_BUTTON        0x1011
#define ID_HEATING_INFO_BUTTON  0x1012
#define ID_COOLING_INFO_BUTTON  0x1013
#define ID_HEATING_WIGET         0x1014

#define ID_START_BUTTON          0x1015
/*============================================================================*/

/* 各种颜色 */
#define	BGCOLOR	RGB888(23,27,83)  //背景色(RGB888).
#define	TEMP_TEXT_COLOR	ARGB8888(220,215,61,50)  
#define	HUMI_TEXT_COLOR	ARGB8888(220,22,167,246)  
#define	TIME_TEXT_COLOR	ARGB8888(255,255,255,255)  


//定义控件的私有消息(所有控件的私有消息要从 WM_WIDGET 开始定义)

//wParam = id, lparam = scrollbar->value
#define MSG_MYSCROLLBAR_OK  WM_USER+2   //点击了OK按钮
//wParam = id
#define MSG_FUNCTION_SELECT  WM_USER+3   //点击了控件按钮

//定义控件的扩展数据结构.
typedef struct
{
  const WCHAR* text;  /* 字符指针 */
  U32 text_color;     /* 字符和三角形的颜色*/
}MY_SCROLLBAR_EXT;

//创建时使用的结构体
typedef struct tagMyScrollBarCreate
{
  SCROLLINFO sif;     /* 进度条数据结构 */
  const WCHAR* text;  /* 字符指针 */
  U32 text_color;     /* 字符和三角形的颜色*/

}MY_SCROLLBAR_CREATE;

/*============================================================================*/

/**
  * @brief  控件的回调函数
  * @param  ds DRAWITEM_HDR结构
  * @param  polygon_color 三角形的颜色 
  * @retval 无
  */
static void scrollbar_owner_draw(DRAWITEM_HDR *ds,COLORREF polygon_color)
{
  POINT pt[3];
	HWND hwnd;
	HDC hdc,hdc_mem;
//  HDC hdc_mem2;
	RECT rc;
//	WCHAR wbuf[128];
	SCROLLINFO sif;
	hwnd =ds->hwnd;
	hdc =ds->hDC;

  /* 是否使用透明绘制 */
  #if 1 
  hdc_mem =CreateMemoryDC(SURF_SCREEN,ds->rc.w,ds->rc.h);

   /* 绘制背景 ds->rc是窗口大小*/  
	SetBrushColor(hdc_mem,MapRGB888(hdc_mem,BGCOLOR));
	FillRect(hdc_mem,&ds->rc);
    
  sif.fMask =SIF_ALL;
	SendMessage(hwnd,SBM_GETSCROLLINFO,0,(LPARAM)&sif);      
  //得到滑块的大小，rc是滑块大小
	SendMessage(hwnd,SBM_GETTRACKRECT,0,(LPARAM)&rc);

  /* 要绘制的三角形的三个点 */
  pt[0].x = 0;
  pt[0].y = ds->rc.h-rc.w/2;
  pt[1].x = ds->rc.w-5;
  pt[1].y = pt[0].y;
  pt[2].x = pt[1].x;
  pt[2].y = 5;   
  
  /* 绘制三角形 */
  SetBrushColor(hdc_mem,MapARGB8888(hdc_mem, polygon_color));
  FillPolygon(hdc_mem,0,0,pt,3);
  
  /* 绘制底边 */
  SetPenColor(hdc_mem,MapRGB(hdc_mem,255,255,255));
  SetPenSize(hdc_mem,2);
  HLine(hdc_mem,rc.x,pt[0].y,pt[1].x);  
  
  /* 绘制底边 */
  SetPenColor(hdc_mem,MapRGB(hdc_mem,0,255,0));
  SetPenSize(hdc_mem,2);
  HLine(hdc_mem,pt[0].x,pt[0].y,rc.x);  

  /* 绘制底部的大圆 */
  SetBrushColor(hdc_mem,MapRGB(hdc_mem, 255,255,255));
  FillCircle(hdc_mem,rc.x+rc.w/2,pt[0].y,rc.w/2-2);
  
  /* 绘制顶层的小圆 */
  SetBrushColor(hdc_mem,MapRGB(hdc_mem, 255,0,0));
  FillCircle(hdc_mem,rc.x+rc.w/2,pt[0].y,rc.w/2-4);
   
  BitBlt(hdc,0,0,ds->rc.w,ds->rc.h,hdc_mem,0,0,SRCCOPY);
	
  DeleteDC(hdc_mem);
  
  #else
  
  hdc_mem =CreateMemoryDC(SURF_ARGB4444,ds->rc.w,ds->rc.h);
  hdc_mem2 =CreateMemoryDC(SURF_SCREEN,ds->rc.w,ds->rc.h);

   /* 绘制背景 */  
	SetBrushColor(hdc_mem,MapARGB(hdc_mem,255,23,27,83));
	FillRect(hdc_mem,&ds->rc);  
    
  sif.fMask =SIF_ALL;
	SendMessage(hwnd,SBM_GETSCROLLINFO,0,(LPARAM)&sif);      
  //得到滑块的大小
	SendMessage(hwnd,SBM_GETTRACKRECT,0,(LPARAM)&rc);

  /* 要绘制的三角形的三个点 */
  pt[0].x = 0;
  pt[0].y = ds->rc.h-rc.w/2;
  pt[1].x = ds->rc.w-5;
  pt[1].y = pt[0].y;
  pt[2].x = pt[1].x;
  pt[2].y = 5;   
  
  /* 绘制三角形 */
  SetBrushColor(hdc_mem,MapARGB4444(hdc_mem,polygon_color));
  AA_FillPolygon(hdc_mem,0,0,pt,3);
  
  /* 绘制底边 */
  SetPenColor(hdc_mem,MapARGB(hdc_mem,255,255,255,255));
  SetPenSize(hdc_mem,2);
  HLine(hdc_mem,rc.x,pt[0].y,pt[1].x);  
  
  /* 绘制底边 */
  SetPenColor(hdc_mem,MapARGB(hdc_mem,255,0,255,0));
  SetPenSize(hdc_mem,2);
  HLine(hdc_mem,pt[0].x,pt[0].y,rc.x);  

  /* 绘制底部的大圆 */
  SetBrushColor(hdc_mem,MapARGB(hdc_mem,255, 255,255,255));
  FillCircle(hdc_mem,rc.x+rc.w/2,pt[0].y,rc.w/2-2);
  
  /* 绘制顶层的小圆 */
  SetBrushColor(hdc_mem,MapARGB(hdc_mem,255, 255,0,0));
  FillCircle(hdc_mem,rc.x+rc.w/2,pt[0].y,rc.w/2-4);
  
  
	SetBrushColor(hdc_mem2,MapRGB888(hdc,BGCOLOR));
	FillRect(hdc_mem2,&ds->rc);
  
  BitBlt(hdc_mem2,0,0,ds->rc.w,ds->rc.h,hdc_mem,0,0,SRCCOPY);
  BitBlt(hdc,0,0,ds->rc.w,ds->rc.h,hdc_mem2,0,0,SRCCOPY);
	
  DeleteDC(hdc_mem);
  DeleteDC(hdc_mem2);

  #endif

}

/* 控件数字框的宽度 */
//#define NUM_TEXT_WIDTH  (rc.w/3)
//#define NUM_TEXT_WIDTH  (130)


//控件的窗口过程函数
static LRESULT	my_scrollbar_proc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	RECT rc;
  HWND wnd;
  WCHAR wbuf[40];

  //滚动条参数结构体
  MY_SCROLLBAR_CREATE *create_info;         
	MY_SCROLLBAR_EXT *scrollbar_ex;

	switch(msg)
	{
		case WM_CREATE: //窗口创建时,会自动产生该消息,在这里做一些初始化的操作或创建子窗口.
		{
      RECT rc0;
      HWND wnd;
      scrollbar_ex =(MY_SCROLLBAR_EXT*)GetWindowLong(hwnd,GWL_EXDATA_ADDR); //获得窗口扩展数据区。


      create_info = (MY_SCROLLBAR_CREATE *)lParam;
      scrollbar_ex->text = create_info->text;
      scrollbar_ex->text_color = create_info->text_color;
      
			GetClientRect(hwnd,&rc); //获得窗口的客户区矩形.
      GetClientRect(hwnd,&rc0); //获得窗口的客户区矩形.
        
      x_wsprintf(wbuf,L"%d %s",create_info->sif.nValue,scrollbar_ex->text);

      rc0.w = rc0.w/3-40;
      CreateWindow(TEXTBOX,wbuf,TBS_FLAT|TBS_CENTER|WS_VISIBLE,
                    rc0.x,rc0.y,rc0.w,rc0.h,hwnd,ID_NUM_TEXT,NULL,NULL); //创建一个文字框.

      OffsetRect(&rc0,rc0.w,0);
      rc0.h /= 2;
      rc0.w = 40;
      CreateWindow(BUTTON,L"+",BS_FLAT|WS_BORDER|WS_VISIBLE,
                   rc0.x,rc0.y,rc0.w,rc0.h,hwnd,ID_PLUS,NULL,NULL);
      
      OffsetRect(&rc0,0,rc0.h);
			CreateWindow(BUTTON,L"-",BS_FLAT|WS_BORDER|WS_VISIBLE,
                    rc0.x,rc0.y,rc0.w,rc0.h,hwnd,ID_MINUS,NULL,NULL);
      
      OffsetRect(&rc0,rc0.w+5,0);
      rc0.y = 5;
      rc0.w = rc.w - rc0.x - 45;      
      rc0.h = rc.h - 2*rc0.y;
     
      //创建滑动条
      wnd = CreateWindow(SCROLLBAR,L"Scrollbar1",WS_OWNERDRAW|WS_VISIBLE,
                            rc0.x,rc0.y,rc0.w,rc0.h,
                            hwnd,ID_SCROLLBAR1,NULL,NULL);
      //配置滑动条的属性
      SendMessage(wnd,SBM_SETSCROLLINFO,TRUE,(LPARAM)&create_info->sif); 

      OffsetRect(&rc0,rc0.w,rc0.h);
      rc0.w = 40; 
      rc0.h = rc.h; 
      rc0.x = rc.w - rc0.w;
      rc0.y = 0;

			CreateWindow(BUTTON,L"OK",BS_FLAT|WS_BORDER|WS_VISIBLE,
                    rc0.x,rc0.y,rc0.w,rc0.h,hwnd,ID_OK,NULL,NULL); //创建一个按钮(示例).


		}
		return TRUE;
		////

		case WM_NOTIFY: //WM_NOTIFY消息:wParam低16位为发送该消息的控件ID,高16位为通知码;lParam指向了一个NMHDR结构体.
		{
			u16 code,id;
		  int scrollbar_value;

			code =HIWORD(wParam); //获得通知码类型.
			id   =LOWORD(wParam); //获得产生该消息的控件ID.

			if(id == ID_OK && code==BN_CLICKED) // 按钮“单击”了.
			{
        wnd =GetDlgItem(hwnd,ID_SCROLLBAR1);
        scrollbar_value = SendMessage(wnd,SBM_GETVALUE,0,0);      

        wnd = GetParent(hwnd);
        //wParam = ID ,lParam=scrollbar_value
        //发送消息给父窗口
				PostMessage(wnd,
                      MSG_MYSCROLLBAR_OK,
                      GetWindowLong(hwnd,GWL_ID),
                      scrollbar_value);         
			}
      else if(id == ID_PLUS || id == ID_MINUS && code == BN_CLICKED)
			{
        /* 增加或减小进度条的值 */
        s32 range_min,range_max;
        wnd =GetDlgItem(hwnd,ID_SCROLLBAR1);

        SendMessage(wnd,SBM_GETRANGE,(WPARAM)&range_min,(LPARAM)&range_max);      
        scrollbar_value = SendMessage(wnd,SBM_GETVALUE,0,0);      
       
        if(id == ID_PLUS && scrollbar_value < range_max)
          scrollbar_value++;
        else if(id == ID_MINUS && scrollbar_value > range_min)
          scrollbar_value--;

        SendMessage(wnd,SBM_SETVALUE,TRUE,scrollbar_value); //设置位置值
       
        scrollbar_ex =(MY_SCROLLBAR_EXT*)GetWindowLong(hwnd,GWL_EXDATA_ADDR); //获得窗口扩展数据区。

        x_wsprintf(wbuf,L"%d %s",scrollbar_value,scrollbar_ex->text);
        wnd =GetDlgItem(hwnd,ID_NUM_TEXT);
        SetWindowText(wnd,wbuf);        
			}
      else if(id == ID_SCROLLBAR1)
      {
     
        NMHDR *nr;		
        NM_SCROLLBAR *sb_nr;

        nr =(NMHDR*)lParam; //lParam参数，是以NMHDR结构体开头.
				sb_nr =(NM_SCROLLBAR*)nr; //Scrollbar的通知消息实际为 NM_SCROLLBAR扩展结构,里面附带了更多的信息.
				switch(nr->code)
				{
					case SBN_CLICKED: //单击
					{
#if 0
						if(sb_nr->cmd==SB_THUMBTRACK) //NM_SCROLLBAR结构体成员cmd指明了单击发生的位置
						{
              //在滑块内单击.
//							GUI_DEBUG("SCROLLBAR CLICK In Track.\r\n");
						}
						else
						{
//							GUI_DEBUG("SCROLLBAR CLICK :%d.\r\n",sb_nr->cmd);
						}
#endif
					}
					break;
					case SBN_THUMBTRACK: //滑块移动
					{
						scrollbar_value =sb_nr->nTrackValue; //获得滑块当前位置值
						SendMessage(nr->hwndFrom,SBM_SETVALUE,TRUE,scrollbar_value); //设置位置值
            
            scrollbar_ex =(MY_SCROLLBAR_EXT*)GetWindowLong(hwnd,GWL_EXDATA_ADDR); //获得窗口扩展数据区。

            x_wsprintf(wbuf,L"%d %s",scrollbar_value,scrollbar_ex->text);
            wnd =GetDlgItem(hwnd,ID_NUM_TEXT);
            SetWindowText(wnd,wbuf);
					}
					break;
					default:
						break;
				}
			}     
		}
		break;
		////
    
   	case WM_CTLCOLOR:
		{
 
      u16 id;      
			id =LOWORD(wParam);

			if(id == ID_PLUS || id == ID_MINUS || id == ID_OK  )
      {
        CTLCOLOR *cr;	  
        cr =(CTLCOLOR*)lParam;	

        cr->TextColor =RGB888(255,255,255);
        cr->BackColor =BGCOLOR;
        cr->BorderColor =RGB888(0,0,255);
        return TRUE;

      }
      else if(id == ID_NUM_TEXT)
      {
        CTLCOLOR *cr;	  
        cr =(CTLCOLOR*)lParam;	
        scrollbar_ex =(MY_SCROLLBAR_EXT*)GetWindowLong(hwnd,GWL_EXDATA_ADDR); //获得窗口扩展数据区。

        cr->TextColor =scrollbar_ex->text_color;
        cr->BackColor =BGCOLOR;
        cr->BorderColor =RGB888(0,0,255);
        return TRUE;
      }
      else /* 其它控件使用默认颜色 */
        return FALSE;
    }
    
    case	WM_DRAWITEM:
		{
			DRAWITEM_HDR *ds;
			ds =(DRAWITEM_HDR*)lParam;
      if(ds->ID == ID_SCROLLBAR1)
			{
        /* 重绘进度条 */
        scrollbar_ex =(MY_SCROLLBAR_EXT*)GetWindowLong(hwnd,GWL_EXDATA_ADDR); //获得窗口扩展数据区。

				scrollbar_owner_draw(ds,scrollbar_ex->text_color);
				return TRUE;
			}
      else
        return FALSE;
		}			
    
    case	WM_ERASEBKGND:
		{
			HDC hdc=(HDC)wParam;
			RECT rc =*(RECT*)lParam;
			//GetClientRect(hwnd,&rc);
			SetBrushColor(hdc,MapRGB888(hdc,BGCOLOR));
			FillRect(hdc,&rc);
			return TRUE;
		}

		case WM_PAINT: //窗口需要绘制时，会自动产生该消息.
		{
        PAINTSTRUCT ps;
        HDC hdc;

        hdc =BeginPaint(hwnd,&ps); //开始绘图

        EndPaint(hwnd,&ps); //结束绘图
		}
		break;
		////

		case WM_CLOSE: //窗口关闭时，会自动产生该消息.
		{
			return DestroyWindow(hwnd); //调用DestroyWindow函数销毁窗口，该函数会使主窗口结束并退出消息循环;否则窗口将继续运行.
		}
//		break;
		////

		default: //用户不关心的消息,由系统处理.
		{
			return DefWindowProc(hwnd,msg,wParam,lParam);
		}

	}

	return WM_NULL;
}

//定义一个控件类结构实例.
const WNDCLASS my_scrollbar=
{
    .Tag         =WNDCLASS_TAG,
    .Style       = 0,
    .lpfnWndProc =my_scrollbar_proc,
    .cbClsExtra  =0,
    .cbWndExtra  =sizeof(MY_SCROLLBAR_EXT),
    .hInstance   =NULL,
    .hIcon       =NULL,
    .hCursor	 =NULL,
};

/*============================================================================*/


/**
  * @brief  窗口的回调函数
  * @param  无
  * @retval 无
  */
static LRESULT	my_temp_humi_wiget_proc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	RECT rc;
  static HWND wnd;
  //滚动条参数结构体
  MY_SCROLLBAR_CREATE create_info;    
  static U8 move_dir = 0;
  
	switch(msg)
	{
		case WM_CREATE: //窗口创建时,会自动产生该消息,在这里做一些初始化的操作或创建子窗口.
		{
      GetClientRect(hwnd,&rc); //获得窗口的客户区矩形.
      rc.h = 70;
      rc.w /=3;
      CreateWindow(BUTTON,L"温湿度调节",BS_FLAT|WS_BORDER|WS_VISIBLE|WS_CLIPSIBLINGS,
                    rc.x,rc.y,rc.w,rc.h,hwnd,ID_TEMP_BUTTON,NULL,NULL);

      rc.x = rc.w;
      CreateWindow(BUTTON,L"80℃\r\n温度",BS_FLAT|WS_BORDER|WS_VISIBLE|WS_CLIPSIBLINGS,
                    rc.x,rc.y,rc.w,rc.h,hwnd,ID_TEMP_INFO_BUTTON,NULL,NULL);
      
      rc.x += rc.w;
      CreateWindow(BUTTON,L"50%\r\n湿度",BS_FLAT|WS_BORDER|WS_VISIBLE|WS_CLIPSIBLINGS,
                    rc.x,rc.y,rc.w,rc.h,hwnd,ID_HUMI_INFO_BUTTON,NULL,NULL);

      
      create_info.text = L"℃\r\n温度";     
      create_info.text_color = TEMP_TEXT_COLOR;
      /* 设置滑动条的属性 */
      create_info.sif.cbSize		=sizeof(create_info.sif);//结构体大小
      create_info.sif.fMask		=SIF_ALL;    //使能所有功能
      //取值范围：0~90
      create_info.sif.nMin		=0;           
      create_info.sif.nMax		=90;
      //当前值为0
      create_info.sif.nValue		=80;
      //滑块的大小为30
      create_info.sif.TrackSize		=30;
      //两端的大小为20
      create_info.sif.ArrowSize		=0;

      GetClientRect(hwnd,&rc); //获得窗口的客户区矩形.
      rc.h = 70;
//      rc.x = rc.w/3;
//      rc.w = rc.w*2/3;
      
			CreateWindow(&my_scrollbar,L"MyScrollbar",WS_BORDER|WS_VISIBLE|WS_CLIPSIBLINGS,
                    rc.w,rc.y,rc.w,rc.h,hwnd,ID_SC_TEMP,NULL,&create_info);
      
      
      create_info.text = L"%\r\n湿度";     
      create_info.text_color = HUMI_TEXT_COLOR;
      //取值范围
      create_info.sif.nMin		=0;           
      create_info.sif.nMax		=100;
      create_info.sif.nValue		=50;

      CreateWindow(&my_scrollbar,L"MyScrollbar2",WS_BORDER|WS_VISIBLE|WS_CLIPSIBLINGS,
                    rc.w,rc.y,rc.w,rc.h,hwnd,ID_SC_HUMI,NULL,&create_info);

     
		}
		return TRUE;
		////
    
    case MSG_MYSCROLLBAR_OK: //MSG_MYSCROLLBAR_OK消息:wParam低16位为发送该消息的控件ID,高16位为通知码;lParam指向了一个NMHDR结构体.
		{
      u16 id;
      u32 scrollbar_value;
      MY_SCROLLBAR_EXT *scrollbar_ex;
      WCHAR wbuf[40];
      
			id = wParam; //获得ID.
			scrollbar_value = lParam; //获得产生该消息的控件ID.

      wnd = (GetDlgItem(hwnd,id));
//      GUI_DEBUG("id = 0x%x,scrollbar_value=%d",id,scrollbar_value);	
      scrollbar_ex =(MY_SCROLLBAR_EXT*)GetWindowLong(wnd,GWL_EXDATA_ADDR); //获得窗口扩展数据区。
      x_wsprintf(wbuf,L"%d %s",scrollbar_value,scrollbar_ex->text);

      if(id == ID_SC_TEMP)
        SetWindowText(GetDlgItem(hwnd,ID_TEMP_INFO_BUTTON),wbuf);
      else if(id == ID_SC_HUMI)
        SetWindowText(GetDlgItem(hwnd,ID_HUMI_INFO_BUTTON),wbuf);

      //向右滑动
      move_dir = 0;
      KillTimer(hwnd,1);
      SetTimer(hwnd,1,10,TMR_START,NULL);
      
		}
		break;
    
    case WM_CTLCOLOR:
		{ 
      u16 id;      
			id =LOWORD(wParam);

			if(id == ID_TEMP_BUTTON )
      {
        CTLCOLOR *cr;	  
        cr =(CTLCOLOR*)lParam;	

        cr->TextColor =RGB888(255,255,255);
        cr->BackColor =BGCOLOR;
        cr->BorderColor =RGB888(0,0,255);
        return TRUE;
      } 
      else if(id == ID_TEMP_INFO_BUTTON )
      {
        CTLCOLOR *cr;	  
        cr =(CTLCOLOR*)lParam;	

        cr->TextColor =TEMP_TEXT_COLOR;
        cr->BackColor =BGCOLOR;
        cr->BorderColor =RGB888(0,0,255);
        return TRUE;
      }
      else if(id == ID_HUMI_INFO_BUTTON)
      {
         CTLCOLOR *cr;	  
        cr =(CTLCOLOR*)lParam;	

        cr->TextColor =HUMI_TEXT_COLOR;
        cr->BackColor =BGCOLOR;
        cr->BorderColor =RGB888(0,0,255);
        return TRUE;
      }
      else /* 其它控件使用默认颜色 */
        return FALSE;
    }
    
		case WM_NOTIFY: //WM_NOTIFY消息:wParam低16位为发送该消息的控件ID,高16位为通知码;lParam指向了一个NMHDR结构体.
		{
			u16 code,id;

			code =HIWORD(wParam); //获得通知码类型.
			id   =LOWORD(wParam); //获得产生该消息的控件ID.

			if(id == ID_TEMP_BUTTON || 
          id == ID_TEMP_INFO_BUTTON||
          id == ID_HUMI_INFO_BUTTON && 
          code == BN_CLICKED) // 按钮“单击”了.
      {      
        if(id == ID_TEMP_BUTTON)
        {
          //向左滑动        
//          move_dir = 1;
//          wnd = GetDlgItem(hwnd,ID_SC_TEMP);
//          
//          
//        wnd = ;
        //wParam = ID ,lParam=scrollbar_value
        //发送消息给父窗口
				PostMessage(GetParent(hwnd),
                      MSG_FUNCTION_SELECT,
                      GetWindowLong(hwnd,GWL_ID),
                      NULL); 
          /* 跳出，不触发定时器 */
         break;
        }
        else if(id == ID_TEMP_INFO_BUTTON)
        {
          //向左滑动        
          move_dir = 1;  
          wnd = GetDlgItem(hwnd,ID_SC_TEMP);
        }
        else if(id == ID_HUMI_INFO_BUTTON)
        {
          //向左滑动        
          move_dir = 1;  
          wnd = GetDlgItem(hwnd,ID_SC_HUMI);
        }
        KillTimer(hwnd,1);
        SetTimer(hwnd,1,10,TMR_START,NULL);
      }      
		}
		break;
    
		////   
     case WM_TIMER:
      {
        /* 定时器消息，上拉或下拉窗口 */
        RECT rc0;
        /* 父窗口 */
        GetWindowRect(hwnd,&rc);
        /* 滚动条窗口 */
        GetWindowRect(wnd,&rc0);
        SetForegroundWindow(wnd);        

        if(move_dir == 0)
        {
//           ShowWindow(GetDlgItem(hwnd,ID_TEMP_INFO_BUTTON),SW_SHOW);
//           ShowWindow(GetDlgItem(hwnd,ID_HUMI_INFO_BUTTON),SW_SHOW);
//           EnableWindow(GetDlgItem(hwnd,ID_TEMP_INFO_BUTTON),ENABLE);
//           EnableWindow(GetDlgItem(hwnd,ID_HUMI_INFO_BUTTON),ENABLE);

          /* 向右滑动 */
          if(rc0.x < rc.w)
          {
            OffsetRect(&rc0,(rc0.w>>3),0);
            rc0.x =MIN(rc0.x,rc.w);
            MoveWindow(wnd,rc0.x,0,rc0.w,rc0.h,TRUE);

          }
          else
          {
            KillTimer(hwnd,1);
          }
        }
        else
        {  
           /* 向左滑动 */
          if(rc0.x > (rc.w - rc0.w))
          {
            OffsetRect(&rc0,-(rc0.w>>3),0);
            rc0.x =MAX(rc0.x,(rc.w - rc0.w));
            MoveWindow(wnd,rc0.x,0,rc0.w,rc0.h,TRUE);

          }
          else
          {
//             ShowWindow(GetDlgItem(hwnd,ID_TEMP_INFO_BUTTON),SW_HIDE);
//             ShowWindow(GetDlgItem(hwnd,ID_HUMI_INFO_BUTTON),SW_HIDE);
//             EnableWindow(GetDlgItem(hwnd,ID_TEMP_INFO_BUTTON),DISABLE);
//             EnableWindow(GetDlgItem(hwnd,ID_HUMI_INFO_BUTTON),DISABLE);

            KillTimer(hwnd,1); 
          }            
        }

      }
//			InvalidateRect(hwnd,&rc,FALSE);
			break; 
    
    case	WM_ERASEBKGND:
		{
			HDC hdc=(HDC)wParam;
			RECT rc =*(RECT*)lParam;

			SetBrushColor(hdc,MapRGB888(hdc,BGCOLOR));
			FillRect(hdc,&rc);
			return TRUE;
		}

		case WM_PAINT: //窗口需要绘制时，会自动产生该消息.
		{

        PAINTSTRUCT ps;
        HDC hdc;

        hdc =BeginPaint(hwnd,&ps); //开始绘图

        ////用户的绘制内容...
        //TextOut(hdc,10,10,L"Hello",-1);

        EndPaint(hwnd,&ps); //结束绘图
		}
		break;
		////

		case WM_CLOSE: //窗口关闭时，会自动产生该消息.
		{

			return DestroyWindow(hwnd); //调用DestroyWindow函数销毁窗口，该函数会使主窗口结束并退出消息循环;否则窗口将继续运行.
		}
//		break;
		////

		default: //用户不关心的消息,由系统处理.
		{
			return DefWindowProc(hwnd,msg,wParam,lParam);
		}

	}

	return WM_NULL;
}

//定义一个控件类结构实例.
const WNDCLASS my_temp_humi_wiget=
{
    .Tag         =WNDCLASS_TAG,
    .Style       = 0,
    .lpfnWndProc =my_temp_humi_wiget_proc,
    .cbClsExtra  =0,
    .cbWndExtra  =0,
    .hInstance   =NULL,
    .hIcon       =NULL,
    .hCursor	 =NULL,
};



/**
  * @brief  窗口的回调函数
  * @param  无
  * @retval 无
  */
static LRESULT	my_dwell_time_wiget_proc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	RECT rc;
  static HWND wnd;
  //滚动条参数结构体
  MY_SCROLLBAR_CREATE create_info;    
  static U8 move_dir = 0;
  
	switch(msg)
	{
		case WM_CREATE: //窗口创建时,会自动产生该消息,在这里做一些初始化的操作或创建子窗口.
		{
      GetClientRect(hwnd,&rc); //获得窗口的客户区矩形.
      rc.h = 70;
      rc.w /=3;
      CreateWindow(BUTTON,L"保持时间",BS_FLAT|WS_BORDER|WS_VISIBLE|WS_CLIPSIBLINGS,
                    rc.x,rc.y,rc.w,rc.h,hwnd,ID_DWELL_BUTTON,NULL,NULL);

      rc.x = rc.w;
      rc.w *=2;
      CreateWindow(BUTTON,L"240Min\r\n保持时间",BS_FLAT|WS_BORDER|WS_VISIBLE|WS_CLIPSIBLINGS,
                    rc.x,rc.y,rc.w,rc.h,hwnd,ID_DWELL_INFO_BUTTON,NULL,NULL);
      
         
//      rc.x = rc.w/3;
//      rc.w = rc.w*2/3;
      create_info.text = L"Min\r\n保持时间";     
      create_info.text_color = TIME_TEXT_COLOR;

      /* 设置滑动条的属性 */
      create_info.sif.cbSize		=sizeof(create_info.sif);//结构体大小
      create_info.sif.fMask		=SIF_ALL;    //使能所有功能
      //取值范围
      create_info.sif.nMin		=0;           
      create_info.sif.nMax		=240;
      //当前值为0
      create_info.sif.nValue		=240;
      //滑块的大小为30
      create_info.sif.TrackSize		=30;
      //两端的大小为20
      create_info.sif.ArrowSize		=0;
			     
      GetClientRect(hwnd,&rc); //获得窗口的客户区矩形.
      rc.h = 70;

      CreateWindow(&my_scrollbar,L"MyScrollbar3",WS_BORDER|WS_VISIBLE|WS_CLIPSIBLINGS,
                    rc.w,rc.y,rc.w,rc.h,hwnd,ID_SC_DWELL,NULL,&create_info);
     
		}
		return TRUE;
		////
    
    case MSG_MYSCROLLBAR_OK: //MSG_MYSCROLLBAR_OK消息:wParam低16位为发送该消息的控件ID,高16位为通知码;lParam指向了一个NMHDR结构体.
		{
      u16 id;
      u32 scrollbar_value;
      MY_SCROLLBAR_EXT *scrollbar_ex;
      WCHAR wbuf[40];

			id = wParam; //获得ID.
			scrollbar_value = lParam; //获得产生该消息的控件ID.

      wnd = (GetDlgItem(hwnd,id));
//      GUI_DEBUG("id = 0x%x,scrollbar_value=%d",id,scrollbar_value);	
      scrollbar_ex =(MY_SCROLLBAR_EXT*)GetWindowLong(wnd,GWL_EXDATA_ADDR); //获得窗口扩展数据区。
      x_wsprintf(wbuf,L"%d %s",scrollbar_value,scrollbar_ex->text);

      if(id == ID_SC_DWELL)
        SetWindowText(GetDlgItem(hwnd,ID_DWELL_INFO_BUTTON),wbuf);

      //向右滑动
      move_dir = 0;
      KillTimer(hwnd,1);
      SetTimer(hwnd,1,10,TMR_START,NULL);
      
		}
		break;
    
    case WM_CTLCOLOR:
		{ 
      u16 id;      
			id =LOWORD(wParam);

			if( id == ID_DWELL_BUTTON || 
        id == ID_DWELL_INFO_BUTTON)
      {
        CTLCOLOR *cr;	  
        cr =(CTLCOLOR*)lParam;	

        cr->TextColor =RGB888(255,255,255);
        cr->BackColor =BGCOLOR;
        cr->BorderColor =RGB888(0,0,255);
        return TRUE;
      } 
      else /* 其它控件使用默认颜色 */
        return FALSE;
    }
    
		case WM_NOTIFY: //WM_NOTIFY消息:wParam低16位为发送该消息的控件ID,高16位为通知码;lParam指向了一个NMHDR结构体.
		{
			u16 code,id;

			code =HIWORD(wParam); //获得通知码类型.
			id   =LOWORD(wParam); //获得产生该消息的控件ID.

			if(id == ID_DWELL_BUTTON || 
          id == ID_DWELL_INFO_BUTTON && 
          code == BN_CLICKED) // 按钮“单击”了.
      {      
        if(id == ID_DWELL_BUTTON)
        {
          //向左滑动        
//          move_dir = 1;
//          wnd = GetDlgItem(hwnd,ID_SC_DWELL);
          
          PostMessage(GetParent(hwnd),
                      MSG_FUNCTION_SELECT,
                      GetWindowLong(hwnd,GWL_ID),
                      NULL); 
          /* 跳出，不触发定时器 */
         break;          
        }
        else if(id == ID_DWELL_INFO_BUTTON)
        {
          //向左滑动        
          move_dir = 1;  
          wnd = GetDlgItem(hwnd,ID_SC_DWELL);
        }     
        KillTimer(hwnd,1);
        SetTimer(hwnd,1,10,TMR_START,NULL);
      }      
		}
		break;
    
		////   
     case WM_TIMER:
      {
        /* 定时器消息，上拉或下拉窗口 */
        RECT rc0;
        /* 父窗口 */
        GetClientRect(hwnd,&rc);
        /* 滚动条窗口 */
        GetWindowRect(wnd,&rc0);
        SetForegroundWindow(wnd);        

        if(move_dir == 0)
        {
//           ShowWindow(GetDlgItem(hwnd,ID_TEMP_INFO_BUTTON),SW_SHOW);
//           ShowWindow(GetDlgItem(hwnd,ID_HUMI_INFO_BUTTON),SW_SHOW);
//           EnableWindow(GetDlgItem(hwnd,ID_TEMP_INFO_BUTTON),ENABLE);
//           EnableWindow(GetDlgItem(hwnd,ID_HUMI_INFO_BUTTON),ENABLE);

          /* 向右滑动 */
          if(rc0.x < GUI_XSIZE)
          {
            OffsetRect(&rc0,(rc0.w>>3),0);
            rc0.x =MIN(rc0.x,GUI_XSIZE);
            MoveWindow(wnd,rc0.x,0,rc0.w,rc0.h,TRUE);
          }
          else
          {
            KillTimer(hwnd,1);
          }
        }
        else
        {  
           /* 向左滑动 */
          if(rc0.x > rc.x)
          {
            OffsetRect(&rc0,-(rc0.w>>3),0);
            rc0.x =MAX(rc0.x,rc.x);
            MoveWindow(wnd,rc0.x,0,rc0.w,rc0.h,TRUE);
          }
          else
          {
//             ShowWindow(GetDlgItem(hwnd,ID_TEMP_INFO_BUTTON),SW_HIDE);
//             ShowWindow(GetDlgItem(hwnd,ID_HUMI_INFO_BUTTON),SW_HIDE);
//             EnableWindow(GetDlgItem(hwnd,ID_TEMP_INFO_BUTTON),DISABLE);
//             EnableWindow(GetDlgItem(hwnd,ID_HUMI_INFO_BUTTON),DISABLE);

            KillTimer(hwnd,1); 
          }            
        }

      }
//			InvalidateRect(hwnd,&rc,FALSE);
			break; 

    case	WM_ERASEBKGND:
		{
			HDC hdc =(HDC)wParam;
			RECT rc =*(RECT*)lParam;

			//GetClientRect(hwnd,&rc);
			SetBrushColor(hdc,MapRGB888(hdc,BGCOLOR));
			FillRect(hdc,&rc);
			return TRUE;
		}

		case WM_PAINT: //窗口需要绘制时，会自动产生该消息.
		{

        PAINTSTRUCT ps;
        HDC hdc;

        hdc =BeginPaint(hwnd,&ps); //开始绘图

        ////用户的绘制内容...
        //TextOut(hdc,10,10,L"Hello",-1);

        EndPaint(hwnd,&ps); //结束绘图
		}
		break;
		////

		case WM_CLOSE: //窗口关闭时，会自动产生该消息.
		{

			return DestroyWindow(hwnd); //调用DestroyWindow函数销毁窗口，该函数会使主窗口结束并退出消息循环;否则窗口将继续运行.
		}
//		break;
		////

		default: //用户不关心的消息,由系统处理.
		{
			return DefWindowProc(hwnd,msg,wParam,lParam);
		}

	}

	return WM_NULL;
}

//定义一个控件类结构实例.
const WNDCLASS my_dwell_time_wiget=
{
    .Tag         =WNDCLASS_TAG,
    .Style       = CS_HREDRAW | CS_VREDRAW,
    .lpfnWndProc =my_dwell_time_wiget_proc,
    .cbClsExtra  =0,
    .cbWndExtra  =0,
    .hInstance   =NULL,
    .hIcon       =NULL,
    .hCursor	 =NULL,
};



/**
  * @brief  窗口的回调函数
  * @param  无
  * @retval 无
  */
static LRESULT	my_heating_time_wiget_proc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	RECT rc;
  static HWND wnd;
  //滚动条参数结构体
  MY_SCROLLBAR_CREATE create_info;    
  static U8 move_dir = 0;
  static SURFACE *pSurfTop=NULL;
  static HDC hdcTop=NULL;
  
	switch(msg)
	{
		case WM_CREATE: //窗口创建时,会自动产生该消息,在这里做一些初始化的操作或创建子窗口.
		{
      GetClientRect(hwnd,&rc); //获得窗口的客户区矩形.

      pSurfTop =CreateSurface(SURF_SCREEN,rc.w,rc.h,NULL,0);
			hdcTop =CreateDC(pSurfTop,NULL);
			ClrDisplay(hdcTop,NULL,MapRGB888(hdcTop,BGCOLOR));
      
      GetClientRect(hwnd,&rc); //获得窗口的客户区矩形.

      rc.h = 70;
      rc.w /=3;
      CreateWindow(BUTTON,L"加热冷却时间",BS_FLAT|WS_BORDER|WS_VISIBLE|WS_CLIPSIBLINGS,
                    rc.x,rc.y,rc.w,rc.h,hwnd,ID_HEATING_BUTTON,NULL,NULL);

      rc.x = rc.w;
      CreateWindow(BUTTON,L"30\r\n加热时间",BS_FLAT|WS_BORDER|WS_VISIBLE|WS_CLIPSIBLINGS,
                    rc.x,rc.y,rc.w,rc.h,hwnd,ID_HEATING_INFO_BUTTON,NULL,NULL);
      
      rc.x += rc.w;
      CreateWindow(BUTTON,L"30\r\n冷却时间",BS_FLAT|WS_BORDER|WS_VISIBLE|WS_CLIPSIBLINGS,
                    rc.x,rc.y,rc.w,rc.h,hwnd,ID_COOLING_INFO_BUTTON,NULL,NULL);

      
      create_info.text = L"min\r\n加热时间";     
      create_info.text_color = TIME_TEXT_COLOR;

      /* 设置滑动条的属性 */
      create_info.sif.cbSize		=sizeof(create_info.sif);//结构体大小
      create_info.sif.fMask		=SIF_ALL;    //使能所有功能
      //取值范围
      create_info.sif.nMin		=0;           
      create_info.sif.nMax		=60;

      //当前值为0
      create_info.sif.nValue		=30;
      //滑块的大小为30
      create_info.sif.TrackSize		=30;
      //两端的大小为20
      create_info.sif.ArrowSize		=0;

      GetClientRect(hwnd,&rc); //获得窗口的客户区矩形.
      rc.h = 70;
//      rc.x = rc.w/3;
//      rc.w = rc.w*2/3;
      
			CreateWindow(&my_scrollbar,L"MyScrollbar4",WS_BORDER|WS_VISIBLE|WS_CLIPSIBLINGS,
                    rc.w,rc.y,rc.w,rc.h,hwnd,ID_SC_HEATING,NULL,&create_info);
           
      create_info.text = L"min\r\n冷却时间";     
      create_info.text_color = TIME_TEXT_COLOR;
      //取值范围
      create_info.sif.nMin		=0;           
      create_info.sif.nMax		=60;

      CreateWindow(&my_scrollbar,L"MyScrollbar5",WS_BORDER|WS_VISIBLE|WS_CLIPSIBLINGS,
                    rc.w,rc.y,rc.w,rc.h,hwnd,ID_SC_COOLING,NULL,&create_info);
     
		}
		return TRUE;
		////
    
    case MSG_MYSCROLLBAR_OK: //MSG_MYSCROLLBAR_OK消息:wParam低16位为发送该消息的控件ID,高16位为通知码;lParam指向了一个NMHDR结构体.
		{
      u16 id;
      u32 scrollbar_value;
      MY_SCROLLBAR_EXT *scrollbar_ex;
      WCHAR wbuf[40];

			id = wParam; //获得ID.
			scrollbar_value = lParam; //获得产生该消息的控件ID.

      wnd = (GetDlgItem(hwnd,id));
//      GUI_DEBUG("id = 0x%x,scrollbar_value=%d",id,scrollbar_value);	
      scrollbar_ex =(MY_SCROLLBAR_EXT*)GetWindowLong(wnd,GWL_EXDATA_ADDR); //获得窗口扩展数据区。
      x_wsprintf(wbuf,L"%d %s",scrollbar_value,scrollbar_ex->text);

      if(id == ID_SC_HEATING)
        SetWindowText(GetDlgItem(hwnd,ID_HEATING_INFO_BUTTON),wbuf);
      else if(id == ID_SC_COOLING)
        SetWindowText(GetDlgItem(hwnd,ID_COOLING_INFO_BUTTON),wbuf);

      //向右滑动
      move_dir = 0;
      KillTimer(hwnd,1);
      SetTimer(hwnd,1,10,TMR_START,NULL);
      
		}
		break;
    
    case WM_CTLCOLOR:
		{ 
      u16 id;      
			id =LOWORD(wParam);

			if(id == ID_HEATING_BUTTON || id == ID_HEATING_INFO_BUTTON || 
          id == ID_COOLING_INFO_BUTTON)
      {
        CTLCOLOR *cr;	  
        cr =(CTLCOLOR*)lParam;	

        cr->TextColor =RGB888(255,255,255);
        cr->BackColor =BGCOLOR;
        cr->BorderColor =RGB888(0,0,255);
        return TRUE;
      } 
      else /* 其它控件使用默认颜色 */
        return FALSE;
    }
    
    case	WM_DRAWITEM:
    {
      /*　当控件指定了WS_OWNERDRAW风格，则每次在绘制前都会给父窗口发送WM_DRAWITEM消息。
       *  wParam参数指明了发送该消息的控件ID;lParam参数指向一个DRAWITEM_HDR的结构体指针，
       *  该指针成员包含了一些控件绘制相关的参数.
         */

      DRAWITEM_HDR *ds;

      ds =(DRAWITEM_HDR*)lParam;

      HDC hdc;
      RECT rc;

      rc =ds->rc;

      //把控件的客户区坐标转换成主窗口的客户区坐标。
      ClientToScreen(ds->hwnd,(POINT*)&rc,1);
      ScreenToClient(hwnd,(POINT*)&rc,1);

      hdc =CreateDC(pSurfTop,&rc); //在“顶层”Surface中创建一个DC。
      ds->hDC =hdc;

//      if(ds->ID> ID_BTN_BEGIN && ds->ID<ID_BTN_END)
//      {

//        button_owner_draw(ds); //执行自绘制按钮
//      }

//      if(ds->ID == ID_LISTBOX)
//      {
//        listbox_owner_draw_1(ds);
//      }

      DeleteDC(hdc);

      InvalidateRect(hwnd,&rc,FALSE); //使主窗口重绘.

      return TRUE;
    }
    break;
    
		case WM_NOTIFY: //WM_NOTIFY消息:wParam低16位为发送该消息的控件ID,高16位为通知码;lParam指向了一个NMHDR结构体.
		{
			u16 code,id;

			code =HIWORD(wParam); //获得通知码类型.
			id   =LOWORD(wParam); //获得产生该消息的控件ID.

			if(id == ID_HEATING_BUTTON || 
          id == ID_HEATING_INFO_BUTTON ||
          id == ID_COOLING_INFO_BUTTON && 
          code == BN_CLICKED) // 按钮“单击”了.
      {      
        if(id == ID_HEATING_BUTTON)
        {
          //向左滑动        
//          move_dir = 1;
//          wnd = GetDlgItem(hwnd,ID_SC_HEATING);
          
          PostMessage(GetParent(hwnd),
                      MSG_FUNCTION_SELECT,
                      GetWindowLong(hwnd,GWL_ID),
                      NULL); 
          /* 跳出，不触发定时器 */
         break;    
        }
        else if(id == ID_HEATING_INFO_BUTTON)
        {
          //向左滑动        
          move_dir = 1;  
          wnd = GetDlgItem(hwnd,ID_SC_HEATING);
        }  
        else if(id == ID_COOLING_INFO_BUTTON)
        {
          //向左滑动        
          move_dir = 1;  
          wnd = GetDlgItem(hwnd,ID_SC_COOLING);
        }         
        KillTimer(hwnd,1);
        SetTimer(hwnd,1,10,TMR_START,NULL);
      }      
		}
		break;
    
		////   
     case WM_TIMER:
      {
        /* 定时器消息，上拉或下拉窗口 */
        RECT rc0;
        /* 父窗口 */
        GetWindowRect(hwnd,&rc);
        /* 滚动条窗口 */
        GetWindowRect(wnd,&rc0);
        SetForegroundWindow(wnd);        

        if(move_dir == 0)
        {
//           ShowWindow(GetDlgItem(hwnd,ID_TEMP_INFO_BUTTON),SW_SHOW);
//           ShowWindow(GetDlgItem(hwnd,ID_HUMI_INFO_BUTTON),SW_SHOW);
//           EnableWindow(GetDlgItem(hwnd,ID_TEMP_INFO_BUTTON),ENABLE);
//           EnableWindow(GetDlgItem(hwnd,ID_HUMI_INFO_BUTTON),ENABLE);

          /* 向右滑动 */
          if(rc0.x < rc.w)
          {
            OffsetRect(&rc0,(rc0.w>>3),0);
            rc0.x =MIN(rc0.x,rc.w);
            MoveWindow(wnd,rc0.x,0,rc0.w,rc0.h,TRUE);
          }
          else
          {
            KillTimer(hwnd,1);
          }
        }
        else
        {  
           /* 向左滑动 */
          if(rc0.x > (rc.w - rc0.w))
          {
            OffsetRect(&rc0,-(rc0.w>>3),0);
            rc0.x =MAX(rc0.x,(rc.w - rc0.w));
            MoveWindow(wnd,rc0.x,0,rc0.w,rc0.h,TRUE);
          }
          else
          {
//             ShowWindow(GetDlgItem(hwnd,ID_TEMP_INFO_BUTTON),SW_HIDE);
//             ShowWindow(GetDlgItem(hwnd,ID_HUMI_INFO_BUTTON),SW_HIDE);
//             EnableWindow(GetDlgItem(hwnd,ID_TEMP_INFO_BUTTON),DISABLE);
//             EnableWindow(GetDlgItem(hwnd,ID_HUMI_INFO_BUTTON),DISABLE);

            KillTimer(hwnd,1); 
          }            
        }

      }
//			InvalidateRect(hwnd,&rc,FALSE);
			break; 
    
    case	WM_ERASEBKGND:
		{
			HDC hdc=(HDC)wParam;
			RECT rc =*(RECT*)lParam;

			//GetClientRect(hwnd,&rc);
			SetBrushColor(hdc,MapRGB888(hdc,BGCOLOR));
			FillRect(hdc,&rc);
			return TRUE;
		}

		case WM_PAINT: //窗口需要绘制时，会自动产生该消息.
		{
      PAINTSTRUCT ps;
      HDC hdc,hdc_bk;
      RECT rc;
      
      GetClientRect(hwnd,&rc);
      hdc_bk =CreateMemoryDC(SURF_SCREEN,rc.w,rc.h);

      hdc =BeginPaint(hwnd,&ps); //开始绘图

      ////用户的绘制内容...
      //TextOut(hdc,10,10,L"Hello",-1);
    
//      BitBlt(hdc_bk,0,0,rc.w,rc.h,hdcTop,0,0,SRCCOPY);
//      BitBlt(hdc,0,0,rc.w,rc.h,hdc_bk,0,0,SRCCOPY);

      DeleteDC(hdc_bk);

      EndPaint(hwnd,&ps); //结束绘图
		}
		break;
		////

		case WM_CLOSE: //窗口关闭时，会自动产生该消息.
		{

			return DestroyWindow(hwnd); //调用DestroyWindow函数销毁窗口，该函数会使主窗口结束并退出消息循环;否则窗口将继续运行.
		}
//		break;
		////
    
    case WM_DESTROY:
    {
      DeleteDC(hdcTop);
      DeleteSurface(pSurfTop);

      return DefWindowProc(hwnd,msg,wParam,lParam);
    }

		default: //用户不关心的消息,由系统处理.
		{
			return DefWindowProc(hwnd,msg,wParam,lParam);
		}

	}

	return WM_NULL;
}

//定义一个控件类结构实例.
const WNDCLASS my_heating_time_wiget=
{
    .Tag         =WNDCLASS_TAG,
    .Style       = 0,
    .lpfnWndProc =my_heating_time_wiget_proc,
    .cbClsExtra  =0,
    .cbWndExtra  =0,
    .hInstance   =NULL,
    .hIcon       =NULL,
    .hCursor	 =NULL,
};



/**
  * @brief  窗口的回调函数
  * @param  无
  * @retval 无
  */
static LRESULT	WinProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	RECT rc;
  static U8 move_dir = 0;
  
	switch(msg)
	{
		case WM_CREATE: //窗口创建时,会自动产生该消息,在这里做一些初始化的操作或创建子窗口.
		{
      GetClientRect(hwnd,&rc); //获得窗口的客户区矩形.
    
//      rc.x = 10;
//      rc.w -= 2*rc.x;
      rc.y = 20;
      rc.h = 110;
      CreateWindow(&my_temp_humi_wiget,L"MyTempHumiWiget",WS_VISIBLE,
                    rc.x,rc.y,rc.w,rc.h,hwnd,ID_TEMP_HUMI_WIGET,NULL,NULL);
      
//      rc.x = 10;
//      rc.w -= 100;
      OffsetRect(&rc,0,rc.h);
      CreateWindow(&my_dwell_time_wiget,L"MyDwellWiget",WS_VISIBLE,
                    rc.x,rc.y,rc.w,rc.h,hwnd,ID_DWELL_WIGET,NULL,NULL);

      OffsetRect(&rc,0,rc.h);
      CreateWindow(&my_heating_time_wiget,L"MyHeatingWiget",WS_VISIBLE,
                    rc.x,rc.y,rc.w,rc.h,hwnd,ID_HEATING_WIGET,NULL,NULL);

      OffsetRect(&rc,0,rc.h);

      rc.h=70;
      CreateWindow(BUTTON,L"开始",BS_FLAT|WS_VISIBLE,
                    rc.x,rc.y,rc.w/3,rc.h,hwnd,ID_START_BUTTON,NULL,NULL);

     
		}
    
//    SetTimer(hwnd,1,10,TMR_START,NULL);

		return TRUE;
		////
    //MSG_FUNCTION_SELECT消息:wParam低16位为发送该消息的控件ID,高16位为通知码;lParam指向了一个NMHDR结构体.
    case MSG_FUNCTION_SELECT: 
		{
     RECT rc0;        

      u16 id;
      u32 scrollbar_value;
      MY_SCROLLBAR_EXT *scrollbar_ex;
      WCHAR wbuf[40];

			id = wParam; //获得ID.
      
      GUI_DEBUG("id = 0x%x",id);
//			scrollbar_value = lParam; //获得产生该消息的控件ID.

//      KillTimer(hwnd,1);
//      SetTimer(hwnd,1,10,TMR_START,NULL);
     GetWindowRect(GetDlgItem(hwnd,ID_DWELL_WIGET),&rc0);

//     MoveWindow(GetDlgItem(hwnd,ID_DWELL_WIGET),rc0.x,rc0.y+10,rc0.w,rc0.h,TRUE);
     MoveWindow(GetDlgItem(hwnd,ID_DWELL_WIGET),10,10,rc0.w,rc0.h,TRUE);

      
		}
		break;
    
       
		case WM_NOTIFY: //WM_NOTIFY消息:wParam低16位为发送该消息的控件ID,高16位为通知码;lParam指向了一个NMHDR结构体.
		{
		}
		break;
    
		////   
	////   
     case WM_TIMER:
      {
        HWND wnd;

        /* 定时器消息，上拉或下拉窗口 */
        RECT rc0;        

        /* 父窗口 */
        GetWindowRect(hwnd,&rc);
        wnd = GetDlgItem(hwnd,ID_TEMP_HUMI_WIGET);

        /* 滚动条窗口 */
        GetWindowRect(wnd,&rc0);
        SetForegroundWindow(wnd);   
         GUI_DEBUG("TIMER");

        /* 向上滑动 */
        if(rc0.y > -rc0.h)
        {
          OffsetRect(&rc0,0,-(rc0.h>>3));
          rc0.y =MAX(rc0.y,-rc0.h);
          GUI_DEBUG("TIMER rc0.y =%d",rc0.y);
          MoveWindow(wnd,rc0.x,rc0.y,rc0.w,rc0.h,TRUE);
          GUI_DEBUG("TIMER end");

        }
        else
        {
          KillTimer(hwnd,1);
        }
        
      }
//			InvalidateRect(hwnd,&rc,FALSE);
			break;
    
    case	WM_ERASEBKGND:
		{
			HDC hdc=(HDC)wParam;
			RECT rc = *(RECT*)lParam;

			//GetClientRect(hwnd,&rc);
			SetBrushColor(hdc,MapRGB888(hdc,BGCOLOR));
			FillRect(hdc,&rc);
			return TRUE;
		}
    
    case WM_CTLCOLOR:
		{ 
      u16 id;      
			id =LOWORD(wParam);

			if(id == ID_START_BUTTON )
      {
        CTLCOLOR *cr;	  
        cr =(CTLCOLOR*)lParam;	

        cr->TextColor =RGB888(255,255,255);
        cr->BackColor =RGB888(154,255,2);
        cr->BorderColor =RGB888(0,0,255);
        return TRUE;
      } 
      else /* 其它控件使用默认颜色 */
        return FALSE;
    }

		case WM_PAINT: //窗口需要绘制时，会自动产生该消息.
		{

        PAINTSTRUCT ps;
        HDC hdc;

        hdc =BeginPaint(hwnd,&ps); //开始绘图

        ////用户的绘制内容...
        //TextOut(hdc,10,10,L"Hello",-1);

        EndPaint(hwnd,&ps); //结束绘图
		}
		break;
		////

		case WM_CLOSE: //窗口关闭时，会自动产生该消息.
		{

			return DestroyWindow(hwnd); //调用DestroyWindow函数销毁窗口，该函数会使主窗口结束并退出消息循环;否则窗口将继续运行.
		}
//		break;
		////

		default: //用户不关心的消息,由系统处理.
		{
			return DefWindowProc(hwnd,msg,wParam,lParam);
		}

	}

	return WM_NULL;
}





/*============================================================================*/

void	GUI_DEMO_CustomWidget(void)
{
	HWND	hwnd;
	WNDCLASS	wcex;
	MSG msg;

	/////

	wcex.Tag 		    = WNDCLASS_TAG;

	wcex.Style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WinProc; //设置主窗口消息处理的回调函数.
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= NULL;
	wcex.hIcon			= NULL;
	wcex.hCursor		= NULL;

	//创建主窗口
	hwnd	=CreateWindowEx(	WS_EX_FRAMEBUFFER,
								&wcex,
								_T("emXGUI Demo: Custom Widget"), //窗口名称
								WS_CLIPCHILDREN,
								0,0,GUI_XSIZE,GUI_YSIZE,    //窗口位置和大小
								NULL,NULL,NULL,NULL);

	//显示主窗口
	ShowWindow(hwnd,SW_SHOW);

	//开始窗口消息循环(窗口关闭并销毁时,GetMessage将返回FALSE,退出本消息循环)。
	while(GetMessage(&msg,hwnd))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

/*============================================================================*/
