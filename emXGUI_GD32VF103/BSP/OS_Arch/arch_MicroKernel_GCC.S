
#include "riscv_encoding.h"
#include "riscv_bits.h"
#include "n200_eclic.h"

/*=========================================================================================*/

#if (defined(__VFP_FP__) && !defined(__SOFTFP__))

#define	FPU_USED 1

#else

#define	FPU_USED 0

#endif

/*=========================================================================================*/

#define	RISCV_MSTATUS_MIE         0x08
#define	RISCV_MIE_MSIE            0x08            # M Soft Interrupt bit
#define	RISCV_PRCI_BASE_ADDR      0x44000000

/*=========================================================================================*/
 	.extern g_task_swtich

	.extern  OSTCBCur
	.extern  OSTCBNew
	.extern  OSIntrNesting
//	.extern  Hook_ThreadSwitch
	.extern  Hook_TaskSW

    //.cpu    cortex-m3
    //.fpu    softvfp
    //.syntax unified
	.section .text

/*=========================================================================================*/

 	.align 8
	.global	HAL_EnterCritical
HAL_EnterCritical:
# Save the Machine status register
    csrr   a0, mstatus

# Disable global interupt
//RISCV_MSTATUS_MIE = 0x8 (0b1000) it is the bit of MIE in mstatus
    csrci mstatus,0x8
    ret


	.global	HAL_ExitCritical
HAL_ExitCritical:
# Restore the Machine status register previous state
    csrw   mstatus, a0
    ret

/*=========================================================================================*/
.macro STORE_CTX_REG
	addi sp,sp,-32*REGBYTES
	STORE     ra,   0 * REGBYTES(sp)
    STORE     t0,   4 * REGBYTES(sp)
    STORE     t1,   5 * REGBYTES(sp)
    STORE     t2,   6 * REGBYTES(sp)
    STORE     s0,   7 * REGBYTES(sp)
    STORE     s1,   8 * REGBYTES(sp)
    STORE     a0,   9 * REGBYTES(sp)
    STORE     a1,  10 * REGBYTES(sp)
    STORE     a2,  11 * REGBYTES(sp)
    STORE     a3,  12 * REGBYTES(sp)
    STORE     a4,  13 * REGBYTES(sp)
    STORE     a5,  14 * REGBYTES(sp)
    STORE     a6,  15 * REGBYTES(sp)
    STORE     a7,  16 * REGBYTES(sp)
    STORE     s2,  17 * REGBYTES(sp)
    STORE     s3,  18 * REGBYTES(sp)
    STORE     s4,  19 * REGBYTES(sp)
    STORE     s5,  20 * REGBYTES(sp)
    STORE     s6,  21 * REGBYTES(sp)
    STORE     s7,  22 * REGBYTES(sp)
    STORE     s8,  23 * REGBYTES(sp)
    STORE     s9,  24 * REGBYTES(sp)
    STORE     s10, 25 * REGBYTES(sp)
    STORE     s11, 26 * REGBYTES(sp)
    STORE     t3,  27 * REGBYTES(sp)
    STORE     t4,  28 * REGBYTES(sp)
    STORE     t5,  29 * REGBYTES(sp)
    STORE     t6,  30 * REGBYTES(sp)

.endm

.macro LOAD_CTX_REG
    LOAD     ra,   0 * REGBYTES(sp)
    LOAD     t0,   4 * REGBYTES(sp)
    LOAD     t1,   5 * REGBYTES(sp)
    LOAD     t2,   6 * REGBYTES(sp)
    LOAD     s0,   7 * REGBYTES(sp)
    LOAD     s1,   8 * REGBYTES(sp)
    LOAD     a0,   9 * REGBYTES(sp)
    LOAD     a1,  10 * REGBYTES(sp)
    LOAD     a2,  11 * REGBYTES(sp)
    LOAD     a3,  12 * REGBYTES(sp)
    LOAD     a4,  13 * REGBYTES(sp)
    LOAD     a5,  14 * REGBYTES(sp)
    LOAD     a6,  15 * REGBYTES(sp)
    LOAD     a7,  16 * REGBYTES(sp)
    LOAD     s2,  17 * REGBYTES(sp)
    LOAD     s3,  18 * REGBYTES(sp)
    LOAD     s4,  19 * REGBYTES(sp)
    LOAD     s5,  20 * REGBYTES(sp)
    LOAD     s6,  21 * REGBYTES(sp)
    LOAD     s7,  22 * REGBYTES(sp)
    LOAD     s8,  23 * REGBYTES(sp)
    LOAD     s9,  24 * REGBYTES(sp)
    LOAD     s10, 25 * REGBYTES(sp)
    LOAD     s11, 26 * REGBYTES(sp)
    LOAD     t3,  27 * REGBYTES(sp)
    LOAD     t4,  28 * REGBYTES(sp)
    LOAD     t5,  29 * REGBYTES(sp)
    LOAD     t6,  30 * REGBYTES(sp)
    addi sp,sp,32*REGBYTES
.endm

/*=========================================================================================*/


.global trap_entry
.align 6
trap_entry:

    STORE_CTX_REG

    csrr 	t0,mstatus
    STORE 	t0,1*REGBYTES(sp)


    csrr 	t0,mcause
    andi 	t0, t0, 0xff
    li 		t1, 11
    beq 	t0, t1, ecall_handler
    call 	handle_trap
    j 		restor_handler
ecall_handler:
    csrr 	t0,mepc
    addi 	t0, t0, 4
    STORE 	t0,31*REGBYTES(sp)
    jal 	task_switch
    LOAD 	t0,31*REGBYTES(sp)
    csrw 	mepc,t0

restor_handler:
    LOAD 	t0,1*REGBYTES(sp)
    csrw 	mstatus,t0

    LOAD_CTX_REG

    mret

/*=========================================================================================*/

.align 6
.global irq_entry
irq_entry: // -------------> This label will be set to MTVT2 register
    STORE_CTX_REG

    csrr 	t0,mstatus
    STORE 	t0,1*REGBYTES(sp)
    csrr 	t0,mepc
    STORE 	t0,31*REGBYTES(sp)
service_loop:
    //------This special CSR read/write operation, which is actually Claim the CLIC to find its pending highest
    // ID, if the ID is not 0, then automatically enable the mstatus.MIE, and jump to its vector-entry-label, and
    // update the link register
    csrrw 	ra, CSR_JALMNXTI, ra

    //RESTORE_CONTEXT_EXCPT_X5

    #---- Critical section with interrupts disabled -----------------------
    // Disable interrupts
    csrc 	CSR_MSTATUS, MSTATUS_MIE

    la 		t0, g_task_swtich
    LOAD 	t1,0(t0)
    li 		t0, 1
    beq 	t0, t1, irq_switch_handler
    j 		irq_restore

irq_switch_handler:
    la 		t0, g_task_swtich
    STORE 	x0, 0(t0)
    jal 	task_switch
irq_restore:
    LOAD 	t0,1*REGBYTES(sp)
    csrw 	mstatus,t0
    LOAD 	t0,31*REGBYTES(sp)
    csrw 	mepc,t0

    LOAD_CTX_REG
    // Return to regular code
    mret

/*=========================================================================================*/

.global task_switch
.align 6
task_switch:
	//restore sp when trap occur ,it's important
//	mv sp,a0

	//then store sp
	la 		t0,OSTCBCur
	LOAD 	t1,0(t0)
	STORE 	sp,0(t1)

	STORE 	ra, 3*REGBYTES(sp)
	jal 	Hook_TaskSW
	LOAD 	ra, 3*REGBYTES(sp)
	//load OSTCBCur
	la 		t0,OSTCBNew
	LOAD 	t1,0(t0)
	la 		t0,OSTCBCur
	STORE 	t1,0(t0)


	//load new sp
	la     t0, OSTCBNew
    LOAD   t1, 0(t0)
    LOAD   sp, 0(t1) //load sp
    ret

/*=========================================================================================*/

.global cpu_sr_set
.align 4
cpu_sr_set:
	csrr 	a0,mstatus
	csrci	mstatus,0x8
	ret

.global cpu_sr_reset
cpu_sr_reset:
	csrw 	mstatus,a0
	ret

/*=========================================================================================*/

	.global HAL_SchedulerStartup
HAL_SchedulerStartup:
 	//call Hook_TaskSW
    jal    Hook_TaskSW
    //jal     TickSetup


    la     t0, OSTCBNew
    LOAD   t1, 0(t0)
    LOAD   sp, 0(t1) //load sp

    LOAD 	t0,31*REGBYTES(sp)
    mv 		ra,t0

    LOAD     t0,   4 * REGBYTES(sp)
    LOAD     t1,   5 * REGBYTES(sp)
    LOAD     t2,   6 * REGBYTES(sp)
    LOAD     s0,   7 * REGBYTES(sp)
    LOAD     s1,   8 * REGBYTES(sp)
    LOAD     a0,   9 * REGBYTES(sp)
    LOAD     a1,  10 * REGBYTES(sp)
    LOAD     a2,  11 * REGBYTES(sp)
    LOAD     a3,  12 * REGBYTES(sp)
    LOAD     a4,  13 * REGBYTES(sp)
    LOAD     a5,  14 * REGBYTES(sp)
    LOAD     a6,  15 * REGBYTES(sp)
    LOAD     a7,  16 * REGBYTES(sp)
    LOAD     s2,  17 * REGBYTES(sp)
    LOAD     s3,  18 * REGBYTES(sp)
    LOAD     s4,  19 * REGBYTES(sp)
    LOAD     s5,  20 * REGBYTES(sp)
    LOAD     s6,  21 * REGBYTES(sp)
    LOAD     s7,  22 * REGBYTES(sp)
    LOAD     s8,  23 * REGBYTES(sp)
    LOAD     s9,  24 * REGBYTES(sp)
    LOAD     s10, 25 * REGBYTES(sp)
    LOAD     s11, 26 * REGBYTES(sp)
    LOAD     t3,  27 * REGBYTES(sp)
    LOAD     t4,  28 * REGBYTES(sp)
    LOAD     t5,  29 * REGBYTES(sp)
    LOAD     t6,  30 * REGBYTES(sp)
    addi 	sp,sp,32*REGBYTES
	//run task
    ret

/*=========================================================================================*/

	.global HAL_ContexSwitch
HAL_ContexSwitch:
	ecall
	ret



/*=========================================================================================*/

/*=========================================================================================*/

	.end
