/*
 * Kernel_OAL.h
 *
 *  Created on: 2016年2月19日
 *      Author: Administrator
 */

#ifndef SRC_INC_KERNEL_ARCH_H_
#define SRC_INC_KERNEL_ARCH_H_

#include "def.h"
#include "list.h"
#include "Kernel.h"

/*=========================================================================================*/

typedef struct __heap_obj heap_obj;

heap_obj* HeapCreate(u8 *begin_addr, u32 size,u32 align_cb);
void* HeapAlloc(heap_obj *heap,u32 size);
void* HeapReAlloc(heap_obj *heap,void *rmem, u32 newsize);
void  HeapFree(heap_obj *heap,void *rmem);
void  HeapInfo(heap_obj *heap,u32 *total,u32 *used,u32 *max_used);


/*=========================================================================================*/
typedef struct	__list_mem
{
	struct	__list_mem *next;
	void *addr;

}list_mem;

typedef	struct __mem_page
{
	struct __mem_page *next;
	void *pa;
}mem_page;

struct	__X_PROCESS
{
	struct	__X_PROCESS *next;

	u32	 PID;
	u32  PF_Size;

	u32 *L1_base;
	list_mem *list_L2;
	mem_page *list_PF;

	////////
	void  *VA_Base;
	void  *VA_End;

	void *TEXTBase;
	u32	  TEXTSize;

	void *DATABase;
	u32	  DATASize;

	void *BSSBase;
	u32	  BSSSize;

	void *StackBase;	//for host thread
	u32	  StackSize;

	u32	  TotalSize;

	void  *StartRoutine;


	char  *pPath;		//路径
	char  *pCurDir;   //当前目录

	char  szName[64];

};

typedef	struct	__X_PROCESS X_PROCESS;

/*=========================================================================================*/


//// blk_dev ioctl
#define	BLK_DEV_GET_INFORMATION		0x0001		//获得驱动器信息
#define	BLK_DEV_GET_STATUS			0x0002		//获得驱动器状态
#define	BLK_DEV_FLUSH				0x0003	 	//刷新缓冲区

//// blk_dev type
#define BLK_DEV_TYPE_UNKNOWN     0
#define BLK_DEV_TYPE_NO_ROOT_DIR 1
#define BLK_DEV_TYPE_REMOVABLE   2
#define BLK_DEV_TYPE_FIXED       3
#define BLK_DEV_TYPE_REMOTE      4
#define BLK_DEV_TYPE_CDROM       5
#define BLK_DEV_TYPE_RAMDISK     6

//// blk_dev status
#define	BLK_DEV_STATUS_OK		 0
#define	BLK_DEV_STATUS_ERR		 1
#define	BLK_DEV_STATUS_BUSY		 2
#define	BLK_DEV_STATUS_REMOVED	 3

typedef struct
{
	u16 type;				//块设备类型
	u16 rsv;				//保留
	u32 sector_count;		//扇区数量
    u32 sector_size;		//每扇区字节数
    u32	block_size;			//每块字节数

}blk_dev_info_t;

typedef struct
{

	sys_err_t (*lock)(void);
	sys_err_t (*unlock)(void);

	BOOL	(*create)(const void *pdata);
	BOOL	(*release)(void);
	BOOL	(*open)(void);
	BOOL 	(*close)(void);
	BOOL 	(*read)(u32 blk_addr,void *buf,u32 blk_count);
	BOOL 	(*write)(u32 blk_addr,const void *buf,u32 blk_count);
	BOOL 	(*ioctl)(int cmd,void *argv,int size);

}blk_dev_ops_t;

typedef struct	_blk_dev{

	const	char *name;
	const	void *private_data;
	const	blk_dev_ops_t *ops;
	list_t	node;
}blk_dev_t;

typedef	struct	__fs_obj
{
	const	char  *name;
	const   void  *private_data;
	const	struct	__fs_ops	*ops;
	list_t	node;

} fs_obj;

struct __file_obj
{
//	struct list_head	f_list;
//	struct dentry		*f_dentry;
//	struct vfsmount         *f_vfsmnt;
//	char 	*f_path;

	void	*private_data;
	struct	__fs_obj	*fs;
	const	struct	__fs_ops	*f_ops;

	u32	f_count;
	u16 f_flags;
	u8  f_mode;
	u8	f_error;

//	unsigned long 		f_reada, f_ramax, f_raend, f_ralen, f_rawin;
//	struct fown_struct	f_owner;
//	unsigned int		f_uid, f_gid;


};

struct __dir_obj{
	fs_obj   *fs;
	const void *private_data;
};

typedef	struct __fs_ops {

	sys_err_t	(*lock)		(fs_obj *dev);
	sys_err_t	(*unlock)	(fs_obj *dev);
	sys_err_t	(*mount)	(fs_obj *dev);
	sys_err_t	(*unmount)	(fs_obj *dev);

	sys_err_t 	(*open)		(fs_obj *dev,struct __file_obj *filp,const char *path) ;
	sys_err_t 	(*close)	(struct __file_obj *filp);

    sys_err_t 	(*ioctl)	(fs_obj *dev,struct __file_obj *filp, int cmd, void *arg,int size);
    sys_err_t	(*sync)		(fs_obj *dev,struct __file_obj *filp);
 	s32			(*read)		(struct __file_obj *filp, char *buffer, s32  size/* , loff_t *  p*/);
	s32			(*write)	(struct __file_obj *filp, const char *buffer, s32  count/*, loff_t * ppos*/);
	sys_err_t	(*seek)		(struct __file_obj *filp,s32 offset_l,const s32 *offset_h,int mode);
	u32			(*fpos)		(struct __file_obj *filp,u32 *pos_h);
	u32			(*fsize)	(struct __file_obj *filp,u32 *size_h);

	sys_err_t 	(*flush)	(struct __file_obj *filp);


	////
	sys_err_t	(*getstat)	(fs_obj *dev,const char *path,file_stat_t *fstat);
	sys_err_t	(*fstat)	(fs_obj *dev,struct __file_obj *filp,file_stat_t *fstat);

	sys_err_t	(*mkdir)	(fs_obj *dev,const char *path);
	sys_err_t	(*rename)	(fs_obj *dev,const char *old_name,const char *new_name);
	sys_err_t	(*remove)	(fs_obj *dev,const char *path);

	sys_err_t   (*opendir)(struct __dir_obj *dir,const char *path);
 	sys_err_t	(*readdir)(struct __dir_obj *dir,struct dir_info *d_info);
	sys_err_t   (*closedir)(struct __dir_obj *dir);

} fs_ops_t;




blk_dev_t* 	SYS_get_blk_dev_first(void);
blk_dev_t*	SYS_get_blk_dev_next(blk_dev_t *blk_dev);
BOOL	 	blk_dev_get_name(blk_dev_t *blk_dev,char *buf,int count);
blk_dev_t*	blk_dev_open(const char *name);
BOOL		blk_dev_close(blk_dev_t *blk_dev);
BOOL		blk_dev_read(blk_dev_t *blk_dev,u32 blk_addr,void *buf,u32 blk_count);
BOOL		blk_dev_write(blk_dev_t *blk_dev,u32 blk_addr,const void *buf,u32 blk_count);
BOOL		blk_dev_ioctl(blk_dev_t *blk_dev,int cmd,void *argv,int size);

BOOL	file_system_register(fs_obj *fs,const char *name,const fs_ops_t *ops,const void *private_data);
//BOOL	device_register(const char *name,const dev_ops_t *ops,const void *private_data);
BOOL	blk_dev_register(const char *name,const blk_dev_ops_t *ops,const void *private_data);

void	USBDeviceReceiveData(u8 *dat,u32 size);
int		USB_MassStorageEntry(blk_dev_t *blk_dev);
int		USB_MassStorageExit(blk_dev_t *blk_dev);
void	USB_MassStorageHandler(void);

void	VFS_ModuleInit(void);
void	VFS_DEVFS_ModuleInit(void);

/*=========================================================================================*/
void	kmem_heap_init(void *base,u32 size,u32 align_size);
void    kmem_heap_info(u32 *total,u32 *used,u32 *max_used);
void*	kmalloc(u32	size);
void	kfree(void *p);

void*	stack_malloc(u32 size);
void	stack_free(void *p);

BOOL	SysDrv_SetLocalTime(const local_time_t *tm);
BOOL	SysDrv_GetLocalTime(local_time_t *tm);

int		SysDrv_stdin(char *buf,int size);
int		SysDrv_stdout(const char *buf,int size);
int		SysDrv_stderr(const char *buf,int size);

BOOL	SysDrv_GetSystemPowerStatus(SYSTEM_POWER_STATUS* lpSystemPowerStatus);
u32		SysDrv_SetSystemPowerState(u32 StateFlags);
void	SysDrv_GetProcessorInfo(PROCESSOR_INFO *Info);

void	HAL_FlushCacheAll(void);

/*=========================================================================================*/

BOOL	PROC_MemInit(X_PROCESS *pcb,int pid,u8 *va,u32 size);
void	PROC_MemFree(X_PROCESS *pcb,int pid);


/*=========================================================================================*/

#endif /* SRC_INC_KERNEL_ARCH_H_ */
