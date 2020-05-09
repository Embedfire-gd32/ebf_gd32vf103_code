#ifndef		__HAL_MICROKERNEL_H___
#define		__HAL_MICROKERNEL_H___

#ifdef	__cplusplus
extern "C"{
#endif

#include	"def.h"
#include	"list.h"

typedef u32 OS_CPU_SR;

struct _mk_thread;

/*=========================================================================================*/

void	OS_IRQHandler(void);

/*=========================================================================================*/

OS_CPU_SR	HAL_EnterCritical(void);
void	HAL_ExitCritical(OS_CPU_SR sr);

u32		HAL_TicksToTime(u32 ticks);
u32		HAL_TimeToTicks(u32 time_ms);

void	HAL_SchedulerStartup(void);

void	HAL_ContexSwitch(void);
void	HAL_ContexSwitchInISR(void);
void*	HAL_StackInit(void *entry,void *exit, void *argv, void *stk_base, u32 stk_size);

/*=========================================================================================*/

void	Hook_IdleThreadStart(void);
void	Hook_IdleThreadLoop(void);
void 	Hook_IsrEnter(u8 nest);
void 	Hook_IsrExit(void);
//void	Hook_ThreadSwitch(struct _mk_thread *new_tcb,struct _mk_thread *old_tcb);
void	Hook_ThreadDestroy(struct _mk_thread *tcb);

/*=========================================================================================*/

void	mk_tick_handler(u32 tick_hz);
void 	mk_isr_enter(void);
void  	mk_isr_exit(void);
void	mk_sched_lock(void);
void	mk_sched_unlock(void);

/*=========================================================================================*/

typedef	struct
{
	list_t n;
	void (*cb)(void);

}sys_hook_obj;

void	mk_tick_hook_create(sys_hook_obj *hook,void (*cb)(void));
void	mk_idle_hook_create(sys_hook_obj *hook,void (*cb)(void));

void	mk_hook_delete(sys_hook_obj *hook);

/*=========================================================================================*/
 
#ifdef	__cplusplus
}
#endif

#endif

