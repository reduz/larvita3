//
// C++ Implementation: cube3d
//
// Description:
//
//
// Author: Juan Linietsky <coding@reduz.com.ar>, (C) 2003
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "rect3.h"
#include "face3.h"


void Rect3::get_edge(int p_edge,Vector3& r_from,Vector3& r_to) const {
	
	ERR_FAIL_INDEX(p_edge,12);
	switch(p_edge) {
		
		case 0: {
			
			r_from=Vector3( pos.x+size.x	, pos.y		, pos.z		);
			r_to=Vector3( pos.x	, pos.y		, pos.z		);
		} break;
		case 1: {
			
			r_from=Vector3( pos.x+size.x	, pos.y		, pos.z+size.z	);
			r_to=Vector3( pos.x+size.x	, pos.y		, pos.z		);
		} break;
		case 2: {
			r_from=Vector3( pos.x	, pos.y		, pos.z+size.z	);
			r_to=Vector3( pos.x+size.x	, pos.y		, pos.z+size.z	);
			
		} break;
		case 3: {
			
			r_from=Vector3( pos.x	, pos.y		, pos.z		);
			r_to=Vector3( pos.x	, pos.y		, pos.z+size.z	);
			
		} break;
		case 4: {
			
			r_from=Vector3( pos.x	, pos.y+size.y		, pos.z		);
			r_to=Vector3( pos.x+size.x	, pos.y+size.y		, pos.z		);
		} break;
		case 5: {
			
			r_from=Vector3( pos.x+size.x	, pos.y+size.y		, pos.z		);
			r_to=Vector3( pos.x+size.x	, pos.y+size.y		, pos.z+size.z	);
		} break;
		case 6: {
			r_from=Vector3( pos.x+size.x	, pos.y+size.y		, pos.z+size.z	);
			r_to=Vector3( pos.x	, pos.y+size.y		, pos.z+size.z	);
			
		} break;
		case 7: {
			
			r_from=Vector3( pos.x	, pos.y+size.y		, pos.z+size.z	);
			r_to=Vector3( pos.x	, pos.y+size.y		, pos.z		);
			
		} break;
		case 8: {
			
			r_from=Vector3( pos.x	, pos.y		, pos.z+size.z	);
			r_to=Vector3( pos.x	, pos.y+size.y		, pos.z+size.z	);
			
		} break;
		case 9: {
			
			r_from=Vector3( pos.x	, pos.y		, pos.z		);
			r_to=Vector3( pos.x	, pos.y+size.y	, pos.z		);
			
		} break;
		case 10: {
			
			r_from=Vector3( pos.x+size.x	, pos.y		, pos.z		);
			r_to=Vector3( pos.x+size.x	, pos.y+size.y	, pos.z		);
			
		} break;
		case 11: {
			
			r_from=Vector3( pos.x+size.x	, pos.y		, pos.z+size.z		);
			r_to=Vector3( pos.x+size.x	, pos.y+size.y	, pos.z+size.z		);
			
		} break;
		
	}
	
}
void Rect3::project_range(const Vector3& p_axis,float &r_min,float& r_max) const {
	
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
	
	for (int i=0;i<8;i++) {
		
		float d = p_axis.dot(points[i]);
		if (i==0 || d<r_min)
			r_min=d;
		if (i==0 || d>r_max)
			r_max=d;
	}
}

bool Rect3::intersect_segment(const Vector3& p_from, const Vector3& p_to,Vector3& r_clip,Vector3& r_normal) const {
	
	Plane planes[6]={
		Plane( Vector3(1,0,0),  pos.x+size.x),
		Plane( Vector3(-1,0,0), -pos.x),
		Plane( Vector3(0,1,0),  pos.y+size.y),
		Plane( Vector3(0,-1,0),  -pos.y),
		Plane( Vector3(0,0,1),  pos.z+size.z),
		Plane( Vector3(0,0,-1),  -pos.z),
	};

	Vector3 from=p_from;
	Vector3 to=p_to;
	
	r_normal=Vector3();
	r_clip=from;
	
	for (int i=0;i<6;i++) {
	
		Plane &p=planes[i];
		int side_from,side_to;
		
		side_from = p.has_point(from)?0:(p.is_point_over(from)?1:-1);
		side_to = p.has_point(to)?0:(p.is_point_over(to)?1:-1);
		
		if (side_from==side_to) {
			
			if (side_from>=0)
				return false; // no intersection
			// if both -1, ignore
		} else {
			// intersection			
			
			Vector3 inters;
			Plane::IntersectResult ires =  planes[i].get_intersection_with_segment(from,to,&inters);
			if (ires!=Plane::INTERSECT_POINT)
				continue;
			
			if (side_from==1) {
				from=inters;
				r_clip=from;		
				r_normal=p.normal;
			}
			
			if (side_to==1)
				to=inters;
		}
	}
	
	return true;	
}

bool Rect3::operator==(const Rect3& p_rval) const {

	return ((pos==p_rval.pos) && (size==p_rval.size));

}
bool Rect3::operator!=(const Rect3& p_rval) const {

	return ((pos!=p_rval.pos) || (size!=p_rval.size));

}

bool Rect3::is_empty_area() const {

	return (size.x<=0 || size.y<=0  || size.z<=0);
}

Rect3::IntersectionType Rect3::get_intersection_type( const Rect3 & p_with_rect,Sint32 _epsilon ) const {

	Rect3 intersection = get_intersection_with( p_with_rect );

	Sint32 area = intersection.get_area();

	if ( area <= _epsilon )
		return INTERSECT_NONE;

	if ( ABS(area-get_area()) <= _epsilon ) //resulting area is equal to this, so we are contained
		return INTERSECT_IS_CONTAINED;

	if ( ABS(area-p_with_rect.get_area()) <= _epsilon ) //resulting area is equal to test rect, so we are containing it
		return INTERSECT_CONTAINS;

	return INTERSECT_CROSS; //crossing, so just a regular intersection

}

float Rect3::get_area() const {

	return size.x*size.y*size.z;

}


Rect3 Rect3::get_intersection_with(const Rect3& p_rect) const {

	Vector3 src_min=pos;
	Vector3 src_max=pos+size;
	Vector3 dst_min=p_rect.pos;
	Vector3 dst_max=p_rect.pos+p_rect.size;

	Vector3 min,max;

	if (src_min.x > dst_max.x || src_max.x < dst_min.x )
		return Rect3();
	else {

		min.x= ( src_min.x > dst_min.x ) ? src_min.x : dst_min.x;
		max.x= ( src_max.x < dst_max.x ) ? src_max.x : dst_max.x;

	}

	if (src_min.y > dst_max.y || src_max.y < dst_min.y )
		return Rect3();
	else {

		min.y= ( src_min.y > dst_min.y ) ? src_min.y : dst_min.y;
		max.y= ( src_max.y < dst_max.y ) ? src_max.y : dst_max.y;

	}

	if (src_min.z > dst_max.z || src_max.z < dst_min.z )
		return Rect3();
	else {

		min.z= ( src_min.z > dst_min.z ) ? src_min.z : dst_min.z;
		max.z= ( src_max.z < dst_max.z ) ? src_max.z : dst_max.z;

	}


	return Rect3( min, max-min );
}



void Rect3::merge_with(const Rect3& p_rect) {

	Vector3 beg_1,beg_2;
	Vector3 end_1,end_2;
        Vector3 min,max;

	beg_1=pos;
	beg_2=p_rect.pos;
	end_1=Vector3(size.x,size.y,size.z)+beg_1;
	end_2=Vector3(p_rect.size.x,p_rect.size.y,p_rect.size.z)+beg_2;

	min.x=(beg_1.x<beg_2.x)?beg_1.x:beg_2.x;
	min.y=(beg_1.y<beg_2.y)?beg_1.y:beg_2.y;
	min.z=(beg_1.z<beg_2.z)?beg_1.z:beg_2.z;

	max.x=(end_1.x>end_2.x)?end_1.x:end_2.x;
	max.y=(end_1.y>end_2.y)?end_1.y:end_2.y;
	max.z=(end_1.z>end_2.z)?end_1.z:end_2.z;

	pos=min;
	size=max-min;
}

Vector3 Rect3::get_longest_axis() const {
	
	Vector3 axis(1,0,0);
	real_t max_size=size.x;
	
	if (size.y > max_size ) {
		axis=Vector3(0,1,0);
		max_size=size.y;
	}
	
	if (size.z > max_size ) {
		axis=Vector3(0,0,1);
		max_size=size.z;
	}
	
	return axis;
}
int Rect3::get_longest_axis_index() const {
	
	int axis=0;
	real_t max_size=size.x;
	
	if (size.y > max_size ) {
		axis=1;
		max_size=size.y;
	}
	
	if (size.z > max_size ) {
		axis=2;
		max_size=size.z;
	}
	
	return axis;
}

real_t Rect3::get_longest_axis_size() const {
	
	real_t max_size=size.x;
	
	if (size.y > max_size ) {
		max_size=size.y;
	}
	
	if (size.z > max_size ) {
		max_size=size.z;
	}
	
	return max_size;	
}


void Rect3::grow_by(real_t p_amount) {

	pos.x-=p_amount;
	pos.y-=p_amount;
	pos.z-=p_amount;
	size.x+=2.0*p_amount;
	size.y+=2.0*p_amount;
	size.z+=2.0*p_amount;
}
Rect3::Rect3(const Vector3 &p_pos,const Vector3& p_size) {

	pos=p_pos;
	size=p_size;

}
