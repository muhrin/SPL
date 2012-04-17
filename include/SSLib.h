/*
 * SSLib.h
 *
 *  Created on: Aug 17, 2011
 *      Author: Martin Uhrin
 */

#ifndef SSLIB_H
#define SSLIB_H

// INCLUDES ///////////////////////////////////////////////


// DEFINES ///////////////////////////////////////////////
#ifndef NULL
    #define NULL 0
#endif

#define DEBUG 1

#if !DEBUG
	#define SSE_ASSERT(x)
#else
	#include <iostream>
	#define SSE_ASSERT(x) \
	if (! (x)) \
	{ \
		std::cout << "ERROR!! Assert " << #x << " failed\n"; \
		std::cout << " on line " << __LINE__  << "\n"; \
		std::cout << " in file " << __FILE__ << "\n";  \
	}
#endif

#endif /* SSLIB_H */
