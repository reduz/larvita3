//
// C++ Interface: static_body
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef STATIC_BODY_H
#define STATIC_BODY_H


#include "physics/physics_body.h"

/**
	@author Juan Linietsky <reduzio@gmail.com>
*/
class StaticBody : public PhysicsBody {

	bool changed;
	bool change_computed;
	bool first_set;
	float step_cache;
	Vector3 linear_velocity;
	Vector3 angular_velocity;
	Matrix4 old_transform;
	Matrix4 transform;
	Matrix4 new_transform;
	IRef<Volume> volume;
	bool collidable;

	float friction,bounce;
	Uint32 extra_flags;

	void compute_change();
	Vector3 compute_angular_velocity(const Vector3& p_old,const Vector3& p_new,float p_time);
	
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
	
	virtual void apply_impulse(const Vector3& p_pos, const Vector3& p_dir);
	
	virtual bool is_collidable() const;
	void set_collidable(bool p_active);
	
	virtual bool can_process() const { return changed; }
	
	void set_extra_flags(Uint32 p_flags);
	virtual Uint32 get_flags() const;
	
	/**
	* For objects that don't do any physics, only serve for colliding against it
	*/
	
	virtual bool is_pure_collision() const; ///< Doesn't do physiscs
	virtual void collide_volume(const Volume& p_volume,const Matrix4& p_transform,VolumeContactHandler *p_contact_handler,float &r_friction,float &r_bounce) {}

	virtual void integrate_forces(float p_time);
	virtual void integrate_velocities(float p_time);
				
	void set_friction(float p_friction);
	void set_bounce(float p_bounce);
	
	virtual IRef<IAPI> get_owner() const;
	void set_owner(IRef<IAPI> p_iapi);

	void teleport(const Matrix4& p_new_transform); ///< move to new location
	
	StaticBody(IRef<Volume> p_volume);
	~StaticBody();
};

#endif
