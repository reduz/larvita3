//
// C++ Interface: deserializer_file
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef DESERIALIZER_FILE_H
#define DESERIALIZER_FILE_H

#include "deserializer.h"
#include "dvector.h"

class FileAccess;

/**
	@author Juan Linietsky <reduzio@gmail.com>
*/
class Deserializer_File : public Deserializer {

	FileAccess *f;
	DVector<Uint8> tmp_buf;

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

	
	Deserializer_File(FileAccess* p_fa);
	~Deserializer_File();

};

#endif
