// Class Implementaion for EEprom



#include "Arduino-EEPROM.h"
#include "eeprom.h"

uint8_t EEPROMClass::read(int address)
{
	//return eeprom_read_byte((unsigned char *) address);
	//uint16_t EE_ReadVariable(uint16_t VirtAddress, uint16_t* Data);
	uint16_t Data;

    if(address >= (int)E2END - 3 && address <= (int)E2END)     // last 4 bytes --> txSenderAddress -> UID[0]
    {
            int shift = 3 - (E2END-address);
        	uint32_t *uid;
            uid = (uint32_t*)0x1FFFF7E8;
            //printf("UID[0]: 0x%08X\n\r", (unsigned int)uid[0]);
            Data = (uid[0] >> shift) &0xFF;
    }
    else if(EE_ReadVariable((uint16_t) address, &Data))
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
