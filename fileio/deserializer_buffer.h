//
// C++ Interface: deserializerbuffer
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef DESERIALIZERBUFFER_H
#define DESERIALIZERBUFFER_H

#include "fileio/deserializer.h"

/**
	@author Juan Linietsky <reduzio@gmail.com>
*/
class DeserializerBuffer : public Deserializer {

	const Uint8* buffer;
	int buffer_len;
	
public:

	virtual Sint8 get_int8();
	virtual Uint8 get_uint8();
	
	virtual void get_uint8_array(Uint8* p_array, int p_count);
				  
	virtual Sint16 get_int16();
	virtual Uint16 get_uint16();
	
	virtual void get_uint16_array(Uint16* p_array, int p_count);
	
	virtual Sint32 get_int32();
	virtual Uint32 get_uint32();
	
	virtual void get_uint32_array(Uint32* p_array, int p_count);
	
	virtual Sint64 get_int64();
	virtual Uint64 get_uint64();
	
	virtual void get_uint64_array(Uint64* p_array, int p_count);
	
	virtual float get_float();
	virtual void get_float_array(float* p_array, int p_count);
	
	virtual double get_double();
	virtual void get_double_array(double* p_array, int p_count);
	
	virtual String get_string();
	
	virtual void get_variant(Variant& p_variant);
	
	DeserializerBuffer(const Uint8* p_buffer,int p_len);
	~DeserializerBuffer();

};

#endif
