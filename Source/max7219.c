#include "SPI.h"
#include "max7219.h"


static uint8_t MAX7219_state[8] = {0, 0, 0, 0, 0, 0, 0, 0};

void max7219_init(void)
{
		//initialize spi and gpio
		SPI1_GPIO_Init();
		SPI_Init();
	
		uint8_t data1 = 0x0007;
		uint8_t data2 = 0x0001;
		uint8_t data3 = 0x0000;
		uint8_t data4 = 0x0000;
		uint8_t data5 = 0x00FF;
		writeToMax7219(&data1, MAX7219_REG_SCANLIMIT, 4);
		writeToMax7219(&data2, MAX7219_REG_SHUTDOWN, 4);
		writeToMax7219(&data3, MAX7219_REG_DECODE, 4);
		writeToMax7219(&data4, MAX7219_REG_INTENSITY, 4);
		writeToMax7219(&data5, MAX7219_REG_NOOP, 4);
		
		clearDisplay();
}


void writeToMax7219(uint8_t *pBuffer, uint8_t WriteAddr, uint8_t NumByteToWrite)
{
	uint8_t rxBuffer[32];
	
	/* Configure the MS bit: 
       - When 0, the address will remain unchanged in multiple read/write commands.
       - When 1, the address will be auto incremented in multiple read/write commands.
	*/
	if(NumByteToWrite > 0x01){
		WriteAddr |= (uint8_t) MULTIPLEBYTE_CMD; // Set write mode & auto-increment
	}
	
	// Set chip select Low at the start of the transmission 
	MAX7219_CS_LOW;  // Gyro CS low
	SPI_Delay(10);  
	
	// Send the Address of the indexed register 
	SPI_Write(SPI1, &WriteAddr, rxBuffer, 1);
	
	// Send the data that will be written into the device (MSB First) 
	SPI_Write(SPI1, pBuffer, rxBuffer, NumByteToWrite);
  
	// Set chip select High at the end of the transmission  
	SPI_Delay(10); // Gyro CS high
	MAX7219_CS_HIGH;
}

void clearDisplay(void)
{
	uint8_t data = 0x00;
	for (int i = 0 ; i < 8 ; i++)
	{
			writeToMax7219(&data, i+1, 2);
	}
}

void updateDisplay(int m[])
{
		for (int i = 0; i<8; i++)
		{
			uint8_t data = 0x00;
			switch (m[i])
			{
				case 0: 
					data = 0x00;
					break;
				case 1: 
					data = 0x01;
					break;
				case 2: 
					data = 0x03;
					break;
				case 3: 
					data = 0x07;
					break;
				case 4: 
					data = 0x0F;
					break;
				case 5: 
					data = 0x1F;
					break;
				case 6: 
					data = 0x3F;
					break;
				case 7: 
					data = 0x7F;
					break;
				case 8: 
					data = 0xFF;
					break;
			}
			if (MAX7219_state[i]!=data)
			{
					writeToMax7219(&data, i+1, 2);
					MAX7219_state[i] = data;
			}
		}
}

void setRow(uint8_t row, uint8_t value)
{
		if (row > 7 || value > 8)
				return;
		uint8_t data;
		switch (value){
			case 0: 
				data = 0x0;
				break;
			case 1: 
				data = 0x1;
				break;
			case 2: 
				data = 0x3;
				break;
			case 3: 
				data = 0x7;
				break;
			case 4: 
				data = 0xF;
				break;
			case 5: 
				data = 0x1F;
				break;
			case 6: 
				data = 0x3F;
				break;
			case 7: 
				data = 0x7F;
				break;
			case 8: 
				data = 0xFF;
				break;
			
		}
		writeToMax7219(&data, row+1, 2);
}
