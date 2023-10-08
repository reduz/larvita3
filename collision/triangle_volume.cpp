//
// C++ Implementation: triangle_volume
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "triangle_volume.h"

IRef<IAPI> TriangleVolume::IAPI_DEFAULT_INSTANCE_FUNC(const String& p_type,const  IAPI::CreationParams& p_params) {
	
	return memnew( TriangleVolume );
}
	

void TriangleVolume::set(String p_path, const Variant& p_value) {
	
	
}
Variant TriangleVolume::get(String p_path) const {
	
	return Variant();	
}

void TriangleVolume::get_property_list( List<PropertyInfo> *p_list ) const {
	
}


void TriangleVolume::project_range(const Vector3& p_normal,const Matrix4& p_transform,float& r_min, float& r_max) const {

	face.project_range( p_normal, p_transform, r_min, r_max );
}



void TriangleVolume::get_support(const Vector3& p_normal,const Matrix4& p_transform,Vector3 *p_vertices,int* p_count,int p_max) const {
	
	face.get_support(p_normal, p_transform, p_vertices, p_count, p_max );
	
}

bool TriangleVolume::intersect_segment(const Matrix4& p_transform,const Vector3& p_from,const Vector3& p_to,Vector3& r_clip,Vector3& r_normal) const {
		
	return false;
}

Vector3 TriangleVolume::get_local_inertia(float p_mass) const {

	return Vector3(1,1,1); // not going to be used anyway
}


AABB TriangleVolume::compute_AABB( const Matrix4& p_transform ) const {

	return face.get_aabb();	
}

Vector3 TriangleVolume::get_center( const Matrix4& p_transform) const {
	
	Vector3 center = face.get_median_point();
	p_transform.transform( center );
	return center;
}

void TriangleVolume::set_face(const Face3& p_face3) {
	
	face=p_face3;
}
Face3 TriangleVolume::set_face() const {
	
	return face;
}


TriangleVolume::TriangleVolume() {

}


TriangleVolume::~TriangleVolume()
{
}


