//
// C++ Implementation: physics_body
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "physics_body.h"

#include "physics/physics.h"


void PhysicsBody::notify_contact(const PhysicsBody& p_body, const Vector3& p_contact, const Vector3& p_depth) {
	
	
}

IRef<IAPI> PhysicsBody::get_owner() const {
	
	return IRef<IAPI>();
}


PhysicsWorldSettings * PhysicsBody::get_world_settings() const {

	if (_ps)
		return &_ps->get_world_settings();
		
	return NULL;
}

Vector3 PhysicsBody::get_velocity_at_point(const Vector3& p_point) const {

	return get_linear_velocity() + Vector3::cross( get_angular_velocity(), p_point );
}

bool PhysicsBody::intersect_segment(const Vector3& p_from,const Vector3& p_to,Vector3& r_clip,Vector3& r_normal) const {
	
	ERR_FAIL_COND_V(is_pure_collision(),false);
	IRef<Volume> v=get_volume();
	ERR_FAIL_COND_V(v.is_null(),false);
	return v->intersect_segment(get_transform(),p_from,p_to,r_clip,r_normal);	
}


bool PhysicsBody::is_collidable() const {
	
	return true;
}

Variant PhysicsBody::get_user_data() const {
	
	return user_data;
};

void PhysicsBody::set_user_data(Variant p_ud) {
	
	user_data = p_ud;
};


PhysicsBody::PhysicsBody() {

	_ps=NULL;
}

PhysicsBody::~PhysicsBody() {

	if (_ps)
		_ps->remove_body(this);
		
}
