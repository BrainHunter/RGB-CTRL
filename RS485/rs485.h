#ifndef __RS485_H__
#define __RS485_H__

#include <stdint.h>
#ifdef __cplusplus
#define EXTERNC extern "C"
#else
#define EXTERNC
#endif

#define SEND_BUFFER_SIZE 128
#define RECEIVE_BUFFER_SIZE 128

EXTERNC void rs485Init(void);

EXTERNC int rs485Write(void *buffer, int len);
EXTERNC int rs485Read(void *buffer, int len);
// available bytes to read:
EXTERNC int rs485BytesAvailable(void);

#undef EXTERNC

#endif
