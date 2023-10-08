//
// C++ Implementation: pill_volume
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "pill_volume.h"

#include "error_macros.h"

IRef<IAPI> PillVolume::IAPI_DEFAULT_INSTANCE_FUNC(const String& p_type,const  IAPI::CreationParams& p_params) {
	
	ERR_FAIL_COND_V(p_params.has("radius"),IRef<IAPI>());
	ERR_FAIL_COND_V(p_params.has("height"),IRef<IAPI>());
	ERR_FAIL_COND_V(p_params["radius"].get_type()!=Variant::REAL,IRef<IAPI>());
	ERR_FAIL_COND_V(p_params["height"].get_type()!=Variant::REAL,IRef<IAPI>());
	
	return memnew( PillVolume( p_params.get("height").get_real(), p_params.get("radius").get_real() ) );
}
	

void PillVolume::set(String p_path, const Variant& p_value) {
	
	
}
Variant PillVolume::get(String p_path) const {
	
	if (p_path=="radius")
		return radius;
	if (p_path=="height")
		return height;
	
	return Variant();	
}

void PillVolume::get_property_list( List<PropertyInfo> *p_list ) const {
	
	p_list->push_back( PropertyInfo(Variant::REAL,"radius",PropertyInfo::USAGE_CREATION_PARAM ) );
	p_list->push_back( PropertyInfo(Variant::REAL,"height",PropertyInfo::USAGE_CREATION_PARAM ) );
}


void PillVolume::project_range(const Vector3& p_normal,const Matrix4& p_transform,float& r_min, float& r_max) const {

	Vector3 s;
	int _c;
	get_support( p_normal, p_transform, &s, &_c, 1 );
	
	r_max = p_normal.get_normalized().dot( s  );
	
	get_support( -p_normal, p_transform, &s, &_c, 1 );
	
	r_min = p_normal.get_normalized().dot( s  );

}

#define _EDGE_IS_VALID_SUPPORT_TRESHOLD 0.02

void PillVolume::get_support(const Vector3& p_normal,const Matrix4& p_transform,Vector3 *p_vertices,int* p_count,int p_max) const {
	ERR_FAIL_COND(p_max==0);
	
	Vector3 n=p_normal;
	p_transform.inverse_transform_no_translation(n);
	n.normalize();
	
	float d = n.dot( Vector3(0.0, 1.0, 0.0 ) );	
	
	if (p_max>1 && Math::absf( d )<_EDGE_IS_VALID_SUPPORT_TRESHOLD ) {
	
		// make it flat
		n-=Vector3(0.0, 1.0, 0.0 ) * Vector3(0.0, 1.0, 0.0 ).dot( n );
		n.normalize();
		n*=radius;
		p_vertices[0]=n+Vector3(0.0, height/2.0, 0.0 );
		p_vertices[1]=n-Vector3(0.0, height/2.0, 0.0 );
		p_transform.transform(p_vertices[0]);
		p_transform.transform(p_vertices[1]);
		*p_count=2;
		
	} else if ( d > 0.0 ) {
		
		n*=radius;
		n.y+=height/2.0;
		
		p_transform.transform( n );
		*p_vertices=n;
		*p_count=1;
		
	} else {
		
		n*=radius;
		n.y-=height/2.0;
		
		p_transform.transform( n );
		*p_vertices=n;
		*p_count=1;		
	}
	/*
	printf("normal %f,%f,%f\n",p_normal.x,p_normal.y,p_normal.z);
	for (int i=0;i<*p_count;i++) {
		
		printf("result %i: %f,%f,%f\n",i,p_vertices[i].x,p_vertices[i].y,p_vertices[i].z );
	}*/
	
	
}

bool PillVolume::intersect_segment(const Matrix4& p_transform,const Vector3& p_from,const Vector3& p_to,Vector3& r_clip,Vector3& r_normal) const {
		
	return false;
}

Vector3 PillVolume::get_local_inertia(float p_mass) const {

	AABB aabb=compute_AABB(Matrix4());
	
	Vector3 itmp = aabb.size;
	
	itmp.x*=itmp.x;
	itmp.y*=itmp.y;
	itmp.z*=itmp.z;
	
	return (Vector3(itmp.y+itmp.z,itmp.x+itmp.z,itmp.x+itmp.y)*p_mass)*0.0833333;
}


AABB PillVolume::compute_AABB( const Matrix4& p_transform ) const {

	Vector3 v( radius, height/2.0 + radius, radius );
	
	AABB res;
//	res.pos=-v;
//	res.size=v*2;
	res.pos=-v-Vector3(0.2,0.2,0.2);
	res.size=v*2+Vector3(0.2,0.2,0.2)*2;
	
	p_transform.transform_aabb(res);
	
	return res;
}

Vector3 PillVolume::get_center( const Matrix4& p_transform) const {
	
	return p_transform.get_translation();
}

PillVolume::PillVolume(float p_height,float p_radius) {

	height=p_height;
	radius=p_radius;
}


PillVolume::~PillVolume()
{
}

