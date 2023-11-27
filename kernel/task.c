#include "stdint.h"
#include "stdbool.h"

#include "PSR.h"
#include "task.h"
#include "MemoryMap.h"
#include "switch.h"

static KernelTcb_t  sTask_list[MAX_TASK_NUM];
KernelTcb_t* sCurrent_tcb;
KernelTcb_t* sNext_tcb;

static uint32_t     sAllocated_tcb_index;
static uint32_t     sCurrent_tcb_index;

static KernelTcb_t* Scheduler_round_robin_algorithm(void);
static KernelTcb_t* Scheduler_priority_algorithm(void);

void Kernel_task_scheduler(void);

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
        ctx->spsr = PSR_INIT;    
    }
}

void Kernel_task_start(void)
{
    sNext_tcb = &sTask_list[sCurrent_tcb_index];
    Arch_start();
}


//uint32_t Kernel_task_create(KernelTaskFunc_t startFunc, uint32_t priority)
uint32_t Kernel_task_create(KernelTaskFunc_t startFunc)
{
    KernelTcb_t* new_tcb = &sTask_list[sAllocated_tcb_index++];

    if (sAllocated_tcb_index > MAX_TASK_NUM)
    {
        return NOT_ENOUGH_TASK_NUM;
    }

    //new_tcb->priority = priority;	//priority

    KernelTaskContext_t* ctx = (KernelTaskContext_t*)new_tcb->sp;
    ctx->pc = (uint32_t)startFunc;
    return (sAllocated_tcb_index - 1);
}

uint32_t Kernel_task_clear(void)
{
	uint32_t  MAX = MAX_TASK_NUM;
	KernelTcb_t* c_tcb = &sTask_list[sCurrent_tcb_index];
	c_tcb->priority = c_tcb->priority + 10;
	
	Kernel_task_scheduler();
}

void Kernel_task_recover(void)
{
	uint32_t  MAX = MAX_TASK_NUM;
	KernelTcb_t* c_tcb = &sTask_list[sCurrent_tcb_index];
	KernelTcb_t* n_tcb = &sTask_list[!sCurrent_tcb_index];
	c_tcb->priority = c_tcb->priority - 10;
	n_tcb->priority = n_tcb->priority - 10;
}

uint32_t Kernel_task_get_current_task_id(void)
{
    return sCurrent_tcb_index;
}

void Kernel_task_scheduler(void)
{
    sCurrent_tcb = &sTask_list[sCurrent_tcb_index];
    sNext_tcb = Scheduler_round_robin_algorithm();
    //sNext_tcb = Scheduler_priority_algorithm();
    Arch_context_switching();
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
	    //debug_printf("current pr : %u, next pr: %u\n", sCurrent_tcb->priority, pNextTcb->priority);
            if ((pNextTcb -> priority) <= (sCurrent_tcb -> priority))  //다음 tcb 우선순위 >현재 우선 순위
            {
		//debug_printf("yes change\n");
		sCurrent_tcb_index = i;
                return pNextTcb;   //다음 tcb 우선순위가 높으므로 nextTcb를 return한다.
            }
        }
    }

    return sCurrent_tcb; //nextTcb 우선순위가 낮으므로 현재 tcb가 계속 실행 되도록 return한다.
}

