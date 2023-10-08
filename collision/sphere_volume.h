//
// C++ Interface: sphere_volume
//
// Description: 
//
//
// Author: ,,, <red@lunatea>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef SPHERE_VOLUME_H
#define SPHERE_VOLUME_H

#include "collision/volume.h"

/**
	@author ,,, <red@lunatea>
*/
class SphereVolume : public Volume {

	IAPI_TYPE(SphereVolume,Volume);
	float radius;
	
public:
	
	static IRef<IAPI> IAPI_DEFAULT_INSTANCE_FUNC(const String& p_type,const  IAPI::CreationParams& p_params);
		
	virtual void set(String p_path, const Variant& p_value); 
	virtual Variant get(String p_path) const; 
	virtual void get_property_list( List<PropertyInfo> *p_list ) const; 	
	
	virtual Type get_volume_type() const { return SPHERE; }
	
	virtual void project_range(const Vector3& p_normal,const Matrix4& p_transform,float& r_min, float& r_max) const;
	virtual void get_support(const Vector3& p_normal,const Matrix4& p_transform,Vector3 *p_vertices,int* p_count,int p_max) const; 
	inline float get_radius() const { return radius; }
	void set_radius(float p_radius);
	
	virtual bool intersect_segment(const Matrix4& p_transform,const Vector3& p_from,const Vector3& p_to,Vector3& r_clip,Vector3& r_normal) const;
	
	virtual Vector3 get_local_inertia(float p_mass) const;
	
	virtual AABB compute_AABB( const Matrix4& p_transform ) const;

	virtual Vector3 get_center( const Matrix4& p_transform) const;
		
	SphereVolume(float p_radius=1.0);	
	~SphereVolume();

};

#endif
