
//
// C++ Implementation: static_body
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "static_body.h"
#include "os/memory.h"


void StaticBody::set_extra_flags(Uint32 p_flags) {
	
	extra_flags=p_flags;
}

Uint32 StaticBody::get_flags() const {
	
	return (collidable?FLAG_STATIC:FLAG_INFLUENCE) | extra_flags;
}


bool StaticBody::is_collidable() const {
	
	return collidable;
}

void StaticBody::set_collidable(bool p_active) {
	
	collidable=p_active;
}


float StaticBody::get_friction() const {

	return friction;
}

float StaticBody::get_bounce() const {

	return bounce;
}

float StaticBody::get_inv_mass() const {

	return 0.0; /* infinite mass */
}

Matrix4 StaticBody::get_inv_inertia_tensor() const {

	/* infinite inertia tensor */
	static const Matrix4 z(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);
	return z;
}


Matrix4 StaticBody::get_transform() const {

	return transform;
}
	
IRef<Volume> StaticBody::get_volume() const {

	return volume;
}

AABB StaticBody::get_aabb() const {

	return volume->compute_AABB(transform);
}

Vector3 StaticBody::get_linear_velocity() const {

	if (changed && !change_computed)
		const_cast<StaticBody*>(this)->compute_change();
	
	return linear_velocity;
}

Vector3 StaticBody::get_angular_velocity() const {

	if (changed && !change_computed)
		const_cast<StaticBody*>(this)->compute_change();
	
	return angular_velocity;
}


void StaticBody::apply_impulse(const Vector3& p_pos, const Vector3& p_dir) {

	/* This object won't get impulses applied to it */	
}


/**
* For objects that don't do any physics, only serve for colliding against it
*/

bool StaticBody::is_pure_collision() const {

	return false;
}


/****************/

void StaticBody::teleport(const Matrix4& p_new_transform) {
	
	old_transform=p_new_transform;
	transform=p_new_transform;
	linear_velocity=Vector3();
	angular_velocity=Vector3();
	changed=false;
	change_computed=true;
	transform_changed.call();
}
 
void StaticBody::set_transform(const Matrix4& p_transform) {

	/* We only care about the linear velocity of this object */
	
	new_transform=p_transform;
	changed=true;
	change_computed=false;
			
}


Vector3 StaticBody::compute_angular_velocity(const Vector3& p_old,const Vector3& p_new,float p_time) {
	
	Vector3 vold = p_old.get_normalized();
	Vector3 vnew = p_new.get_normalized();
		
	float voldnewdot=vold.dot(vnew);
	if ( ABS(voldnewdot)> 0.99999) {
		
		return Vector3();		
	}
	
	Vector3 vperp = Vector3::cross(vold,vnew).get_normalized();
	Vector3 vtangent = Vector3::cross(vperp,vold).get_normalized() ;
	
	float x = vold.dot(vnew);
	float y = vtangent.dot(vnew);
	
	float ang = Math::atan2(y,x);
	
	vperp *= ang / p_time;
	
	return vperp;	
	
}


void StaticBody::compute_change() {
		
	if (step_cache==0) {
	
		return;
	}
	
	/* compute velocity */
	Vector3 distance=new_transform.get_translation() - old_transform.get_translation();
	
	linear_velocity = distance/step_cache;

	
	/* compute linear_velocity */
	
	Vector3 axes_old[3]={ Vector3(1,0,0) , Vector3(0,1,0), Vector3(0,0,1) };
	Vector3 axes_new[3]={ Vector3(1,0,0) , Vector3(0,1,0), Vector3(0,0,1) };
	
	
	Matrix4 clean_transform=new_transform;
	Matrix4 clean_old_transform=old_transform;
	clean_transform.clear_translation();
	clean_old_transform.clear_translation();
	
	angular_velocity=Vector3();
	
	for (int i=0;i<3;i++) {
		clean_transform.transform(axes_new[i]);
		clean_old_transform.transform(axes_old[i]);
		
		Vector3 av=compute_angular_velocity( axes_old[i], axes_new[i],step_cache);
		angular_velocity += av;
	}
	
	change_computed=false;
}



void StaticBody::integrate_forces(float p_time) {

	step_cache=p_time;
	
	if (!changed)
		return;
	
	
	
	
	if (first_set) {
	
		old_transform=transform;
		first_set=false;
		return;
	}
		
	if (p_time==0.0)
		return;
	
	if (!change_computed)
		compute_change();

}
void StaticBody::integrate_velocities(float p_time) {
		
	if (!changed)
		return;
		
	transform=new_transform;
	old_transform=transform;
	linear_velocity=Vector3();
	angular_velocity=Vector3();
	changed=false;
	change_computed=true;
	
	transform_changed.call();

}

void StaticBody::set_friction(float p_friction) {

	friction=0;
}

void StaticBody::set_bounce(float p_bounce) {

	bounce=0;
}

IRef<IAPI> StaticBody::get_owner() const {
	
	return owner.get_iref();
}

void StaticBody::set_owner(IRef<IAPI> p_iapi) {
	
	owner=p_iapi;
}


StaticBody::StaticBody(IRef<Volume> p_volume) {
	
	first_set=true;
	changed=true;
	change_computed=false;
	volume=p_volume;
	friction=1.0;
	bounce=0.0;
	collidable=true;
	extra_flags=0;
	step_cache=0;
}


StaticBody::~StaticBody() {

}


