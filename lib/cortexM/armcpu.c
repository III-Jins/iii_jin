#include "armcpu.h"

void enable_irq(void)
{
	__asm__ ("cpsie i");
}

void disable_irq(void)
{
	__asm__ ("cpsid i");
}
