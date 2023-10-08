//
// C++ Interface: serializer
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef SERIALIZER_H
#define SERIALIZER_H

#include "variant.h"

/**
	@author Juan Linietsky <reduzio@gmail.com>
*/
class Serializer {
public:
	
	virtual void add_int8(Sint8 p_int)=0;
	virtual void add_uint8(Uint8 p_int)=0;
	
	virtual void add_uint8_array(const Uint8* p_array, int p_count)=0;
				    
	virtual void add_int16(Sint16 p_int)=0;
	virtual void add_uint16(Uint16 p_int)=0;
	
	virtual void add_uint16_array(const Uint16* p_array, int p_count)=0;
	
	virtual void add_int32(Sint32 p_int)=0;
	virtual void add_uint32(Uint32 p_int)=0;
	
	virtual void add_uint32_array(const Uint32* p_array, int p_count)=0;
	
	virtual void add_int64(Sint64 p_int)=0;
	virtual void add_uint64(Uint64 p_int)=0;
	
	virtual void add_uint64_array(const Uint64* p_array, int p_count)=0;
	
	virtual void add_float(float p_float)=0;
	virtual void add_float_array(const float* p_array, int p_count)=0;
	
	virtual void add_double(double p_double)=0;
	virtual void add_double_array(const double* p_array, int p_count)=0;
	
	virtual void add_string(String p_string)=0;
	
	virtual void add_variant(const Variant& p_variant)=0;
	
	/* helpers */
	virtual void add_vector3(const Vector3& p_v);
	virtual void add_matrix4(const Matrix4& p_m);
	virtual void add_plane(const Plane& p_p);
	virtual void add_rect3(const Rect3& p_r3);
	virtual void add_quat(const Quat& p_q);
	virtual void add_color(const Color& p_c);
					
	Serializer();
	virtual ~Serializer();

};

#endif
