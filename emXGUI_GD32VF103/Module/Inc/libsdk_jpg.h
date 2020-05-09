
#ifndef __LIBSDK_JPG_H__
#define __LIBSDK_JPG_H__


#ifdef	__cplusplus
extern "C"{
#endif


typedef	struct	__jpg_dec JPG_DEC;

JPG_DEC*	JPG_Open(const void *dat,int cbSize);
BOOL 	JPG_GetImageSize(U16 *width,U16 *height,JPG_DEC* jdec);
BOOL	JPG_Draw(HDC hdc,int x,int y,JPG_DEC *jdec);
void	JPG_Close(JPG_DEC *jdec);

/*============================================================================*/
BOOL JPEG_Info(u16 *w,u16 *h,const u8 *jpg_dat);
BOOL JPEG_Out(HDC hdc,int x,int y,const u8 *jpg_dat,long size);


typedef	BOOL cbJPGOUT(BITMAP *bm,void *pdata);

BOOL JPEG_OutEx(const u8 *jpg_dat,long size,cbJPGOUT *cb, void *pdata);

/*============================================================================*/

u32 JPEG_Encode_XRGB8888 (u8 *dst,u32 dst_bytes,u8 *image_buffer,u32 image_width,u32 image_height, u32 pitch,int quality);

/*============================================================================*/
#ifdef	__cplusplus
}
#endif


#endif /* __LIBSDK_JPG_H__ */
