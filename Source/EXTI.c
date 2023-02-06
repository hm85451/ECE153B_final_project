
#include "EXTI.h"
#include "max7219.h"

uint8_t iData = 0x01;
uint8_t sData = 0x01;

void EXTI_Init(void) {
	// Initialize Joystick
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
	
	GPIOA->MODER &= ~GPIO_MODER_MODE0;
	GPIOA->MODER &= ~GPIO_MODER_MODE3;
	GPIOA->MODER &= ~GPIO_MODER_MODE5;
	
	GPIOA->PUPDR &= ~GPIO_PUPDR_PUPD0;
	GPIOA->PUPDR |= GPIO_PUPDR_PUPD0_1;
	GPIOA->PUPDR &= ~GPIO_PUPDR_PUPD3;
	GPIOA->PUPDR |= GPIO_PUPDR_PUPD3_1;
	GPIOA->PUPDR &= ~GPIO_PUPDR_PUPD5;
	GPIOA->PUPDR |= GPIO_PUPDR_PUPD5_1;

	
	// Configure SYSCFG EXTI
	SYSCFG->EXTICR[0] &= ~SYSCFG_EXTICR1_EXTI0;
	SYSCFG->EXTICR[0] |= SYSCFG_EXTICR1_EXTI0_PA;
	
	SYSCFG->EXTICR[0] &= ~SYSCFG_EXTICR1_EXTI3;
	SYSCFG->EXTICR[0] |= SYSCFG_EXTICR1_EXTI3_PA;
	
	SYSCFG->EXTICR[0] &= ~SYSCFG_EXTICR2_EXTI5;
	SYSCFG->EXTICR[0] |= SYSCFG_EXTICR2_EXTI5_PA;
	
	// Configure EXTI Trigger
	EXTI->FTSR1 |= EXTI_FTSR1_FT0;
	EXTI->FTSR1 |= EXTI_FTSR1_FT3;
	EXTI->FTSR1 |= EXTI_FTSR1_FT5;
	
	
	// Enable EXTI
	EXTI->IMR1 |= EXTI_IMR1_IM0;
	EXTI->IMR1 |= EXTI_IMR1_IM3;
	EXTI->IMR1 |= EXTI_IMR1_IM5;
	
	// Configure and Enable in NVIC
	NVIC_EnableIRQ(EXTI0_IRQn);
	NVIC_EnableIRQ(EXTI3_IRQn);
	NVIC_EnableIRQ(EXTI9_5_IRQn);
	
	NVIC_SetPriority(EXTI0_IRQn, 0);
	NVIC_SetPriority(EXTI3_IRQn, 0);
	NVIC_SetPriority(EXTI9_5_IRQn, 0);
	
}


void EXTI0_IRQHandler(void){//select
	EXTI->PR1 |= EXTI_PR1_PIF0;
	if (sData == 0x01)
		sData = 0x00;
	else
		sData = 0x01;
	writeToMax7219(&sData, MAX7219_REG_SHUTDOWN, 2);
}


void EXTI3_IRQHandler(void){//up
	EXTI->PR1 |= EXTI_PR1_PIF3;
	if (iData < 0x07)
		iData++;
	else
		iData = 0x07;
	writeToMax7219(&iData, MAX7219_REG_INTENSITY, 2);
	
}

void EXTI9_5_IRQHandler(void){//down
	EXTI->PR1 |= EXTI_PR1_PIF5;
	if (iData > 0x0)
		iData--;
	else
		iData = 0x0;
	writeToMax7219(&iData, MAX7219_REG_INTENSITY, 2);
	
}
