#define DEBUG_HMW
#include <stdio.h>

#ifdef DEBUG_HMW
	#define HMWDEBUG( ...) printf( __VA_ARGS__)
#else
	#define HMWDEBUG(...)
#endif
