//
// C++ Implementation: ellipsoid_volume
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "ellipsoid_volume.h"
#include "error_macros.h"

IRef<IAPI> EllipsoidVolume::IAPI_DEFAULT_INSTANCE_FUNC(const String& p_type,const  IAPI::CreationParams& p_params) {
	
	ERR_FAIL_COND_V(p_params.has("radius"),IRef<IAPI>());
	ERR_FAIL_COND_V(p_params["radius"].get_type()!=Variant::VECTOR3,IRef<IAPI>());
	
	return memnew( EllipsoidVolume( p_params.get("radius").get_vector3() ) );
}
	

void EllipsoidVolume::set(String p_path, const Variant& p_value) {
	
	
}
Variant EllipsoidVolume::get(String p_path) const {
	
	if (p_path=="radius")
		return radius;
	
	return Variant();	
}

void EllipsoidVolume::get_property_list( List<PropertyInfo> *p_list ) const {
	
	p_list->push_back( PropertyInfo(Variant::VECTOR3,"radius",PropertyInfo::USAGE_CREATION_PARAM ) );
}


void EllipsoidVolume::project_range(const Vector3& p_normal,const Matrix4& p_transform,float& r_min, float& r_max) const {

	Vector3 s;
	int _c;
	get_support( p_normal, p_transform, &s, &_c, 1 );
	
	r_max = p_normal.dot( s  );
	
	get_support( -p_normal, p_transform, &s, &_c, 1 );
	
	r_min = p_normal.dot( s  );
	
	return;
	
		
	float d = p_normal.dot( p_transform.get_translation() );
	
	Vector3 n = p_normal;
	p_transform.inverse_transform_no_translation(n);
	n.normalize();
	
	float w=Math::absf(radius.dot(n));;
	//printf("d is %f, w is %f\n",d,w);
	r_min = d - w;
	r_max = d + w;

}




void EllipsoidVolume::get_support(const Vector3& p_normal,const Matrix4& p_transform,Vector3 *p_vertices,int* p_count,int p_max) const {
	
	
	Vector3 n=p_normal;
	p_transform.inverse_transform_no_translation(n);
	n.normalize();
	
	Vector3 res = n*radius;
		
	p_transform.transform_copy( res, *p_vertices );
	*p_count=1;	
}

bool EllipsoidVolume::intersect_segment(const Matrix4& p_transform,const Vector3& p_from,const Vector3& p_to,Vector3& r_clip,Vector3& r_normal) const {
		
	return false;
}

Vector3 EllipsoidVolume::get_local_inertia(float p_mass) const {

	
	return radius * radius * 0.4 * p_mass;
	
	
	AABB aabb=compute_AABB(Matrix4());
	
	Vector3 itmp = aabb.size;
	
	itmp.x*=itmp.x;
	itmp.y*=itmp.y;
	itmp.z*=itmp.z;
	
	return (Vector3(itmp.y+itmp.z,itmp.x+itmp.z,itmp.x+itmp.y)*p_mass)*0.0833333;
}


AABB EllipsoidVolume::compute_AABB( const Matrix4& p_transform ) const {
	
	AABB res;
	res.pos=-radius;
	res.size=radius*2;
		
	p_transform.transform_aabb(res);
	
	return res;

}

Vector3 EllipsoidVolume::get_center( const Matrix4& p_transform) const {
	
	return p_transform.get_translation();
}

EllipsoidVolume::EllipsoidVolume(Vector3 p_radius) {

	radius=p_radius;
}


EllipsoidVolume::~EllipsoidVolume()
{
}


