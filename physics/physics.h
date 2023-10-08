//
// C++ Interface: physics
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef PHYSICS_H
#define PHYSICS_H

#include "physics/physics_world_settings.h"
#include "physics/physics_body.h"
#include "physics/narrow_pair.h"
#include "physics/broad_phase.h"


/**
	@author Juan Linietsky <reduzio@gmail.com>
*/
class Physics : public SignalTarget {

public:
	enum {
		
		DEFAULT_MAX_ITERATIONS=10
	};

protected:
	
	PhysicsWorldSettings world_settings;
	
	typedef List<PhysicsBody*> BodyList;
	BodyList body_list;
	int iterations;
	
	struct Collision {
		PhysicsBody* body1;
		PhysicsBody* body2;
		Vector3 normal;
	};
	
	List<Collision> collision_list;	
	
	BroadPhase *broad_phase;

	void narrow_pair_collision(PhysicsBody* p_b1, PhysicsBody* p_b2, const Vector3& p_normal);
	void narrow_pair_created(NarrowPair* p_np);

public:

	Method3< PhysicsBody*, PhysicsBody*,const Vector3&> collision_notify;

	PhysicsWorldSettings &get_world_settings();
	
	BroadPhase* get_broad_phase() { return broad_phase; };
	
	virtual void register_body(PhysicsBody *p_body);
	virtual void remove_body(PhysicsBody *p_body);
	
	virtual void process(float p_time);
	
	virtual PhysicsBody * intersect_segment(const Vector3& p_from,const Vector3& p_to,Vector3& r_clip,Vector3& r_normal,Uint32 p_flags=PhysicsBody::FLAGS_ALL) const;
	

	virtual void intersect_volume_and_fetch_contacts(IRef<Volume> p_volume,const Matrix4& p_transform, const Vector3& p_motion,BroadPhase::ContactData * p_contacts,int *p_contact_count,int p_contact_max,Uint32 p_flags=PhysicsBody::FLAGS_ALL);
	
		
	Physics(int p_iterations=DEFAULT_MAX_ITERATIONS);
	virtual ~Physics();

};

#endif
