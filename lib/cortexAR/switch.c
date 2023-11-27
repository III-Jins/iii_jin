#include "stdint.h"
#include "stdbool.h"
#include "armcpu.h"
#include "switch.h"
#include "task.h"

extern KernelTcb_t* sCurrent_tcb;
extern KernelTcb_t* sNext_tcb;

__attribute__ ((naked)) void Arch_start(void)
{
	__asm__ volatile ("B Arch_Restore_context");
}

__attribute__ ((naked)) void Arch_context_switching(void)
{
    __asm__ ("B Arch_Save_context");
    __asm__ ("B Arch_Restore_context");
}

__attribute__ ((naked)) void Arch_Save_context(void)
{
    // save current task context into the current task stack
    __asm__ ("PUSH {lr}");
    __asm__ ("PUSH {r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12}");
    __asm__ ("MRS   r0, cpsr");
    __asm__ ("PUSH {r0}");
    // save current task stack pointer into the current TCB
    __asm__ ("LDR   r0, =sCurrent_tcb");
    __asm__ ("LDR   r0, [r0]");
    __asm__ ("STMIA r0!, {sp}");
}

__attribute__ ((naked)) void Arch_Restore_context(void)
{
    // restore next task stack pointer from the next TCB
    __asm__ ("LDR   r0, =sNext_tcb");
    __asm__ ("LDR   r0, [r0]");
    __asm__ ("LDMIA r0!, {sp}");
    // restore next task context from the next task stack
    __asm__ ("POP  {r0}");
    __asm__ ("MSR   cpsr, r0");
    __asm__ ("POP  {r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12}");
    __asm__ ("POP  {pc}");
}