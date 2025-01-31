#ifndef __LIBSDK_FREETYPE__
#define	__LIBSDK_FREETYPE__

 
#ifdef	__cplusplus
extern "C"{
#endif


#include	"emXGUI.h"


/*=========================================================================================*/
typedef	int (FN_EDF_GetData)(void *buf,int offset,int size,const void *pdata);

typedef	struct	__freetype_font_obj freetype_obj;


/*=========================================================================================*/

freetype_obj* freetype_font_obj_new(int w,int h,int dpi_horz,int dpi_vert,const u8 *pFontData);

void 	freetype_font_obj_delete(freetype_obj *ft_obj);

BOOL	freetype_font_set_size(freetype_obj *ft,int w,int h,int dpi_horz,int dpi_vert);
BOOL	freetype_font_get_bitmap(freetype_obj *ft,u8 bpp,u8 *out,int line_bytes,int max_h,u16 wchr,s8 xoff,s8 yoff);

HFONT   freetype_CreateFont(freetype_obj *ft_obj);


/*=========================================================================================*/
 
#ifdef	__cplusplus
}
#endif

#endif
