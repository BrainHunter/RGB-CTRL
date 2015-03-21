#ifndef DEBUGSTREAM_H_
#define DEBUGSTREAM_H_

#include "stream.h"

class DebugStream: public Stream
{
public:
    size_t write(uint8_t c);
	int read(void);
	int available(void);
	void flush(void);

    size_t print(const char[]);
    size_t print(int, int = DEC);

};

#endif /* DEBUGSTREAM_H_ */
