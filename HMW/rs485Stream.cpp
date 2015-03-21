#include "rs485Stream.h"
#include "rs485.h"

size_t RS485StreamClass::write(uint8_t c){

	while(rs485Write((void *)&c, 1)!=1)
	{
		// block until byte is written
	}
	return 1;

}

int RS485StreamClass::read(void){
	int buffer;
	int ret = rs485Read((void*) &buffer, 1);
	if(ret != 1)
	{
		buffer = -1;
	}
	return buffer;
}

int RS485StreamClass::available(void){
	return rs485BytesAvailable();
}

RS485StreamClass RS485Stream;
