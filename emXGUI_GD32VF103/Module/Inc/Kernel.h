#ifndef	__KERNEL_H_20170314_1035__
#define	__KERNEL_H_20170314_1035__

#ifdef	__cplusplus
extern "C"{
#endif

#include	"def.h"

/*=========================================================================================*/

typedef	int	sys_err_t;


/*=========================================================================================*/
// SYS_ERR Code
#define	SYS_NO_ERR				(0)
#define	SYS_ERR_PTR_NULL		(-1)
#define	SYS_ERR_TIMEOUT			(-2)
#define	SYS_ERR_UNKNOW			(-32767)

#define	SYS_ERR_NONE			SYS_NO_ERR
#define	SYS_ERR					SYS_ERR_UNKNOW

/*=========================================================================================*/

typedef		struct _mk_mutex 	mutex_obj;
typedef		struct _mk_sem 		sem_obj;
typedef		struct _mk_queue	mbox_obj;
typedef		struct _mk_thread	thread_obj;

typedef	int	(*pfn_thread_entry)(void *argv);

typedef struct _local_time{
	u8  rsv;
  	u8  sec;
  	u8  min;
	u8  hour;
	u8  day;
  	u8  mon;
	u16 year;	
		
}local_time_t;

/*=========================================================================================*/

#define	SYS_stdin	(file_obj*)0
#define	SYS_stdout	(file_obj*)1
#define	SYS_stderr	(file_obj*)2

/*=========================================================================================*/

#define	PROC_PID		0x0001
#define	PROC_PATH		0x0002
#define	PROC_CUR_DIR	0x0003

/*=========================================================================================*/

//COM Flags
#define	COM_DATA_BITS_MASK		(3<<0)
#define	COM_DATA_BITS_5			(0<<0)
#define	COM_DATA_BITS_6			(1<<0)
#define	COM_DATA_BITS_7			(2<<0)
#define	COM_DATA_BITS_8			(3<<0)

#define	COM_STOP_BITS_MASK		(3<<2)
#define	COM_STOP_BITS_1			(0<<2)
#define	COM_STOP_BITS_1_5		(1<<2)
#define	COM_STOP_BITS_2			(2<<2)	

#define	COM_PARITY_MASK			(7<<4)
#define	COM_PARITY_NONE			(0<<4)
#define	COM_PARITY_ODD			(1<<4)
#define	COM_PARIRY_EVEN			(2<<4)
#define	COM_PARITY_MARK			(3<<4)
#define	COM_PARITY_SPACE		(4<<4)

#define	COM_FLOW_CONTROL_MASK	(3<<7)
#define	COM_FLOW_CONTROL_NONE	(0<<7)
#define	COM_FLOW_CONTROL_AUTO	(1<<7)
#define	COM_FLOW_CONTROL_MANUAL	(2<<7)

#define	COM_INTERFACE_MASK		(1<<9)
#define	COM_INTERFACE_NORMAL	(0<<9)
#define	COM_INTERFACE_IR		(1<<9)

//COM Event
#define	COM_EVT_RXCHAR		(1<<0)	// Any Character received
#define	COM_EVT_TXREADY		(1<<1)	// Transmitt Ready
#define	COM_EVT_RXTIMEOUT	(1<<2)	// Received Timeout
#define	COM_EVT_CTS			(1<<3)	// CTS changed state
#define	COM_EVT_DSR			(1<<4)	// DST changed state
#define	COM_EVT_RLSD		(1<<5)	// RLSD changed state
#define	COM_EVT_ERR			(1<<6)	// Line status error occurred

//COM IOCTL
#define	COM_IOCTL_SET_CFG		0x0001
#define	COM_IOCTL_GET_CFG		0x0002
#define	COM_IOCTL_WAIT_EVENT	0x0003

typedef	struct	com_cfg
{
	u32	Baudrate;   //波特率
	u32	Flags;      //串口标记
	u32	RxTimeOut;  //接收超时
 	
}com_cfg_t;

/*=========================================================================================*/

#define	KERNEL_IOCTL_GET_SYSTEM_INFO		0x0001
#define	KERNEL_IOCTL_GET_PROCESSOR_INFO		0x0002
#define	KERNEL_IOCTL_GET_MEMORY_INFO		0x0003
#define	KERNEL_IOCTL_GET_CPU_USAGE			0x0004
#define	KERNEL_IOCTL_GET_SYSTEM_POWER_STATE	0x0005
#define	KERNEL_IOCTL_SYSTEM_SLEEP			0x0006
#define	KERNEL_IOCTL_SYSTEM_REBOOT			0x0007
#define	KERNEL_IOCTL_SYSTEM_POWER_OFF		0x0008
#define	KERNEL_IOCTL_SYSTEM_RANDVAL			0x0010

typedef	struct	_SYSTEM_INFO
{
	
	u32	Version;
	u32	BuildDate;
	u32	BuildTime;
	u32	PageSize;
	
}SYSTEM_INFO;

typedef	struct	_PROCESSOR_INFO
{
	char	Family[16];
	char	String[16];
	u32	ID;
	u32	Clock;
}PROCESSOR_INFO;

typedef	struct	_MEMORY_INFO
{
	u32	TotalSize;
	u32	UsedSize;
	
}MEMORY_INFO;

/*=========================================================================================*/

//
// Power Management
//

//SetSystemPowerState
#define POWER_NAME              (u32)(0x00000001)
#define POWER_FORCE             (u32)(0x00001000)

#define POWER_STATE(f)           ((f) & 0xFFFF0000)       // power state mask
#define POWER_STATE_ON           (u32)(0x00010000)        // on state
#define POWER_STATE_OFF          (u32)(0x00020000)        // no power, full off
#define POWER_STATE_CRITICAL     (u32)(0x00040000)        // critical off
#define POWER_STATE_BOOT         (u32)(0x00080000)        // boot state
#define POWER_STATE_IDLE         (u32)(0x00100000)        // idle state
#define POWER_STATE_SUSPEND      (u32)(0x00200000)        // suspend state
#define POWER_STATE_UNATTENDED   (u32)(0x00400000)        // Unattended state.
#define POWER_STATE_RESET        (u32)(0x00800000)        // reset state
#define POWER_STATE_USERIDLE     (u32)(0x01000000)        // user idle state
#define POWER_STATE_PASSWORD     (u32)(0x10000000)        // This state is password protected.

//GetSystemPowerStatus

//PowerFlag
#define POWER_FLAG_OFFLINE	      0x00
#define POWER_FLAG_CRITICAL       0x01
#define POWER_FLAG_LOW            0x02
#define POWER_FLAG_HIGH           0x04
#define POWER_FLAG_CHARGING       0x08
#define POWER_FLAG_ACLINE         0x10
#define POWER_FLAG_UNKNOWN        0xFF


#define POWER_PERCENTAGE_UNKNOWN    0xFF
#define	POWER_VOLTAGE_UNKNOWN		0xFFFF
#define	POWER_CURRENT_UNKNOWN		0xFFFF
#define POWER_LIFE_UNKNOWN          0xFFFFFFFF

typedef struct {

	   u8  ExPowerFlag;
	   u8  ExPowerLifePercent;
	   u8  BatteryFlag;
	   u8  BatteryLifePercent;

	   u16 ExPowerVoltage;
	   u16 ExPowerCurrent;
	   u32 ExPowerLifeTime;
	   u32 ExPowerFullLifeTime;

	   u16 BatteryVoltage;
	   u16 BatteryCurrent;
	   u32 BatteryLifeTime;
	   u32 BatteryFullLifeTime;

}SYSTEM_POWER_STATUS;

/*=========================================================================================*/
/*函数原型*/

//// Common
u32	RandVal(void);

void*	kmalloc(u32	size);
void	kfree(void *p);

void*	vmalloc(u32 size);
void	vfree(void *p);

int		SYS_kernel_ioctl(int cmd,void *pdata,int size);

////Debug
//void	DebugPutch(char data);
//int		DebugGetch(char *data);
void 	DebugPuts(const char *lpString);
int 	DebugGets(char *lpBuffer,u32 size);
void 	DebugPutHex(u32 Offset,u32 Count,CONST void *Buf);
void	DebugPrintf(const char *fmt,...);

void    ErrorOutput(const char *fmt,...);
void    WarningOutput(const char *fmt,...);
void    DebugOutput(const char *fmt,...);

/*=========================================================================================*/


//// OS
u32	SYS_get_time_count(u32 *time_h);
sys_err_t	SYS_set_local_time(const local_time_t *tm);
sys_err_t	SYS_get_local_time(local_time_t *tm);


mutex_obj*	SYS_mutex_create(const char *name);
mutex_obj*	SYS_mutex_open(const char *name);
sys_err_t	SYS_mutex_lock(mutex_obj *mutex,u32 wait_ms);
sys_err_t	SYS_mutex_unlock(mutex_obj *mutex);
sys_err_t	SYS_mutex_destroy(mutex_obj *mutex);

sem_obj* 	SYS_sem_create(u32 init_val,u32 max_val,const char *name);
sem_obj* 	SYS_sem_open(const char *name);
sys_err_t	SYS_sem_wait(sem_obj *sem,u32 wait_ms);
sys_err_t	SYS_sem_post(sem_obj *sem);
sys_err_t 	SYS_sem_getvalue(sem_obj *sem,s32 *val);
sys_err_t	SYS_sem_destroy(sem_obj *sem);

mbox_obj*	SYS_mbox_create(u32 mbox_size,u32 flag);
sys_err_t	SYS_mbox_send(mbox_obj *mbox,void *msg);
sys_err_t	SYS_mbox_recv(mbox_obj *mbox,void **msg,u32 wait_ms);
sys_err_t	SYS_mbox_flush(mbox_obj *mbox);
sys_err_t	SYS_mbox_destroy(mbox_obj *mbox);

thread_obj*	SYS_thread_create(pfn_thread_entry entry,void *argv,u32 stk_size,void *stk_base,u32 flags);
thread_obj*	SYS_thread_self(void);
void*	SYS_thread_user_data(thread_obj *thrd);
int		SYS_thread_set_priority(thread_obj *thrd,int prio);
int		SYS_thread_get_priority(thread_obj *thrd);
int		SYS_thread_suspend(thread_obj *thrd);
int		SYS_thread_resume(thread_obj *thrd);
int		SYS_thread_destroy(thread_obj *thrd,int exit_code);
HANDLE	SYS_execu(const char *path, void *p_argv,u32 flag);
void	SYS_exit(int exit_code);
void 	SYS_msleep(u32 time_ms);
sys_err_t	SYS_set_process_data(int cmd,void *pdata,int size);
sys_err_t	SYS_get_process_data(int cmd,void *pdata,int size);


/*=========================================================================================*/


#define	MAX_PATH	255

 ////open: oflags
#define	O_ACCMODE	0x00000003
#define	O_RDONLY	0x00000000	//只读模式
#define	O_WRONLY	0x00000001	//只写模式
#define	O_RDWR		0x00000002	//读写模式


#define	O_CREATE	0x00000100	//如果指定文件不存在，则创建这个文件
#define	O_EXCL		0x00000200 	//如果同时指定了O_CREAT，而文件已经存在，则出错
#define	O_NOCTTY	0x00000400 	//如果要创建的文件已存在，则返回 -1，并且修改 errno 的值

#define	O_TRUNC		0x00001000	//如果此文件存在，而且为只读或只写成功打开，则将其长度截短为0(总是创建)
#define	O_APPEND	0x00002000  //每次写操作都写入文件的末尾
#define O_NONBLOCK  0x00004000	//如果pathname指的是一个FIFO,一个块特殊文件或一个字符特殊文件,则此选择项为此文件的本次打开操作和后续的I / O操作设置非阻塞方式 .


#define	O_SYNC      0x00010000	//使每次write都等到物理I/O操作完成
#define	O_DSYNC     0x00020000 	//等待物理 I/O 结束后再 write。在不影响读取新写入的数据的前提下，不等待文件属性更新。
#define	O_RSYNC     0x00040000	//read 等待所有写入同一区域的写操作完成后再进行

#define	O_DIRECT	0x00100000	//直接磁盘操作标识，每次读写都不使用内核提供的缓存，直接读写磁盘设备
#define	O_LARGEFILE	0x00200000	//大文件标识
#define	O_DIRECTORY	0x00400000	//必须是目录

#define O_NDELAY    O_NONBLOCK

//// SEEK
#define	SEEK_SET	0
#define	SEEK_CUR	1
#define	SEEK_END	2

//File  Attribute
#define	FA_RDO	0x01	/* Read only */
#define	FA_HID	0x02	/* Hidden */
#define	FA_SYS	0x04	/* System */
#define FA_DIR	0x08	/* Directory */

typedef	struct	file_stat
{
 	u32 	attr; 			// 文件属性
	u32 	size_l; 		// 文件长度低32位
	u32 	size_h; 		// 文件长度高32位
	local_time_t time_c; 	// 文件创建时间
	local_time_t time_a; 	// 文件最后一次访问时间
	local_time_t time_m; 	// 文件最后一次修改时间

} file_stat_t;

typedef struct __file_obj file_obj;
typedef struct __dir_obj  dir_obj;

struct dir_info{
	u32 index;
	u32 attr;
	u32 fsize;
	int   name_buf_size;
	char *name_buf;
};

file_obj*   SYS_open(const char *path,int flags,int mode);
sys_err_t 	SYS_close(file_obj *fp);
sys_err_t	SYS_ioctl(file_obj *fp,int cmd,void *argv,int size);
sys_err_t	SYS_sync(file_obj *fp);
sys_err_t	SYS_seek(file_obj *fp,s32 offset_l,const s32 *offset_h,int mode);
u32			SYS_fpos(file_obj *fp,s32 *pos_h);
u32			SYS_fsize(file_obj *fp,s32 *size_h);
s32 	    SYS_write(file_obj *fp,const void *buf,s32 size);
s32 	    SYS_read(file_obj *fp,void *buf,s32 size);
sys_err_t	SYS_remove(const char *path);
sys_err_t	SYS_getstat(const char *path,file_stat_t *fstat);
sys_err_t	SYS_mkdir(const char *path);
dir_obj*    SYS_opendir(const char *path);
sys_err_t   SYS_readdir(dir_obj *dir,struct dir_info *d_info);
sys_err_t   SYS_closedir(dir_obj *dir);
sys_err_t	SYS_rename(const char *old_path,const char *new_path);

/*=========================================================================================*/

typedef	struct	__dev_obj
{
	const void *dev_obj_pdata;

}dev_obj_t;

typedef	struct	__dev_open
{
	const void *dev_open_pdata;
	//u32	flags;

}dev_open_t;

typedef	struct	__dev_ops
{
	sys_err_t	(*create)	(dev_obj_t *dev,const void *create_data);
	sys_err_t	(*release)	(dev_obj_t *dev);
	sys_err_t	(*open)		(dev_open_t *open,dev_obj_t *dev);
	sys_err_t	(*close)	(dev_open_t *open);
	sys_err_t	(*ioctl)	(dev_open_t *open,int cmd,void *argv,int size);
	sys_err_t	(*sync)		(dev_open_t *open);

	s32		(*write)	(dev_open_t *open, const void *buf,s32 size);
	s32		(*read)		(dev_open_t *open, void *buf,s32 size);

}dev_ops_t;

BOOL	SYS_dev_create(const char *name,const dev_ops_t *ops,const void *private_data);
BOOL	SYS_dev_delete(const char *name);

/*=========================================================================================*/
//// NET

typedef	struct	__net_config{
	u8 ip_addr[8];
	u8 gateway[8];
	u8 net_mask[8];
	u8 mac_addr[8];

} net_config_t;

typedef enum {
  NET_TYPE_INVALID    = 0,
  /* NETCONN_TCP Group */
  NET_TYPE_TCP        = 0x10,
  /* NETCONN_UDP Group */
  NET_TYPE_UDP        = 0x20,
  NET_TYPE_UDPLITE    = 0x21,
  NET_TYPE_UDPNOCHKSUM= 0x22,
  /* NETCONN_RAW Group */
  NET_TYPE_RAW        = 0x40
}net_type_t;

//// Net Error Codes;
#define NET_ERR_NONE      0    /* No error, everything OK. */
#define NET_ERR_MEM    	 -1    /* Out of memory error.     */
#define NET_ERR_BUF    	 -2    /* Buffer error.            */
#define NET_ERR_TIMEOUT  -3    /* Timeout.                 */
#define NET_ERR_RTE      -4    /* Routing problem.         */
#define NET_ERR_ABORT    -5    /* Connection aborted.      */
#define NET_ERR_RESET    -6    /* Connection reset.        */
#define NET_ERR_CLOSED   -7    /* Connection closed.       */
#define NET_ERR_NOTCONN  -8    /* Not connected.           */
#define NET_ERR_VAL        -9    /* Illegal value.           */
#define NET_ERR_ARG        -10   /* Illegal argument.        */
#define NET_ERR_USE        -11   /* Address in use.          */
#define NET_ERR_IF         -12   /* Low-level netif error    */
#define NET_ERR_ISCONN     -13   /* Already connected.       */
#define NET_ERR_INPROGRESS -14   	/* Operation in progress    */
#define	NET_ERR_UNKNOW    -127


typedef	u16 	net_port_t;
typedef	int		net_err_t;

typedef	struct	__net_obj	net_obj_t,net_t;

typedef	struct{
	u8 addr[8];
} net_addr_t;

///////////////////////////

void		SYS_net_thread_new(void (*thread)(void *arg),void *argv,u32 stk_size,int prio);
void		SYS_net_thread_exit(void);

net_err_t	SYS_net_addr_make(net_addr_t *n_addr,u8 a1,u8 a2,u8 a3,u8 a4);
net_err_t	SYS_net_remote_addr(net_obj_t *net,net_addr_t *n_addr,net_port_t *port);
net_err_t	SYS_net_local_addr(net_obj_t *net,net_addr_t *n_addr,net_port_t *port);
net_type_t	SYS_get_net_type(net_obj_t *net);
net_err_t	SYS_net_get_error(net_obj_t *net);
net_err_t	SYS_net_gethostbyname(const char *url,net_addr_t *n_addr);
net_obj_t*	SYS_net_new(net_type_t type);
net_err_t	SYS_net_shutdown(net_obj_t *net);
net_err_t	SYS_net_close(net_obj_t *net);
net_err_t	SYS_net_connect(net_obj_t *net,net_addr_t *n_addr,net_port_t port);
net_err_t	SYS_net_bind(net_obj_t *net,net_addr_t *n_addr,int port);
net_err_t	SYS_net_listen(net_obj_t *net,int backlog);
net_err_t	SYS_net_accept(net_obj_t *net,net_obj_t **net_new);
s32		SYS_net_send(net_obj_t *net,void *data,s32 len);
s32		SYS_net_sendto(net_obj_t *net,void *data,s32 len,net_addr_t *n_addr,net_port_t port);
s32		SYS_net_recv(net_obj_t *net,void *data,s32 len);
s32		SYS_net_recvfrom(net_obj_t *net,void *data,s32 len,net_addr_t *n_addr,net_port_t *port);

/*=========================================================================================*/

//// PowerManage
BOOL	GetSystemPowerStatus(SYSTEM_POWER_STATUS* lpSystemPowerStatus);
u32	    SetSystemPowerState(const char *psState,u32 StateFlags,u32 Options);


//// Kernel Misc
BOOL	QueryPerformanceCounter(u32 *HighPart,u32 *LowPart);
BOOL	QueryPerformanceFrequency(u32 *fre);
void	GetProcessorInfo(PROCESSOR_INFO *Info);


/*=========================================================================================*/
//// PlaySound Mode
/*
#define	PLAYSND_STOP	(0<<0)
#define	PLAYSND_SINGLE	(1<<0)
#define	PLAYSND_LOOP	(2<<0)
#define	PLAYSND_ASYNC	(1<<2)
*/
#define PLAYSND_CH_MASK (0xF<<0)
#define PLAYSND_CH0     (0<<0)
#define PLAYSND_CH1     (1<<0)

BOOL	PlaySound(const char *file_name,u32 flag);

/*=========================================================================================*/
/*=========================================================================================*/

#ifdef	__cplusplus
}
#endif

#endif	//__KERNEL_H_20150809_0044__
