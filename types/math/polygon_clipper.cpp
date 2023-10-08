
//
// C++ Implementation: polygon_clipper
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "polygon_clipper.h"
#include "os/memory.h"

void PolygonClipper::clear() {

	polygon_len=0;
}
void PolygonClipper::add_point(const Vector3& p_point){

	ERR_FAIL_COND(polygon_len>=polygon_max);
	
	polygon[polygon_len++]=p_point;

}

void PolygonClipper::clip(const Plane& p_plane){
	
	int inside_count = 0;
	int outside_count = 0;
	
	for (int a = 0; a < polygon_len; a++) {
		//float p_plane.d = (*this) * polygon[a];
		float dist = p_plane.distance_to_point(polygon[a]);
		if (dist <-CMP_POINT_IN_PLANE_EPSILON) {
			location_cache[a] = LOC_INSIDE;
			inside_count++;
		} else {
			if (dist > CMP_POINT_IN_PLANE_EPSILON) {
				location_cache[a] = LOC_OUTSIDE;
				outside_count++;
			} else {
				location_cache[a] = LOC_BOUNDARY;
			}
		}
	}

	if (outside_count == 0) {
		
		return; // no changes
		
	} else if (inside_count == 0) {
		
		polygon_len=0;
		return;
	}

	long count = 0;
	long previous = polygon_len - 1;
	
	for (long index = 0; index < polygon_len; index++) {
		long loc = location_cache[index];
		if (loc == LOC_OUTSIDE) {
			if (location_cache[previous] == LOC_INSIDE) {
				const Vector3& v1 = polygon[previous];
				const Vector3& v2 = polygon[index];
					
				Vector3 segment= v1 - v2;
				double den=p_plane.normal.dot( segment );
				double dist=p_plane.distance_to_point( v1 ) / den;
				dist=-dist;
				polygon_aux[count++] = v1 + segment * dist;
			}
		} else {
			const Vector3& v1 = polygon[index];
			if ((loc == LOC_INSIDE) && (location_cache[previous] == LOC_OUTSIDE)) {
				const Vector3& v2 = polygon[previous];
				Vector3 segment= v1 - v2;
				double den=p_plane.normal.dot( segment );
				double dist=p_plane.distance_to_point( v1 ) / den;
				dist=-dist;
				polygon_aux[count++] = v1 + segment * dist;
			}
			
			polygon_aux[count++] = v1;
		}
		
		previous = index;
	}
	
	SWAP(polygon,polygon_aux);
	polygon_len=count;

}

int PolygonClipper::get_point_count() const{

	return polygon_len;
}
Vector3 PolygonClipper::get_point(int p_which) const{

	ERR_FAIL_INDEX_V(p_which,polygon_len,Vector3());
	
	return polygon[p_which];	
}

PolygonClipper::PolygonClipper() {

	
	polygon=_polygon;
	polygon_aux=_polygon_aux;
	polygon_max=MAX_POINTS;
	polygon_len=0;
}


PolygonClipper::~PolygonClipper() {


}
