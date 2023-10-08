//
// C++ Interface: volume
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef VOLUME_H
#define VOLUME_H

#include "iapi.h"
#include "face3.h"
/**
	@author Juan Linietsky <reduzio@gmail.com>
*/
class Volume : public IAPI {
	
	IAPI_TYPE(Volume,IAPI);
public:
	
	enum {
	
		MAX_SUPPORTS=32, ///< Max possible supports for object
  		SUPPORT_TYPE_CIRCLE=-1 // support is circle, 2 cpoints returned
	};
		
	enum Type { // volumes requiere fast casting
		SPHERE,
  		CONVEX_POLYGON,
    		PLANE,
		TRIANGLE,
  		PILL,
    		ELLIPSOID,
		TRIANGLE_MESH,
	};
	
	virtual bool is_concave() const { return false; }
	
	virtual Type get_volume_type() const=0;
	
	virtual void project_range(const Vector3& p_normal,const Matrix4& p_transform,float& r_min, float& r_max) const =0; ///< project this volume to the given normal axis, creating a segment. r_min contains the lowest projected value, and r_max the highest.
	virtual void get_support(const Vector3& p_normal,const Matrix4& p_transform,Vector3 *p_vertices,int* p_count,int p_max) const=0; ///< Get the support points given a certain direction, if more than one, they are returned in clockwise order. if p_count == SUPPORT_TYPE_CIRCLE, then the support is a circle (two vertices, pos and radius).
	
	virtual bool intersect_segment(const Matrix4& p_transform,const Vector3& p_from,const Vector3& p_to,Vector3& r_clip,Vector3& r_normal) const =0;
					
	virtual Vector3 get_local_inertia(float p_mass) const=0;
	
	virtual AABB compute_AABB( const Matrix4& p_transform ) const=0;
	
	virtual Vector3 get_center( const Matrix4& p_transform) const=0;
	
	virtual void intersect_AABB_with_faces(const Matrix4& p_transform,const AABB& p_AABB, Method2<const Face3&,const Vector3&>& p_face_callback) const; ///< only available in convex volumes
	
	
	Volume();
	~Volume();

};

#endif
