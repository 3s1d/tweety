#include <avr/io.h>
#include <avr/interrupt.h>

#include "TWI_Master.h"

/*******************************************************
 Public Function: TWIM_Start

 Purpose: Start the TWI Master Interface

 Input Parameter:
 - uint8_t	Device address
 - uint8_t	Type of required Operation:
 TWIM_READ: Read data from the slave
 TWIM_WRITE: Write data to the slave

 Return Value: uint8_t
 - TRUE:		OK, TWI Master accessible
 - FALSE:	Error in starting TWI Master

 *******************************************************/
uint8_t TWIM_Start(uint8_t Address, uint8_t TWIM_Type)
{
	uint8_t twst;
	/*
	 ** Send START condition
	 */
	TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
	/*
	 ** Wait until transmission completed
	 */
	while(!(TWCR & (1 << TWINT)))
		;
	/*
	 ** Check value of TWI Status Register. Mask prescaler bits.
	 */
	twst = TWSR & 0xF8;
	if((twst != TWI_START) && (twst != TWI_REP_START))
		return FALSE;
	/*
	 ** Send device address
	 */
	TWDR = (Address << 1) + TWIM_Type;
	TWCR = (1 << TWINT) | (1 << TWEN);
	/*
	 ** Wait until transmission completed and ACK/NACK has been received
	 */
	while(!(TWCR & (1 << TWINT)))
		;
	/*
	 ** Check value of TWI Status Register. Mask prescaler bits.
	 */
	twst = TWSR & 0xF8;
	if((twst != TWI_MTX_ADR_ACK) && (twst != TWI_MRX_ADR_ACK))
		return FALSE;

	return TRUE;
}
/*******************************************************
 Public Function: TWIM_Stop

 Purpose: Stop the TWI Master

 Input Parameter: None

 Return Value: None

 *******************************************************/
void TWIM_Stop(void)
{
	/*
	 ** Send stop condition
	 */
	TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);
	/*
	 ** Wait until stop condition is executed and bus released
	 */
	while(TWCR & (1 << TWINT))
		;
}
/*******************************************************
 Public Function: TWIM_Write

 Purpose: Write a byte to the slave

 Input Parameter:
 - uint8_t	Byte to be sent

 Return Value: uint8_t
 - TRUE:		OK, Byte sent
 - FALSE:	Error in byte transmission

 *******************************************************/
uint8_t TWIM_Writec(uint8_t byte)
{
	uint8_t twst;
	/*
	 ** Send data to the previously addressed device
	 */
	TWDR = byte;
	TWCR = (1 << TWINT) | (1 << TWEN);
	/*
	 ** Wait until transmission completed
	 */
	while(!(TWCR & (1 << TWINT)))
		;
	/*
	 ** Check value of TWI Status Register. Mask prescaler bits
	 */
	twst = TWSR & 0xF8;
	if(twst != TWI_MTX_DATA_ACK)
		return 1;

	return 0;
}
/*******************************************************
 Public Function: TWIM_ReadAck

 Purpose: Read a byte from the slave and request next byte

 Input Parameter: None

 Return Value: uint8_t
 - uint8_t	Read byte

 *******************************************************/
uint8_t TWIM_ReadAck(void)
{
	TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA);
	while(!(TWCR & (1 << TWINT)))
		;

	return TWDR;
}
/*******************************************************
 Public Function: TWIM_ReadAck

 Purpose: Read the last byte from the slave

 Input Parameter: None

 Return Value: uint8_t
 - uint8_t	Read byte

 *******************************************************/
uint8_t TWIM_ReadNack(void)
{
	TWCR = (1 << TWINT) | (1 << TWEN);
	while(!(TWCR & (1 << TWINT)))
		;

	return TWDR;
}

uint8_t TWIM_read(uint8_t addr, uint8_t *buf, uint8_t length)
{
	/* start twi */
	if(!TWIM_Start(addr, TWIM_READ))
	{
		TWIM_Stop();
		return FALSE;
	}

	/* get data */
	for(uint8_t i=0; i<length-1; i++)
		buf[i] = TWIM_ReadAck();
	buf[length-1] = TWIM_ReadNack();

	/* stop twi */
	TWIM_Stop();

	return TRUE;
}

uint8_t TWIM_write(uint8_t addr, uint8_t *buf, uint8_t length)
{
	/* start twi */
	if(!TWIM_Start(addr, TWIM_WRITE))
	{
		TWIM_Stop();
		return FALSE;
	}

	/* write data */
	for(uint8_t i=0; i<length; i++)
		TWIM_Writec(buf[i]);

	/* stop twi */
	TWIM_Stop();

	return TRUE;
}

