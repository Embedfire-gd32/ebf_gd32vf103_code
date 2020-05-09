
#ifndef	__EMXGUI_PNG_H__
#define	__EMXGUI_PNG_H__

#ifdef	__cplusplus
extern	"C"{
#endif

#include "emXGUI.h"

/*===================================================================================*/

typedef	struct __x_png_dec PNG_DEC;


////PNG�����λͼ����(BITMAP).
PNG_DEC*    PNG_DecodeOpen(const char *png_dat);
BOOL		PNG_DecodeGetBitmap(PNG_DEC *dec,BITMAP *bm);
void		PNG_DecodeClose(PNG_DEC *dec);

////���PNGλͼ��Ϣ.
BOOL	PNG_GetInfo(BITMAPINFO *info,const char *png_dat);

////PNGֱ����ʾ.
BOOL	PNG_Draw(HDC hdc,int x,int y,const char *png_dat);


#define	PNG_Open		PNG_DecodeOpen
#define	PNG_GetBitmap	PNG_DecodeGetBitmap
#define	PNG_Close		PNG_DecodeClose


#if 0
PNG_DEC* PNG_Open(const u8 *png_dat,int png_size);
BOOL PNG_GetBitmap(PNG_DEC *png_dec,BITMAP *bm);
void PNG_Close(PNG_DEC *png_dec);

BOOL	PNG_Draw(HDC hdc,int x,int y,const char *png_dat);
#endif

/*===================================================================================*/

#ifdef	__cplusplus
}
#endif
#endif
