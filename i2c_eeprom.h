#include <avr/io.h>
#include <util/delay.h>
#define F_CPU 16000000UL

#define EEPROM_Write_Addess		0xA0
#define EEPROM_Read_Addess		0xA1
#define EEPROM_Write_Addess_1024_LOW		0xA4
#define EEPROM_Write_Addess_1024_High		0xA6
#define EEPROM_Read_Addess_1024_LOW		0xA5
#define EEPROM_Read_Addess_1024_High		0xA7


#define SCL_CLK 1000000
#define BITRATE(TWSR)	((F_CPU/SCL_CLK)-16)/(2*pow(4,(TWSR&((1<<TWPS0)|(1<<TWPS1)))))
#define get_bit(reg,bitnum) ((reg & (1<<bitnum))>>bitnum) // get bit macro used to get the value of a certain bit.

void I2C_Init();
uint8_t I2C_Start(char write_address);
uint8_t I2C_Repeated_Start(char read_address);
uint8_t I2C_Write(char data);
char I2C_Read_Ack();
char I2C_Read_Nack();
void I2C_Stop();
unsigned char write_to_eeprom(unsigned char eeprom_write_address, uint16_t address, int num_bytes, unsigned char bytes_to_write[]);
unsigned char read_from_eeprom(unsigned char eeprom_write_address, unsigned char eeprom_read_address, uint16_t address, int num_bytes, unsigned char bytes_to_write[]);
unsigned char write_to_eeprom_1024(unsigned char eeprom_write_address, uint16_t address, int num_bytes, unsigned char bytes_to_write[]);
unsigned char read_from_eeprom_1024(unsigned char eeprom_write_address, unsigned char eeprom_read_address, uint16_t address, int num_bytes, unsigned char bytes_to_write[]);

