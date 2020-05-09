/**
  ******************************************************************
  * @file    bsp_lcd.c
  * @author  fire
  * @version V2.0
  * @date    2018-xx-xx
  * @brief   lcd应用函数接口
  ******************************************************************
  * @attention
  *
  * 实验平台:野火  i.MXRT1052开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************
  */
#include "BSP.h"
#include "emXGUI.h"


extern HDC hdc_pal;

/*******************************************************************************
 * 变量
 ******************************************************************************/

/* 帧中断标志 */
//volatile bool s_frameDone = false;

/* 显存 */
//AT_NONCACHEABLE_SECTION_ALIGN( uint32_t s_psBufferLcd[2][LCD_PIXEL_HEIGHT][LCD_PIXEL_WIDTH], FRAME_BUFFER_ALIGN);

/*用于存储当前选择的字体格式*/
//static sFONT *LCD_Currentfonts = &Font24x48;
/* 用于存储当前字体颜色和字体背景颜色的变量*/
static uint32_t CurrentTextColor   = 0x00FFFFFF;
static uint32_t CurrentBackColor   = 0x00000000;

/* 指向当前的显存 */
//static uint32_t CurrentFrameBuffer = (uint32_t)s_psBufferLcd[0];

/***************************显示应用相关******************************/

/***************************显示字符相关******************************/

/**
  * @brief  设置字体的颜色及字体的背景颜色
  * @param  TextColor: 字体颜色
  * @param  BackColor: 字体的背景颜色
  * @retval None
  */
void LCD_SetColors(uint32_t TextColor, uint32_t BackColor) 
{
  CurrentTextColor = TextColor; 
  CurrentBackColor = BackColor;

  SetTextColor(hdc_pal,MapRGB565(hdc_pal,TextColor));


}

/**
  * @brief 获取当前设置的字体颜色和字体的背景颜色
  * @param  TextColor: 指向字体颜色的指针
  * @param  BackColor: 指向字体背景颜色的指针
  * @retval None
  */
void LCD_GetColors(uint32_t *TextColor, uint32_t *BackColor)
{
  *TextColor = CurrentTextColor;
  *BackColor = CurrentBackColor;
}

/**
  * @brief  设置字体颜色
  * @param  Color: 字体颜色
  * @retval None
  */
void LCD_SetTextColor(uint32_t Color)
{
  CurrentTextColor = Color;
  SetTextColor(hdc_pal,MapRGB565(hdc_pal,Color));

}

/**
  * @brief  设置字体的背景颜色
  * @param  Color: 字体的背景颜色
  * @retval None
  */
void LCD_SetBackColor(uint32_t Color)
{
  CurrentBackColor = Color;

}
#if 0
/**
  * @brief  设置字体格式(英文)
  * @param  fonts: 选择要设置的字体格式
  * @retval None
  */
void LCD_SetFont(sFONT *fonts)
{
  LCD_Currentfonts = fonts;
}

/**
  * @brief  获取当前字体格式(英文)
  * @param  None.
  * @retval 当前应用的格式
  */
sFONT *LCD_GetFont(void)
{
  return LCD_Currentfonts;
}
#endif

/**
  * @brief  在显示器上显示一个英文字符
  * @param  Xpos ：字符的起始X坐标
  * @param  Ypos ：字符的起始Y坐标
  * @param  Ascii: 要显示的字符的ASCII码
  * @retval None
  */
void LCD_DisplayChar(uint16_t Xpos, uint16_t Ypos, WCHAR Ascii)
{
	WCHAR wbuf[2];

	wbuf[0] =Ascii;
	wbuf[1] =L'\0';
	TextOut(hdc_pal,Xpos,Ypos,wbuf,1);
}

/**
 * @brief  在显示器上显示中英文字符串,超出液晶宽度时会自动换行。
 * @param  Xpos ：字符的起始X坐标
 * @param  Ypos ：字符的起始Y坐标
 * @param  pStr ：要显示的字符串的首地址
 * @retval 无
 */
void LCD_TextOut(uint16_t Xpos, uint16_t Ypos, const WCHAR *pStr )
{
	TextOut(hdc_pal,Xpos,Ypos,pStr,-1);
} 

/**
 * @brief  在显示器上指定区域居中显示中英文字符串,超出液晶宽度时会自动换行。
 * @param  Xpos ：字符的起始X坐标
 * @param  Ypos ：字符的起始Y坐标
 * @param  pStr ：要显示的字符串的首地址
 * @retval 无
 */
void LCD_DrawText(uint16_t Xpos, uint16_t Ypos,uint16_t Width, uint16_t Height, const WCHAR *pStr )
{
	RECT rc;

	rc.x =Xpos;
	rc.y =Ypos;
	rc.w =Width;
	rc.h =Height;
	DrawText(hdc_pal,pStr,-1,&rc,DT_VCENTER|DT_CENTER);

}


#if 0
/**
  * @brief  显示字符串(英文)
  * @param  Line: 根据当前字体而变的行号
  *     @arg Line(1),Line(2)等
  * @param  *ptr: 要显示的字符串
  * @retval None
  */
void LCD_DisplayStringLine(uint16_t Line, const WCHAR *ptr)
{  

}
#endif

#if 0
/**
  * @brief  清除指定行的字符
  * @param  Line: 要清除的行,注意LINE宏是根据当前字体而变的
  *     @arg LINE(1),LINE(2)
  * @retval None
  */
void LCD_ClearLine(uint16_t Line)
{

  uint16_t refcolumn = 0;
  /* 循环显示至屏幕最右侧 */
  while ((refcolumn < LCD_PIXEL_WIDTH) && 
    (((refcolumn + LCD_Currentfonts->Width)& 0xFFFF) >= LCD_Currentfonts->Width))
  {
    /* 显示空格（相当于清除的效果） */
    LCD_DisplayChar(refcolumn, Line, ' ');
    /* 偏移字符显示位置 */
    refcolumn += LCD_Currentfonts->Width;
  }

}
#endif

/**
  * @brief  设置显示坐标
  * @param  Xpos: x坐标
  * @param  Ypos: y坐标
  * @retval 显存的地址
  */
uint32_t LCD_SetCursor(uint16_t Xpos, uint16_t Ypos)
{  
 // return CurrentFrameBuffer + APP_BPP*(Xpos + (LCD_PIXEL_WIDTH*Ypos));
}

/***************************显示图形相关******************************/
/**
  * @brief 选择当前要操作的显存区域
  * @param  index: 0或1
  * @retval None
  */
void LCD_SetFrameBuffer(uint8_t index)
{
 // CurrentFrameBuffer = (uint32_t)s_psBufferLcd[index];
}

/**
  * @brief 设置后面要显示的显存区域
  * @param  index: 0或1
  * @retval None
  */
void LCD_SetDisplayBuffer(uint8_t index)
{
  /* 设置ELCDIF的下一个缓冲区地址 */
//  ELCDIF_SetNextBufferAddr(APP_ELCDIF, (uint32_t)s_psBufferLcd[index]);

}

/**
  * @brief 使用当前颜色在指定的位置绘制一个像素点
  * @param  Xpos: x坐标
  * @param  Ypos: y坐标
  * @note 可使用LCD_SetBackColor、LCD_SetTextColor、LCD_SetColors函数设置颜色
  * @retval None
  */
void PutPixel(uint16_t Xpos, uint16_t Ypos)
{   
	SetPixel(hdc_pal,Xpos,Ypos,MapRGB565(hdc_pal,CurrentTextColor));
}

/**
  * @brief  以当前背景颜色清除整个屏幕
  * @param  无
  * @note 可使用LCD_SetBackColor、LCD_SetTextColor、LCD_SetColors函数设置颜色
  * @retval 无
  */
void LCD_Clear(uint32_t Color)
{
	ClrDisplay(hdc_pal,NULL,MapRGB565(hdc_pal,Color));
}
#if 0
/**
  * @brief 显示一条直线
  * @param Xpos: 直线起点的x坐标
  * @param Ypos: 直线起点的y坐标
  * @param Length: 直线的长度
  * @param Direction: 直线的方向，可输入
      @arg LINE_DIR_HORIZONTAL(水平方向) 
      @arg LINE_DIR_VERTICAL(垂直方向).
  * @note 可使用LCD_SetBackColor、LCD_SetTextColor、LCD_SetColors函数设置颜色
  * @retval None
  */
void LCD_DrawLine(uint16_t Xpos, uint16_t Ypos, uint16_t Length, bool Direction)
{

	if(Direction==LINE_DIR_HORIZONTAL)
	{
		HLine(hdc_pal,Xpos,Ypos,Xpos+Length);
	}
	else
	{
		VLine(hdc_pal,Xpos,Ypos,Ypos+Length);
	}
}
#endif
/**
 * @brief  在液晶屏上使用 Bresenham 算法画线段（基于两点） 
 * @param  Xpos1 ：线段的一个端点X坐标
 * @param  Ypos1 ：线段的一个端点Y坐标
 * @param  Xpos2 ：线段的另一个端点X坐标
 * @param  Ypos2 ：线段的另一个端点Y坐标
 * @note 可使用LCD_SetBackColor、LCD_SetTextColor、LCD_SetColors函数设置颜色
 * @retval 无
 */
void LCD_DrawUniLine ( uint16_t Xpos1, uint16_t Ypos1, uint16_t Xpos2, uint16_t Ypos2 )
{
	SetBrushColor(hdc_pal,MapRGB565(hdc_pal,CurrentTextColor));
	Line(hdc_pal,Xpos1,Ypos1,Xpos2,Ypos2);
}   

/**
  * @brief  绘制空心矩形
  * @param  Xpos ：矩形左上角端点X坐标
  * @param  Ypos ：矩形左上角端点Y坐标
  * @param  Width ：矩形宽
  * @param  Height ：矩形高
  * @note 可使用LCD_SetBackColor、LCD_SetTextColor、LCD_SetColors函数设置颜色
  * @retval 无
  */
void LCD_DrawRect(uint16_t Xpos, uint16_t Ypos, uint16_t Width, uint16_t Height)
{  
	RECT rc;

	SetBrushColor(hdc_pal,MapRGB565(hdc_pal,CurrentTextColor));
	rc.x =Xpos;
	rc.y =Ypos;
	rc.w =Width;
	rc.h =Height;
	DrawRect(hdc_pal,&rc);
}

/**
  * @brief  绘制实心矩形
  * @param  Xpos ：矩形左上角端点X坐标
  * @param  Ypos ：矩形左上角端点Y坐标
  * @param  Width ：矩形宽
  * @param  Height ：矩形高
  * @note 可使用LCD_SetBackColor、LCD_SetTextColor、LCD_SetColors函数设置颜色
  * @retval 无
  */
void LCD_DrawFullRect(uint16_t Xpos, uint16_t Ypos, uint16_t Width, uint16_t Height)
{
	RECT rc;

	SetBrushColor(hdc_pal,MapRGB565(hdc_pal,CurrentTextColor));
	rc.x =Xpos;
	rc.y =Ypos;
	rc.w =Width;
	rc.h =Height;
	FillRect(hdc_pal,&rc);
}

/**
 * @brief  绘制一个空心圆
 * @param  Xpos: 圆心X坐标
 * @param  Ypos: 圆心Y坐标
 * @param  Radius: 半径
 * @retval None
 */
void LCD_DrawCircle(uint16_t Xpos, uint16_t Ypos, uint16_t Radius)
{
	SetBrushColor(hdc_pal,MapRGB565(hdc_pal,CurrentTextColor));
	DrawCircle(hdc_pal,Xpos,Ypos,Radius);
}

/**
 * @brief  绘制一个实心圆
 * @param  Xpos: 圆心X坐标
 * @param  Ypos: 圆心Y坐标
 * @param  Radius: 半径
 * @retval None
 */
void LCD_DrawFullCircle(uint16_t Xpos, uint16_t Ypos, uint16_t Radius)
{  
	SetBrushColor(hdc_pal,MapRGB565(hdc_pal,CurrentTextColor));
	FillCircle(hdc_pal,Xpos,Ypos,Radius);
}
/*********************************************END OF FILE**********************/
