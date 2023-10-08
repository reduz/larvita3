//
// C++ Interface: convex_group
//
// Description:
//
//
// Author: Juan Linietsky <reduz@codenix.com>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef SPATIALCONVEX_GROUP_H
#define SPATIALCONVEX_GROUP_H


#include "plane.h"
#include "dvector.h"
#include "aabb.h"
#include "matrix4.h"


/**
	@author Juan Linietsky <reduz@codenix.com>
 * Simple group of planes which define a convex area. Useful for frustum culling or
 * defining custom convex areas.
 * Convex groups have to be impklemented as DVector, with lockable memory..
 * this is because they may get pretty big.
*/

class ConvexGroup : public DVector<Plane> {


public:

	bool intersects_with_aabb(const AABB& p_aabb) const;
	void create_from_projection(const Matrix4& p_proj_matrix); ///< Create a convex group of planes (delimiting the frustum volume) from either a projection matrix, or a combined modelview and projection.
	bool intersects(Vector3* p_points, int p_count) const;
	
	ConvexGroup() {};
};




#endif
