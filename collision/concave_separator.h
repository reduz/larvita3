//
// C++ Interface: concave_separator
//
// Description: 
//
//
// Author: ,,, <red@lunatea>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef CONCAVE_SEPARATOR_H
#define CONCAVE_SEPARATOR_H


#include "collision/volume_separator.h"
#include "collision/sphere_volume.h"
#include "collision/pill_volume.h"
#include "collision/convex_polygon_volume.h"
#include "collision/gjk_separator.h"
#include "collision/triangle_volume.h"
/**
	@author ,,, <red@lunatea>
*/
class ConcaveSeparator : public VolumeSeparator {

	
	mutable VolumeContactHandler *contact_handler;
	mutable const ConvexPolygonVolume *B_convex_polygon;
	mutable Matrix4 convex_transform;
	mutable const SphereVolume *B_sphere;
	mutable Vector3 sphere_pos;
	mutable float sphere_radius;
	mutable bool swap;
	mutable const PillVolume *B_pill;
	mutable Matrix4 pill_transform;

	TriangleVolume triangle_vol;
	GJK_Separator gjk;

	inline bool triangle_callback_sphere_intersection_test(const Face3& p_triangle,const Vector3& p_normal,Vector3& r_triangle_contact,Vector3& r_sphere_contact) const;
	
	void triangle_callback_sphere(const Face3& p_triangle,const Vector3& p_normal);
	void triangle_callback_convex_polygon(const Face3& p_triangle,const Vector3& p_normal);
	void triangle_callback_pill(const Face3& p_triangle,const Vector3& p_normal);

	mutable const Volume *volume_generic;
	mutable Matrix4 transform_generic;
	void triangle_callback_generic(const Face3& p_triangle,const Vector3& p_normal);
	
public:

	virtual bool handles( const Volume& p_A, const Volume& p_B ) const;
	virtual void separate( const Volume& p_A,const Matrix4& p_A_transform, const Volume& p_B, const Matrix4 &p_B_transform, VolumeContactHandler *p_contact_handler) const;
};

#endif
