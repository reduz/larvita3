//
// C++ Implementation: deserializer_file
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "deserializer_file.h"
#include "os/file_access.h"
#include "marshalls.h"

Sint8 Deserializer_File::get_int8() {
	
	return f->get_8();
};

Uint8 Deserializer_File::get_uint8() {
	
	return f->get_8();
};

void Deserializer_File::get_uint8_array(Uint8* p_array, int p_count) {
	
	f->get_buffer(p_array, p_count);
};
	  
Sint16 Deserializer_File::get_int16() {
	
	return f->get_16();
};
Uint16 Deserializer_File::get_uint16() {
	
	return f->get_16();
};

void Deserializer_File::get_uint16_array(Uint16* p_array, int p_count) {
	
	for (int i=0; i<p_count; i++) {
		
		*p_array = f->get_16();
		++p_array;
	};
};

Sint32 Deserializer_File::get_int32() {

	return f->get_32();	
};

Uint32 Deserializer_File::get_uint32() {
	
	return f->get_32();
};

void Deserializer_File::get_uint32_array(Uint32* p_array, int p_count) {
	
	for (int i=0; i<p_count; i++) {
		
		*p_array = f->get_32();
		++p_array;
	};
};

Sint64 Deserializer_File::get_int64() {
	
	return f->get_64();
};

Uint64 Deserializer_File::get_uint64() {
	
	return f->get_64();
};

void Deserializer_File::get_uint64_array(Uint64* p_array, int p_count) {
	
	for (int i=0; i<p_count; i++) {
		
		*p_array = f->get_64();
		++p_array;
	};
};

float Deserializer_File::get_float() {

	float fl;
	f->get_buffer((Uint8*)&fl, sizeof(fl));

	return decode_float((Uint8*)&fl);
};

void Deserializer_File::get_float_array(float* p_array, int p_count) {
	
	for (int i=0; i<p_count; i++) {
		
		*p_array = get_float();
		++p_array;
	};
};

double Deserializer_File::get_double() {

	double d;
	f->get_buffer((Uint8*)&d, sizeof(d));
	
	return decode_double((Uint8*)&d);
};

void Deserializer_File::get_double_array(double* p_array, int p_count) {
	
	for (int i=0; i<p_count; i++) {
		
		*p_array = get_double();
		++p_array;
	};
};

String Deserializer_File::get_string() {
	
	int len = get_uint32();
	if (tmp_buf.size() < len+1) tmp_buf.resize(len+1);
	
	tmp_buf.write_lock();
	f->get_buffer(tmp_buf.write(), len);
	tmp_buf.write()[len] = 0;
	String s = String::utf8((const char*)tmp_buf.write());
	tmp_buf.write_unlock();
	
	return s;
};

void Deserializer_File::get_variant(Variant& p_variant) {

	p_variant.deserialize(this);

};



Deserializer_File::Deserializer_File(FileAccess* p_fa) {

	f = p_fa;
}


Deserializer_File::~Deserializer_File()
{
}


