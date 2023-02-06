#ifndef __STM32L476G_DISCOVERY_FFT_H
#define __STM32L476G_DISCOVERY_FFT_H

#include "stm32l476xx.h"
#include <stdio.h>
#include <math.h>
#include <complex.h>

typedef double complex cplx;

void show(const char * s, cplx buf[]);
void fft(cplx buf[], int n);
void _fft(cplx buf[], cplx out[], int n, int step);

#endif
