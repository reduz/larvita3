//
// C++ Implementation: plane
//
// Description:
//
//
// Author: Juan Linietsky <red@lunatea>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "plane.h"


Vector3 Plane::get_any_point() const {

	return get_normal()*d;
}

Vector3 Plane::get_any_perpendicular_normal() const {

	static const Vector3 p1 = Vector3(1,0,0);
	static const Vector3 p2 = Vector3(0,1,0);
	Vector3 p;
	
	if (ABS(normal.dot(p1)) > 0.99) // if too similar to p1
		p=p2; // use p2
	else
		p=p1; // use p1
	
	p-=normal * normal.dot(p);
	p.normalize();

	return p;
}

real_t Plane::get_length() const {

	return normal.length();
}

void Plane::normalize() {

	real_t l = get_length();
	if (l==0) {
		*this=Plane(0,0,0,0);
		return;
	}
	normal/=l;
	d/=l;

}





/**
 *
 * @return true if the points is in the halfspace where the normal points.
 */


bool Plane::get_segment_over(Vector3 &p_begin, Vector3& p_end,real_t _epsilon) const {
	
	Vector3 intersection;
	IntersectResult res=get_intersection_with_segment( p_begin, p_end,&intersection,_epsilon );
	
	if (res!=INTERSECT_POINT) {
		
		return is_point_over( p_begin );
	} else {
		
		if (is_point_over( p_begin )) {
			
			p_end=intersection;
			
		} else {
			
			p_begin=intersection;
		}
	}
	
	return true;	
}
bool Plane::get_segment_under(Vector3 &p_begin, Vector3& p_end,real_t _epsilon) const {
	
	Vector3 intersection;
	IntersectResult res=get_intersection_with_segment( p_begin, p_end,&intersection,_epsilon );
	
	if (res!=INTERSECT_POINT) {
		
		return !is_point_over( p_begin );
	} else {
		
		if (!is_point_over( p_begin )) {
			
			p_end=intersection;
			
		} else {
			
			p_begin=intersection;
		}
	}
	
	return true;	
	
}

void Plane::set_normal(const Vector3& p_normal) {
	
	normal=p_normal;
}



/**
 * Get the intersection point (if it exists) from the intersection of 3
 * planes.
 *
 * @return false if no intersection was found.
 * @return @true if intersection (the value will be returned in the vector pased
 *
 * @warning function not tested!
 *
 */
bool Plane::get_intersection_with_planes(const Plane &p_plane1, const Plane &p_plane2, Vector3 *result) const {

	Vector3 normal0=normal;
	Vector3 normal1=p_plane1.normal;
	Vector3 normal2=p_plane2.normal;
	
	real_t denom = Vector3::cross(normal0, normal1).dot(normal2);
	
	if (ABS(denom)<=CMP_EPSILON)
		return false;
	
	*result = 	( (Vector3::cross(normal1, normal2) * d) +
			(Vector3::cross(normal2, normal0) * p_plane1.d) +
			(Vector3::cross(normal0, normal1) * p_plane2.d) )/denom;

	return true;
}

Plane::IntersectResult Plane::get_intersection_with_segment(Vector3 p_begin, Vector3 p_end, Vector3* p_intersection,real_t _epsilon)  const {

	Vector3 segment= p_begin - p_end;
	real_t den=normal.dot( segment );

	//printf("den is %i\n",den);
	if (Math::absf(den)<=_epsilon) {

		return INTERSECT_NONE;
	}

	real_t dist=(normal.dot( p_begin ) - d) / den;
	//printf("dist is %i\n",dist);

	if (dist<-_epsilon || dist > (1.0 +_epsilon)) {

		return INTERSECT_NONE;
	}

	dist=-dist;
	*p_intersection = p_begin + segment * dist;

	return INTERSECT_POINT;
}

Plane::IntersectResult Plane::get_intersection_with_ray(Vector3 p_from, Vector3 p_dir, Vector3* p_intersection,real_t _epsilon)  const {

	Vector3 segment=p_dir;
	real_t den=normal.dot( segment );

	//printf("den is %i\n",den);
	if (Math::absf(den)<=_epsilon) {

		return INTERSECT_NONE;
	}

	real_t dist=(normal.dot( p_from ) - d) / den;
	//printf("dist is %i\n",dist);

	if (dist>_epsilon) { //this is a ray, before the emiting pos (p_from) doesnt exist

		return INTERSECT_NONE;
	}

	dist=-dist;
	*p_intersection = p_from + segment * dist;

	return INTERSECT_POINT;
}

Plane::IntersectResult Plane::get_intersection_with_rect(Vector3 p_begin, Vector3 p_direction, Vector3* p_intersection,real_t _epsilon) const {


	Vector3 normal;
	real_t temp, temp2, t;
	temp =	normal.dot(p_begin) - d;
	temp2 = normal.dot(p_direction);

	if (Math::absf(temp) <= _epsilon)
	{
		if (Math::absf(temp2) <= _epsilon)
		{
			*p_intersection = p_begin;
			return INTERSECT_COPLANAR;
		}
		else return INTERSECT_NONE;
	}

	t = -(temp/temp2);
	*p_intersection = p_direction * t + p_begin;
	return INTERSECT_POINT; 
}


