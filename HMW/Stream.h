#ifndef __STREAM_H__
#define __STREAM_H__

#include <stddef.h>
#include <stdint.h>

#define DEC 10
#define HEX 16
#define OCT 8
#define BIN 2

class Stream {
	public:

		//Stream();
		//virtual ~Stream();

		virtual size_t write(uint8_t c)=0;
		virtual int read(void)=0;
		/*Get the number of bytes (characters) available for reading from the serial port.
		 *This is data that's already arrived and stored in the serial
		 *receive buffer (which holds	4 bytes). available() inherits from
		 *the Stream utility class.
		 *
		 */
		virtual int available(void)=0;
		virtual void flush(void)=0;

        virtual size_t print(const char[])=0;
        virtual size_t print(int, int = DEC)=0;
};

#endif
