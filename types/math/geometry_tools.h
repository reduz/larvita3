//
// C++ Interface: geometry_tools
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef GEOMETRY_TOOLS_H
#define GEOMETRY_TOOLS_H

#include <stdio.h> // for printfs

#include "dvector.h"
#include "face3.h"

/**
	@author Juan Linietsky <reduzio@gmail.com>
*/
class GeometryTools{
	GeometryTools();
public:
	
	static inline bool ray_intersects_triangle( const Vector3& p_from, const Vector3& p_dir, const Vector3& p_v0,const Vector3& p_v1,const Vector3& p_v2,Vector3* r_res=0) {
		Vector3 e1=p_v1-p_v0;
		Vector3 e2=p_v2-p_v0;
		Vector3 h = Vector3::cross( p_dir, e2 );
		real_t a =e1.dot(h);
		if (a>-CMP_EPSILON && a < CMP_EPSILON) // parallel test
			return false;
				
		real_t f=1.0/a;
		
		Vector3 s=p_from-p_v0;
		real_t u = f * s.dot(h);
		
		if ( u< 0.0 || u > 1.0)
			return false;
		
		Vector3 q=Vector3::cross(s,e1);
		
		real_t v = f * p_dir.dot(q);
		
		if (v < 0.0 || u + v > 1.0)
			return false;
		
		// at this stage we can compute t to find out where 
		// the intersection point is on the line
		real_t t = f * e2.dot(q);
		
		if (t > 0.00001) {// ray intersection
			if (r_res)
				*r_res=p_from+p_dir*t;
			return true;
		} else // this means that there is a line intersection  
			// but not a ray intersection
			return false;
	}	
	
	static inline bool segment_intersects_triangle( const Vector3& p_from, const Vector3& p_to, const Vector3& p_v0,const Vector3& p_v1,const Vector3& p_v2,Vector3* r_res=0) {
	
		Vector3 rel=p_to-p_from;
		Vector3 e1=p_v1-p_v0;
		Vector3 e2=p_v2-p_v0;
		Vector3 h = Vector3::cross( rel, e2 );
		real_t a =e1.dot(h);
		if (a>-CMP_EPSILON && a < CMP_EPSILON) // parallel test
			return false;
				
		real_t f=1.0/a;
		
		Vector3 s=p_from-p_v0;
		real_t u = f * s.dot(h);
		
		if ( u< 0.0 || u > 1.0)
			return false;
		
		Vector3 q=Vector3::cross(s,e1);
		
		real_t v = f * rel.dot(q);
		
		if (v < 0.0 || u + v > 1.0)
			return false;
		
		// at this stage we can compute t to find out where 
		// the intersection point is on the line
		real_t t = f * e2.dot(q);
		
		if (t > CMP_EPSILON && t<=1.0) {// ray intersection
			if (r_res)
				*r_res=p_from+rel*t;
			return true;
		} else // this means that there is a line intersection  
			// but not a ray intersection
			return false;
	}	
	
	
	static inline bool point_in_projected_triangle(const Vector3& p_point,const Vector3& p_v1,const Vector3& p_v2,const Vector3& p_v3) {
	
	
		Vector3 face_n =  Vector3::cross(p_v1-p_v3,p_v1-p_v2);		
		
		Vector3 n1 =  Vector3::cross(p_point-p_v3,p_point-p_v2);		
		
		if (face_n.dot(n1)<0)
			return false;
			
		Vector3 n2 =  Vector3::cross(p_v1-p_v3,p_v1-p_point);		
		
		if (face_n.dot(n2)<0)
			return false;
		
		Vector3 n3 =  Vector3::cross(p_v1-p_point,p_v1-p_v2);		
	
		if (face_n.dot(n3)<0)
			return false;
	
		return true;
	
	}
	
	
	
	static DVector< DVector< Face3 > > separate_objects( DVector< Face3 > p_array );
	static DVector< Face3 > wrap_geometry( DVector< Face3 > p_array ); ///< create a "wrap" that encloses the given geometry

};


#endif
