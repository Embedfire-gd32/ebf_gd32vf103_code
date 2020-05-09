
#ifndef   __MICRO_KERNEL_H__
#define   __MICRO_KERNEL_H__

#ifdef __cplusplus
extern "C" {
#endif

#include	"def.h"
#include	"list.h"

typedef	struct	_mk_thread	mk_thread_t;
typedef	struct	_mk_mutex	mk_mutex_t;
typedef	struct	_mk_sem		mk_sem_t;
typedef	struct	_mk_queue	mk_queue_t;

typedef	int		fn_mk_thread_entry(void *argv);
typedef	void	fn_mk_thread_exit(int exit_code);

/*============================================================================================*/

#define	MK_ERR_NONE			0
#define	MK_ERR_PTR_NULL		1	//NULL Pointer
#define	MK_ERR_PTR_INVALID  2
#define	MK_ERR_NOT_RUNNING	3
#define	MK_ERR_PEND_ISR		4
#define	MK_ERR_PEND_LOCKED	5
#define	MK_ERR_TIMEOUT		6
#define	MK_ERR_THREAD_CREATE_ISR	7
#define	MK_ERR_THREAD_DEL	8
#define	MK_ERR_PRIO_INVALID	9

#define	MK_ERR_SEM_OVF		10
#define	MK_ERR_QUEUE_FULL	11

#define	MK_ERR				128

/*============================================================================================*/

struct _mk_thread{
   
    void    *stk_cur;       // 指向当前栈指针
	list_t   node;
    list_t   node_event;    // 事件等待节点
    list_t   node_delay;    // 延时节点

    u32 sleep_ticks;        // 延时节拍数

    u8  status;             // 线程状态                                      
    u8  rsv1;
    u8  prio_cur;           // 当前优先级
    u8	prio_base;			// 基本优先级  
        
    u32	flags;				// 高16位:系统标记;低16位:用户标记
    u32	pid;
  
    fn_mk_thread_exit    *exit;
    fn_mk_thread_entry   *entry;
	void	*argv;   
	void	*stk_base;
	u32	     stk_size;
	void	*msg_ptr;
	void	*user_data_ptr;
	
};

/*===================================================================================*/

typedef	struct	_mk_event_obj
{
	list_t	list_wait;
	
}mk_event_obj_t;



struct	_mk_mutex
{
	mk_event_obj_t	event_obj;
    mk_thread_t	*owner;
 	u16	recursive;
 	u16	value;
};

struct	_mk_sem
{
    mk_event_obj_t	event_obj;	
    u32	value;
    u32	max;
};

struct	_mk_queue
{

    mk_event_obj_t	event_obj;
	
    u32	flag;
	u32	size;	//可容纳消息总数量
 
 	void **msg_buffer;
	u32	msg_in;
	u32	msg_out;   
 	u32	msg_num;	
		
};


/*============================================================================================*/

void  	mk_tick_handler(u32 tick_hz);

void  	mk_init(void *idle_stk_base,u32 idle_stk_size);
void    mk_startup(void);
void	mk_tick_handler(u32 tick_hz);
void	mk_sched(void);

void 	mk_sched_lock(void);
void  	mk_sched_unlock(void);
void 	mk_isr_enter(void);
void  	mk_isr_exit(void);

/*============================================================================================*/

int		sizeof_mk_thread_obj(void);
int		sizeof_mk_mutex_obj(void);
int		sizeof_mk_sem_obj(void);
int		sizeof_mk_queue_obj(void);

void*		mk_thread_entry(mk_thread_t *ptcb);
u32	    	mk_thread_flags(mk_thread_t *ptcb);
mk_thread_t*	mk_thread_self(void);
void*		mk_thread_handle(mk_thread_t *ptcb);
u32	    	mk_thread_id(mk_thread_t *ptcb);
int		mk_thread_priority(mk_thread_t *ptcb);
void*		mk_thread_stack_base(mk_thread_t *ptcb);
void*		mk_thread_user_data(mk_thread_t *ptcb);
u32	    	mk_thread_pid(mk_thread_t *ptcb);

u8	   mk_cpu_usage(u8 cpu_idx);
u32	   mk_ticks_per_second(void);

void  	mk_sleep_ticks(u32 ticks);
void  	mk_msleep(u32 ms);
u32 	mk_get_tick(u32 *count_h);
u32 	mk_get_time(u32 *time_ms_h);

err_t  	mk_thread_init(mk_thread_t *ptcb,fn_mk_thread_entry *entry,fn_mk_thread_exit *exit,void *argv,void *stk_base,u32 stk_size,u8 prio,u32 flags,void *user_data,u32 pid);
err_t	mk_thread_change_priority(mk_thread_t *ptcb,u8 newprio);
err_t	mk_thread_suspend(mk_thread_t *ptcb);
err_t	mk_thread_resume(mk_thread_t *ptcb);
err_t	mk_thread_destroy(mk_thread_t *ptcb);

err_t	mk_sem_init(mk_sem_t *sem,u32 init_count,u32 max_count,const char *name);
err_t	mk_sem_open(mk_sem_t **sem,const char *name);
err_t	mk_sem_wait(mk_sem_t *sem,u32 wait_ms);
err_t	mk_sem_post(mk_sem_t *sem);
u32	mk_sem_count(mk_sem_t *sem);
err_t	mk_sem_destroy(mk_sem_t *sem);

err_t	mk_mutex_init(mk_mutex_t *mutex,const char *name);
err_t	mk_mutex_open(mk_mutex_t **mutex,const char *name);
err_t	mk_mutex_lock(mk_mutex_t *mutex,u32 wait_ms);
err_t	mk_mutex_unlock(mk_mutex_t *mutex);
err_t	mk_mutex_destroy(mk_mutex_t *mutex);

err_t	mk_queue_init(mk_queue_t *que,void **msg_buf,u32 queue_size,u32 flag);
err_t	mk_queue_send(mk_queue_t *que,void *msg);
err_t	mk_queue_recv(mk_queue_t *que,void **msg,u32 wait_ms);
err_t	mk_queue_flush(mk_queue_t *que);
err_t	mk_queue_destroy(mk_queue_t *que);


/*============================================================================================*/

#ifdef __cplusplus
}
#endif

#endif

