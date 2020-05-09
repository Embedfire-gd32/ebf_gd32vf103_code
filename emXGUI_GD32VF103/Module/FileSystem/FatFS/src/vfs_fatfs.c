
#include	<stddef.h>
#include	<string.h>

#include	"ff.h"

#include	"x_libc.h"
#include	"Kernel.h"
#include	"Kernel_Arch.h"


/*=========================================================================================*/


typedef struct	fatfs_data
{
	char root[4];
	char buf[256]; //g_buf
	FATFS fs_ctx;
	fs_obj fs_dev;

}vfs_fatfs_t;

/*=========================================================================================*/

#define	BLK_DEV_NUM	4

blk_dev_t	*fatfs_blk_dev[BLK_DEV_NUM];

static	fs_ops_t fatfs_ops;

/*=========================================================================================*/


static	const char *fs_name[] =
{
	"---",
	"FAT12",
	"FAT16",
	"FAT32"
};

/*=========================================================================================*/
/*=========================================================================================*/

void fatfs_mkfs(const char *path)
{
	u8 *pbuf=NULL;
	int i;

	pbuf =(u8*)vmalloc(4096);
	if(pbuf!=NULL)
	{
		i=f_mkfs("2:",FM_FAT32,1024,pbuf,4096);
		vfree(pbuf);
	}
}

/*=========================================================================================*/
/*=========================================================================================*/

static	mutex_obj *mutex_lock_fatfs=NULL;

static	sys_err_t	fatfs_lock(fs_obj *fs)
{
	return	SYS_mutex_lock(mutex_lock_fatfs,0xFFFFFFFF);
}

static	sys_err_t	fatfs_unlock(fs_obj *fs)
{
	return	SYS_mutex_unlock(mutex_lock_fatfs);
}

static	void	show_fs_info(FATFS *fs);

static	sys_err_t	fatfs_mount(fs_obj *fs)
{
    DIR dir;
    FRESULT res;

    vfs_fatfs_t *fatfs;
    //blk_dev_info_t info;
    
    fatfs =(vfs_fatfs_t*)fs->private_data;

	DebugPrintf("%s: '%s'\r\n",__func__,fatfs->root);
	
	res=f_mount(&fatfs->fs_ctx,fatfs->root,0);

	DebugPrintf("f_mount: %d.\r\n",res);
	
	if(f_opendir(&dir,fatfs->root)!=FR_OK)
	{
		//f_mkfs(fatfs->root,0,0);
	}
	f_closedir(&dir);
	
	show_fs_info(&fatfs->fs_ctx);
	return	SYS_NO_ERR;
		
} 

static	sys_err_t	fatfs_unmount(fs_obj *fs_drv)
{
 	DebugPrintf("%s:%08XH\r\n",__func__,fs_drv);
	return	SYS_NO_ERR;		
} 

static	sys_err_t	fatfs_open(fs_obj *fs,file_obj *filp,const char *path)
{
	U32	flag;	
	char *buf;
	vfs_fatfs_t *vfs_fatfs;
	FIL *fp;
	int i;
	
	flag	=0;
	switch(filp->f_flags&O_ACCMODE)
	{
		case	O_RDONLY:
				flag	=FA_READ;
				break;
				////
				
		case	O_WRONLY:
				flag	=FA_WRITE;
				break;
				////
				
		case	O_RDWR:
				flag	=FA_READ|FA_WRITE;
				break;
				////
		default:
				break;				  
	}  

	if(filp->f_flags&O_CREATE)
	{	
		if(filp->f_flags&O_EXCL)
		{
			flag |=FA_CREATE_NEW;
		}
		
		else if(filp->f_flags&O_TRUNC)
		{
			flag |=FA_CREATE_ALWAYS;
		}
		else
		{
			flag |=FA_OPEN_ALWAYS;	
		}
				
	}
	else
	{
		flag |=FA_OPEN_EXISTING;		
	}
	
	vfs_fatfs	=(vfs_fatfs_t*)fs->private_data;
	
	i =MIN(128,x_strlen(path) + x_strlen(vfs_fatfs->root));
	buf	=vmalloc(i);

	buf[0]	='\0';
	
 //   DebugPrintf("fatfs_open_0:%08XH,'%s%s'\r\n",vfs_fatfs,vfs_fatfs->root,p);
	
	x_strcat(buf,vfs_fatfs->root);
	x_strcat(buf,path);
	
//	DebugPrintf("fatfs_open_1:'%s'\r\n",path);
	
	fp =(FIL*)vmalloc(sizeof(FIL));
	filp->private_data	=fp;

	i=f_open(fp,buf,flag);
	//	DebugPrintf("fatfs_open_file:'%s'\r\n",path);
	if(i != FR_OK)
	{
		vfree(filp->private_data);
		
		filp->private_data =NULL;
		ErrorOutput("fatfs,f_open=%d,path='%s%s'\r\n",i,vfs_fatfs->root,path);
	
		vfree(buf);
		return	SYS_ERR;
		
	}
	else
	{
		vfree(buf);
		return	SYS_NO_ERR;
	}


}

static	sys_err_t	fatfs_close(file_obj* filp)
{

  	f_close((FIL*)filp->private_data);

  	vfree(filp->private_data);
  	
  	filp->private_data	=NULL;
  	return	SYS_NO_ERR;
}

static   sys_err_t 	fatfs_ioctl(fs_obj *dev,file_obj *filp, int cmd, void *arg,int size)
{
	return SYS_NO_ERR;
}

static	sys_err_t	fatfs_sync(fs_obj *fs,file_obj* filp)
{
	FIL *fp;
	int res;
	
	fp	=filp->private_data;
	
	res=f_sync(fp);
//	DebugPrintf("fatfs_sync:%08XH,%d\r\n",fp,res);

	if(res!=FR_OK)
	{
		return	SYS_ERR;
	}
	else
	{
		return	SYS_NO_ERR;
	}
}


static	sys_err_t	fatfs_seek(file_obj *filp,S32 offset_l,const S32 *offset_h,int mode)
{
	FIL *fp;
	
	fp	=filp->private_data;
	
	switch(mode)
	{
		default:	
		case	SEEK_SET:
				offset_l	=(U32)offset_l;
				break;
				////
					
		case	SEEK_CUR:
				offset_l	=(S32)offset_l + (S32)f_tell(fp);
				break;
				////
					
		case	SEEK_END:
				
				offset_l	=(S32)offset_l + (S32)f_size(fp);
				break;
				////
	}		
		
	if(f_lseek(fp,(DWORD)offset_l)!=FR_OK)
	{		
		return SYS_ERR;
	}

	return	SYS_NO_ERR;


}

static	u32 fatfs_fpos(file_obj *filp,u32 *pos_h)
{
	FIL *fp;
	
	fp	=filp->private_data;

	if(pos_h!=NULL)
	{
		*pos_h	=(S32)0;
	}
	
	return	f_tell(fp);
	
}


static	s32	fatfs_write(file_obj* filp,const char* buf,s32 size)
{
	FIL *fp;
	UINT n;
	int res;
	
	
	fp	=filp->private_data;
    res	=f_write(fp,buf,size,&n);
	if(res!=FR_OK)
	{
	//	DebugPrintf("Err[%d]:fatfs_write\r\n",res);	
		n	=-1;
	}

	return	n;	
}

static	s32	fatfs_read(file_obj* filp,char* buf,s32 size)
{
	FIL *fp;
	UINT n;
	int res;
	
	
	fp	=filp->private_data;
    res	=f_read(fp,buf,size,&n);
	if(res!=FR_OK)
	{
	//	DebugPrintf("Err[%d]:fatfs_read\r\n",res);	
		n	=-1;
	}
	return	n;	
}

static	sys_err_t	fatfs_getstat(fs_obj *fs,const char *path,file_stat_t *fstat)
{
	int res;
    vfs_fatfs_t *vfs_fatfs;
    FILINFO f_info;

    char *buf;

	vfs_fatfs	=(vfs_fatfs_t*)fs->private_data;

	buf =vfs_fatfs->buf;
	x_strcpy(buf,vfs_fatfs->root);
	x_strcat(buf,path);

	res=f_stat(buf,&f_info);

	if(res==FR_OK)
	{
		
		fstat->attr	=0;
		if(f_info.fattrib&AM_RDO)
		{
			fstat->attr	|=FA_RDO;
		}
		
		if(f_info.fattrib&AM_HID)
		{
			fstat->attr	|=FA_HID;
		}
		
		if(f_info.fattrib&AM_DIR)
		{
			fstat->attr	|=FA_DIR;
		}
		
		
		
		fstat->size_l	=f_info.fsize;
		fstat->size_h	=0;

		fstat->time_c.year	=0;
		fstat->time_c.mon	=0;
		fstat->time_c.day	=0;	
		fstat->time_c.hour	=0;
		fstat->time_c.min	=0;
		fstat->time_c.sec	=0;
		
		fstat->time_a.year	=0;
		fstat->time_a.mon	=0;
		fstat->time_a.day	=0;	
		fstat->time_a.hour	=0;
		fstat->time_a.min	=0;
		fstat->time_a.sec	=0;
		
		fstat->time_m.year	=0;
		fstat->time_m.mon	=0;
		fstat->time_m.day	=0;	
		fstat->time_m.hour	=0;
		fstat->time_m.min	=0;
		fstat->time_m.sec	=0;
		
		return	SYS_NO_ERR;
	}
	else
	{
		return	SYS_ERR;
	}
	
}

static	size_t	fatfs_fsize(file_obj *filp,size_t* size_h)
{
	FIL *fp;
	
	fp	=filp->private_data;
	if(fp!=NULL)
	{
		if(size_h)
		{
			*size_h	=(S32)0;
		}
		return f_size(fp);
	}
	return	0;	
	
}

static	sys_err_t	fatfs_mkdir(fs_obj *fs,const char *path)
{
    vfs_fatfs_t *vfs_fatfs;
	char *buf;
	 
	vfs_fatfs	=(vfs_fatfs_t*)fs->private_data;

	buf =vfs_fatfs->buf;

	x_strcpy(buf,vfs_fatfs->root);
	x_strcat(buf,path);
	DebugPrintf("%s:'%s'\r\n",__func__,buf);
	 
	f_mkdir(buf);

	return	SYS_NO_ERR;
}

static	sys_err_t	fatfs_rename(fs_obj *fs,const char *old_path,const char *new_path)
{ 	
	char	*buf0,*buf1;
	vfs_fatfs_t *vfs_fatfs;
	
	buf0 =(char*)vmalloc(256*2);
	if(buf0==NULL)
	{
		return SYS_ERR;
	}
	buf1 = &buf0[256];

	vfs_fatfs	=(vfs_fatfs_t*)fs->private_data;

	x_strcpy(buf0,vfs_fatfs->root);
	x_strcat(buf0,old_path);

	x_strcpy(buf1,vfs_fatfs->root);
	x_strcat(buf1,new_path);

	f_rename(buf0,buf1);
	
	vfree(buf0);
	return	SYS_NO_ERR;
		
}
 
static	sys_err_t	fatfs_remove(fs_obj *fs,const char *path)
{ 	
	char *buf;
	vfs_fatfs_t *vfs_fatfs;
	
	vfs_fatfs	=(vfs_fatfs_t*)fs->private_data;

	buf =vfs_fatfs->buf;

	x_strcpy(buf,vfs_fatfs->root);
	x_strcat(buf,path);

	f_unlink(buf);

	return	SYS_NO_ERR;
		
}
 
/*=========================================================================================*/
struct __fatfs_dir_ctx{
	DIR dir;
	int idx;
};

static sys_err_t fatfs_opendir(struct __dir_obj *dir,const char *path)
{
	vfs_fatfs_t *vfs_fatfs;
	char *buf;

	vfs_fatfs	=(vfs_fatfs_t*)dir->fs->private_data;
	if(vfs_fatfs!=NULL)
	{
		struct __fatfs_dir_ctx *dir_ctx;

		dir_ctx =(struct __fatfs_dir_ctx*)vmalloc(sizeof(struct __fatfs_dir_ctx));
		dir_ctx->idx =0;

		dir->private_data =dir_ctx;

		buf =vfs_fatfs->buf;
		buf[0] =0;

		x_strcat(buf,vfs_fatfs->root);
		x_strcat(buf,path);
		if(f_opendir(&dir_ctx->dir,buf) == FR_OK)
		{/*
			FILINFO fno;

			fno.lfname =buf;
			fno.lfsize =256;
			f_readdir(&dir_ctx->dir,&fno);
			*/
			return SYS_NO_ERR;
		}

		vfree(dir_ctx);
		dir->private_data =NULL;
	}

	return SYS_ERR;
}

static sys_err_t	fatfs_readdir(struct __dir_obj *dir,struct dir_info *d_info)
{
	vfs_fatfs_t *vfs_fatfs;

	vfs_fatfs	=(vfs_fatfs_t*)dir->fs->private_data;
	if(vfs_fatfs!=NULL)
	{
		struct __fatfs_dir_ctx *dir_ctx;
		FILINFO fno;

		dir_ctx =(struct __fatfs_dir_ctx*)dir->private_data;

		if(f_readdir(&dir_ctx->dir,&fno) == FR_OK)
		{
			u32 attr=0;

			if(fno.fattrib & AM_RDO)
			{
				attr |= FA_RDO;
			}
			if(fno.fattrib & AM_HID)
			{
				attr |= FA_HID;
			}
			if(fno.fattrib & AM_DIR)
			{
				attr |= FA_DIR;
			}
#if 0
			if(fno.fattrib & AM_VOL)
			{
				attr |= 0;
			}
#endif

			dir_ctx->idx++;

			d_info->index = dir_ctx->idx;;
			d_info->attr  = attr;
			d_info->fsize = fno.fsize;

			x_strncpy(d_info->name_buf,fno.fname,d_info->name_buf_size);

			return SYS_NO_ERR;
		}

	}

	return SYS_ERR;
}

static sys_err_t   fatfs_closedir(struct __dir_obj *dir)
{
	vfs_fatfs_t *vfs_fatfs;

	vfs_fatfs	=(vfs_fatfs_t*)dir->fs->private_data;
	if(vfs_fatfs!=NULL)
	{
		struct __fatfs_dir_ctx *dir_ctx;

		dir_ctx =(struct __fatfs_dir_ctx*)dir->private_data;

		if(f_closedir(&dir_ctx->dir) == FR_OK)
		{

		}

		vfree(dir_ctx);

	}

	return SYS_NO_ERR;

}

/*=========================================================================================*/



/*=========================================================================================*/

static	void	show_fs_info(FATFS *fs)
{
	////
		
	if(fs!=NULL)
	if(fatfs_blk_dev[fs->pdrv]!=NULL)
	{
		DebugPrintf("\r\n");
	 	DebugPrintf("***********************\r\n");
	 	DebugPrintf("****   fatfs info  ****\r\n");
	 	DebugPrintf("***********************\r\n");
	 	DebugPrintf("drive name:  %s\r\n",fatfs_blk_dev[fs->pdrv]->name);
	 	DebugPrintf("drive:       %d\r\n",fs->pdrv);
	 	DebugPrintf("fs_type:     %s\r\n",fs_name[fs->fs_type]);
	 	DebugPrintf("csize:       %d\r\n",fs->csize);
	 	DebugPrintf("ssize:       %d\r\n",fs->ssize);
	 	DebugPrintf("n_fats:      %d\r\n",fs->n_fats);
	 	DebugPrintf("wflag:       %d\r\n",fs->wflag);
	 	DebugPrintf("id:          %d\r\n",fs->id);
	 	DebugPrintf("n_rootdir:   %d\r\n",fs->n_rootdir);
	 	DebugPrintf("fsize:       %d\r\n",fs->fsize);
	 	DebugPrintf("n_fatent:    %d\r\n",fs->n_fatent);
	 	DebugPrintf("free_clust:  %d\r\n",fs->free_clst);
	 	DebugPrintf("fat_base:    %d\r\n",fs->fatbase);
	 	DebugPrintf("dir_base:    %d\r\n",fs->dirbase);
	 	DebugPrintf("data_base:   %d\r\n",fs->database);
	 	
	 	DebugPrintf("++++++++++++++++\r\n");
	 
	 //	GetDriveFreeSpace(drv,0,&total,&free);		
	//	DebugPrintf("TotalSize: %d Byte\r\n",total.LowPart);
	//	DebugPrintf("FreeSize:  %d Byte\r\n",free.LowPart);
	//	DebugPrintf("***********************\r\n",free.LowPart);

	}

}


void	vfs_fatfs_init(void)
{

	mutex_lock_fatfs =SYS_mutex_create(NULL);
	
	memset(&fatfs_ops,0,sizeof(fs_ops_t));
	
	fatfs_ops.lock		=fatfs_lock;
	fatfs_ops.unlock	=fatfs_unlock;
	
	fatfs_ops.mount		=fatfs_mount;
	fatfs_ops.unmount	=fatfs_unmount;
		
	fatfs_ops.open		=fatfs_open;
	fatfs_ops.close		=fatfs_close;
	fatfs_ops.ioctl		=fatfs_ioctl;
	fatfs_ops.sync		=fatfs_sync;

	fatfs_ops.seek		=fatfs_seek;
	fatfs_ops.fpos		=fatfs_fpos;
	fatfs_ops.fsize		=fatfs_fsize;	
	fatfs_ops.write		=fatfs_write;
	fatfs_ops.read		=fatfs_read;
	fatfs_ops.getstat	=fatfs_getstat;
	fatfs_ops.mkdir		=fatfs_mkdir;
	fatfs_ops.rename	=fatfs_rename;
	fatfs_ops.remove	=fatfs_remove;
	
	fatfs_ops.opendir   =fatfs_opendir;
	fatfs_ops.readdir 	=fatfs_readdir;
	fatfs_ops.closedir   =fatfs_closedir;

}


void	vfs_fatfs_register(u8 drv,const char *fs_name,const char *blk_dev_name)
{

	vfs_fatfs_t *vfs_fatfs;

	if(drv > BLK_DEV_NUM)
	{
		return;
	}

	fatfs_blk_dev[drv] =blk_dev_open(blk_dev_name);

	vfs_fatfs =vmalloc(sizeof(vfs_fatfs_t));

	vfs_fatfs->root[0] = '0'+drv;
	vfs_fatfs->root[1] = ':';
	vfs_fatfs->root[2] = '\0';
	vfs_fatfs->root[3] = '\0';

	file_system_register(&vfs_fatfs->fs_dev,fs_name,&fatfs_ops,vfs_fatfs);
}

void	vfs_fatfs_check(void)
{
	DIR dir;
		
	if(f_opendir(&dir,"1:apps")==FR_OK)
	{	
		f_closedir(&dir);
	}
	else
	{
		f_mkdir("1:apps");
	}
		
		
	if(f_opendir(&dir,"1:Kernel")==FR_OK)
	{
		f_closedir(&dir);
	}
	else
	{		
		f_mkdir("1:Kernel");
	}
		
	if(f_opendir(&dir,"1:Kernel\\drivers")==FR_OK)
	{
		f_closedir(&dir);
	}
	else
	{		
		f_mkdir("1:Kernel\\drivers");
	}

}

/*=========================================================================================*/



#if 0
void	vfs_fatfs_test(void)
{ 
 	int	i;
	int fd;
	char buf[128];

  //	register_vfs_fatfs();
  	
 //	fd	=vfs_open("/B/test.txt",O_RDWR,0);
//	fd	=vfs_open("/B/test.txt",O_RDWR,0);
	fd	=SYS_open("B:/test.txt",O_RDWR,0);
	if(fd>=0)
	{
		DebugPrintf("fd: %d\r\n",fd);
		memset(buf,0,128);
		SYS_read(fd,buf,128);
		
		DebugPrintf("****************\r\n");
		DebugPrintf("%s\r\n",buf);
		DebugPrintf("****************\r\n");		
		SYS_close(fd);
					
	}
	return;
	
	
	////
	if(1)
	{
		SYSTEMTIME time;
		
		GetLocalTime(&time);
	
		SYS_mkdir("B:test_dir1");
	
		fd	=SYS_open("B:test_dir1/test1.txt",O_RDWR|O_CREATE,0);
		DebugPrintf("test1.txt -> fd:%d\r\n",fd);
		if(fd>=0)
		{
			
			x_sprintf(buf,"test_file:%04d-%02d-%02d %d:%02d:%02d\r\n",time.wYear,time.wMonth,time.wDay,time.wHour,time.wMinute,time.wSecond);
		 	SYS_write(fd,buf,x_strlen(buf));	
		 	SYS_close(fd);
		}
	
	}
	////

	while(1)
	{
		
		Sleep(100);
	}
}
#endif


/*=========================================================================================*/

#if 0

dir_obj* SYS_opendir(const char *path);
sys_err_t SYS_closedir(dir_obj *dir);

static char list_buf[4096];

void vfs_test_scan_dir(void)
{
	dir_obj *dir;
	struct dir_info d_info;
	char buf[256];
	////

	d_info.name_buf_size =256;
	d_info.name_buf =buf;
	while(1)
	{
		memset(list_buf,0,sizeof(list_buf));

		dir =SYS_opendir("B:apps");
		if(dir!=NULL)
		{
			while(SYS_readdir(dir,&d_info)==SYS_NO_ERR)
			{
				if(d_info.name_buf[0] == 0)
				{
					break;
				}
				if(d_info.attr & FA_DIR)
				{
					DebugPrintf("<dir> '%s'\r\n",d_info.name_buf);
				}
				else
				{
					DebugPrintf("      '%s'\r\n",d_info.name_buf);
				}
				x_strcat(list_buf,d_info.name_buf);
			}
			SYS_closedir(dir);
		}
		SYS_msleep(100);
	}

}
#endif

/*=========================================================================================*/
#if 0
int SYS_fgets(char *buf,int max_count,file_obj *fp)
{
	int i=0;
	char ch;

	while(i<max_count)
	{

		if(SYS_read(fp,&ch,1)==1)
		{
			if(ch=='\r')
			{
				SYS_read(fp,&ch,1);

				ch ='\0';
			}

			buf[i] =ch;

			if(ch=='\0')
			{
				break;
			}

			i++;
		}
		else
		{
			buf[i] ='\0';
			break;
		}

	}

	return i;
}
#endif
/*=========================================================================================*/
