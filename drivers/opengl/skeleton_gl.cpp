#ifdef OPENGL_ENABLED
//
// C++ Implementation: skeleton_gl
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "skeleton_gl.h"

IRef<IAPI> Skeleton_GL::IAPI_DEFAULT_INSTANCE_FUNC(const String& p_type,const  IAPI::CreationParams& p_params) {
	
	ERR_FAIL_COND_V(!p_params.has("bone_count"), IRef<IAPI>());
	ERR_FAIL_COND_V(!p_params["bone_count"].get_type()!=Variant::INT,IRef<IAPI>());
	
	Skeleton_GL *sgl = memnew( Skeleton_GL );
	sgl->create( p_params["bone_count"].get_int() );
	
	return sgl;
}

void Skeleton_GL::set(String p_path,const Variant& p_value) {
	
	if (p_path.find("bone[")==0) {
		
		p_path.erase(0,5);
		int idx=p_path.to_int();
		set_bone_matrix(idx,p_value.get_matrix4());
	}
}

Variant Skeleton_GL::get(String p_path) const {
	
	if (p_path.find("bone[")==0) {
		
		p_path.erase(0,5);
		int idx=p_path.to_int();
		ERR_FAIL_INDEX_V(idx,bone_count,Variant());
		return bones[idx];
	}
		
	if (p_path=="bone_count")
		return bone_count;
		
	return Variant();
}
void Skeleton_GL::get_property_list( List<PropertyInfo> *p_list ) const {
	
	p_list->push_back( PropertyInfo( Variant::INT, "bone_count", PropertyInfo::USAGE_CREATION_PARAM ));
	for (int i=0;i<bone_count;i++) {
		
		p_list->push_back(PropertyInfo( Variant::MATRIX4, "bone["+String::num(i)+"]"));
	}
	
}

void Skeleton_GL::set_bone_matrix(int p_bone_idx,const Matrix4& p_matrix) {
	
	ERR_FAIL_INDEX(p_bone_idx,bone_count);	
	bones[p_bone_idx]=p_matrix;
}
Matrix4 Skeleton_GL::get_bone_matrix(int p_bone_idx) const {
	
	ERR_FAIL_INDEX_V(p_bone_idx,bone_count,Matrix4());	
	return bones[p_bone_idx];
	
}

void Skeleton_GL::create(int p_bone_count) {
	
	ERR_FAIL_COND(p_bone_count<=0 || p_bone_count >0xFFF);
	if (bones)
		memdelete(bones);
	
	bones = memnew_arr( Matrix4,p_bone_count);
	bone_count=p_bone_count;

}

Skeleton_GL::Skeleton_GL()
{
	bones=NULL;
	bone_count=0;
}


Skeleton_GL::~Skeleton_GL()
{
	if (bones)
		memdelete_arr(bones);
}


#endif // OPENGL_ENABLED
