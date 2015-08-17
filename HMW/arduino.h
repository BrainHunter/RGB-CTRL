#ifndef ARDUINO_H
#define ARDUINO_H
// This is for using the arduino type HMW-lib

#include <stdbool.h>
#include <stdint.h>
#include <String.h>
#include <stdio.h>
#include "Stream.h"
#include "Arduino-EEPROM.h"

typedef bool boolean;
typedef uint8_t byte;


#define bitRead(value, bit) (((value) >> (bit)) & 0x01)
#define bitSet(value, bit) ((value) |= (1UL << (bit)))

extern unsigned int systime;
#define millis() systime

// remove the digital Write. don't need it in this project.
#define digitalWrite(...)
// remove the F() -> ARM core here...
#define F(val)  (val)

#endif

// WMath prototypes
long random(long);
long random(long, long);
void randomSeed(unsigned int);
long map(long, long, long, long, long);


