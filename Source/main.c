#include "stm32l476xx.h"
#include "DFSDM.h"
#include "SPI.h"
#include "SysTimer.h"
#include "max7219.h"
#include "SysClock.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "EXTI.h"
#include "fft2.h"

#define bufferSize  16

//sample rate = 20khz
//captured frequency (nyquist) 0 - 10khz
//sample FFT size = 16
//frequnecy resolution = 20000/16 = 1250hz

complex fftBuffer[bufferSize];
complex scratch[bufferSize];
int row[8];

int main() {
	// Enable High Speed Internal Clock (HSI = 16 MHz)
	RCC->CR |= RCC_CR_HSION;
	while ((RCC->CR & RCC_CR_HSIRDY) == 0); // Wait until HSI is ready
	
	// Select HSI as system clock source 
	RCC->CFGR &= ~RCC_CFGR_SW;
	RCC->CFGR |= RCC_CFGR_SW_HSI;
	while ((RCC->CFGR & RCC_CFGR_SWS) == 0); // Wait until HSI is system clock source
	
	DFSDM_Init();
	max7219_init();
	EXTI_Init();
	
	uint32_t sample;
	int freqs[bufferSize];
	int aveFreqs[bufferSize];
	int i = 0;
	int interval = 0;

	while(1) {
		DFSDM_Filter0->CR1 |= DFSDM_CR1_RSWSTART;
		while ((DFSDM_Filter0->ISR & DFSDM_ISR_REOCF) == 0);
		sample = DFSDM_Filter0->RDATAR;
	
		fftBuffer[i].Re = (sample >> 8);
		fftBuffer[i].Im = 0;
		i++;

		if (i == bufferSize)
		{
			fft(fftBuffer, bufferSize ,scratch);
			int offset = 50;//noise reduction

			for (int k = 0; k < bufferSize; k++)
			{	
				float mag = sqrtf(fftBuffer[k].Re*fftBuffer[k].Re + fftBuffer[k].Im*fftBuffer[k].Im);
					freqs[k] = (int)(20*log10f(mag)) - offset;
				if (freqs[k]<0)
					freqs[k]=0;
				
				scratch[i].Re = 0;
				scratch[i].Im = 0;	
			}
		
				int n = 0;
				for (int j = 0; j < bufferSize/2; j+=bufferSize/16)
				{
						if(freqs[j] / 12 > 8)
								row[n] = 8;
						else
								row[n] = freqs[j] / 12;
						n++;
				}
				updateDisplay(row);
	
			i = 0;
		}
	}
	return 0;
}
/*
Note for Professor and TAs:
		I could not get the pcm signal processed correctly eventhough I 
tried multiple fft functions and using different sampling frequencies.
I am not sure if there is an issue with the DFSDM configuration, 
or the number of fft samples. I tried to debug it for a long time 
but there wasn't a lot of online resource that can help me debug my
 situation. 
*/
