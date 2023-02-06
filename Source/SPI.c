#include "SPI.h"
#include "SysTimer.h"

// Note: When the data frame size is 8 bit, "SPIx->DR = byte_data;" works incorrectly. 
// It mistakenly send two bytes out because SPIx->DR has 16 bits. To solve the program,
// we should use "*((volatile uint8_t*)&SPIx->DR) = byte_data";

// LSM303C eCompass (a 3D accelerometer and 3D magnetometer module) SPI Interface: 
//   MAG_DRDY = PC2   MEMS_SCK  = PD1 (SPI2_SCK)   XL_CS  = PE0             
//   MAG_CS   = PC0   MEMS_MOSI = PD4 (SPI2_MOSI)  XL_INT = PE1       
//   MAG_INT  = PC1 
//
// L3GD20 Gyro (three-axis digital output) SPI Interface: 
//   MEMS_SCK  = PD1 (SPI2_SCK)    GYRO_CS   = PD7 (GPIO)
//   MEMS_MOSI = PD4 (SPI2_MOSI)   GYRO_INT1 = PD2
//   MEMS_MISO = PD3 (SPI2_MISO)   GYRO_INT2 = PB8

extern uint8_t Rx1_Counter;
extern uint8_t Rx2_Counter;

void SPI1_GPIO_Init(void) {//PE12 PE13 PE14 PE15
	// [TODO]
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOEEN;//enable GPIO clock
	
	GPIOE->OSPEEDR |= GPIO_OSPEEDR_OSPEED12;//very high speed
	GPIOE->OSPEEDR |= GPIO_OSPEEDR_OSPEED13;
	GPIOE->OSPEEDR |= GPIO_OSPEEDR_OSPEED14;
	GPIOE->OSPEEDR |= GPIO_OSPEEDR_OSPEED15;
	
	GPIOE->OTYPER &= ~GPIO_OTYPER_OT12;//push pull
	GPIOE->OTYPER &= ~GPIO_OTYPER_OT13;
	GPIOE->OTYPER &= ~GPIO_OTYPER_OT14;
	GPIOE->OTYPER &= ~GPIO_OTYPER_OT15;
	
	GPIOE->PUPDR &= ~GPIO_PUPDR_PUPDR12;
	GPIOE->PUPDR &= ~GPIO_PUPDR_PUPDR13;
	GPIOE->PUPDR &= ~GPIO_PUPDR_PUPDR14;//no pull up/pull down
	GPIOE->PUPDR &= ~GPIO_PUPDR_PUPDR15;

	GPIOE->MODER &= ~GPIO_MODER_MODER12;
	GPIOE->MODER &= ~GPIO_MODER_MODER13;
	GPIOE->MODER &= ~GPIO_MODER_MODER14;
	GPIOE->MODER &= ~GPIO_MODER_MODER15;
	GPIOE->MODER |= GPIO_MODER_MODER12_1;// alternate function
	GPIOE->MODER |= GPIO_MODER_MODER13_1;// alternate function
	GPIOE->MODER |= GPIO_MODER_MODER14_1;// alternate function
	GPIOE->MODER |= GPIO_MODER_MODER15_1;// alternate function
	
	GPIOE->AFR[1] &= ~GPIO_AFRH_AFRH4;
	GPIOE->AFR[1] |= 5UL << 16;//SPI1 NSS
	GPIOE->AFR[1] &= ~GPIO_AFRH_AFRH5;
	GPIOE->AFR[1] |= 5UL << 20;//SPI1 SCK
	GPIOE->AFR[1] &= ~GPIO_AFRH_AFRH6;
	GPIOE->AFR[1] |= 5UL << 24;//SPI1 MISO
	GPIOE->AFR[1] &= ~GPIO_AFRH_AFRH7;
	GPIOE->AFR[1] |= 5UL << 28;//SPI1 MOSI
}

void SPI_Init(void){
	// [TODO] 
	RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
	RCC->APB1RSTR1 |= RCC_APB2RSTR_SPI1RST;//reset SPI1
	RCC->APB1RSTR1 &= ~RCC_APB2RSTR_SPI1RST;//clear reset SPI1
	
	SPI1->CR1 &= ~SPI_CR1_SPE;//disable SPI
	SPI1->CR1 &= ~SPI_CR1_RXONLY; // full duplex
	SPI1->CR1 &= ~SPI_CR1_BIDIMODE; // 2-line unidirectional data mode selected
	SPI1->CR1 |= SPI_CR1_BIDIOE; // Output enable
	SPI1->CR1 &= ~SPI_CR1_LSBFIRST;// received with the MSB first
	
	SPI1->CR2 &= ~SPI_CR2_DS;
	SPI1->CR2 |= SPI_CR2_DS_0 | SPI_CR2_DS_1 | SPI_CR2_DS_2; //data size = 8
	SPI1->CR2 &= ~SPI_CR2_FRF; // motorola mode
	
	SPI1->CR1 &= ~SPI_CR1_CPOL;// CLK to 0 when idle
	SPI1->CR1 &= ~SPI_CR1_CPHA;//The first clock transition is the first data capture edge
	SPI1->CR1 &= ~SPI_CR1_BR;
	SPI1->CR1 |= SPI_CR1_BR_0| SPI_CR1_BR_2;//baud rate ct prescaler = 64
	SPI1->CR1 &= ~SPI_CR1_CRCEN; // disable CRC calculation
	SPI1->CR1 |= SPI_CR1_MSTR;//master configuration
	SPI1->CR1 |= SPI_CR1_SSM;//Software slave management enabled
	SPI1->CR2 |= SPI_CR2_NSSP; // enable pulse management
	SPI1->CR1 |= SPI_CR1_SSI; // set Internal slave select
	
	SPI1->CR2 &= ~SPI_CR2_FRXTH;
	SPI1->CR2 |= SPI_CR2_FRXTH;//1/4 FIFO
	
	SPI1->CR1 |= SPI_CR1_SPE;//enable SPI
	
}
 
void SPI_Write(SPI_TypeDef * SPIx, uint8_t *txBuffer, uint8_t * rxBuffer, int size) {
	volatile uint32_t tmpreg; 
	int i = 0;
	for (i = 0; i < size; i++) {
		while( (SPIx->SR & SPI_SR_TXE ) != SPI_SR_TXE );  // Wait for TXE (Transmit buffer empty)
		*((volatile uint8_t*)&SPIx->DR) = txBuffer[i];
		while((SPIx->SR & SPI_SR_RXNE ) != SPI_SR_RXNE); // Wait for RXNE (Receive buffer not empty)
		rxBuffer[i] = *((__IO uint8_t*)&SPIx->DR);
	}
	while( (SPIx->SR & SPI_SR_BSY) == SPI_SR_BSY ); // Wait for BSY flag cleared
}

void SPI_Read(SPI_TypeDef * SPIx, uint8_t *rxBuffer, int size) {
	int i = 0;
	for (i = 0; i < size; i++) {
		while( (SPIx->SR & SPI_SR_TXE ) != SPI_SR_TXE ); // Wait for TXE (Transmit buffer empty)
		*((volatile uint8_t*)&SPIx->DR) = rxBuffer[i];	
		// The clock is controlled by master. Thus the master has to send a byte
		// data to the slave to start the clock. 
		while((SPIx->SR & SPI_SR_RXNE ) != SPI_SR_RXNE); 
		rxBuffer[i] = *((__IO uint8_t*)&SPIx->DR);
	}
	while( (SPIx->SR & SPI_SR_BSY) == SPI_SR_BSY ); // Wait for BSY flag cleared
}

void SPI_Delay(uint32_t us) {
	uint32_t i, j;
	for (i = 0; i < us; i++) {
		for (j = 0; j < 18; j++) // This is an experimental value.
			(void)i;
	}
}
