#ifndef _RGB_H_
#define _RGB_H_

#ifdef __cplusplus
#define EXTERNC extern "C"
#else
#define EXTERNC
#endif

#include <stdint.h>

#define RED         0
#define GREEN       1
#define BLUE        2
#define HUE         3
#define SATUATION   4
#define VALUE       5

EXTERNC int RGB_init(void);
EXTERNC uint16_t val2pwm(int val);
EXTERNC void hsv_to_rgb(float* r, float* g, float* b, float h, float s, float v);
EXTERNC void RGB_set(float r, float g, float b);
EXTERNC void RGB_setChannel(int channel, float value);
EXTERNC float RGB_getChannel(int channel);
EXTERNC void RGB_loop(void);

#endif /* _RGB_H_ */
