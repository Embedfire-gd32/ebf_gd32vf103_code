									 
#include "Include.h"
#include "x_libc.h"

/*===============================================================================================================*/

void SystemModuleInit(void)
{
	SYS_msleep(300);
	////
	Board_ModuleInit();
}

/*===============================================================================================================*/
/*===============================================================================================================*/
/*===============================================================================================================*/

void	HAL_FlushCacheAll(void)
{
	//mmu_flush_cache_all();
	//mmu_flush_tlb_all();

}

/*===============================================================================================================*/


void HAL_RemapPage(int pid,void*va,void *pa,U32 flags)
{
	
}
						
							
/*===============================================================================================================*/

static volatile int PF_in_use=FALSE;
static mem_page app_PF;

BOOL	PROC_MemInit(X_PROCESS *pcb,int pid,u8 *va,u32 size)
{
	OS_CPU_SR a;
	int res=FALSE;

#if	APP_RAM_SIZE > 0
	if(size > APP_RAM_SIZE)
	{
		return FALSE;
	}

	if((u32)va < APP_RAM_BASE)
	{
		return FALSE;
	}

	if(((u32)va+size) > APP_RAM_END)
	{
		return FALSE;
	}

	a =HAL_EnterCritical();
	if(PF_in_use==FALSE)
	{
		PF_in_use=TRUE;

		app_PF.pa   =(void*)APP_RAM_BASE;
		app_PF.next =NULL;

		pcb->PF_Size =APP_RAM_SIZE;
		pcb->list_L2 =NULL;
		pcb->list_PF =&app_PF;
		pcb->L1_base =NULL;

		res =TRUE;
	}
	HAL_ExitCritical(a);
#endif

	return res;

}

void	PROC_MemFree(X_PROCESS *pcb,int pid)
{
	OS_CPU_SR a;

	a =HAL_EnterCritical();
	PF_in_use=FALSE;
	HAL_ExitCritical(a);
}

/*=========================================================================================*/

u8*	PageFrame_PA2VA(u8 *pa)
{
	return pa;
}

/*=========================================================================================*/
/*===============================================================================================================*/
/*===============================================================================================================*/
	
void	HAL_MTT_Switch(U32 new_pid,U32 old_pid)
{

	
}


/*===============================================================================================================*/

int	HAL_SetNetConfig(const net_config_t *cfg)
{
//	return	NetDev_SetConfig(cfg);
}

int	HAL_GetNetConfig(net_config_t *cfg)
{
	//return	NetDev_GetConfig(cfg);
}


/*===============================================================================================================*/

void	SysDrv_GetProcessorInfo(PROCESSOR_INFO *info)
{
	if(info)
	{
		
		x_wstrcpy((WCHAR*)info->Family,L"Cortex-M4");
		x_wstrcpy((WCHAR*)info->String,L"STM32F4xx");
		info->ID	=0x0000F427;
		info->Clock	=168;

	}
}

/*===============================================================================================================*/
