//
// C++ Implementation: serializer_file
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "serializer_file.h"
#include "marshalls.h"

void Serializer_File::add_int8(Sint8 p_int) {
	
	f->store_8(p_int);	
}
void Serializer_File::add_uint8(Uint8 p_int) {
	
	f->store_8(p_int);	
	
}

void Serializer_File::add_uint8_array(const Uint8* p_array, int p_count) {
	
	f->store_buffer(p_array,p_count);
	
}
				
void Serializer_File::add_int16(Sint16 p_int) {
	
	f->store_16(p_int);	
	
}
void Serializer_File::add_uint16(Uint16 p_int) {
	
	f->store_16(p_int);	
	
}

void Serializer_File::add_uint16_array(const Uint16* p_array, int p_count) {
	
	for(int i=0;i<p_count;i++)
		f->store_16(p_array[i]);
	
}

void Serializer_File::add_int32(Sint32 p_int) {
	
	f->store_32(p_int);	
}
void Serializer_File::add_uint32(Uint32 p_int) {
	
	f->store_32(p_int);	
	
}

void Serializer_File::add_uint32_array(const Uint32* p_array, int p_count) {
	
	for(int i=0;i<p_count;i++)
		f->store_32(p_array[i]);
	
}

void Serializer_File::add_int64(Sint64 p_int) {
	
	f->store_64(p_int);	
	
}
void Serializer_File::add_uint64(Uint64 p_int) {
	
	f->store_64(p_int);	
	
}

void Serializer_File::add_uint64_array(const Uint64* p_array, int p_count) {
	
	for(int i=0;i<p_count;i++)
		f->store_64(p_array[i]);
	
}

void Serializer_File::add_float(float p_float) {
	
	Uint8 float_arr[4];
	encode_float(p_float,float_arr);	
	f->store_buffer(float_arr,4);
	
}
void Serializer_File::add_float_array(const float* p_array, int p_count) {
	
	for (int i=0;i<p_count;i++) {
		
		add_float(p_array[i]);
	}
	
}

void Serializer_File::add_double(double p_double) {
	
	Uint8 double_arr[8];
	encode_double(p_double,double_arr);	
	f->store_buffer(double_arr,8);
	
}
void Serializer_File::add_double_array(const double* p_array, int p_count) {
	
	for (int i=0;i<p_count;i++) {
		
		add_double(p_array[i]);
	}
	
}

void Serializer_File::add_string(String p_string) {
	
	CharString cs = p_string.utf8();
	int len=cs.length();
	f->store_32(len);
	f->store_buffer((const Uint8*)cs.get_data(),len);
}

void Serializer_File::add_variant(const Variant& p_variant) {
	
	p_variant.serialize(this); // get size		
}


Serializer_File::Serializer_File(FileAccess *p_file) {
	
	f=p_file;
}


Serializer_File::~Serializer_File()
{
}


