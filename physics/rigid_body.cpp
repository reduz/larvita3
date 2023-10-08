
//
// C++ Implementation: rigid_body
//
// Description:
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "rigid_body.h"
#include "os/memory.h"
#include "physics_world_settings.h"

Uint32 RigidBody::get_flags() const {

	return FLAG_DYNAMIC;
}

float RigidBody::get_friction() const {

	return friction;
}
float RigidBody::get_bounce() const {

	return bounce;

}

void RigidBody::set_friction(float p_friction) {

	friction=p_friction;
}
void RigidBody::set_bounce(float p_bounce) {

	bounce=p_bounce;
}

float RigidBody::get_inv_mass() const {

	return inv_mass;
}
Matrix4 RigidBody::get_inv_inertia_tensor() const {

	return inv_inertia_tensor;
}

Matrix4 RigidBody::get_transform() const {

	return transform;
}
void RigidBody::set_transform(const Matrix4& p_transform)  {

	transform=p_transform;
}

IRef<Volume> RigidBody::get_volume() const {

	return volume;
}

AABB RigidBody::get_aabb() const{

	return volume->compute_AABB( transform );
}

Vector3 RigidBody::get_linear_velocity() const {

	return linear_velocity;
}

Vector3 RigidBody::get_angular_velocity() const {
	return angular_velocity;
}

void RigidBody::set_linear_velocity(const Vector3& p_velocity) {
	linear_velocity.set(p_velocity);
}

void RigidBody::set_angular_velocity(const Vector3& p_velocity) {
	angular_velocity.set(p_velocity);
}

void RigidBody::reset_velocity() {
	Vector3 vector3;
	set_linear_velocity(vector3);
	set_angular_velocity(vector3);
}

void RigidBody::apply_impulse(const Vector3& p_pos, const Vector3& p_dir) {


	if (inv_mass==0)
		return;

	linear_velocity += p_dir * inv_mass;
	Vector3 torque = Vector3::cross(p_pos,p_dir);
	inv_inertia_tensor.transform( torque );
	//printf("torque is %f,%f,%f\n",torque.x,torque.y,torque.z);
	angular_velocity += torque;


}

void RigidBody::set_owner(IRef<IAPI> p_owner) {

	owner = p_owner;
};

IRef<IAPI> RigidBody::get_owner() const {

	return owner;
};


/**
* For objects that don't do any physics, only serve for colliding against it
*/

bool RigidBody::is_pure_collision() const {

	return false;
}

/************************************************
 ************************************************/



void RigidBody::recreate_inertia_tensor() {


	Matrix4 n = transform;
	n.clear_translation();
	inv_inertia_tensor=n;
	inv_inertia_tensor.scale(inv_inertia_local);
	n.transpose();
	inv_inertia_tensor = n * inv_inertia_tensor;

}


void RigidBody::integrate_forces(float p_time) {

	float time=p_time;

	Vector3 force;

	if (get_world_settings())
		force = get_world_settings()->get_gravity() * mass; /* weight force */
	//force += applied_force; /* applied force */

	linear_velocity+=force*(inv_mass*p_time);

	float damp_l = 1.0 - time*linear_damping;

	if (damp_l<0)
		damp_l=0; /* basically, if linear_damping < 0, means that the object deacelerated to zero linear_velocity in the elapsed time, so just make it stand still */

	linear_velocity*=damp_l; /* apply friction */

	float damp_a = 1.0 - time*angular_damping;

	if (damp_a<0)
		damp_a=0; /* basically, if angular_damping < 0, means that the object deacelerated to zero angular_velocity in the elapsed time, so just make it stand still */

	angular_velocity*=damp_a; /* apply friction */

}
void RigidBody::integrate_velocities(float p_time) {

	Vector3 translation = transform.get_translation();
	Matrix4 rotation;
	//if (angular_velocity.length()>0.01)
		rotation.rotate( angular_velocity.get_normalized(), angular_velocity.length()*p_time );
	transform.clear_translation();
	transform = rotation * transform;
	//if (linear_velocity.length()>0.5)
		transform.set_translation( translation + linear_velocity*p_time );
	//else
		//transform.set_translation( translation  );

	recreate_inertia_tensor();
	transform_changed.call();

}



void RigidBody::set_mass(float p_mass) {


	if (p_mass<0) {

		mass=1e7;
		inv_mass=0;
	} else {

		if (p_mass==0.0)
			p_mass=0.000001;
		inv_mass=1.0/p_mass;
		if (inv_mass<0.0000001)
			inv_mass=0;
		mass=p_mass;

	}

	inv_inertia_local=Vector3(
		volume->get_local_inertia(p_mass).x ? 1.0 / volume->get_local_inertia(p_mass).x : 0,
		volume->get_local_inertia(p_mass).y ? 1.0 / volume->get_local_inertia(p_mass).y : 0,
		volume->get_local_inertia(p_mass).z ? 1.0 / volume->get_local_inertia(p_mass).z : 0 );

	recreate_inertia_tensor();

}

RigidBody::RigidBody(IRef<Volume> p_volume,float p_mass) {


	if (p_mass<0) { //infinite mass

		mass=1e7;
		inv_mass=0;
	} else {

		if (p_mass==0.0)
			p_mass=0.000001;

		inv_mass=1.0/p_mass;
		if (inv_mass<0.0000001)
			inv_mass=0;

		mass=p_mass;

	}


	volume=p_volume;

	linear_damping=0.1;
	angular_damping=0.1;
	friction=1.0;
	bounce=0.5;
	volume_aabb=volume->compute_AABB( Matrix4() );
	inv_inertia_local=Vector3(
		p_volume->get_local_inertia(p_mass).x ? 1.0 / p_volume->get_local_inertia(mass).x : 0,
		p_volume->get_local_inertia(p_mass).y ? 1.0 / p_volume->get_local_inertia(mass).y : 0,
		p_volume->get_local_inertia(p_mass).z ? 1.0 / p_volume->get_local_inertia(mass).z : 0 );

	recreate_inertia_tensor();


}


RigidBody::~RigidBody() {

}


