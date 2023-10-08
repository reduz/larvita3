//
// C++ Implementation: deserializerbuffer
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "deserializer_buffer.h"
#include "marshalls.h"

Sint8 DeserializerBuffer::get_int8() {

	ERR_FAIL_COND_V(buffer_len<1,0);
	
	Uint8 res = *(buffer++); buffer_len--;
	
	return (Sint8)res;
}
Uint8 DeserializerBuffer::get_uint8() {

	ERR_FAIL_COND_V(buffer_len<1,0);

	Uint8 res = *(buffer++); buffer_len--;
	
	return res;

}

void DeserializerBuffer::get_uint8_array(Uint8* p_array, int p_count) {

	ERR_FAIL_COND(buffer_len<p_count);
	ERR_FAIL_COND(p_count<0);

	for (int i=0;i<p_count;i++)
		p_array[i]=buffer[i];
	buffer+=p_count;
	buffer_len-=p_count;
}
				
Sint16 DeserializerBuffer::get_int16() {

	ERR_FAIL_COND_V(buffer_len<2,0);
	Uint16 res = decode_uint16(buffer);
	buffer+=2;
	buffer_len-=2;	
	return (Sint16)res;
}
Uint16 DeserializerBuffer::get_uint16() {

	ERR_FAIL_COND_V(buffer_len<2,0);
	Uint16 res = decode_uint16(buffer);
	buffer+=2;
	buffer_len-=2;
	return res;

}

void DeserializerBuffer::get_uint16_array(Uint16* p_array, int p_count) {

	ERR_FAIL_COND(buffer_len< (p_count*2));
	ERR_FAIL_COND(p_count<0);

	for (int i=0;i<p_count;i++)
		p_array[i]=get_uint16();

}

Sint32 DeserializerBuffer::get_int32() {

	ERR_FAIL_COND_V(buffer_len<4,0);

	Uint32 res = decode_uint32(buffer);
	buffer+=4;
	buffer_len-=4;
	return (Sint32)res;

}
Uint32 DeserializerBuffer::get_uint32() {

	ERR_FAIL_COND_V(buffer_len<4,0);

	Uint32 res = decode_uint32(buffer);
	buffer+=4;
	buffer_len-=4;
	return res;

}

void DeserializerBuffer::get_uint32_array(Uint32* p_array, int p_count) {

	ERR_FAIL_COND(buffer_len<(4*p_count));
	ERR_FAIL_COND(p_count<0);

	for (int i=0;i<p_count;i++)
		p_array[i]=get_uint32();

}

Sint64 DeserializerBuffer::get_int64() {

	ERR_FAIL_COND_V(buffer_len<8,0);

	Uint64 res = decode_uint64(buffer);
	buffer+=8;
	buffer_len-=8;
	return (Sint64)res;

}
Uint64 DeserializerBuffer::get_uint64() {

	ERR_FAIL_COND_V(buffer_len<8,0);

	Uint64 res = decode_uint64(buffer);
	buffer+=8;
	buffer_len-=8;
	return res;
}

void DeserializerBuffer::get_uint64_array(Uint64* p_array, int p_count) {

	ERR_FAIL_COND(buffer_len<(8*p_count));
	ERR_FAIL_COND(p_count<0);

	for (int i=0;i<p_count;i++)
		p_array[i]=get_uint64();
		
}

float DeserializerBuffer::get_float() {

	ERR_FAIL_COND_V(buffer_len<4,0);

	float res = decode_float(buffer);
	buffer+=4;
	buffer_len-=4;
	return res;


}
void DeserializerBuffer::get_float_array(float* p_array, int p_count) {

	ERR_FAIL_COND(buffer_len<(4*p_count));
	ERR_FAIL_COND(p_count<0);

	for (int i=0;i<p_count;i++)
		p_array[i]=get_float();

}

double DeserializerBuffer::get_double() {

	ERR_FAIL_COND_V(buffer_len<8,0);

	double res = decode_double(buffer);
	buffer+=8;
	buffer_len-=8;
	return res;

}
void DeserializerBuffer::get_double_array(double* p_array, int p_count) {

	ERR_FAIL_COND(buffer_len<(8*p_count));
	ERR_FAIL_COND(p_count<0);

	for (int i=0;i<p_count;i++)
		p_array[i]=get_double();

}

String DeserializerBuffer::get_string() {

	ERR_FAIL_COND_V(buffer_len<4,"");
	Sint32 len = get_uint32();
	ERR_FAIL_COND_V(buffer_len<len,"");	
	
	String s;
	s.parse_utf8((const char*)buffer);
	buffer+=len;
	buffer_len-=len;
	return s;

}

void DeserializerBuffer::get_variant(Variant& p_variant) {

	p_variant.deserialize(this);

}

DeserializerBuffer::DeserializerBuffer(const Uint8* p_buffer,int p_len) {

	ERR_FAIL_COND(p_len<=0);
	buffer=p_buffer;
	buffer_len=p_len;

}


DeserializerBuffer::~DeserializerBuffer() {
}


