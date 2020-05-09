/**
  ******************************************************************
  * @file    bsp_lcd.c
  * @author  fire
  * @version V2.0
  * @date    2018-xx-xx
  * @brief   lcdӦ�ú����ӿ�
  ******************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ��  i.MXRT1052������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************
  */
#include "BSP.h"
#include "emXGUI.h"


extern HDC hdc_pal;

/*******************************************************************************
 * ����
 ******************************************************************************/

/* ֡�жϱ�־ */
//volatile bool s_frameDone = false;

/* �Դ� */
//AT_NONCACHEABLE_SECTION_ALIGN( uint32_t s_psBufferLcd[2][LCD_PIXEL_HEIGHT][LCD_PIXEL_WIDTH], FRAME_BUFFER_ALIGN);

/*���ڴ洢��ǰѡ��������ʽ*/
//static sFONT *LCD_Currentfonts = &Font24x48;
/* ���ڴ洢��ǰ������ɫ�����屳����ɫ�ı���*/
static uint32_t CurrentTextColor   = 0x00FFFFFF;
static uint32_t CurrentBackColor   = 0x00000000;

/* ָ��ǰ���Դ� */
//static uint32_t CurrentFrameBuffer = (uint32_t)s_psBufferLcd[0];

/***************************��ʾӦ�����******************************/

/***************************��ʾ�ַ����******************************/

/**
  * @brief  �����������ɫ������ı�����ɫ
  * @param  TextColor: ������ɫ
  * @param  BackColor: ����ı�����ɫ
  * @retval None
  */
void LCD_SetColors(uint32_t TextColor, uint32_t BackColor) 
{
  CurrentTextColor = TextColor; 
  CurrentBackColor = BackColor;

  SetTextColor(hdc_pal,MapRGB565(hdc_pal,TextColor));


}

/**
  * @brief ��ȡ��ǰ���õ�������ɫ������ı�����ɫ
  * @param  TextColor: ָ��������ɫ��ָ��
  * @param  BackColor: ָ�����屳����ɫ��ָ��
  * @retval None
  */
void LCD_GetColors(uint32_t *TextColor, uint32_t *BackColor)
{
  *TextColor = CurrentTextColor;
  *BackColor = CurrentBackColor;
}

/**
  * @brief  ����������ɫ
  * @param  Color: ������ɫ
  * @retval None
  */
void LCD_SetTextColor(uint32_t Color)
{
  CurrentTextColor = Color;
  SetTextColor(hdc_pal,MapRGB565(hdc_pal,Color));

}

/**
  * @brief  ��������ı�����ɫ
  * @param  Color: ����ı�����ɫ
  * @retval None
  */
void LCD_SetBackColor(uint32_t Color)
{
  CurrentBackColor = Color;

}
#if 0
/**
  * @brief  ���������ʽ(Ӣ��)
  * @param  fonts: ѡ��Ҫ���õ������ʽ
  * @retval None
  */
void LCD_SetFont(sFONT *fonts)
{
  LCD_Currentfonts = fonts;
}

/**
  * @brief  ��ȡ��ǰ�����ʽ(Ӣ��)
  * @param  None.
  * @retval ��ǰӦ�õĸ�ʽ
  */
sFONT *LCD_GetFont(void)
{
  return LCD_Currentfonts;
}
#endif

/**
  * @brief  ����ʾ������ʾһ��Ӣ���ַ�
  * @param  Xpos ���ַ�����ʼX����
  * @param  Ypos ���ַ�����ʼY����
  * @param  Ascii: Ҫ��ʾ���ַ���ASCII��
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
 * @brief  ����ʾ������ʾ��Ӣ���ַ���,����Һ�����ʱ���Զ����С�
 * @param  Xpos ���ַ�����ʼX����
 * @param  Ypos ���ַ�����ʼY����
 * @param  pStr ��Ҫ��ʾ���ַ������׵�ַ
 * @retval ��
 */
void LCD_TextOut(uint16_t Xpos, uint16_t Ypos, const WCHAR *pStr )
{
	TextOut(hdc_pal,Xpos,Ypos,pStr,-1);
} 

/**
 * @brief  ����ʾ����ָ�����������ʾ��Ӣ���ַ���,����Һ�����ʱ���Զ����С�
 * @param  Xpos ���ַ�����ʼX����
 * @param  Ypos ���ַ�����ʼY����
 * @param  pStr ��Ҫ��ʾ���ַ������׵�ַ
 * @retval ��
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
  * @brief  ��ʾ�ַ���(Ӣ��)
  * @param  Line: ���ݵ�ǰ���������к�
  *     @arg Line(1),Line(2)��
  * @param  *ptr: Ҫ��ʾ���ַ���
  * @retval None
  */
void LCD_DisplayStringLine(uint16_t Line, const WCHAR *ptr)
{  

}
#endif

#if 0
/**
  * @brief  ���ָ���е��ַ�
  * @param  Line: Ҫ�������,ע��LINE���Ǹ��ݵ�ǰ��������
  *     @arg LINE(1),LINE(2)
  * @retval None
  */
void LCD_ClearLine(uint16_t Line)
{

  uint16_t refcolumn = 0;
  /* ѭ����ʾ����Ļ���Ҳ� */
  while ((refcolumn < LCD_PIXEL_WIDTH) && 
    (((refcolumn + LCD_Currentfonts->Width)& 0xFFFF) >= LCD_Currentfonts->Width))
  {
    /* ��ʾ�ո��൱�������Ч���� */
    LCD_DisplayChar(refcolumn, Line, ' ');
    /* ƫ���ַ���ʾλ�� */
    refcolumn += LCD_Currentfonts->Width;
  }

}
#endif

/**
  * @brief  ������ʾ����
  * @param  Xpos: x����
  * @param  Ypos: y����
  * @retval �Դ�ĵ�ַ
  */
uint32_t LCD_SetCursor(uint16_t Xpos, uint16_t Ypos)
{  
 // return CurrentFrameBuffer + APP_BPP*(Xpos + (LCD_PIXEL_WIDTH*Ypos));
}

/***************************��ʾͼ�����******************************/
/**
  * @brief ѡ��ǰҪ�������Դ�����
  * @param  index: 0��1
  * @retval None
  */
void LCD_SetFrameBuffer(uint8_t index)
{
 // CurrentFrameBuffer = (uint32_t)s_psBufferLcd[index];
}

/**
  * @brief ���ú���Ҫ��ʾ���Դ�����
  * @param  index: 0��1
  * @retval None
  */
void LCD_SetDisplayBuffer(uint8_t index)
{
  /* ����ELCDIF����һ����������ַ */
//  ELCDIF_SetNextBufferAddr(APP_ELCDIF, (uint32_t)s_psBufferLcd[index]);

}

/**
  * @brief ʹ�õ�ǰ��ɫ��ָ����λ�û���һ�����ص�
  * @param  Xpos: x����
  * @param  Ypos: y����
  * @note ��ʹ��LCD_SetBackColor��LCD_SetTextColor��LCD_SetColors����������ɫ
  * @retval None
  */
void PutPixel(uint16_t Xpos, uint16_t Ypos)
{   
	SetPixel(hdc_pal,Xpos,Ypos,MapRGB565(hdc_pal,CurrentTextColor));
}

/**
  * @brief  �Ե�ǰ������ɫ���������Ļ
  * @param  ��
  * @note ��ʹ��LCD_SetBackColor��LCD_SetTextColor��LCD_SetColors����������ɫ
  * @retval ��
  */
void LCD_Clear(uint32_t Color)
{
	ClrDisplay(hdc_pal,NULL,MapRGB565(hdc_pal,Color));
}
#if 0
/**
  * @brief ��ʾһ��ֱ��
  * @param Xpos: ֱ������x����
  * @param Ypos: ֱ������y����
  * @param Length: ֱ�ߵĳ���
  * @param Direction: ֱ�ߵķ��򣬿�����
      @arg LINE_DIR_HORIZONTAL(ˮƽ����) 
      @arg LINE_DIR_VERTICAL(��ֱ����).
  * @note ��ʹ��LCD_SetBackColor��LCD_SetTextColor��LCD_SetColors����������ɫ
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
 * @brief  ��Һ������ʹ�� Bresenham �㷨���߶Σ��������㣩 
 * @param  Xpos1 ���߶ε�һ���˵�X����
 * @param  Ypos1 ���߶ε�һ���˵�Y����
 * @param  Xpos2 ���߶ε���һ���˵�X����
 * @param  Ypos2 ���߶ε���һ���˵�Y����
 * @note ��ʹ��LCD_SetBackColor��LCD_SetTextColor��LCD_SetColors����������ɫ
 * @retval ��
 */
void LCD_DrawUniLine ( uint16_t Xpos1, uint16_t Ypos1, uint16_t Xpos2, uint16_t Ypos2 )
{
	SetBrushColor(hdc_pal,MapRGB565(hdc_pal,CurrentTextColor));
	Line(hdc_pal,Xpos1,Ypos1,Xpos2,Ypos2);
}   

/**
  * @brief  ���ƿ��ľ���
  * @param  Xpos ���������ϽǶ˵�X����
  * @param  Ypos ���������ϽǶ˵�Y����
  * @param  Width �����ο�
  * @param  Height �����θ�
  * @note ��ʹ��LCD_SetBackColor��LCD_SetTextColor��LCD_SetColors����������ɫ
  * @retval ��
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
  * @brief  ����ʵ�ľ���
  * @param  Xpos ���������ϽǶ˵�X����
  * @param  Ypos ���������ϽǶ˵�Y����
  * @param  Width �����ο�
  * @param  Height �����θ�
  * @note ��ʹ��LCD_SetBackColor��LCD_SetTextColor��LCD_SetColors����������ɫ
  * @retval ��
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
 * @brief  ����һ������Բ
 * @param  Xpos: Բ��X����
 * @param  Ypos: Բ��Y����
 * @param  Radius: �뾶
 * @retval None
 */
void LCD_DrawCircle(uint16_t Xpos, uint16_t Ypos, uint16_t Radius)
{
	SetBrushColor(hdc_pal,MapRGB565(hdc_pal,CurrentTextColor));
	DrawCircle(hdc_pal,Xpos,Ypos,Radius);
}

/**
 * @brief  ����һ��ʵ��Բ
 * @param  Xpos: Բ��X����
 * @param  Ypos: Բ��Y����
 * @param  Radius: �뾶
 * @retval None
 */
void LCD_DrawFullCircle(uint16_t Xpos, uint16_t Ypos, uint16_t Radius)
{  
	SetBrushColor(hdc_pal,MapRGB565(hdc_pal,CurrentTextColor));
	FillCircle(hdc_pal,Xpos,Ypos,Radius);
}
/*********************************************END OF FILE**********************/
