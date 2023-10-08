//
// C++ Interface: character_body
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef CHARACTER_BODY_H
#define CHARACTER_BODY_H

#include "physics/physics_body.h"

/**
	@author Juan Linietsky <reduzio@gmail.com>
*/
class CharacterBody : public PhysicsBody {

	Vector3 pos;

	float max_walk_speed;
	float walk_accel;
	float walk_damp;
	float turn_speed;
	float walk_speed; // uset to obtain
	float jump_accel_damp;
	Vector3 linear_velocity;
	Vector3 target_linear_velocity;
	Vector3 target_normal; // dir towards where it wants to go
	Vector3 facing_dir;
	float jump_impulse;
	float jump_damp;
	
	bool influence;
	
	bool jump;
	bool jumping;

	bool hover;	// flag to know if we're flying or not
	float gravity_factor; // this is the factor the gravity is multiplied by.
	
	bool collidable;
	
	Vector3 floor_velocity_old;
	Vector3 floor_velocity;
	Vector3 floor_velocity_max;
	
	int on_floor_frames;
	bool on_floor;
	float last_on_floor_time;
	float airborne_min; // max time since it touched floor for the last time allowed to be considered on floor
	
	Vector3 char_velocity;
	Vector3 applied_impulse;
		
	Vector3 gn; // gravity normal cache
	
	float orientation; // orientation angle, around gravity vector

	Matrix4 transform; /// used as cache mainly
	AABB volume_aabb;
	
	IRef<Volume> volume;
	
	float mass,inv_mass;
	Vector3 inv_inertia_local;
	Matrix4 inv_inertia_tensor;
	
	float linear_damping;
	
	bool friction_request;
	
	void recreate_inertia_tensor();
	
	void adjust_turn_velocity(Vector3 &p_h_linear_vel,const Vector3& p_target_dir,float p_time,bool p_deaccel_sudden_turns);
	void process_jump(float p_time);
	void process_floor(float p_time);
	
	WeakRef<IAPI> owner;

	void fly(float p_time);
	
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
	
	virtual void notify_friction_request(bool p_request);
	
	virtual void apply_impulse(const Vector3& p_pos, const Vector3& p_dir);
	
	virtual bool has_kinetic_friction() const { return false; }
	
	virtual void notify_contact(const PhysicsBody& p_body, const Vector3& p_contact, const Vector3& p_depth);
	
	virtual bool is_collidable() const;
	void set_collidable(bool p_active);
	
	virtual Uint32 get_flags() const;
	
	void set_influence(bool p_inf);
	bool is_influence();
	
	/**
	* For objects that don't do any physics, only serve for colliding against it
	*/
	
	virtual bool is_pure_collision() const; ///< Doesn't do physiscs
	virtual void collide_volume(const Volume& p_volume,const Matrix4& p_transform,VolumeContactHandler *p_contact_handler,float &r_friction,float &r_bounce) {}

	virtual void integrate_forces(float p_time);
	virtual void integrate_velocities(float p_time);
	
	void set_mass(float p_mass);
	
	void set_walk_max_velocity(float p_max_vel);
	void set_target_dir(const Vector3& p_normal); //< instruct the character to move towards a given direction
	Vector3 get_target_dir() const;
	
	void set_jump(bool p_jump);
	bool is_jump() const;
	void set_hover(bool p_hover);

	void set_gravity_factor(float p_factor);
	float get_gravity_factor();
		
	bool is_on_floor() const;
	bool is_going_up() const;
	
	/* configuration */
	
	void set_walk_accel(float p_walk_accel);
	void set_walk_damp(float p_walk_damp);
	void set_turn_speed(float p_turn_speed);;
	void set_jump_impulse(float p_jump_impulse);
	void set_jump_damp(float p_jump_damp);
	void set_jump_accel_dump(float p_jump_accel_damp);
	
	void set_facing_vector(const Vector3& p_dir);
	Vector3 get_facing_vector() const;
	
	float get_walk_velocity() const;
	float get_max_walk_velocity() const;
	float get_walk_accel() const;
	float get_walk_damp() const;
	float get_turn_speed() const;
	float get_jump_impulse() const;
	float get_jump_damp() const;
	
	virtual IRef<IAPI> get_owner() const;
	void set_owner(IRef<IAPI> p_iapi);
	void spin(float p_angle);
	void reset_velocity();
	
	virtual bool is_cb() { return true; }
	CharacterBody(IRef<Volume> p_volume,float p_mass=0);
	~CharacterBody();

};

#endif
