//
// C++ Interface: plane_volume
//
// Description: 
//
//
// Author: ,,, <red@lunatea>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef PLANE_VOLUME_H
#define PLANE_VOLUME_H

#include "collision/volume.h"
/**
	@author ,,, <red@lunatea>
*/
class PlaneVolume : public Volume {
	
	IAPI_TYPE(PlaneVolume,Volume);
	Plane plane;

public:
	static IRef<IAPI> IAPI_DEFAULT_INSTANCE_FUNC(const String& p_type,const  IAPI::CreationParams& p_params);
	

	
	virtual void set(String p_path, const Variant& p_value); 
	virtual Variant get(String p_path) const; 
	virtual void get_property_list( List<PropertyInfo> *p_list ) const; 	
	
	
	virtual Type get_volume_type() const { return PLANE; }
	
	virtual void project_range(const Vector3& p_normal,const Matrix4& p_transform,float& r_min, float& r_max) const;
	virtual void get_support(const Vector3& p_normal,const Matrix4& p_transform,Vector3 *p_vertices,int* p_count,int p_max) const;
	
	bool intersect_segment(const Matrix4& p_transform,const Vector3& p_from,const Vector3& p_to,Vector3& r_clip,Vector3& r_normal) const;
	
	inline Plane get_plane() const { return plane; }
	void set_plane(const Plane& p_plane);
	
	virtual Vector3 get_local_inertia(float p_mass) const;
	
	virtual AABB compute_AABB( const Matrix4& p_transform ) const;
	
	virtual Vector3 get_center( const Matrix4& p_transform) const;
	
	PlaneVolume(const Plane& p_plane=Plane(0.0,1.0,0.0,0.0));


};

#endif
