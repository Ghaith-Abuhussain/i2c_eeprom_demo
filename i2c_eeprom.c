#include "i2c_eeprom.h"

#ifndef i2c_functions
/* Define bit rate */

void I2C_Init()			/* I2C initialize function */
{
	//TWBR = BITRATE(TWSR=0x00);	/* Get bit rate register value by formula */
	TWBR = 5;
	TWSR &= (~((1<<TWPS1)|(1<<TWPS0)));
}

uint8_t I2C_Start(char write_address)/* I2C start function */
{
	uint8_t status;		/* Declare variable */
	TWCR=(1<<TWSTA)|(1<<TWEN)|(1<<TWINT); /* Enable TWI, generate START */
	while(!(TWCR&(1<<TWINT)));	/* Wait until TWI finish its current job */
	status=TWSR&0xF8;		/* Read TWI status register */
	if(status!=0x08)		/* Check weather START transmitted or not? */
	return 0;			/* Return 0 to indicate start condition fail */
	TWDR = write_address;		/* Write SLA+W in TWI data register */
	TWCR=(1<<TWEN)|(1<<TWINT);	/* Enable TWI & clear interrupt flag */
	while(!(TWCR&(1<<TWINT)));	/* Wait until TWI finish its current job */
	status=TWSR&0xF8;		/* Read TWI status register */
	if(status==0x18)		/* Check for SLA+W transmitted &ack received */
	return 1;			/* Return 1 to indicate ack received */
	if(status==0x20)		/* Check for SLA+W transmitted &nack received */
	return 2;			/* Return 2 to indicate nack received */
	else
	return 3;			/* Else return 3 to indicate SLA+W failed */
}

uint8_t I2C_Repeated_Start(char read_address) /* I2C repeated start function */
{
	uint8_t status;		/* Declare variable */
	TWCR=(1<<TWSTA)|(1<<TWEN)|(1<<TWINT);/* Enable TWI, generate start */
	while(!(TWCR&(1<<TWINT)));	/* Wait until TWI finish its current job */
	status=TWSR&0xF8;		/* Read TWI status register */
	if(status!=0x10)		/* Check for repeated start transmitted */
	return 0;			/* Return 0 for repeated start condition fail */
	TWDR=read_address;		/* Write SLA+R in TWI data register */
	TWCR=(1<<TWEN)|(1<<TWINT);	/* Enable TWI and clear interrupt flag */
	while(!(TWCR&(1<<TWINT)));	/* Wait until TWI finish its current job */
	status=TWSR&0xF8;		/* Read TWI status register */
	if(status==0x40)		/* Check for SLA+R transmitted &ack received */
	return 1;			/* Return 1 to indicate ack received */
	if(status==0x48)		/* Check for SLA+R transmitted &nack received */
	return 2;			/* Return 2 to indicate nack received */
	else
	return 3;			/* Else return 3 to indicate SLA+W failed */
}

uint8_t I2C_Write(char data)	/* I2C write function */
{
	uint8_t status;		/* Declare variable */
	TWDR=data;			/* Copy data in TWI data register */
	TWCR=(1<<TWEN)|(1<<TWINT);	/* Enable TWI and clear interrupt flag */
	while(!(TWCR&(1<<TWINT)));	/* Wait until TWI finish its current job */
		status=TWSR&0xF8;		/* Read TWI status register */
		if(status==0x28)		/* Check for data transmitted &ack received */
		return 0;			/* Return 0 to indicate ack received */
		if(status==0x30)		/* Check for data transmitted &nack received */
		return 1;			/* Return 1 to indicate nack received */
		else
		return 2;			/* Else return 2 for data transmission failure */
}

char I2C_Read_Ack()		/* I2C read ack function */
{
	TWCR=(1<<TWEN)|(1<<TWINT)|(1<<TWEA); /* Enable TWI, generation of ack */
	while(!(TWCR&(1<<TWINT)));	/* Wait until TWI finish its current job */
	return TWDR;			/* Return received data */
}

char I2C_Read_Nack()		/* I2C read nack function */
{
	TWCR=(1<<TWEN)|(1<<TWINT);	/* Enable TWI and clear interrupt flag */
	while(!(TWCR&(1<<TWINT)));	/* Wait until TWI finish its current job */
	return TWDR;		/* Return received data */
}

void I2C_Stop()			/* I2C stop function */
{
	TWCR=(1<<TWSTO)|(1<<TWINT)|(1<<TWEN);/* Enable TWI, generate stop */
	while(TWCR&(1<<TWSTO));	/* Wait until stop condition execution */
}

unsigned char write_to_eeprom(unsigned char eeprom_write_address, uint16_t address, int num_bytes, unsigned char bytes_to_write[])
{
	if(I2C_Start(EEPROM_Write_Addess) == 3)              /* Start I2C with device write address */
	{
		//USART0_SendString("Failure With I2C_START\n");
		return 0x01;
	}
	if(I2C_Write((address >> 8) & 0xff) == 2)		/* Write MSB memory address for data write */
	{
		//USART0_SendString("Failure With I2C_Write MSB ADDRESS\n");
		return 0x02;
	}
	if(I2C_Write(address & 0xff) == 2)		/* Write LSB memory address for data write */
	{
		//USART0_SendString("Failure With I2C_Write LSB ADDRESS\n");
		return 0x03;
	}
	for (int i = 0; i<num_bytes; i++)/* Write array data */
	{
		if(I2C_Write(bytes_to_write[i]) == 2)		/* Write start memory address for data write */
		{
			//USART0_SendString("Failure With I2C_Write array Byte\n");
			return 0x04;
		}
	}
	I2C_Stop();
	_delay_ms(50);
}

unsigned char read_from_eeprom(unsigned char eeprom_write_address,
								unsigned char eeprom_read_address, uint16_t address, int num_bytes, unsigned char bytes_to_write[])
{
	memset(bytes_to_write, 0, sizeof(bytes_to_write));
	if(I2C_Start(EEPROM_Write_Addess) == 3)/* Start I2C with device write address */
	{
		//USART0_SendString("Failure With I2C_Start Read\n");
		return 0x01;
	}
	if(I2C_Write((address >> 8) & 0xff) == 2)		/* Write start memory address for data write */
	{
		//USART0_SendString("Failure With I2C_Write MSB ADDRESS Read\n");
		return 0x02;
	}
	if(I2C_Write(address & 0xff) == 2)		/* Write start memory address for data write */
	{
		//USART0_SendString("Failure With I2C_Write MSB ADDRESS Read\n");
		return 0x03;
	}
	if(I2C_Repeated_Start(EEPROM_Read_Addess) == 3)/* Repeat start I2C SLA+R */
	{
		//USART0_SendString("Failure With I2C_Repeated_Start Read\n");
		return 0x04;
	}
	for (int i = 0; i < num_bytes - 1; i++)/* Read data with acknowledgment */
	{
		//USART0_TxChar(0xE0);
		bytes_to_write[i] = I2C_Read_Ack();
	}
	bytes_to_write[num_bytes - 1] = I2C_Read_Nack();
	I2C_Stop();			/* Stop I2C */
}

unsigned char write_to_eeprom_1024(unsigned char eeprom_write_address, uint16_t address, int num_bytes, unsigned char bytes_to_write[])
{
	if(I2C_Start(eeprom_write_address) == 3)              /* Start I2C with device write address */
	{
		//USART0_SendString("Failure With I2C_START\n");
		return 0x01;
	}
	if(I2C_Write((address >> 8) & 0xff) == 2)		/* Write MSB memory address for data write */
	{
		//USART0_SendString("Failure With I2C_Write MSB ADDRESS\n");
		return 0x02;
	}
	if(I2C_Write(address & 0xff) == 2)		/* Write LSB memory address for data write */
	{
		//USART0_SendString("Failure With I2C_Write LSB ADDRESS\n");
		return 0x03;
	}
	for (int i = 0; i<num_bytes; i++)/* Write array data */
	{
		if(I2C_Write(bytes_to_write[i]) == 2)		/* Write start memory address for data write */
		{
			//USART0_SendString("Failure With I2C_Write array Byte\n");
			return 0x04;
		}
	}
	I2C_Stop();
	_delay_ms(50);
}

unsigned char read_from_eeprom_1024(unsigned char eeprom_write_address,
									unsigned char eeprom_read_address, uint16_t address, int num_bytes, unsigned char bytes_to_write[])
{
	memset(bytes_to_write, 0, sizeof(bytes_to_write));
	if(I2C_Start(eeprom_write_address) == 3)/* Start I2C with device write address */
	{
		//USART0_SendString("Failure With I2C_Start Read\n");
		return 0x01;
	}
	if(I2C_Write((address >> 8) & 0xff) == 2)		/* Write start memory address for data write */
	{
		//USART0_SendString("Failure With I2C_Write MSB ADDRESS Read\n");
		return 0x02;
	}
	if(I2C_Write(address & 0xff) == 2)		/* Write start memory address for data write */
	{
		//USART0_SendString("Failure With I2C_Write MSB ADDRESS Read\n");
		return 0x03;
	}
	if(I2C_Repeated_Start(eeprom_read_address) == 3)/* Repeat start I2C SLA+R */
	{
		//USART0_SendString("Failure With I2C_Repeated_Start Read\n");
		return 0x04;
	}
	for (int i = 0; i < num_bytes - 1; i++)/* Read data with acknowledgment */
	{
		//USART0_TxChar(0xE0);
		bytes_to_write[i] = I2C_Read_Ack();
	}
	bytes_to_write[num_bytes - 1] = I2C_Read_Nack();
	I2C_Stop();			/* Stop I2C */
}
	
#endif