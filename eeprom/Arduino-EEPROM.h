#ifndef __ARDUINO_EEPROM_H__
#define __ARDUINO_EEPROM_H__

#include <stdint.h>
#include "eeprom.h"
// Arduino compatible EEProm:

class EEPROMClass {
  public:
    uint8_t read(int);
    void write(int, uint8_t);
};

extern EEPROMClass EEPROM;

#define E2END	PAGE1_END_ADDRESS

#endif
