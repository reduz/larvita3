//
// C++ Interface: body
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef PHYSICS_ENTITY_H
#define PHYSICS_ENTITY_H

/**
	@author Juan Linietsky <reduzio@gmail.com>
*/


#include "iapi.h"
#include "collision/volume_separator.h"


class Physics;
class PhysicsWorldSettings;



class PhysicsBody {
friend class Physics;
friend class NarrowPair;
	Physics *_ps; // used for auto-removal upon delete
protected:
	PhysicsBody();
	PhysicsWorldSettings * get_world_settings() const;
	inline Physics * get_physics_system() const { return _ps; }
	
	Variant user_data;
	
public:
	
	Signal<> transform_changed;
	
	enum Flags { // default flags
		
  		FLAG_STATIC=1, ///< object isn't affected by physics (or controled by user), can be collided
  		FLAG_DYNAMIC=4, ///< Body is affected by physics, collides
  		FLAG_INFLUENCE=8, ///< body influences others but doesn't collide
    		FLAGS_ALL=0xFFFFFFFF
	};	
	
	virtual float get_friction() const=0;
	virtual float get_bounce() const=0;
	virtual float get_inv_mass() const=0;
	virtual Matrix4 get_inv_inertia_tensor() const=0;
	virtual Uint32 get_flags() const=0;
	
	virtual Matrix4 get_transform() const=0;
	virtual void set_transform(const Matrix4& p_transform)=0;
		
	virtual IRef<Volume> get_volume() const=0;
	virtual AABB get_aabb() const=0; ///< Get AABB for this object.
	
	virtual Vector3 get_linear_velocity() const=0;
	virtual Vector3 get_angular_velocity() const=0;
	
	virtual Vector3 get_velocity_at_point(const Vector3& p_point) const;
	
	virtual void apply_impulse(const Vector3& p_pos, const Vector3& p_dir)=0;
		
	virtual void notify_contact(const PhysicsBody& p_body, const Vector3& p_contact, const Vector3& p_depth);
	
	virtual bool is_collidable() const;
	
	virtual bool can_process() const { return true; }
	/**
	 * For objects that don't do any physics, only serve for colliding against it
	 */	 
	virtual bool is_pure_collision() const=0; ///< Doesn't do physiscs
	virtual void collide_volume(const Volume& p_volume,const Matrix4& p_transform,VolumeContactHandler *p_contact_handler,float &r_friction,float &r_bounce)=0;
	
	virtual bool intersect_segment(const Vector3& p_from,const Vector3& p_to,Vector3& r_clip,Vector3& r_normal) const;
					
	virtual IRef<IAPI> get_owner() const;
	virtual bool need_collision_notify() const { return false; }

	/* user data */
	Variant get_user_data() const;
	void set_user_data(Variant p_ud);
	
	/* process */				
	
	virtual void integrate_forces(float p_time)=0;
	virtual void integrate_velocities(float p_time)=0;
				
	virtual ~PhysicsBody();
};



#endif
