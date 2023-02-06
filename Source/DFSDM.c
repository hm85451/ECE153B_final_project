#include "stm32l476xx.h"
#include "DFSDM.h"

void GPIO_Init(void) {//PE9 and PE7
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOEEN;
	
	GPIOE->MODER &= ~GPIO_MODER_MODE7;
	GPIOE->MODER &= ~GPIO_MODER_MODE9;
	GPIOE->MODER |= GPIO_MODER_MODE7_1;
	GPIOE->MODER |= GPIO_MODER_MODE9_1;//alternate function
	
	GPIOE->OTYPER &= ~GPIO_OTYPER_OT7;
	GPIOE->OTYPER &= ~GPIO_OTYPER_OT9;//push pull
	
	GPIOE->PUPDR &= ~GPIO_PUPDR_PUPD7;//npupd
	GPIOE->PUPDR &= ~GPIO_PUPDR_PUPD9;//npupd
	
	GPIOE->OSPEEDR |= GPIO_OSPEEDR_OSPEED7;
	GPIOE->OSPEEDR |= GPIO_OSPEEDR_OSPEED9;//very high speed
	
	GPIOE->AFR[0] &= ~GPIO_AFRL_AFSEL7;
	GPIOE->AFR[1] &= ~GPIO_AFRH_AFSEL9;
	GPIOE->AFR[0] |= GPIO_AFRL_AFSEL7_2 | GPIO_AFRL_AFSEL7_1;//dfsdm datain2
	GPIOE->AFR[1] |= GPIO_AFRH_AFSEL9_1 | GPIO_AFRH_AFSEL9_2;//dfsdm ck
	
}

void DFSDM_Init(void)
{
	GPIO_Init();
	
	RCC->APB2ENR |= RCC_APB2ENR_DFSDMEN;// enable DFSDM
	RCC->APB2RSTR |= RCC_APB2RSTR_DFSDMRST;
	RCC->APB2RSTR &= ~RCC_APB2RSTR_DFSDMRST;//reset DFSDM
	
	//channel 2 for dfsdm datain2
	DFSDM_Channel2->CHCFGR1 &= ~DFSDM_CHCFGR1_CHEN;//disable channel 2
	DFSDM_Channel2->CHCFGR1 &= ~DFSDM_CHCFGR1_SITP;// SPI with rising edge to strobe data//question
	DFSDM_Channel2->CHCFGR1 &= ~DFSDM_CHCFGR1_SPICKSEL;
	DFSDM_Channel2->CHCFGR1 |= DFSDM_CHCFGR1_SPICKSEL_0;//:clock coming from internal
	DFSDM_Channel2->CHCFGR2 &= ~DFSDM_CHCFGR2_OFFSET;// 0 offset
	DFSDM_Channel2->CHCFGR2 &= ~DFSDM_CHCFGR2_DTRBS;//0 right shift
	DFSDM_Channel2->CHCFGR1 &= ~DFSDM_CHCFGR1_CHINSEL;//Channel inputs are taken from pins of the same channel y
	DFSDM_Channel2->CHCFGR1 |= DFSDM_CHCFGR1_CHEN;//enable channel 2
	
	//channel 0 for dfsdm CKOUT 2Mhz
	DFSDM_Channel0->CHCFGR1 &= ~DFSDM_CHCFGR1_CHEN;
	DFSDM_Channel0->CHCFGR1 &= ~DFSDM_CHCFGR1_CKOUTSRC;// system clock for CKOUT
	DFSDM_Channel0->CHCFGR1 &= ~DFSDM_CHCFGR1_CKOUTDIV;
	DFSDM_Channel0->CHCFGR1 |= 8 << 16UL;//set clock divider to 8 => CKOUT is 80Mhz / 40 = 2Mhz
	DFSDM_Channel0->CHCFGR1 |= DFSDM_CHCFGR1_CHEN;//enable channel 0
	DFSDM_Channel0->CHCFGR1 |= DFSDM_CHCFGR1_DFSDMEN; // enable DFSDM interface
	
	//filter 0
	DFSDM_Filter0->CR1 &= ~DFSDM_CR1_DFEN;//disable filter 0
	DFSDM_Filter0->CR1 &= ~DFSDM_CR1_RCH;
	DFSDM_Filter0->CR1 |= 2 << 24UL;//channel 2 selected as regular channel
	DFSDM_Filter0->CR1 |= DFSDM_CR1_RCONT;//continuous mode
	DFSDM_Filter0->CR1 |= DFSDM_CR1_FAST;//enable fast mode
	
	
	//filter 0 config
	DFSDM_Filter0->FCR &= ~DFSDM_FCR_FORD;
	DFSDM_Filter0->FCR |= DFSDM_FCR_FORD_1 | DFSDM_FCR_FORD_0;// sinc filter order 3
	DFSDM_Filter0->FCR &= ~DFSDM_FCR_FOSR;
	DFSDM_Filter0->FCR |= 100 << 16;// set Fosr to 100 to get 20kHz sampling rate 2Mhz / 100 = 20khz
	DFSDM_Filter0->FCR &= ~DFSDM_FCR_IOSR;
	DFSDM_Filter0->FCR |= 1 << 0;// set Iosr to 1
	
	DFSDM_Filter0->CR1 |= DFSDM_CR1_DFEN;//enable filter 0
}