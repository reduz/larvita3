#ifdef OPENGL_ENABLED
//
// C++ Interface: skeleton_gl
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef SKELETON_GL_H
#define SKELETON_GL_H

/**
	@author Juan Linietsky <reduzio@gmail.com>
*/
#include "render/skeleton.h"
/**
	@author Juan Linietsky <reduzio@gmail.com>
*/
class Skeleton_GL : public Skeleton {

	IAPI_TYPE(Skeleton_GL,Skeleton);

	Matrix4* bones;
	int bone_count;
public:

	inline const Matrix4 * get_bones_ptr() const { return bones; }
	
	static IRef<IAPI> IAPI_DEFAULT_INSTANCE_FUNC(const String& p_type,const  IAPI::CreationParams& p_params);	
	
	virtual void set(String p_path,const Variant& p_value);
	virtual Variant get(String p_path) const;
	virtual void get_property_list( List<PropertyInfo> *p_list ) const;
	
	virtual void set_bone_matrix(int p_bone_idx,const Matrix4& p_matrix);
	virtual Matrix4 get_bone_matrix(int p_bone_idx) const;

	virtual int get_bone_count() const { return bone_count; }
	
	virtual void create(int p_bone_count);
	
	Skeleton_GL();
	~Skeleton_GL();

};


#endif
#endif // OPENGL_ENABLED
