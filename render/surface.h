//
// C++ Interface: surface
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef SURFACE_H
#define SURFACE_H

#include "iapi.h"
#include "render/material.h"
/**
	@author Juan Linietsky <reduzio@gmail.com>
*/
class Surface : public IAPI {
	
	IAPI_TYPE(Surface,IAPI);
public:
	enum {
  		NO_INDEX_ARRAY=-1,
    		MAX_WEIGHTS=4,
		MAX_WEIGHT_BYTES=MAX_WEIGHTS*2
			
	};
	
	enum ArrayType {
			
		ARRAY_VERTEX=0,
		ARRAY_NORMAL=2,
		ARRAY_BINORMAL=3,
		ARRAY_TANGENT=4,
		ARRAY_COLOR=5,
		ARRAY_TEX_UV_0=8,
		ARRAY_TEX_UV_1=9,
		ARRAY_TEX_UV_2=10,
		ARRAY_TEX_UV_3=11,
		ARRAY_TEX_UV_4=12,
		ARRAY_TEX_UV_5=13,
		ARRAY_TEX_UV_6=14,
  		ARRAY_TEX_UV_7=15,
		ARRAY_WEIGHTS=16,
		ARRAY_CUSTOM_0=22,
		ARRAY_CUSTOM_1=23,
		ARRAY_CUSTOM_2=24,
		ARRAY_CUSTOM_3=25,
		ARRAY_CUSTOM_4=26,
		ARRAY_CUSTOM_5=27,
		ARRAY_CUSTOM_6=28,
		ARRAY_CUSTOM_7=29,
		ARRAY_INDEX=30
  		
	};
	
	enum Format {
		/* ARRAY FORMAT FLAGS */
		FORMAT_ARRAY_VERTEX=1<<ARRAY_VERTEX, // mandatory
		FORMAT_ARRAY_NORMAL=1<<ARRAY_NORMAL,
		FORMAT_ARRAY_BINORMAL=1<<ARRAY_BINORMAL,
		FORMAT_ARRAY_TANGENT=1<<ARRAY_TANGENT,
		FORMAT_ARRAY_COLOR=1<<ARRAY_COLOR,
		FORMAT_ARRAY_TEX_UV_0=1<<ARRAY_TEX_UV_0,
		FORMAT_ARRAY_TEX_UV_1=1<<ARRAY_TEX_UV_1,
		FORMAT_ARRAY_TEX_UV_2=1<<ARRAY_TEX_UV_2,
		FORMAT_ARRAY_TEX_UV_3=1<<ARRAY_TEX_UV_3,
		FORMAT_ARRAY_TEX_UV_4=1<<ARRAY_TEX_UV_4,
		FORMAT_ARRAY_TEX_UV_5=1<<ARRAY_TEX_UV_5,
		FORMAT_ARRAY_TEX_UV_6=1<<ARRAY_TEX_UV_6,
		FORMAT_ARRAY_TEX_UV_7=1<<ARRAY_TEX_UV_7,
		FORMAT_ARRAY_WEIGHTS=1<<ARRAY_WEIGHTS,
		FORMAT_ARRAY_CUSTOM_0=1<<ARRAY_CUSTOM_0,
		FORMAT_ARRAY_CUSTOM_1=1<<ARRAY_CUSTOM_1,
		FORMAT_ARRAY_CUSTOM_2=1<<ARRAY_CUSTOM_2,
		FORMAT_ARRAY_CUSTOM_3=1<<ARRAY_CUSTOM_3,
		FORMAT_ARRAY_CUSTOM_4=1<<ARRAY_CUSTOM_4,
		FORMAT_ARRAY_CUSTOM_5=1<<ARRAY_CUSTOM_5,
		FORMAT_ARRAY_CUSTOM_6=1<<ARRAY_CUSTOM_6,
		FORMAT_ARRAY_CUSTOM_7=1<<ARRAY_CUSTOM_7,
		FORMAT_ARRAY_INDEX=1<<ARRAY_INDEX,
	
	};
			
	enum Flags {
  		FLAG_PACK_VERTICES=1<<1
  	};
			
	enum CustomFormat {
	
		CUSTOM_FLOAT=1,
		CUSTOM_FLOAT_2=2,
		CUSTOM_FLOAT_3=3,
		CUSTOM_INT32=4,
		CUSTOM_INT16=5,
		CUSTOM_INT8=6,
	};

	enum PrimitiveType {
		PRIMITIVE_POINTS,	
		PRIMITIVE_LINES,	
		PRIMITIVE_LINE_STRIP,	
		PRIMITIVE_LINE_LOOP,
		PRIMITIVE_TRIANGLES,
		PRIMITIVE_TRIANGLE_STRIP,
		PRIMITIVE_TRIANGLE_FAN,
	};
	
	/* Properties supported:
	
		material:IOBJ(Material) surface material
		AABB:rect3 object AABB (read only!)
	       	
		note, the surface should export only the arrays it
		was created with, and that supports:
		
		array/vertex:float_array (len*3,x,y,z)
		array/normal:float_array (len*3,x,y,z)
		array/binormal_tangent:float_array (len*3,x,y,z,x,y,z)
		array/color:int_array( [msb]a,b,g,r[lsb] 8,8,8,8 encoded 32 bits ), used for both COLOR and COLOR_ALPHA
		array/tex_uv_2:float_array (len*2 u,v)
		(..)
		array/tex_uv_8:float_array (len*2 u,v)
		array/weight_1:byte_array (len,weight)
		array/bone_index_1:int  bone index for weight_1
		(..)			
		array/weight_8:byte_array (len,weight)
		array/bone_index_8:int  bone index for weight_8
		array/custom_1:custom!
		(..)
		array/custom_4:custom!	
		array/index:int (index len)
	
		creation params:
	
		format/primitive - types of PrimitiveType
		format/array:int - array format flags
		format/flags:int - format flags
		format/array_len:int - vertex array lenght
		format/index_array_len:int - index array lenght
		format/custom_array:int_array[4]
			
	*/	
		
	virtual void set_material(IRef<Material> p_material)=0;
	virtual IRef<Material> get_material() const=0;
		
	virtual void set_array(ArrayType p_type,const Variant& p_array)=0;
	virtual Variant get_array(ArrayType p_type) const=0;
	
	virtual int get_array_len() const=0;
	virtual int get_index_array_len() const=0;
		
	virtual int get_format() const=0;
	virtual int get_flags() const=0;
	virtual void get_custom_array_format_flags(Uint8*) const=0;
	virtual PrimitiveType get_primitive_type() const=0;

	virtual bool has_alpha() const=0;
			
	virtual void create(PrimitiveType p_primitive,int p_format,int p_flags,int p_array_len,int p_index_array_len=NO_INDEX_ARRAY,const Uint8* p_custom_array_fmt=NULL)=0;
		
	virtual AABB get_AABB() const =0;
	
	Surface();
	~Surface();

};

#endif
