
#ifndef __RES_MGR__
#define __RES_MGR__

#ifdef	__cplusplus
extern "C"{
#endif

#include "def.h"

/*=========================================================================================*/

struct _file_dir
{
	char 	name[24];
	u32  	size;
	u32 	offset;
};



int	Dlg_RES_Maker(int x,int y,int w,int h,const char *out_file,const char *res_dir);
int	Dlg_Flash_Writer(int x,int y,int w,int h,const char *file_name,u32 addr);

typedef	struct __res_obj res_obj;

res_obj* x_res_open(const char *res_name);
u32 x_res_size(res_obj *res);
u32 x_res_seek(res_obj *res,int pos,int mode);
u32 x_res_read(res_obj *res,u8 *buf,u32 size);
void x_res_close(res_obj *res);



/*=========================================================================================*/

#ifdef	__cplusplus
}
#endif

#endif /*__RES_MGR__*/

