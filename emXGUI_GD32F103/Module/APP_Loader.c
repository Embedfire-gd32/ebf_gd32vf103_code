
#include "Include.h"
#include "x_file.h"

//static APP_Execu(void *addr,)

void APP_Loader(void)
{
	void *p;
	X_FILE *fp;
	int fsize;

	char buf[128];

//	SYS_msleep(1000);
#if 0
	api =(__api_entry_t*)((u32)asm_test&0xFFFFFFFE);
	api->addr =(u32)GetSysApiById(0x0040); //DebugPuts

	asm_test("0");
	asm_test("1");
#endif



	if(!CDlg_FileExpoler(NULL,20,30,300,300,L"APP Loader","B:",buf))
	{
		return;
	}

	SYS_execu(buf,NULL,0);
	return;

#if 0
	p =(void*)0x80C00000;
	//p =(void*)0x80C40000;
	//fp =x_fopen("B:AppTemplate.app","rb");
	fp =x_fopen(buf,"rb");
	if(fp!=NULL)
	{
		fsize =x_fsize(fp);
		x_fread(p,fsize,1,fp);
		x_fclose(fp);

		APP_InitHead(p,1);
		SCB_CleanInvalidateDCache();

#if 1
		const char *cur_dir;
		cur_dir ="B:apps/AATech_CN";
		//cur_dir ="B:apps/Fit240";
		SYS_set_process_data(PROC_CUR_DIR,(void*)cur_dir,x_strlen(cur_dir)+1);
#endif

		//SYS_thread_create(app_entry,NULL,8192,NULL,0);
		SYS_thread_create((pfn_thread_entry)p,NULL,8192,NULL,0);
	}
#endif

}
