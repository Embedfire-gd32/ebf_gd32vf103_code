
#include <stdint.h>
#include <string.h>

#include "riscv_encoding.h"
#include "riscv_bits.h"
#include "n200_eclic.h"
#include "n200_timer.h"
#include "n200_func.h"

#include  "gd32vf103.h"

#include "BSP.h"
#include "MicroKernel_Arch.h"

/*=========================================================================================*/

#if (__FPU_PRESENT==1)&&(__FPU_USED==1)
#define	OS_CPU_FP_EN 1
#else
#define	OS_CPU_FP_EN 0
#endif

typedef	u32	CPU_STK;

u32 g_task_swtich=0;

extern struct _mk_thread *OSTCBCur;
extern struct _mk_thread *OSTCBNew;

void*	HAL_StackInit(void *entry,void *exit, void *argv, void *stk_base, U32 stk_size)
{

	volatile CPU_STK  *stk;

	memset(stk_base,'C',stk_size);

	//pdata should be restore in a0
	stk   = (CPU_STK *)stk_base;
	stk   = &stk[stk_size>>2];
	stk--;
	stk   = (CPU_STK *)((CPU_STK)(stk) & 0xFFFFFFF8u);
	stk  -= 32;


	stk[0] =(CPU_STK)exit; //ra,meaningless ,task shuldn't exit.
	stk[1] =0x1880;//mstatus.set 0x80 ,global interrupt will be set when task begin.
	stk[2] =0;//unused
	stk[3] =0;//unused
	stk[4] =0;
	stk[5] =0;
	stk[6] =0;
	stk[7] =0;
	stk[8] =0;
	stk[9] =(CPU_STK)argv;

	stk[10] =0;
	stk[11] =0;
	stk[12] =0;
	stk[13] =0;
	stk[14] =0;
	stk[15] =0;
	stk[16] =0;
	stk[17] =0;
	stk[18] =0;
	stk[19] =0;
	stk[20] =0;
	stk[21] =0;
	stk[22] =0;
	stk[23] =0;
	stk[24] =0;
	stk[25] =0;
	stk[26] =0;
	stk[27] =0;
	stk[28] =0;
	stk[29] =0;
	stk[30] =0;
	stk[31] =(CPU_STK)entry;//mepc

    return (void*)stk;
}

/*=================================================================================================*/


void HAL_ContexSwitchInISR(void)
{
	 g_task_swtich = 1;
}

/*=================================================================================================*/


#ifndef	OS_TICK_HZ
#define	OS_TICK_HZ	200
#endif

static void	TickTimer_Init(void)
{
	   /* ----------------------------------------------------------------------------
	    TIMER1 Configuration:
	    TIMER1CLK = SystemCoreClock/5400 = 20KHz.
	    TIMER1 configuration is timing mode, and the timing is 0.2s(4000/20000 = 0.2s).
	    CH0 update rate = TIMER1 counter clock/CH0CV = 20000/4000 = 5Hz.
	    ---------------------------------------------------------------------------- */
	    timer_oc_parameter_struct timer_ocinitpara;
	    timer_parameter_struct timer_initpara;

	    rcu_periph_clock_enable(RCU_TIMER1);

	    timer_deinit(OS_TIMER);
	    /* initialize TIMER init parameter struct */
	    timer_struct_para_init(&timer_initpara);
	    /* TIMER1 configuration */
	    timer_initpara.prescaler         = (16-1);
	    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
	    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
	    timer_initpara.period            = SystemCoreClock/(OS_TICK_HZ*16);
	    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
	    timer_init(OS_TIMER, &timer_initpara);
#if 0
	    /* initialize TIMER channel output parameter struct */
	    timer_channel_output_struct_para_init(&timer_ocinitpara);
	    /* CH0,CH1 and CH2 configuration in OC timing mode */
	    timer_ocinitpara.outputstate  = TIMER_CCX_ENABLE;
	    timer_ocinitpara.ocpolarity   = TIMER_OC_POLARITY_HIGH;
	    timer_ocinitpara.ocidlestate  = TIMER_OC_IDLE_STATE_LOW;
	    timer_channel_output_config(OS_TIMER, TIMER_CH_0, &timer_ocinitpara);
#endif


#if 0
	    /* CH0 configuration in OC timing mode */
	    timer_channel_output_pulse_value_config(OS_TIMER, TIMER_CH_0, 2000);
	    timer_channel_output_mode_config(OS_TIMER, TIMER_CH_0, TIMER_OC_MODE_TIMING);
	    timer_channel_output_shadow_config(OS_TIMER, TIMER_CH_0, TIMER_OC_SHADOW_DISABLE);
#endif

	    timer_interrupt_enable(OS_TIMER, TIMER_INT_CH0);
	    timer_enable(OS_TIMER);

		eclic_irq_enable(OS_TimerIRQn,1,0);
		eclic_set_posedge_trig(OS_TimerIRQn);

}

/*=============================================================================================================*/

u32	HAL_TicksToTime(u32 ticks)
{
	u32 time;
 	
	time	=((ticks*1000U)/OS_TICK_HZ);
 	return	time;	

}

u32	HAL_TimeToTicks(u32 time_ms)
{
	u32 ticks;
 	
 	ticks	=(time_ms*OS_TICK_HZ)/1000U;
 	if(ticks<=0)	
 	{
 		ticks=1;
 	}
 	
 	return ticks;

}


/*=================================================================================================*/
//static int a=0;
void  OS_TimerIRQHandler(void)
//void  xxMTIME_HANDLER(void)
{

	mk_isr_enter();
	
    mk_tick_handler(OS_TICK_HZ);
   // Board_OnTick();

#if	0
	{
	  static int a=0;
	  if(a++ > TICK_HZ)
	  {
		LED1_INVERT();
	    //DebugPuts("^");
		a=0;
	  }
	 }
#endif
	timer_interrupt_flag_clear(TIMER1,TIMER_INT_FLAG_CH0);
	eclic_clear_pending(TIMER1_IRQn);

    mk_isr_exit();
	 
 }	 



/*=================================================================================================*/

void	Hook_TaskSW(void)
{

}

void	Hook_ThreadDestroy(struct _mk_thread *tcb)
{

	//SCB_CleanInvalidateDCache();
	//SCB_InvalidateICache();
}

/*=================================================================================================*/

void Hook_IsrEnter(u8 nest)
{

}

void Hook_IsrExit(void)
{

}


void Hook_IdleThreadStart(void)
{
	TickTimer_Init();
	//DebugPuts("Hook_IdleStart.\r\n");
}
 
void Hook_IdleThreadLoop(void)
{
	
}

/*=================================================================================================*/
