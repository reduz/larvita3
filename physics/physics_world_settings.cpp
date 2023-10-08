//
// C++ Implementation: physics_world_settings
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "physics_world_settings.h"


void PhysicsWorldSettings::set_gravity(const Vector3& p_vector) {

	gravity=p_vector;
}
Vector3 PhysicsWorldSettings::get_gravity() const {

	return gravity;
}


PhysicsWorldSettings::PhysicsWorldSettings() {

	gravity=Vector3(0,-9.8,0);
}


PhysicsWorldSettings::~PhysicsWorldSettings()
{
}


