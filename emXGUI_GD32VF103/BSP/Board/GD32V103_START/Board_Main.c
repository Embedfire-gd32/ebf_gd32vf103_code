
/*=========================================================================================*/

#include  "gd32vf103.h"
#include   "BSP.h"

#define	LED1_ON()	GPIO_BC(GPIOB) =GPIO_PIN_0
#define	LED1_OFF()	GPIO_BOP(GPIOB) =GPIO_PIN_0


static void delay(int n)
{
	volatile int i;
	while(n-- > 0)
	{
		for(i=0;i<5000;i++);

	}
}
void	led_test_0(int n)
{

    rcu_periph_clock_enable(RCU_GPIOB);
    gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_0);

    while(n-- > 0)
    {
    	LED1_ON();
    	delay(200);
    	//mk_msleep(200);
    	LED1_OFF();
    	delay(200);
    	//mk_msleep(200);

    }

}

void	led_test(void)
{

 //   rcu_periph_clock_enable(RCU_GPIOB);
 //   gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_0);

    while(1)
    {
    	LED1_ON();
    	//delay(500);
    	mk_msleep(50);
    	LED1_OFF();
    	//delay(500);
    	mk_msleep(100);

    }

}

/*=========================================================================================*/

static mk_thread_t task1_tcb,task2_tcb;

static	u32 idle_stk[256];
static	u32 main_stk[512];

static int main_task(void *argv)
{
	led_test();
	return 0;
}

int main(void)
{
	SystemCoreClockUpdate();
	led_test_0(2);

	eclic_priority_group_set(ECLIC_PRIGROUP_LEVEL3_PRIO1);


	  //  eclic_set_nlbits(ECLIC_GROUP_LEVEL3_PRIO1);
	 //   eclic_irq_enable(TIMER2_IRQn,1,0);
    eclic_global_interrupt_enable();
#if 0
    eclic_irq_enable(ECLIC_INT_MTIP,1,0);
    eclic_set_posedge_trig(ECLIC_INT_MTIP);
	TickTimer_Init();
#endif

#if 0
	eclic_irq_enable(TIMER1_IRQn,1,0);
	eclic_set_posedge_trig(TIMER1_IRQn);
	timer_configuration();

#endif
	//while(1);

	mk_init(idle_stk,sizeof(idle_stk));
	mk_thread_init(&task1_tcb,main_task,NULL,NULL,main_stk,sizeof(main_stk),8,0,NULL,0);
	mk_startup();

	return 0;
}

/*=========================================================================================*/
