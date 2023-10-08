//
// C++ Implementation: serializer_buffer
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "serializer_buffer.h"
#include "marshalls.h"
void SerializerBuffer::add_int8(Sint8 p_int) {
	
	*(buffer++)=p_int;
	
}
void SerializerBuffer::add_uint8(Uint8 p_int) {
	
	*(buffer++)=p_int;

}

void SerializerBuffer::add_uint8_array(const Uint8* p_array, int p_count) {
	
	for (int i=0;i<p_count;i++) {
		
		*(buffer++)=*(p_array)++;;
	}
}
				
void SerializerBuffer::add_int16(Sint16 p_int) {
	
	buffer+=encode_uint16( p_int, buffer );

}
void SerializerBuffer::add_uint16(Uint16 p_int) {
	
	buffer+=encode_uint16( p_int, buffer );

}

void SerializerBuffer::add_uint16_array(const Uint16* p_array, int p_count) {
	
	for (int i=0;i<p_count;i++) {
		
		add_uint16(p_array[i]);
	}
	
}

void SerializerBuffer::add_int32(Sint32 p_int) {
	
	buffer+=encode_uint32( p_int, buffer );

}
void SerializerBuffer::add_uint32(Uint32 p_int) {
	
	buffer+=encode_uint32( p_int, buffer );

}

void SerializerBuffer::add_uint32_array(const Uint32* p_array, int p_count) {
	
	for (int i=0;i<p_count;i++) {
		
		add_uint32(p_array[i]);
	}
	
}

void SerializerBuffer::add_int64(Sint64 p_int) {
	
	buffer+=encode_uint64( p_int, buffer );


}
void SerializerBuffer::add_uint64(Uint64 p_int) {
	
	buffer+=encode_uint64( p_int, buffer );
	
}

void SerializerBuffer::add_uint64_array(const Uint64* p_array, int p_count) {
	
	for (int i=0;i<p_count;i++) {
		
		add_uint64(p_array[i]);
	}
	
}

void SerializerBuffer::add_float(float p_float) {
	
	buffer+=encode_float( p_float, buffer );
	
}

void SerializerBuffer::add_float_array(const float* p_array, int p_count) {
	
	for (int i=0;i<p_count;i++) {
		
		add_float(p_array[i]);
	}
	
}

void SerializerBuffer::add_double(double p_double) {
		
	buffer+=encode_double( p_double, buffer );	
}

void SerializerBuffer::add_double_array(const double* p_array, int p_count) {
	
	for (int i=0;i<p_count;i++) {
		
		add_double(p_array[i]);
	}
}

void SerializerBuffer::add_string(String p_string) {
	
	
	
	CharString utf8=p_string.utf8();

	int len = encode_cstring(utf8.get_data(), NULL );
	
	add_uint32(len);
		
	buffer+=encode_cstring(utf8.get_data(), buffer );
}

void SerializerBuffer::add_variant(const Variant& p_variant) {
	
	p_variant.serialize(this);

}


SerializerBuffer::SerializerBuffer(Uint8* p_buffer) {
	buffer=p_buffer;
}


SerializerBuffer::~SerializerBuffer()
{
}


