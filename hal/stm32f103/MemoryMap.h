#ifndef _HAL_MEMORYMAP_H_
#define _HAL_MEMORYMAP_H_

extern uint32_t __bss_end__;

#define TASK_STACK_START    ((uint32_t)&__bss_end__)
#define USR_TASK_STACK_SIZE (2 * 1024)
#define TASK_STACK_SIZE     (USR_TASK_STACK_SIZE * 3)
//use 3 task
#endif /*_HAL_MEMORYMAP_H_ */


