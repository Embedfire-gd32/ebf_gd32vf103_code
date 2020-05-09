#ifndef _PALETTE_LCD_H
#define _PALETTE_LCD_H

//#include "fsl_common.h"
#include "BSP.h"
#include "emXGUI.h"
 
void LCD_DisplayChar(uint16_t Xpos, uint16_t Ypos, WCHAR Ascii);
void LCD_TextOut(uint16_t Xpos, uint16_t Ypos, const WCHAR *pStr );
void LCD_DrawText(uint16_t Xpos, uint16_t Ypos,uint16_t Width, uint16_t Height, const WCHAR *pStr );

void LCD_DisplayStringLine(uint16_t Line, const WCHAR *ptr);

#endif //_PALETTE_LCD_H

