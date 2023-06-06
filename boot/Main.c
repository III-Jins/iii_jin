#include "stdint.h"
#include "stdbool.h"

#include "Kernel.h"
#include "HalUart.h"
#include "stdio.h"
#include "stdlib.h"
#include "HalInterrupt.h"
#include "HalTimer.h"

static void Hw_init(void);
static void Kernel_init(void);

static void Printf_test(void);
static void Timer_test(void);
//static void Test_critical_section(uint32_t p, uint32_t taskId);

void User_task0(void);
void User_task1(void);
void User_task2(void);

void main(void)
{
	Hw_init();

	uint32_t i =10;
	while(i--)
	{
		uint8_t ch = Hal_uart_get_char();
		Hal_uart_put_char(ch);
	}
	Hal_uart_put_char('\n');
	putstr("Hello World!\n");
	
	Timer_test();

	Kernel_init();

	while(1);
}

static void Hw_init(void)
{
	Hal_interrupt_init();
	Hal_uart_init();
	Hal_timer_init();
}

static void Timer_test(void)
{
	    for(uint32_t i=0; i<5; i++)
	    {
		debug_printf("current count : %u\n", Hal_timer_get_1ms_counter());
        	delay(1000);
	    }
        
}

static void Kernel_init(void)
{
    uint32_t taskId;

    Kernel_task_init();
    Kernel_event_flag_init();
    Kernel_msgQ_init();
    Kernel_sem_init(1);
    //Kernel_mutex_init();

    taskId = Kernel_task_create(User_task0,3);
    if (NOT_ENOUGH_TASK_NUM == taskId)
    {
        putstr("Task0 creation fail\n");
    }
    /*
    taskId = Kernel_task_create(User_task1,4);
    if (NOT_ENOUGH_TASK_NUM == taskId)
    {
        putstr("Task1 creation fail\n");
    }

    taskId = Kernel_task_create(User_task2,1);
    if (NOT_ENOUGH_TASK_NUM == taskId)
    {
        putstr("Task2 creation fail\n");
    }
    */
    Kernel_start();
   
}

void User_task0(void)
{
   uint32_t local=0;
   debug_printf("User task0 SP=0x%x\n", &local);
   while(1);

}

void User_task1(void)
{
   uint32_t local=0;
   debug_printf(" User task1 SP=0x%x\n", &local);
   while(1);

}

void User_task2(void)
{
   uint32_t local=0;
   debug_printf(" User task2 SP=0x%x\n", &local);
   while(1);
}
 /*
static uint32_t shared_value;
static void Test_critical_section(uint32_t p, uint32_t taskId){
    Kernel_lock_mutex();

    debug_printf("User Task #%u Send=%u\n", taskId, p);
    shared_value = p;
    Kernel_yield();
    delay(1000);
    debug_printf("User Task #%u Shared Value=%u\n", taskId, shared_value);

    Kernel_unlock_mutex();
}
*/
