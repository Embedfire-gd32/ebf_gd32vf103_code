

#include "BSP.h"
#include "x_libc.h"

/*===============================================================================================*/

#if 1
#pragma	pack(4)

static u8 vmem_buf[8*KB];

#pragma	pack()
#endif

/*=========================================================================================*/

static heap_t heap_vmem;

void*	vmalloc(u32 size)
{
	//return kmalloc(size);
	return x_heap_alloc(&heap_vmem,size);

}

void	vfree(void *p)
{
	//return kfree(p);
	x_heap_free(&heap_vmem,p);
}

/*===============================================================================================*/

#if 0
void*	stack_malloc(u32 size)
{
	return kmalloc(size);

}

void	stack_free(void *p)
{
	kfree(p);
}
#endif

/*===============================================================================================*/

//static heap_obj *heap_dma=NULL;

void*	dma_mem_alloc(u32 size)
{
	//STM32F103 不能将外部SRAM作DMA ?

	//return HeapAlloc(heap_vmem,size);
	return vmalloc(size);

}

void	dma_mem_free(void *p)
{
	//HeapFree(heap_vmem,p);
	vfree(p);
}

/*===============================================================================================*/

void Board_MemInit(void)
{

//	kmem_heap_init(kmem_buf,sizeof(kmem_buf),32);
	x_heap_init(&heap_vmem,(void*)vmem_buf,sizeof(vmem_buf),32);

}

/*===============================================================================================*/

u32	GetMemTotSize(void)
{
	u32 m1;

	//x_heap_info(&heap_vmem,&m1,NULL,NULL);
	m1 =heap_vmem.total_size;

	return (m1);
}

u32	GetMemCurSize(void)
{
	u32 m1;

	//x_heap_info(&heap_vmem,NULL,&m1,NULL);
	m1 =heap_vmem.used_cur;
	return (m1);
}

