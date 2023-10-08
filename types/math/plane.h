//
// C++ Interface: plane
//
// Description:
//
//
// Author: Juan Linietsky <red@lunatea>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef PLANE_H
#define PLANE_H

/**
	@author Juan Linietsky <red@lunatea>
*/

#include "vector3.h"
#include "math_defs.h"
/**
 * Plane class, with many useful methods. 
 * This plane is in hessian form, with a normal, and a distance in the normal direction
 * to reach and touch the plane.
 * d (distance) is NOT negated, unlike in many other implementations, which means
 * the plane equation is normal.x * x + normal.y * y + normal.z * z + d = 0
 */

#define _PLANE_EQ_DOT_EPSILON 0.999
#define _PLANE_EQ_D_EPSILON 0.0001
#include "math_funcs.h"

SCRIPT_BIND_BEGIN;

class Plane {
public:

	enum IntersectResult {

		INTERSECT_NONE,
		INTERSECT_COPLANAR,
		INTERSECT_POINT
	};

	Vector3 normal;
	real_t d;

	/* Length Operations */
	real_t get_length() const;
	void normalize();


	_FORCE_INLINE_ Plane operator-() const { return Plane(-normal,-d); }

	Vector3 get_any_point() const;
	Vector3 get_any_perpendicular_normal() const;

	_FORCE_INLINE_ bool is_point_over(const Vector3 &p_point) const; ///< Point is over plane
	_FORCE_INLINE_ real_t distance_to_point(const Vector3 &p_point) const;

	_FORCE_INLINE_ bool has_point(const Vector3 &p_point,real_t _epsilon=CMP_POINT_IN_PLANE_EPSILON) const;
	void set_normal(const Vector3& p_normal);
	inline Vector3 get_normal() const { return normal; }; ///Point is coplanar, CMP_EPSILON for precision

	bool get_intersection_with_planes(const Plane &p_plane1, const Plane &p_plane2, Vector3 *p_destination_vector) const; ///< Intersection with planes, returned to a vector

	IntersectResult get_intersection_with_segment(Vector3 p_begin, Vector3 p_end, Vector3* p_intersection,real_t _epsilon=CMP_EPSILON) const; ///< Intersectio with segnment

	bool get_segment_over(Vector3 &p_begin, Vector3& p_end,real_t _epsilon=CMP_EPSILON) const; ///< Return true of segment above exists
	bool get_segment_under(Vector3 &p_begin, Vector3& p_end,real_t _epsilon=CMP_EPSILON) const; ///< Return true if segment below exists

	IntersectResult get_intersection_with_ray(Vector3 p_from, Vector3 p_dir, Vector3* p_intersection,real_t _epsilon=CMP_EPSILON)  const;


	IntersectResult get_intersection_with_rect(Vector3 p_begin, Vector3 p_direction, Vector3* p_intersection,real_t _epsilon=CMP_EPSILON) const; ///< Intersection with rect
	
	_FORCE_INLINE_ bool is_almost_like(const Plane& p_plane) const;

	_FORCE_INLINE_ Plane(real_t p_a=0, real_t p_b=0, real_t p_c=0, real_t p_d=0) :normal(p_a,p_b,p_c), d(p_d) { };

	_FORCE_INLINE_ Plane(const Vector3 &p_normal, real_t p_d);
	_FORCE_INLINE_ Plane(const Vector3 &p_point, const Vector3& p_normal);
	_FORCE_INLINE_ Plane(const Vector3 &p_point1, const Vector3 &p_point2,const Vector3 &p_point3,ClockDirection p_dir = CLOCKWISE);


};

SCRIPT_BIND_END;

inline bool Plane::is_almost_like(const Plane& p_plane) const {

	return (normal.dot( p_plane.normal ) > _PLANE_EQ_DOT_EPSILON && Math::absd(d-p_plane.d) < _PLANE_EQ_D_EPSILON);

}


bool Plane::is_point_over(const Vector3 &p_point) const {
	
	return ((normal.dot(p_point)) > d);
}

real_t Plane::distance_to_point(const Vector3 &p_point) const {


	return (normal.dot(p_point)-d);
}

inline bool Plane::has_point(const Vector3 &p_point,real_t _epsilon) const {


	//printf("has point by %i\n",Math::absf( normal.dot(p_point) - d ));
	return (Math::absf( normal.dot(p_point) - d ) <= _epsilon);

}

Plane::Plane(const Vector3 &p_point, const Vector3& p_normal) {

	
	normal=p_normal;
	d=p_normal.dot(p_point);

}

Plane::Plane(const Vector3 &p_normal, real_t p_d) {

	normal=p_normal;
	d=p_d;
}

Plane::Plane(const Vector3 &p_point1, const Vector3 &p_point2, const Vector3 &p_point3,ClockDirection p_dir) {

	if (p_dir == CLOCKWISE)
		normal.set_as_cross_of(p_point1-p_point3, p_point1-p_point2);
	else
		normal.set_as_cross_of(p_point1-p_point2, p_point1-p_point3);
	

	normal.normalize();
	d = normal.dot(p_point1);
	
	
}


#endif
