
#ifndef	__X_FILE_H__
#define	__X_FILE_H__

#ifdef	__cplusplus
extern	"C"{
#endif

#include "def.h"
#include "Kernel.h"

/*===================================================================================*/

#define SEEK_SET         0
#define SEEK_CUR         1
#define SEEK_END         2

typedef	struct	_X_FILE	X_FILE;

X_FILE* x_fopen(const char *filename, const char *mode);
long 	x_fread(void *buffer,long size, long count, X_FILE *fp);
long 	x_fwrite(const void *buffer,long size, long count, X_FILE *fp);

int 	x_fseek(X_FILE *fp, long offset, int whence);
unsigned long 	x_ftell(X_FILE *fp);
unsigned long 	x_fsize(X_FILE *fp);
int 	x_feof(X_FILE *fp);
int 	x_fflush(X_FILE *fp);
int 	x_fclose(X_FILE *fp);
int 	x_remove(const char *filename);

/*===================================================================================*/

typedef	enum {

	SCAN_EXIT =0,
	SCAN_NEXT,
	SCAN_DIR,

}eFILESCAN_CMD;


typedef	eFILESCAN_CMD	cbFileScan(const char *fpath,const char *fname,struct dir_info *d_info,const void* pdata);

typedef struct	scan_dir_data
{
	char	*path_buf;
	int		 path_buf_bytes;
	char	*fname_buf;
	int		 fname_buf_bytes;
	cbFileScan	*callback;
	const void	*pdata;
	struct dir_info d_info;

	//eFILESCAN_CMD	scan_cmd;

}scan_dir_data_t;

void x_file_scan(scan_dir_data_t *scan_data);

/*===================================================================================*/

#ifdef	__cplusplus
}
#endif
#endif
