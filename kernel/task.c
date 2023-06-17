#include "stdint.h"
#include "stdbool.h"

#include "ARMv7AR.h"
#include "task.h"
#include "stdio.h"
static KernelTcb_t  sTask_list[MAX_TASK_NUM];
static KernelTcb_t* sCurrent_tcb;
static KernelTcb_t* sNext_tcb;

static uint32_t     sAllocated_tcb_index;
static uint32_t     sCurrent_tcb_index;

static KernelTcb_t* Scheduler_round_robin_algorithm(void);
static KernelTcb_t* Scheduler_priority_algorithm(void);
static void Save_context(void);
static void Restore_context(void);

void Kernel_task_init(void)
{
    sAllocated_tcb_index = 0;
    sCurrent_tcb_index = 0;

    for(uint32_t i = 0 ; i < MAX_TASK_NUM ; i++)
    {
        sTask_list[i].stack_base = (uint8_t*)(TASK_STACK_START + (i * USR_TASK_STACK_SIZE));
        sTask_list[i].sp = (uint32_t)sTask_list[i].stack_base + USR_TASK_STACK_SIZE - 4;
        sTask_list[i].sp -= sizeof(KernelTaskContext_t);
        KernelTaskContext_t* ctx = (KernelTaskContext_t*)sTask_list[i].sp;
        ctx->pc = 0;
        ctx->spsr = ARM_MODE_BIT_SYS;    
    }
}

void Kernel_task_start(void)
{

    sNext_tcb = &sTask_list[sCurrent_tcb_index];
    Restore_context();
}


uint32_t Kernel_task_create(KernelTaskFunc_t startFunc)
{
    KernelTcb_t* new_tcb = &sTask_list[sAllocated_tcb_index++];

    if (sAllocated_tcb_index > MAX_TASK_NUM)
    {
        return NOT_ENOUGH_TASK_NUM;
    }

    //new_tcb->priority = priority;

    //debug_printf("priority 생성 완료 : %u \n", new_tcb->priority);
    KernelTaskContext_t* ctx = (KernelTaskContext_t*)new_tcb->sp;
    ctx->pc = (uint32_t)startFunc;
    //debug_printf("priority 생성 완료 : %u \n", startFunc);
    return (sAllocated_tcb_index - 1);
}

uint32_t Kernel_task_clear(void)
{
	uint32_t  MAX = MAX_TASK_NUM;
	KernelTcb_t* c_tcb = &sTask_list[sCurrent_tcb_index];
	c_tcb->priority = MAX_TASK_NUM - 10;		
	
	/*
	if(sCurrent_tcb_index < sAllocated_tcb_index)
	{	
        	for(uint32_t i = sCurrent_tcb_index ; i < sAllocated_tcb_index ; i++)
        	{       
    			
    			KernelTaskContext_t* ctx = (KernelTaskContext_t*)sTask_list[i]->sp;
			//KernelTcb_t* new_tcb = &sTask_list[i+1];
                	debug_printf("sp : %u \n", ctx->pc);
                	sTask_list[i] = sTask_list[i+1];
                	debug_printf("sp : %u \n", sTask_list[i].sp->pc);
        	}
        	sAllocated_tcb_index--;
	}
	else //마지막요소?
	{
        	sAllocated_tcb_index--;
	}
	*/
        debug_printf("sAllo : %u \n", sAllocated_tcb_index);
	//uint32_t next_index = 0;
	uint32_t high_prio = sTask_list[0].priority;
        debug_printf("high prio : %u \n", high_prio);
	for(uint32_t i = 0 ; i < sAllocated_tcb_index ; i++)
        {
                debug_printf("i : %u \n", i);
                KernelTcb_t* tcb_prio = &sTask_list[i];
                debug_printf("prio : %u \n", sTask_list[i].priority);
                if(tcb_prio->priority <= high_prio){
			high_prio = tcb_prio->priority;
			sCurrent_tcb_index = i;
                	debug_printf("change i : %u \n", i);
		}
        }
        debug_printf("current tcb index : %u \n", sCurrent_tcb_index);
    	sNext_tcb = &sTask_list[sCurrent_tcb_index];
	//sNext_tcb = &sTask_list[next_index];
	//Kernel_task_scheduler();
    	Kernel_task_context_switching();
	debug_printf("반납?\n");
	return sCurrent_tcb_index;
}

uint32_t Kernel_task_get_current_task_id(void)
{
    return sCurrent_tcb_index;
}

void Kernel_task_scheduler(void)
{
    sCurrent_tcb = &sTask_list[sCurrent_tcb_index];
    sNext_tcb = Scheduler_round_robin_algorithm();
    Kernel_task_context_switching();
}

__attribute__ ((naked)) void Kernel_task_context_switching(void)
{
    __asm__ ("B Save_context");
    __asm__ ("B Restore_context");
}

static __attribute__ ((naked)) void Save_context(void)
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

static __attribute__ ((naked)) void Restore_context(void)
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

static KernelTcb_t* Scheduler_round_robin_algorithm(void)
{
    sCurrent_tcb_index++;
    sCurrent_tcb_index %= sAllocated_tcb_index;

    return &sTask_list[sCurrent_tcb_index];
}

static KernelTcb_t* Scheduler_priority_algorithm(void)
{
    for(uint32_t i = 0; i < sAllocated_tcb_index; i++)
    {
        KernelTcb_t* pNextTcb = &sTask_list[i];
        if (pNextTcb != sCurrent_tcb)
        {
	    debug_printf("current pr : %u, next pr: %u\n", sCurrent_tcb->priority, pNextTcb->priority);
            if ((pNextTcb -> priority) <= (sCurrent_tcb -> priority))  //다음 tcb 우선순위 >현재 우선 순위
            {
		debug_printf("yes change\n");
		sCurrent_tcb_index = i;
                return pNextTcb;   //다음 tcb 우선순위가 높으므로 nextTcb를 return한다.
            }
        }
    }

    return sCurrent_tcb; //nextTcb 우선순위가 낮으므로 현재 tcb가 계속 실행 되도록 return한다.
}
