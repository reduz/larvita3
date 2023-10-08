//
// C++ Implementation: sphere_volume
//
// Description: 
//
//
// Author: ,,, <red@lunatea>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "sphere_volume.h"
#include "error_macros.h"

IRef<IAPI> SphereVolume::IAPI_DEFAULT_INSTANCE_FUNC(const String& p_type,const  IAPI::CreationParams& p_params) {
	
	ERR_FAIL_COND_V(!p_params.has("radius"),IRef<IAPI>());
	ERR_FAIL_COND_V(p_params["radius"].get_type()!=Variant::REAL,IRef<IAPI>());
	ERR_FAIL_COND_V(p_params.get("radius").get_real()<=0.0,IRef<IAPI>());
	
	return memnew( SphereVolume( p_params.get("radius").get_real() ) );
}
	

void SphereVolume::set(String p_path, const Variant& p_value) {
	
	
}
Variant SphereVolume::get(String p_path) const {

	if (p_path=="radius")
		return radius;
	
	return Variant();	
}

void SphereVolume::get_property_list( List<PropertyInfo> *p_list ) const {
	
	p_list->push_back( PropertyInfo(Variant::REAL,"radius",PropertyInfo::USAGE_CREATION_PARAM ) );
}


void SphereVolume::project_range(const Vector3& p_normal,const Matrix4& p_transform,float& r_min, float& r_max) const {

	
	float d = p_normal.dot( p_transform.get_translation() );
	
	r_min = d - radius;
	r_max = d + radius;
}


void SphereVolume::set_radius(float p_radius) {

	ERR_FAIL_COND( p_radius <=0.0 );

	radius=p_radius;
}


void SphereVolume::get_support(const Vector3& p_normal,const Matrix4& p_transform,Vector3 *p_vertices,int* p_count,int p_max) const {
	
	if (p_max<1) {
		ERR_FAIL_COND(p_max<1);
		*p_count=0;
	}
	Vector3 v=p_normal;
	v*=radius;
	v+=p_transform.get_translation();
	*p_vertices=v;
	*p_count=1;
	
}

bool SphereVolume::intersect_segment(const Matrix4& p_transform,const Vector3& p_from,const Vector3& p_to,Vector3& r_clip,Vector3& r_normal) const {
		
	return false;
}

Vector3 SphereVolume::get_local_inertia(float p_mass) const {

	float s = 0.4 * p_mass * radius * radius;
	
	return Vector3(s,s,s);
}


AABB SphereVolume::compute_AABB( const Matrix4& p_transform ) const {

	AABB res;
	res.pos = p_transform.get_translation() - Vector3( radius, radius, radius );
	res.size = Vector3( radius*2.0, radius*2.0, radius*2.0 );
	
	return res;
}

Vector3 SphereVolume::get_center( const Matrix4& p_transform) const {
	
	return p_transform.get_translation();
}

SphereVolume::SphereVolume(float p_radius) {

	radius=p_radius;
}


SphereVolume::~SphereVolume()
{
}


