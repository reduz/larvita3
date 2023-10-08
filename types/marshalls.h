//
// C++ Interface: marshalls
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//


#ifndef MARSHALLS_H
#define MARSHALLS_H

#include "typedefs.h"


/**
  * Miscelaneous helpers for marshalling data types, and encoding
  * in an endian independent way
  */
  

union MarshallFloat {

	Uint32 i; ///< int
	float f; ///< float
};

union MarshallDouble {

	Uint64 l; ///< long long
	double d; ///< double
};

static inline unsigned int encode_uint16(Uint16 p_uint, Uint8 *p_arr) {

	for (int i=0;i<2;i++) {
	
		*p_arr=p_uint&0xFF;
		p_arr++; p_uint>>=8;
	}
	
	return sizeof( Uint16 );
}

static inline unsigned int encode_uint32(Uint32 p_uint, Uint8 *p_arr) {

	for (int i=0;i<4;i++) {
	
		*p_arr=p_uint&0xFF;
		p_arr++; p_uint>>=8;
	}
	
	return sizeof( Uint32 );
}

static inline unsigned int encode_float(float p_float, Uint8 *p_arr) {

	MarshallFloat mf;
	mf.f=p_float;
	encode_uint32( mf.i, p_arr );

	return sizeof( Uint32 );
}

static inline unsigned int encode_uint64(Uint64 p_uint, Uint8 *p_arr) {

	for (int i=0;i<8;i++) {
	
		*p_arr=p_uint&0xFF;
		p_arr++; p_uint>>=8;
	}
	
	return sizeof(Uint64);
}

static inline unsigned int encode_double(double p_double, Uint8 *p_arr) {

	MarshallDouble md;
	md.d=p_double;
	encode_uint64( md.l, p_arr );

	return sizeof(Uint64);
	
}


static inline int encode_cstring(const char *p_string, Uint8 * p_data) {

	int len=0;
	
	while (*p_string) {

		if (p_data) {
		
			*p_data=(Uint8)*p_string;
			p_data++;
		}
		p_string++;
		len++;
	};
	
	if (p_data) *p_data = 0;
	return len+1;
}

static inline Uint16 decode_uint16(const Uint8 *p_arr) {

	Uint16 u=0;
	
	for (int i=0;i<2;i++) {
	
		Uint16 b = *p_arr;
		b<<=(i*8);
		u|=b;
		p_arr++;
	}
	
	return u;
}

static inline Uint32 decode_uint32(const Uint8 *p_arr) {

	Uint32 u=0;
	
	for (int i=0;i<4;i++) {
	
		Uint32 b = *p_arr;
		b<<=(i*8);
		u|=b;
		p_arr++;
	}
	
	return u;
}

static inline float decode_float(const Uint8 *p_arr) {
	
	MarshallFloat mf;
	mf.i = decode_uint32(p_arr);
	return mf.f;
}

static inline Uint64 decode_uint64(const Uint8 *p_arr) {

	Uint64 u=0;
	
	for (int i=0;i<8;i++) {
	
		Uint64 b = (*p_arr)&0xFF;
		b<<=(i*8);
		u|=b;
		p_arr++;
	}
	
	return u;
}

static inline double decode_double(const Uint8 *p_arr) {

	MarshallDouble md;
	md.l = decode_uint64( p_arr );
	return md.d;

}

#endif
