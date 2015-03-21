#ifndef __RS485STREAM_H__
#define __RS485STREAM_H__

#include "Stream.h"
#include <stdint.h>
// Wrapper Class for using the rs485 Implementation as a Stream with the HM485-Lib

class RS485StreamClass : public Stream
{
public:
	size_t write(uint8_t c);
	int read(void);
	int available(void);

};

extern RS485StreamClass RS485Stream;

#endif
