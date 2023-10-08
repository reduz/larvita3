//
// C++ Implementation: serialize_array
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "serializer_array.h"
#include "marshalls.h"

void SerializerArray::add_int8(Sint8 p_int) {
	
	int pos=array.size();
	array.resize(array.size()+1);
	array.set(pos,p_int);
	
}
void SerializerArray::add_uint8(Uint8 p_int) {
	
	int pos=array.size();
	array.resize(array.size()+1);
	array.set(pos,p_int);
}

void SerializerArray::add_uint8_array(const Uint8* p_array, int p_count) {
	
	int pos=array.size();
	array.resize( pos + p_count);
	array.write_lock();
	for (int i=0;i<p_count;i++) {
		
		array.write()[pos+i]=p_array[i];
	}
	array.write_unlock();
}
				
void SerializerArray::add_int16(Sint16 p_int) {
	
	int pos=array.size();
	array.resize( pos + 2 );
	
	array.write_lock();
	encode_uint16( p_int, &array.write()[pos] );
	array.write_unlock();				
}
void SerializerArray::add_uint16(Uint16 p_int) {
	
	int pos=array.size();
	array.resize( pos + 2 );
	
	array.write_lock();
	encode_uint16( p_int, &array.write()[pos] );
	array.write_unlock();			
}

void SerializerArray::add_uint16_array(const Uint16* p_array, int p_count) {
	
	for (int i=0;i<p_count;i++) {
		
		add_uint16(p_array[i]);
	}
	
}

void SerializerArray::add_int32(Sint32 p_int) {
	
	int pos=array.size();
	array.resize( pos + 4 );
	
	array.write_lock();
	encode_uint32( p_int, &array.write()[pos] );
	array.write_unlock();		
}
void SerializerArray::add_uint32(Uint32 p_int) {
	
	int pos=array.size();
	array.resize( pos + 4 );
	
	array.write_lock();
	encode_uint32( p_int, &array.write()[pos] );
	array.write_unlock();	
}

void SerializerArray::add_uint32_array(const Uint32* p_array, int p_count) {
	
	for (int i=0;i<p_count;i++) {
		
		add_uint32(p_array[i]);
	}
	
}

void SerializerArray::add_int64(Sint64 p_int) {
	
	int pos=array.size();
	array.resize( pos + 8 );
	
	array.write_lock();
	encode_uint64( p_int, &array.write()[pos] );
	array.write_unlock();	

}
void SerializerArray::add_uint64(Uint64 p_int) {
	
	int pos=array.size();
	array.resize( pos + 8 );
	
	array.write_lock();
	encode_uint64( p_int, &array.write()[pos] );
	array.write_unlock();	
	
}

void SerializerArray::add_uint64_array(const Uint64* p_array, int p_count) {
	
	for (int i=0;i<p_count;i++) {
		
		add_uint64(p_array[i]);
	}
	
}

void SerializerArray::add_float(float p_float) {
	
	int pos=array.size();
	array.resize( pos + 4 );
	
	array.write_lock();
	encode_float( p_float, &array.write()[pos] );
	array.write_unlock();	
	
}

void SerializerArray::add_float_array(const float* p_array, int p_count) {
	
	for (int i=0;i<p_count;i++) {
		
		add_float(p_array[i]);
	}
	
}

void SerializerArray::add_double(double p_double) {
		
	int pos=array.size();
	array.resize( pos + 8 );
	
	array.write_lock();
	encode_double( p_double, &array.write()[pos] );
	array.write_unlock();	
	
}

void SerializerArray::add_double_array(const double* p_array, int p_count) {
	
	for (int i=0;i<p_count;i++) {
		
		add_double(p_array[i]);
	}
}

void SerializerArray::add_string(String p_string) {
	
	
	
	CharString utf8=p_string.utf8();

	int len = encode_cstring(utf8.get_data(), NULL );
	
	add_uint32(len);
	
	int pos=array.size();
	array.resize( pos + len );
	
	array.write_lock();
	encode_cstring(utf8.get_data(), &array.write()[pos] );
	array.write_unlock();	
}

void SerializerArray::add_variant(const Variant& p_variant) {
	
	p_variant.serialize(this);	
}

void SerializerArray::clear() {
	
	array.clear();	
}
const DVector<Uint8>& SerializerArray::get_array() const {
	
	return array;
}

SerializerArray::SerializerArray() {
	
}


SerializerArray::~SerializerArray()
{
}


