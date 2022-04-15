#include "stm32l476xx.h"

volatile uint32_t TimeDelay;

// ticks: number of ticks between two interrupts
void SysTick_Initialize(uint32_t ticks) {
	
	// Disable SysTick IRQ and SysTick counter
	SysTick->CTRL = 0;
	
	// Set reload register
	SysTick->LOAD = ticks - 1;
	
	// Set interrupt priority of SysTick
	// Make SysTick least urgent (i.e. highest priority number)
	// __NVIC_PRIO_BITS: number of bits for priority levels, defined in CMSIS
	//NVIC_SetPriority(SysTick_IRQn, (1<<__NVIC_PRIO_BITS) - 1);
	
	NVIC_SetPriority(SysTick_IRQn, 1);		// Set Priority to 1
	NVIC_EnableIRQ(SysTick_IRQn);					// Enable EXTI0_1 interrupt in NVIC

	
	// Reset the SysTick counter value
	SysTick->VAL = 0;
	
	// Select processor clock
	// 1 = processor clock; 0 = external clock
	SysTick->CTRL &= ~SysTick_CTRL_CLKSOURCE_Msk;
	//SysTick->CTRL = 0;
	// Enable SysTick exception request
	// 1 = counting down to zero asserts the SysTick exception request
	// 0 = counting down to zero does no assert the SysTick exception request
	SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;
	
	// Enable SysTick timer
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
}

// SysTick interrupt service routine
void SysTick_Handler(void) {
	TimeDelay--;
}

// nTime: specifies the delay time length
void delay(uint32_t nTime) {
	TimeDelay = nTime;
	while(TimeDelay != 0);
}
