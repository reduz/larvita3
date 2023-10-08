//
// C++ Interface: rigid_body
//
// Description:
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef RIGID_BODY_H
#define RIGID_BODY_H

#include "physics/physics_body.h"

/**
	@author Juan Linietsky <reduzio@gmail.com>
*/
class RigidBody : public PhysicsBody {

	/* physics stuff */
	float mass;
	float inv_mass;
	float linear_damping;
	float angular_damping;
	Vector3 linear_velocity;
	Vector3 angular_velocity;
	Vector3 applied_force;
	Vector3 applied_torque;
	Vector3 inv_inertia_local;
	Matrix4 inv_inertia_tensor;

	/* info get */

	Matrix4 transform;

	/* Collision Stuff */

	float friction;
	float bounce;

	IRef<Volume> volume;
	bool own_volume;
	AABB volume_aabb;

	void recreate_inertia_tensor();

	WeakRef<IAPI> owner;

public:
	virtual float get_friction() const;
	virtual float get_bounce() const;
	virtual float get_inv_mass() const;
	virtual Matrix4 get_inv_inertia_tensor() const;

	virtual Matrix4 get_transform() const;
	virtual void set_transform(const Matrix4& p_transform);

	virtual IRef<Volume> get_volume() const;
	virtual AABB get_aabb() const; ///< Get AABB for this object.

	virtual Vector3 get_linear_velocity() const;
	virtual Vector3 get_angular_velocity() const;

	virtual void set_linear_velocity(const Vector3& p_velocity);
	virtual void set_angular_velocity(const Vector3& p_velocity);
	virtual void reset_velocity();

	virtual void apply_impulse(const Vector3& p_pos, const Vector3& p_dir);

	virtual Uint32 get_flags() const;

	/**
	* For objects that don't do any physics, only serve for colliding against it
	*/

	virtual bool is_pure_collision() const; ///< Doesn't do physiscs
	virtual void collide_volume(const Volume& p_volume,const Matrix4& p_transform,VolumeContactHandler *p_contact_handler,float &r_friction,float &r_bounce) {}

	virtual void integrate_forces(float p_time);
	virtual void integrate_velocities(float p_time);

	void set_mass(float p_mass);

	void set_friction(float p_friction);
	void set_bounce(float p_bounce);

	void set_owner(IRef<IAPI> p_owner);
	IRef<IAPI> get_owner() const;

	RigidBody(IRef<Volume> p_volume,float p_mass=0);
	~RigidBody();
};

#endif
