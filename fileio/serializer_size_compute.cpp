//
// C++ Implementation: serializer_size_compute
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "serializer_size_compute.h"

#include "marshalls.h"

void SerializerSizeCompute::add_int8(Sint8 p_int) {
	
	count++;	
}
void SerializerSizeCompute::add_uint8(Uint8 p_int) {
	
	count++;	

}

void SerializerSizeCompute::add_uint8_array(const Uint8* p_array, int p_count) {
	
	count+=p_count;;	

}
				
void SerializerSizeCompute::add_int16(Sint16 p_int) {
	
	count+=2;
}
void SerializerSizeCompute::add_uint16(Uint16 p_int) {
	
	count+=2;
	
}

void SerializerSizeCompute::add_uint16_array(const Uint16* p_array, int p_count) {
	
	count+=p_count*2;
	
}

void SerializerSizeCompute::add_int32(Sint32 p_int) {
	
	count+=4;

}
void SerializerSizeCompute::add_uint32(Uint32 p_int) {
	
	count+=4;

}

void SerializerSizeCompute::add_uint32_array(const Uint32* p_array, int p_count) {
	
	count+=4*p_count;
	
}

void SerializerSizeCompute::add_int64(Sint64 p_int) {
	
	count+=8;

}
void SerializerSizeCompute::add_uint64(Uint64 p_int) {
	
	count+=8;
	
}

void SerializerSizeCompute::add_uint64_array(const Uint64* p_array, int p_count) {
	
	count+=8*p_count;
}

void SerializerSizeCompute::add_float(float p_float) {
	
	count+=4;

}

void SerializerSizeCompute::add_float_array(const float* p_array, int p_count) {
	
	count+=4*p_count;
	
}

void SerializerSizeCompute::add_double(double p_double) {
		
	count+=8;

}

void SerializerSizeCompute::add_double_array(const double* p_array, int p_count) {
	
	count+=p_count*8;

}

void SerializerSizeCompute::add_string(String p_string) {
	
	
	
	CharString utf8=p_string.utf8();

	int len = encode_cstring(utf8.get_data(), NULL );
	
	add_uint32(len);
	count+=len;	

}

void SerializerSizeCompute::add_variant(const Variant& p_variant) {
	
	p_variant.serialize(this);
	
}

void SerializerSizeCompute::reset() {
	
	count=0;
}

int SerializerSizeCompute::get_size() const {
	
	return count;
}

SerializerSizeCompute::SerializerSizeCompute() {
	count=0;
}


SerializerSizeCompute::~SerializerSizeCompute()
{
}


