#ifndef __STM32L476G_DISCOVERY_MAX7219_H
#define __STM32L476G_DISCOVERY_MAX7219_H

#include "stm32l476xx.h"
#include <stdbool.h>

#define MAX7219_CS_LOW	GPIOE->ODR &= ~(1U << 12);
#define MAX7219_CS_HIGH	GPIOE->ODR |= (1U << 12);


#define MAX7219_REG_NOOP		0x00
#define MAX7219_REG_D0			0x01
#define MAX7219_REG_D1			0x02
#define MAX7219_REG_D2			0x03
#define MAX7219_REG_D3			0x04
#define MAX7219_REG_D4			0x05
#define MAX7219_REG_D5			0x06
#define MAX7219_REG_D6			0x07
#define MAX7219_REG_D7			0x08
#define MAX7219_REG_DECODE		0x09
#define MAX7219_REG_INTENSITY	0x0A
#define MAX7219_REG_SCANLIMIT	0x0B
#define MAX7219_REG_SHUTDOWN	0x0C
#define MAX7219_REG_TEST		0x0F

#define MAX7219_IC_NUM		4

/* Multiple byte read/write command */ 
#define MULTIPLEBYTE_CMD                        ((uint8_t)0x40)

void max7219_init(void);
void setRegister(uint8_t reg, uint8_t data);
void writeToMax7219(uint8_t *pBuffer, uint8_t WriteAddr, uint8_t NumByteToWrite);
void clearDisplay(void);
void updateDisplay(int m[]);
void setRow(uint8_t row, uint8_t value);
#endif