/*
 * Reset_Handler.c
 *
 *  Created on: 2016Äê11ÔÂ6ÈÕ
 *      Author: Administrator
 */


#include "def.h"
/*=========================================================================================*/

extern const u32 __data_load_addr;
extern const u32 __data_base;
extern const u32 __data_end;
extern const u32 __bss_base;
extern const u32 __bss_end;

extern const struct __copy_table __copy_tabel_start__;
extern const struct __copy_table __copy_tabel_end__;

extern const struct __clr_table __clr_tabel_start__;
extern const struct __clr_table __clr_tabel_end__;

void MPU_MemInit(void);

/*=========================================================================================*/

struct __copy_table
{
	u32 *dst,*src,size;
};

struct __clr_table
{
	u32 *dst,size;
};

/*=========================================================================================*/

static void __memcpy32(u32 *dst,u32 *src,s32 size)
{

	while(size > 0)
	{
		*dst++ = *src++;
		 size -= 4;
	}
}

static void __memset32(u32 *dst,s32 size)
{

	while(size > 0 )
	{
		*dst++ = 0x00000000;
		size -= 4;
	}
}

void Reset_Handler(void)
{
//	MPU_MemInit();

	if(1)
	{
		struct __copy_table *copy;

		copy =(struct __copy_table*)&__copy_tabel_start__;
		while((u32)copy <(u32)&__copy_tabel_end__)
		{
			__memcpy32(copy->dst,copy->src,copy->size);
			copy++;
		}
	}

	if(1)
	{
		struct __clr_table *clr;

		clr =(struct __clr_table*)&__clr_tabel_start__;
		while((u32)clr < (u32)&__clr_tabel_end__)
		{
			__memset32(clr->dst,clr->size);
			clr++;
		}
	}

#if(0)
{
	u32 *src,*dst,*end;

	src = (u32*)__data_load_addr;
	dst = (u32*)__data_base;
	end = (u32*)__data_end;
	while(dst < end)
	{
		*dst++ = *src++;
	}

	dst = (u32*)__bss_base;
	end = (u32*)__bss_end;
	while(dst < end)
	{
		*dst++ =0x0000000;
	}
}
#endif

	SystemInit();
	Board_Startup();

}

/*=========================================================================================*/
