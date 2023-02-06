#ifndef __STM32L476G_DISCOVERY_FFT2_H
#define __STM32L476G_DISCOVERY_FFT2_H

#include "stm32l476xx.h"
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

typedef float real;
typedef struct{real Re; real Im;} complex;

static void print_vector(
	     const char *title,
	     complex *x,
	     int n);

void fft( complex *v, int n, complex *tmp );
void ifft( complex *v, int n, complex *tmp );
#endif
