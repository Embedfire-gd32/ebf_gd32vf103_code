
/*=========================================================================================*/

#include  "gd32vf103.h"
#include   "BSP.h"

void	LED1_ON(void)
{
	GPIO_BC(GPIOB) =GPIO_PIN_0;
}
void	LED1_OFF(void)
{
	GPIO_BOP(GPIOB) =GPIO_PIN_0;
}


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

void	led_test(int n)
{

 //   rcu_periph_clock_enable(RCU_GPIOB);
 //   gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_0);

    while(n-- > 0)
    {
    	LED1_ON();
    	//delay(500);
    	mk_msleep(10);
    	LED1_OFF();
    	//delay(500);
    	mk_msleep(1000);

    }

}

/*=========================================================================================*/

static mk_thread_t task1_tcb,task2_tcb;

static	u32 idle_stk[256];
static	u32 task1_stk[512];
static	u32 task2_stk[1024];

void exmc_lcd_init(void);
void Board_MemInit(void);
void GUI_Startup(void);

static int task1(void *argv)
{
	mk_msleep(100);
	exmc_lcd_init();
	Board_MemInit();
	GUI_Startup();

	return 0;
}

/*=========================================================================================*/

void GUI_DEMO_RectSpeed(void);


static int task2(void *argv)
{
	gui_app_thread(NULL);
	while(1)
	{
		led_test(3);
		GUI_DEMO_RectSpeed();
	}

}



void	GUI_AppStartup(void)
{
	mk_thread_init(&task2_tcb,task2,NULL,NULL,task2_stk,sizeof(task2_stk),6,0,NULL,0);
}

int main(void)
{
	SystemCoreClockUpdate();
	//eclic_priority_group_set(ECLIC_PRIGROUP_LEVEL3_PRIO1);
    eclic_global_interrupt_enable();
	led_test_0(2);


	mk_init(idle_stk,sizeof(idle_stk));
	mk_thread_init(&task1_tcb,task1,NULL,NULL,task1_stk,sizeof(task1_stk),8,0,NULL,0);

	mk_startup();
	return 0;
}

/*=========================================================================================*/
