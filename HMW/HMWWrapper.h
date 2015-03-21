#ifndef __HMWWRAPPER_H__
#define __HMWWRAPPER_H__


 #ifdef __cplusplus
 #define EXTERNC extern "C"
 #else
 #define EXTERNC
 #endif

//Initialize HMW
EXTERNC void Init_HMW(void);
EXTERNC void HMW_loop(void);

EXTERNC void HMWbroadcast(void);

#undef EXTERNC

#endif
