//
// C++ Interface: physics_world_settings
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef PHYSICS_WORLD_SETTINGS_H
#define PHYSICS_WORLD_SETTINGS_H

#include "vector3.h"
/**
	@author Juan Linietsky <reduzio@gmail.com>
*/
class PhysicsWorldSettings {

	Vector3 gravity;
public:

	void set_gravity(const Vector3& p_vector);
	Vector3 get_gravity() const;

	PhysicsWorldSettings();	
	~PhysicsWorldSettings();

};

#endif
