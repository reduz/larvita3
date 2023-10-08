//
// C++ Interface: hashfuncs
//
// Description: 
//
//
// Author: Juan Linietsky <red@lunatea>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef HASHFUNCS_H
#define HASHFUNCS_H


#include "typedefs.h"

/**
 * Hashing functions
 */


/**
 * DJB2 Hash function
 * @param C String
 * @return 32-bits hashcode
 */
static inline Uint32 hash_djb2(const char *p_cstr) {

	const unsigned char* chr=(const unsigned char*)p_cstr;
	Uint32 hash = 5381;
	Uint32 c;
	
	while ((c = *chr++))
		hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
	
	return hash;
}

template <class T>
static inline Uint64 hash_djb2(const T& p_val, Uint64 p_seed = 5381) {
	static union {
		T val;
		Uint64 ival;
	} u;
	u.val = p_val;
	return ((p_seed << 5) + p_seed) + u.ival;
};



#endif
