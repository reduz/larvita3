//
// C++ Implementation: volume
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "volume.h"


void Volume::intersect_AABB_with_faces(const Matrix4& p_transform,const AABB& p_AABB, Method2<const Face3&,const Vector3&>& p_face_callback) const {

	// doesn't need to do anything by defauult
}


Volume::Volume()
{
}


Volume::~Volume()
{
}


