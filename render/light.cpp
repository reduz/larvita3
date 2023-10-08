//
// C++ Implementation: light
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "light.h"

#include "math_funcs.h"

const AABB Light::get_AABB() const {
	
	ERR_FAIL_COND_V( get_mode() == MODE_DIRECTIONAL, AABB() );
	
	if (get_mode() == MODE_POINT) {
		
		float r = get_radius();
		Rect3 aabb = Rect3( Vector3(-r, -r, -r), Vector3(r*2, r*2, r*2) );
		return aabb;
	} else if (get_mode() == MODE_SPOT) {
		
		float r = get_radius();
		float ang = get_spot_angle() / 180.0 * Math_PI;;
		float h = Math::tan(ang) * r;
		return AABB( Vector3( -h, -h, 0 ), Vector3( 2*h, 2*h, -r ) );
	};
	
	return AABB();
};

Light::Light()
{
}


Light::~Light()
{
}


