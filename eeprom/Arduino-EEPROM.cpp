// Class Implementaion for EEprom



#include "Arduino-EEPROM.h"
#include "eeprom.h"

uint8_t EEPROMClass::read(int address)
{
	//return eeprom_read_byte((unsigned char *) address);
	//uint16_t EE_ReadVariable(uint16_t VirtAddress, uint16_t* Data);
	uint16_t Data;

	if(EE_ReadVariable((uint16_t) address, &Data))
	{
		// != 0 --> Variable not found || Page Not Found
		Data = 0xFFFF;
	}

	return (uint8_t)Data;
}

void EEPROMClass::write(int address, uint8_t value)
{
	//eeprom_write_byte((unsigned char *) address, value);
	//uint16_t EE_WriteVariable(uint16_t VirtAddress, uint16_t Data);

	EE_WriteVariable((uint16_t) address, value);

}

EEPROMClass EEPROM;
