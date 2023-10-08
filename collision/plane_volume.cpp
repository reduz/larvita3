//
// C++ Implementation: plane_volume
//
// Description: 
//
//
// Author: ,,, <red@lunatea>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "plane_volume.h"
#include "error_macros.h"

IRef<IAPI> PlaneVolume::IAPI_DEFAULT_INSTANCE_FUNC(const String& p_type,const  IAPI::CreationParams& p_params) {
	
	ERR_FAIL_COND_V( !p_params.has("plane"), IRef<IAPI>() );
	ERR_FAIL_COND_V( !p_params["plane"].get_type()!=Variant::PLANE, IRef<IAPI>() );
	
	return memnew( PlaneVolume( p_params["plane"].get_plane() ) );
}

void PlaneVolume::set(String p_path, const Variant& p_value) {
	
	
}
Variant PlaneVolume::get(String p_path) const {
	
	if (p_path=="plane")
		return plane;
	return Variant();	
}

void PlaneVolume::get_property_list( List<PropertyInfo> *p_list ) const {
	
	p_list->push_back( PropertyInfo( Variant::PLANE, "plane", PropertyInfo::USAGE_CREATION_PARAM ) );
}

void PlaneVolume::project_range(const Vector3& p_normal,const Matrix4& p_transform,float& r_min, float& r_max) const {

	// not correct, not needed to be correct
	r_min=0;
	r_max=0;
}
	
void PlaneVolume::get_support(const Vector3& p_normal,const Matrix4& p_transform,Vector3 *p_vertices,int* p_count,int p_max) const {

	// not correct, not needed to be correct
	*p_count=0;
}

Vector3 PlaneVolume::get_local_inertia(float p_mass) const {


	return Vector3(99999999,99999999,99999999); // useless since plane mass will likely to be infinite
}

bool PlaneVolume::intersect_segment(const Matrix4& p_transform,const Vector3& p_from,const Vector3& p_to,Vector3& r_clip,Vector3& r_normal) const {
	
	
	return false;
}


AABB PlaneVolume::compute_AABB( const Matrix4& p_transform ) const {

	return AABB( Vector3(-1e10,-1e10,-1e10), Vector3(2*1e10,2*1e10,2*1e10));
}

Vector3 PlaneVolume::get_center( const Matrix4& p_transform) const {
	
	return p_transform.get_translation();	
}

void PlaneVolume::set_plane(const Plane& p_plane) {

	plane=p_plane;
}

PlaneVolume::PlaneVolume(const Plane& p_plane) {

	plane=p_plane;
}

