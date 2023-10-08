//
// C++ Interface: serializer_file
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef SERIALIZER_FILE_H
#define SERIALIZER_FILE_H

#include "fileio/serializer.h"
#include "os/file_access.h"
#include "dvector.h"

/**
	@author Juan Linietsky <reduzio@gmail.com>
*/
class Serializer_File : public Serializer {
	
	FileAccess *f;
	DVector<Uint8> variant_sl;
public:
	
	virtual void add_int8(Sint8 p_int);
	virtual void add_uint8(Uint8 p_int);
	
	virtual void add_uint8_array(const Uint8* p_array, int p_count);
				    
	virtual void add_int16(Sint16 p_int);
	virtual void add_uint16(Uint16 p_int);
	
	virtual void add_uint16_array(const Uint16* p_array, int p_count);
	
	virtual void add_int32(Sint32 p_int);
	virtual void add_uint32(Uint32 p_int);
	
	virtual void add_uint32_array(const Uint32* p_array, int p_count);
	
	virtual void add_int64(Sint64 p_int);
	virtual void add_uint64(Uint64 p_int);
	
	virtual void add_uint64_array(const Uint64* p_array, int p_count);
	
	virtual void add_float(float p_float);
	virtual void add_float_array(const float* p_array, int p_count);
	
	virtual void add_double(double p_double);
	virtual void add_double_array(const double* p_array, int p_count);
	
	virtual void add_string(String p_string);
	
	virtual void add_variant(const Variant& p_variant);
	
	Serializer_File(FileAccess *p_file);
	~Serializer_File();

};

#endif
