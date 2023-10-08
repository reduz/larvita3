//
// C++ Interface: deserializer
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef DESERIALIZER_H
#define DESERIALIZER_H


/**
	@author Juan Linietsky <reduzio@gmail.com>
*/

#include "variant.h"

class Deserializer {
public:
	
	virtual Sint8 get_int8()=0;
	virtual Uint8 get_uint8()=0;
	
	virtual void get_uint8_array(Uint8* p_array, int p_count)=0;
				  
	virtual Sint16 get_int16()=0;
	virtual Uint16 get_uint16()=0;
	
	virtual void get_uint16_array(Uint16* p_array, int p_count)=0;
	
	virtual Sint32 get_int32()=0;
	virtual Uint32 get_uint32()=0;
	
	virtual void get_uint32_array(Uint32* p_array, int p_count)=0;
	
	virtual Sint64 get_int64()=0;
	virtual Uint64 get_uint64()=0;
	
	virtual void get_uint64_array(Uint64* p_array, int p_count)=0;
	
	virtual float get_float()=0;
	virtual void get_float_array(float* p_array, int p_count)=0;
	
	virtual double get_double()=0;
	virtual void get_double_array(double* p_array, int p_count)=0;
	
	virtual String get_string()=0;
	
	virtual void get_variant(Variant& p_variant)=0;
	
	/* helpers */
	virtual Vector3 get_vector3();
	virtual Matrix4 get_matrix4();
	virtual Plane get_plane();
	virtual Rect3 get_rect3();
	virtual Quat get_quat();
	virtual Color get_color();
					
	Deserializer();
	virtual ~Deserializer();

};


#endif
