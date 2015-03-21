// This is for using the arduino type HMW-lib

#include <stdbool.h>
#include <stdint.h>
#include <String.h>
#include "Stream.h"
#include "Arduino-EEPROM.h"

typedef bool boolean;
typedef uint8_t byte;


#define bitRead(value, bit) (((value) >> (bit)) & 0x01)
#define bitSet(value, bit) ((value) |= (1UL << (bit)))

extern unsigned int systime;
#define millis() systime
