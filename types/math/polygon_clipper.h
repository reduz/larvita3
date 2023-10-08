//
// C++ Interface: polygon_clipper
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef POLYGON_CLIPPER_H
#define POLYGON_CLIPPER_H

#include "vector3.h"
#include "plane.h"


/**
	@author Juan Linietsky <reduzio@gmail.com>
 * This class is used to clip a polygon given by a set of points, by arbitrary planes.
 * The resulting polygon can be retrieved
*/

class PolygonClipper{

	enum {
	
		MAX_POINTS=128
	};

	enum LocationCache {
		LOC_INSIDE=1,
		LOC_BOUNDARY=0,
		LOC_OUTSIDE=-1
	};

	Vector3 _polygon[MAX_POINTS];
	Vector3 _polygon_aux[MAX_POINTS];
	LocationCache location_cache[MAX_POINTS];

	Vector3 *polygon;
	Vector3 *polygon_aux;
	
	int polygon_len;
	int polygon_max;

public:
	
	void clear();
	void add_point(const Vector3& p_point);
	
	void clip(const Plane& p_plane);

	int get_point_count() const;
	Vector3 get_point(int p_which) const;

	PolygonClipper();
	~PolygonClipper();

};

#endif
