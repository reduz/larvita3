#ifdef OPENGL_ENABLED
//
// C++ Implementation: surface_gl14
//
// Description: 
//
//
// Author: ,,, <red@lunatea>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "surface_gl14.h"
#include "os/copymem.h"
/*

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

		format/flags:int - format flags
		format/array_len:int - vertex array lenght
		format/index_array_len:int - index array lenght
		format/custom_array:int_array[4]
	*/

static const char* __array_name_str[32]={
"array/vertex",
"",
"array/normal",
"array/binormal",
"array/tangent",
"array/color",
"",
"",
"array/tex_uv_0",
"array/tex_uv_1",
"array/tex_uv_2",
"array/tex_uv_3",
"array/tex_uv_4",
"array/tex_uv_5",
"array/tex_uv_6",
"array/tex_uv_7",
"array/weight_0",
"array/weight_1",
"array/weight_2",
"array/weight_3",
"array/weight_4",
"array/weight_5",
"array/weight_6",
"array/weight_7",
"array/custom_0",
"array/custom_1",
"array/custom_2",
"array/custom_3",
"",
"",
"array/index",
""
};

static const Variant::Type __array_type[32]={
	Variant::REAL_ARRAY,
	Variant::NIL,
	Variant::REAL_ARRAY,
	Variant::REAL_ARRAY,
	Variant::REAL_ARRAY,
	Variant::INT_ARRAY,
	Variant::NIL,
	Variant::NIL,
	Variant::REAL_ARRAY,
	Variant::REAL_ARRAY,
	Variant::REAL_ARRAY,
	Variant::REAL_ARRAY,
	Variant::REAL_ARRAY,
	Variant::REAL_ARRAY,
	Variant::REAL_ARRAY,
	Variant::REAL_ARRAY,
	Variant::BYTE_ARRAY,
	Variant::BYTE_ARRAY,
	Variant::BYTE_ARRAY,
	Variant::BYTE_ARRAY,
	Variant::BYTE_ARRAY,
	Variant::BYTE_ARRAY,
	Variant::BYTE_ARRAY,
	Variant::BYTE_ARRAY,
	Variant::NIL,
	Variant::NIL,
	Variant::NIL,
	Variant::NIL,
	Variant::NIL,
	Variant::NIL,
	Variant::INT_ARRAY,
	Variant::NIL,
};

IRef<IAPI> Surface_GL14::IAPI_DEFAULT_INSTANCE_FUNC(const String& p_type,const  IAPI::CreationParams& p_params) {

	Surface_GL14 * s = memnew( Surface_GL14 );

	if (
	    	p_params.has("format/primitive") &&
		p_params.has("format/array") &&
		p_params.has("format/array_len") &&
		p_params.has("format/index_array_len") &&
		p_params.has("format/custom_array")
	   ) {

		PrimitiveType p=(PrimitiveType)p_params["format/primitive"].get_int();
		int format=p_params["format/array"].get_int();
		int flags=p_params["format/flags"].get_int();
		int array_len=p_params["format/array_len"].get_int();
		int index_array_len=p_params["format/index_array_len"].get_int();

		s->create(p,format,flags,array_len,index_array_len);

	}

	return s;
}

void Surface_GL14::array_lock(ArrayTarget p_target) const {
	
	if (use_VBO) {
		
		if ( p_target==TARGET_VERTEX ) 
			glBindBufferARB(GL_ARRAY_BUFFER,vertex_id);
		else
			glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER,index_id);
	} else {
		
		MID mid = (p_target==TARGET_INDEX)?index_mem:vertex_mem;		
		Memory::lock( mid );
	}
		
}

void Surface_GL14::copy_to_array(ArrayTarget p_target,const void * p_src,int p_offset, int p_bytes) {
	
	if (use_VBO) {
	
		if (p_target==TARGET_VERTEX)
			glBufferSubDataARB( GL_ARRAY_BUFFER, p_offset, p_bytes, p_src );
		else
			glBufferSubDataARB( GL_ELEMENT_ARRAY_BUFFER, p_offset, p_bytes, p_src );
	
	} else {
		MID mid = (p_target==TARGET_INDEX)?index_mem:vertex_mem;
		ERR_FAIL_COND( p_bytes+p_offset > Memory::get_size( mid ) );
		
		const Uint8 * src = (const Uint8*)p_src;
		Uint8 * dst = (Uint8*)Memory::get( mid );	
		ERR_FAIL_COND( !dst );
		dst+=p_offset;
		
		while(p_bytes--) {
			
			*dst++ = *src++;
		}
	}
	
	
}
void Surface_GL14::copy_from_array(ArrayTarget p_target,void * p_dst,int p_offset,int p_bytes ) const {
	
	
	if (use_VBO) {
		
		if (p_target==TARGET_VERTEX)
			glGetBufferSubDataARB( GL_ARRAY_BUFFER, p_offset, p_bytes, p_dst );
		else
			glGetBufferSubDataARB( GL_ELEMENT_ARRAY_BUFFER, p_offset, p_bytes, p_dst );
			
	} else {
		MID mid = (p_target==TARGET_INDEX)?index_mem:vertex_mem;
		ERR_FAIL_COND( p_bytes+p_offset > Memory::get_size( mid ) );
		
		Uint8 * dst = (Uint8*)p_dst;
		const Uint8 * src = (const Uint8*)Memory::get( mid );	
		ERR_FAIL_COND( !src );
		src+=p_offset;
		
		while(p_bytes--) {
			
			*dst++ = *src++;
		}
	}

}

void Surface_GL14::array_unlock(ArrayTarget p_target) const {
	
	if (use_VBO) {
		
		if ( p_target==TARGET_VERTEX ) 
			glBindBufferARB(GL_ARRAY_BUFFER,0);
		else
			glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER,0);
	} else {
		
		MID mid = (p_target==TARGET_INDEX)?index_mem:vertex_mem;
		Memory::unlock( mid );	
	}
}


void Surface_GL14::set(String p_path,const Variant& p_value) {

	if (!configured)
		return;
	// material
	if (p_path=="material") {

		material=p_value.get_IOBJ();
	}

	if (p_path.find("array/")==0) {

		int type=-1;
		for (int i=0;i<32;i++) {

			if (p_path==__array_name_str[i]) {

				type=i;
				break;
			}
		}

		if (type==-1)
			return;
		return set_array( (ArrayType)type,p_value );
	}

	

	// arrays


}
Variant Surface_GL14::get(String p_path) const {

	if (!configured)
		return Variant();

	if (p_path=="material")
		return IObjRef(material);
	if (p_path=="format/primitive") {
		return (int)primitive;
	};
	if (p_path=="format/array")
		return format;
	if (p_path=="format/flags")
		return flags;
	if (p_path=="format/array_len")
		return array_len;
	if (p_path=="format/index_array_len")
		return index_array_len;
	if (p_path=="format/custom_array") {
		Variant cf(Variant::BYTE_ARRAY,4);
		//fill
		return cf;
	}

	
	if (p_path.find("array/")==0) {

		int type=-1;
		for (int i=0;i<32;i++) {

			if (p_path==__array_name_str[i]) {

				type=i;
				break;
			}
		}

		if (type==-1)
			return Variant();

		return get_array( (ArrayType)type );
	}


	ERR_FAIL_V(Variant());
}
void Surface_GL14::get_property_list( List<PropertyInfo> *p_list ) const {

	if (!configured)
		return;
	p_list->push_back( PropertyInfo( Variant::INT, "format/primitive", PropertyInfo::USAGE_CREATION_PARAM) );
	p_list->push_back( PropertyInfo( Variant::INT, "format/array", PropertyInfo::USAGE_CREATION_PARAM) );
	p_list->push_back( PropertyInfo( Variant::INT, "format/flags", PropertyInfo::USAGE_CREATION_PARAM) );
	p_list->push_back( PropertyInfo( Variant::INT, "format/array_len", PropertyInfo::USAGE_CREATION_PARAM) );
	p_list->push_back( PropertyInfo( Variant::INT, "format/index_array_len", PropertyInfo::USAGE_CREATION_PARAM) );
	p_list->push_back( PropertyInfo( Variant::INT_ARRAY, "format/custom_array", PropertyInfo::USAGE_CREATION_PARAM) );

	p_list->push_back( PropertyInfo( Variant::IOBJ, "material") );
	for (int i=0;i<array_count;i++) {

		ERR_FAIL_INDEX(array[i].type,32);
		p_list->push_back( PropertyInfo( __array_type[ array[i].type ], __array_name_str[ array[i].type ] ) );
	}

}

IAPI::PropertyHint Surface_GL14::get_property_hint(String p_path) const {

	if (p_path=="material")
		return PropertyHint( PropertyHint::HINT_IOBJ_TYPE, "Material" );

	return PropertyHint();
}


int Surface_GL14::_find_array(ArrayType p_type) const {

	for (int i=0;i<array_count;i++)
		if ((int)array[i].type==p_type)
			return i;

	return -1;
}

void Surface_GL14::set_material(IRef<Material> p_material) {

	material=p_material;
}


IRef<Material> Surface_GL14::get_material() const {

	return material;
}

void Surface_GL14::set_array(ArrayType p_type,const Variant& p_array) {

	ERR_FAIL_COND(!configured);
	ERR_FAIL_INDEX(p_type,32);

	int idx=_find_array(p_type);

	ERR_FAIL_COND(idx==-1);

	ArrayData &a=array[idx];

	if (p_type==ARRAY_INDEX) {

		ERR_FAIL_COND(index_array_len<=0);
		ERR_FAIL_COND( p_array.get_type() != Variant::INT_ARRAY );
		ERR_FAIL_COND( p_array.size() != index_array_len );

		/* determine wether using 16 or 32 bits indices */

		array_lock(TARGET_INDEX);

		DVector<int> array = p_array.get_int_array();
		array.read_lock();
		const int *src=array.read();
		
		for (int i=0;i<index_array_len;i++) {

			if (a.size<=(1<<16)) {
				Uint16 v=src[i];

				copy_to_array(TARGET_INDEX,&v,i*a.size,a.size);
			} else {
				Uint32 v=src[i];
				copy_to_array(TARGET_INDEX,&v,i*a.size,a.size);

			}
		}

		array.read_unlock();

		array_unlock(TARGET_INDEX);

		a.configured=true;

		return;
	};

	switch(p_type) {

		case ARRAY_VERTEX:
		case ARRAY_NORMAL:
		case ARRAY_BINORMAL:
		case ARRAY_TANGENT: {

			int elems = 3;
			if (p_type == ARRAY_TANGENT) {
				elems = 4;
			};
			ERR_FAIL_COND( p_array.get_type() != Variant::REAL_ARRAY );
			ERR_FAIL_COND( p_array.size() != array_len*elems );

			array_lock(TARGET_VERTEX);

			DVector<double> array = p_array.get_real_array();
			array.read_lock();
			const double * src=array.read();
	
			if (p_type==ARRAY_VERTEX)		
				aabb=AABB();
			
			for (int i=0;i<array_len;i++) {

				float vector[4]={ src[i*elems+0] , src[i*elems+1], src[i*elems+2], 0};
				if (p_type == ARRAY_TANGENT) {
					vector[3] = src[i*elems+3];
				};
				
				copy_to_array(TARGET_VERTEX,vector,a.ofs+i*stride,a.size);
				
				if (p_type==ARRAY_VERTEX) {
					Vector3 vtx=Vector3( vector[0],vector[1],vector[2] );
					if (i==0) {

						aabb=AABB(vtx,Vector3());
					} else {

						aabb.expand_to( vtx );
					}

				}


			}

			array.read_unlock();

			array_unlock(TARGET_VERTEX);
		} break;
		case ARRAY_COLOR: {


			ERR_FAIL_COND( p_array.get_type() != Variant::INT_ARRAY );
			ERR_FAIL_COND( p_array.size() != array_len );

			array_lock(TARGET_VERTEX);

			DVector<int> array = p_array.get_int_array();
			array.read_lock();

			const Uint32 * src = (const Uint32*)array.read();
			has_alpha_cache=false;

			for (int i=0;i<array_len;i++) {

				if ((src[i]>>24)!=255)
					has_alpha_cache=true;

				copy_to_array(TARGET_VERTEX,&src[i],a.ofs+i*stride,a.size);
				

			}

			array.read_unlock();

			array_unlock(TARGET_VERTEX);

		} break;
		case ARRAY_TEX_UV_0:
		case ARRAY_TEX_UV_1:
		case ARRAY_TEX_UV_2:
		case ARRAY_TEX_UV_3:
		case ARRAY_TEX_UV_4:
		case ARRAY_TEX_UV_5:
		case ARRAY_TEX_UV_6:
		case ARRAY_TEX_UV_7: {


			ERR_FAIL_COND( p_array.get_type() != Variant::REAL_ARRAY );
			ERR_FAIL_COND( p_array.size() != array_len*2 );

			array_lock(TARGET_VERTEX);

			DVector<double> array = p_array.get_real_array();
			array.read_lock();
			const double * src=array.read();
			for (int i=0;i<array_len;i++) {

				float uv[2]={ src[i*2+0] , src[i*2+1] };

				copy_to_array(TARGET_VERTEX,uv,a.ofs+i*stride,a.size);

			}

			array.read_unlock();

			array_unlock(TARGET_VERTEX);

		} break;
		case ARRAY_WEIGHTS: {


			ERR_FAIL_COND( p_array.get_type() != Variant::BYTE_ARRAY );
			ERR_FAIL_COND( p_array.size() != array_len*8 );

			array_lock(TARGET_VERTEX);

			DVector<Uint8> array = p_array.get_byte_array();
			array.read_lock();

			const Uint8 * src = array.read();

			for (int i=0;i<array_len;i++) {


				Uint8 weights[Surface::MAX_WEIGHT_BYTES];
				for (int j=0;j<Surface::MAX_WEIGHT_BYTES;j++)
					weights[j]=src[i*Surface::MAX_WEIGHT_BYTES+j];
				copy_to_array(TARGET_VERTEX,weights,a.ofs+i*stride,a.size);
				
			}

			array.read_unlock();

			array_unlock(TARGET_VERTEX);
		} break;
		case ARRAY_CUSTOM_0:
		case ARRAY_CUSTOM_1:
		case ARRAY_CUSTOM_2:
		case ARRAY_CUSTOM_3: {


		} break;
		default: { ERR_FAIL();}
	}

	a.configured=true;

}
Variant Surface_GL14::get_array(ArrayType p_type) const {

	ERR_FAIL_COND_V(!configured,Variant());
	ERR_FAIL_INDEX_V(p_type,32,Variant());

	int idx=_find_array(p_type);

	ERR_FAIL_COND_V(idx==-1,Variant());

	ArrayData &a=array[idx];

	ERR_FAIL_COND_V(!a.configured,Variant());

	if (p_type==ARRAY_INDEX) {

		ERR_FAIL_COND_V(index_array_len<=0,Variant());
		Variant varr( Variant::INT_ARRAY, index_array_len );

		array_lock(TARGET_INDEX);


		for (int i=0;i<index_array_len;i++) {

			if (a.size<=(1<<16)) {
				Uint16 v;
				copy_from_array(TARGET_INDEX,&v,i*a.size,a.size);			
				varr.array_set_int(i,v);
			} else {
				Uint32 v;
				
				copy_from_array(TARGET_INDEX,&v,i*a.size,a.size);
				varr.array_set_int(i,v);
			}
		}
		array_unlock(TARGET_INDEX);
		return varr;
	};

	switch(p_type) {


		case ARRAY_VERTEX:
		case ARRAY_NORMAL:
		case ARRAY_BINORMAL: {


			Variant varr( Variant::REAL_ARRAY, array_len*3 );

			array_lock(TARGET_VERTEX);

			for (int i=0;i<array_len;i++) {

				float vector[3];
				copy_from_array(TARGET_VERTEX,vector,a.ofs+i*stride,a.size);
				varr.array_set_real(i*3 + 0,vector[0]);
				varr.array_set_real(i*3 + 1,vector[1]);
				varr.array_set_real(i*3 + 2,vector[2]);
			}


			array_unlock(TARGET_VERTEX);

			return varr;
		} break;
		case ARRAY_TANGENT: {

			Variant varr( Variant::REAL_ARRAY, array_len*4 );

			array_lock(TARGET_VERTEX);

			for (int i=0;i<array_len;i++) {

				float vector[4];
				copy_from_array(TARGET_VERTEX,vector,a.ofs+i*stride,a.size);
				varr.array_set_real(i*4 + 0,vector[0]);
				varr.array_set_real(i*4 + 1,vector[1]);
				varr.array_set_real(i*4 + 2,vector[2]);
				varr.array_set_real(i*4 + 3,vector[3]);
			}


			array_unlock(TARGET_VERTEX);

			return varr;

		} break;
		case ARRAY_COLOR: {

			Variant carr( Variant::INT_ARRAY, array_len );

			array_lock(TARGET_VERTEX);

			for (int i=0;i<array_len;i++) {

				Uint32 col;

				copy_from_array(TARGET_VERTEX,&col,a.ofs+i*stride,a.size);

				carr.array_set_int(i,(int)col);
			}

			array_unlock(TARGET_VERTEX);

			return carr;

		} break;
		case ARRAY_TEX_UV_0:
		case ARRAY_TEX_UV_1:
		case ARRAY_TEX_UV_2:
		case ARRAY_TEX_UV_3:
		case ARRAY_TEX_UV_4:
		case ARRAY_TEX_UV_5:
		case ARRAY_TEX_UV_6:
		case ARRAY_TEX_UV_7: {

			Variant uvarr( Variant::REAL_ARRAY, array_len*2 );

			array_lock(TARGET_VERTEX);

			for (int i=0;i<array_len;i++) {

				float vector[2];
				copy_from_array(TARGET_VERTEX,vector,a.ofs+i*stride,a.size);
				
				uvarr.array_set_real(i*2 + 0,vector[0]);
				uvarr.array_set_real(i*2 + 1,vector[1]);
			}

			array_unlock(TARGET_VERTEX);

			return uvarr;
		} break;
		case ARRAY_WEIGHTS: {

			Variant warr( Variant::BYTE_ARRAY, array_len*8 );

			array_lock(TARGET_VERTEX);

			for (int i=0;i<array_len;i++) {

				Uint8 weights[Surface::MAX_WEIGHT_BYTES];
				copy_from_array(TARGET_VERTEX,weights,a.ofs+i*stride,a.size);

				for (int j=0;j<Surface::MAX_WEIGHT_BYTES;j++)
					warr.array_set_byte(i*Surface::MAX_WEIGHT_BYTES+j,weights[j]);				
			}

			array_unlock(TARGET_VERTEX);

			return warr;
		} break;
		case ARRAY_CUSTOM_0:
		case ARRAY_CUSTOM_1:
		case ARRAY_CUSTOM_2:
		case ARRAY_CUSTOM_3: {


		} break;
		default: { ERR_FAIL_V(Variant());}
	}

	ERR_FAIL_V(Variant());
	return Variant();
}

int Surface_GL14::get_array_len() const {

	return array_len;
}
int Surface_GL14::get_index_array_len() const {

	return index_array_len;
}

int Surface_GL14::get_format() const {

	return format;
}

int Surface_GL14::get_flags() const {

	return format;
}

Surface::PrimitiveType Surface_GL14::get_primitive_type() const {

	return primitive;
}


void Surface_GL14::get_custom_array_format_flags(Uint8*) const {


}

void Surface_GL14::create(PrimitiveType p_primitive,int p_format,int p_flags,int p_array_len,int p_index_array_len,const Uint8* p_custom_array_fmt) {

	ERR_FAIL_COND(configured);
	ERR_FAIL_COND((p_format&FORMAT_ARRAY_VERTEX)==0); // mandatory

	
	array_count=0;
	for (int i=0;i<32;i++) {

		if (!(p_format&(1<<i))) // no array
			continue;
		array_count++;
	}

	array = memnew_arr( ArrayData, array_count);

	int array_idx=0;
	int total_elem_size=0;

	use_VBO=true; //glGenBuffersARB!=NULL; // TODO detect if it's in there
	if (p_format&FORMAT_ARRAY_WEIGHTS) {
		
		use_VBO=false;
	}

	/* create arrays */
	
	for (int i=0;i<32;i++) {


		if (!(p_format&(1<<i))) // no array
			continue;

		ERR_FAIL_INDEX(array_idx,array_count);
		ArrayData&ad=array[array_idx++];
		ad.type=i;
		int elem_size=0;

		switch(ad.type) {

			case ARRAY_VERTEX:
			case ARRAY_NORMAL:
			case ARRAY_BINORMAL: {

				elem_size=3*sizeof(float); // vertex
			} break;
			case ARRAY_TANGENT: {
				elem_size=4*sizeof(float); // vertex
			} break;
			case ARRAY_COLOR: {

				elem_size=4; /* RGBA */
			} break;
			case ARRAY_TEX_UV_0:
			case ARRAY_TEX_UV_1:
			case ARRAY_TEX_UV_2:
			case ARRAY_TEX_UV_3:
			case ARRAY_TEX_UV_4:
			case ARRAY_TEX_UV_5:
			case ARRAY_TEX_UV_6:
			case ARRAY_TEX_UV_7: {

				elem_size=2*sizeof(float);
			} break;
			case ARRAY_WEIGHTS: {

				elem_size=Surface::MAX_WEIGHT_BYTES;

			} break;
			case ARRAY_CUSTOM_0:
			case ARRAY_CUSTOM_1:
			case ARRAY_CUSTOM_2:
			case ARRAY_CUSTOM_3: {

				/* for these types, it's best to start with aligned-to-4 */
				if (total_elem_size&3)
					total_elem_size+=4-(total_elem_size&3);
				elem_size=0;
			};
			case ARRAY_INDEX: {

				if (p_index_array_len<=0) {
					ERR_PRINT("p_index_array_len==NO_INDEX_ARRAY");
					break;
				}
				/* determine wether using 16 or 32 bits indices */
				if (p_index_array_len>(1<<16)) {

					elem_size=4;
				} else {
					elem_size=2;

				}
				
				if (use_VBO) {
					
					glGenBuffersARB(1,&index_id);
					ERR_FAIL_COND(index_id==0);
					glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER,index_id);
					glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER,p_index_array_len*elem_size,NULL,GL_STATIC_DRAW);
					glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER,0); //unbind
				} else {
					index_mem = Memory::alloc_dynamic(p_index_array_len*elem_size);
				}
				index_array_len=p_index_array_len; // only way it can exist
				ad.ofs=0;
				ad.size=elem_size;
				ad.configured=false;
				continue;
			} break;
		}

		ad.ofs=total_elem_size;
		ad.size=elem_size;
		total_elem_size+=elem_size;
		ad.configured=false;
	}

	stride=total_elem_size;
	array_len=p_array_len;
	format=p_format;
	flags=p_flags;
	primitive=p_primitive;
	configured=true;


	/* bind the bigass buffers */

	if (use_VBO) {
		

		glGenBuffersARB(1,&vertex_id);
		ERR_FAIL_COND(vertex_id==0);
		glBindBufferARB(GL_ARRAY_BUFFER,vertex_id);
		glBufferDataARB(GL_ARRAY_BUFFER,array_len*stride,NULL,GL_STATIC_DRAW);
		glBindBufferARB(GL_ARRAY_BUFFER,0); //unbind
	} else {				
		vertex_mem = Memory::alloc_dynamic(array_len*stride);
	}

}

Surface_GL14::Surface_GL14() {

	configured=false;
	array_len=0;
	array_count=0;
	array=NULL;
	primitive=PRIMITIVE_POINTS;
	index_array_len=0;
	format=0;
	flags=0;
	stride=0;
	vertex_id=0;
	index_id=0;
	has_alpha_cache=false;
	use_VBO=false;

}

AABB Surface_GL14::get_AABB() const {

	return aabb;
}

bool Surface_GL14::has_alpha() const {
	
	bool has=false;
	if (!material.is_null() && material->has_alpha())
		return true;
	if (has_alpha_cache)
		return true;
	
	return false;
}



Surface_GL14::~Surface_GL14() {

	if (vertex_id)
		glDeleteBuffersARB(1,&vertex_id);
	if (index_id)
		glDeleteBuffersARB(1,&index_id);
	
	if (!vertex_mem.is_null())
		Memory::free_dynamic( vertex_mem );
		
	if (!index_mem.is_null())
		Memory::free_dynamic( index_mem );
	if (array)
		memdelete_arr( array );

}



#endif // OPENGL_ENABLED
