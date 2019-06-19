#ifndef _DEBUG_H_
#define _DEBUG_H_

// Enable/Disable Debug Printing
// #define DEBUG

// Debug Printing Macros
#ifdef DEBUG 
	#define DEBUG_PRINT(x) x
#else 
	#define DEBUG_PRINT(x)
#endif

#endif