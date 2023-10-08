
//
// C++ Interface: cube3d
//
// Description:
//
//
// Author: Juan Linietsky <coding@reduz.com.ar>, (C) 2003
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef RECT3D_H
#define RECT3D_H

#include "vector3.h"
#include "plane.h"
/**
 * Rect3 / AABB (Axis Aligned Bounding Box)
 * This is implemented by a point (pos) and the box size
 */

SCRIPT_BIND_BEGIN;
 
class Rect3 {
public:
	Vector3 pos; /// Position
	Vector3 size; /// Size


	enum IntersectionType {
		INTERSECT_NONE, // intersection result, none
		INTERSECT_CROSS, // they cross, but none contains eachother
		INTERSECT_CONTAINS, //contains p_with_rect inside
		INTERSECT_IS_CONTAINED, // is contained inside p_with_rect
	};


	bool is_empty_area() const; /// Box is empty
	bool operator==(const Rect3& p_rval) const;
	bool operator!=(const Rect3& p_rval) const;

	_FORCE_INLINE_ bool intersects(const Rect3& p_rect) const {

		if ( pos.x > (p_rect.pos.x + p_rect.size.x) )
			return false;
		if ( (pos.x+size.x) < p_rect.pos.x  )
			return false;
		if ( pos.y > (p_rect.pos.y + p_rect.size.y) )
			return false;
		if ( (pos.y+size.y) < p_rect.pos.y  )
			return false;
		if ( pos.z > (p_rect.pos.z + p_rect.size.z) )
			return false;
		if ( (pos.z+size.z) < p_rect.pos.z  )
			return false;

		return true;
	}
	IntersectionType get_intersection_type( const Rect3 & p_with_rect,Sint32 _epsilon=CMP_EPSILON ) const; /// get intersection type with another AABox
	_FORCE_INLINE_ bool contains(const Rect3 & p_rect) const; ///contains another Rect3/AABB
	float get_area() const; /// get area
        void merge_with(const Rect3& p_rect); ///merge with another AABB
	Rect3 get_intersection_with(const Rect3& p_rect) const; ///get box where two intersect, empty if no intersection occurs

	Vector3 get_longest_axis() const;
	int get_longest_axis_index() const;
	real_t get_longest_axis_size() const;
	
	void grow_by(real_t p_amount);
	
	bool intersect_segment(const Vector3& p_from, const Vector3& p_to,Vector3& r_clip,Vector3& r_normal) const;
	Rect3(const Vector3 &p_pos,const Vector3& p_size);
	_FORCE_INLINE_ Rect3() {}
	
	void get_edge(int p_edge,Vector3& r_from,Vector3& r_to) const;
	void project_range(const Vector3& p_axis,float &r_min,float& r_max) const;
	_FORCE_INLINE_ void expand_to(const Vector3& p_vector); /** expand to contain a point if necesary */
	
	inline Plane get_plane(int idx) const;
	_FORCE_INLINE_ bool intersects_convex_poly(const Plane *p_plane, int p_plane_count) const;
	inline bool intersects_plane(const Plane &p_plane) const;
	
};

SCRIPT_BIND_END;


Plane Rect3::get_plane(int idx) const {

	switch(idx) {
	
		case 0: return Plane( Vector3(-1, 0, 0), -pos.x );
		case 1: return Plane( Vector3(0,-1, 0), -pos.y );
		case 2: return Plane( Vector3(0,0, -1), -pos.z );
		case 3: return Plane( Vector3(1, 0, 0), pos.x+size.x );
		case 4: return Plane( Vector3(0,1, 0), pos.y+size.x );
		case 5: return Plane( Vector3(0,0, 1), pos.z+size.y );
	
	}
	ERR_FAIL_V( Plane() );
}


inline bool Rect3::contains(const Rect3 & p_rect) const {

	Vector3 src_min=pos;
	Vector3 src_max=pos+size;
	Vector3 dst_min=p_rect.pos;
	Vector3 dst_max=p_rect.pos+p_rect.size;

	return  (
		 (src_min.x <= dst_min.x) && 
			(src_max.x >= dst_max.x) && 
			(src_min.y <= dst_min.y) && 
			(src_max.y >= dst_max.y) && 
			(src_min.z <= dst_min.z) && 
			(src_max.z >= dst_max.z) );

}

inline bool Rect3::intersects_convex_poly(const Plane *p_planes, int p_plane_count) const {
	
	//cache all points to check against!
	
	Vector3 points[8] = {
		Vector3( pos.x	, pos.y		, pos.z		),
		Vector3( pos.x	, pos.y		, pos.z+size.z	),
		Vector3( pos.x	, pos.y+size.y	, pos.z		),
		Vector3( pos.x	, pos.y+size.y	, pos.z+size.z	),
		Vector3( pos.x+size.x	, pos.y		, pos.z		),
		Vector3( pos.x+size.x	, pos.y		, pos.z+size.z	),
		Vector3( pos.x+size.x	, pos.y+size.y	, pos.z		),
		Vector3( pos.x+size.x	, pos.y+size.y	, pos.z+size.z	),
	};

	for (int i=0;i<p_plane_count;i++) { //for each plane

		const Plane & plane=p_planes[i];
		bool all_points_over=true;
		//test if it has all points over!

		for (int j=0;j<8;j++) {


			if (!plane.is_point_over( points[j] )) {

				all_points_over=false;
				break;
			}

		}

		if (all_points_over) {

			return false;
		}
	}

	return true;		
}

inline void Rect3::expand_to(const Vector3& p_vector) {

	Vector3 begin=pos;
	Vector3 end=pos+size;

	if (p_vector.x<begin.x)
		begin.x=p_vector.x;
	if (p_vector.y<begin.y)
		begin.y=p_vector.y;
	if (p_vector.z<begin.z)
		begin.z=p_vector.z;

	if (p_vector.x>end.x)
		end.x=p_vector.x;
	if (p_vector.y>end.y)
		end.y=p_vector.y;
	if (p_vector.z>end.z)
		end.z=p_vector.z;

	pos=begin;
	size=end-begin;
}

bool Rect3::intersects_plane(const Plane &p_plane) const {
	
	Vector3 points[8] = {
		Vector3( pos.x	, pos.y		, pos.z		),
		Vector3( pos.x	, pos.y		, pos.z+size.z	),
		Vector3( pos.x	, pos.y+size.y	, pos.z		),
		Vector3( pos.x	, pos.y+size.y	, pos.z+size.z	),
		Vector3( pos.x+size.x	, pos.y		, pos.z		),
		Vector3( pos.x+size.x	, pos.y		, pos.z+size.z	),
		Vector3( pos.x+size.x	, pos.y+size.y	, pos.z		),
		Vector3( pos.x+size.x	, pos.y+size.y	, pos.z+size.z	),
	};
	
	bool over=false;
	bool under=false;	
	
	for (int i=0;i<8;i++) {
		
		if (p_plane.distance_to_point(points[i])>0)
			over=true;
		else
			under=true;
		
	}
	
	return under && over;
}

typedef Rect3 AABB; /// rect3 and AABB are the same

#endif
