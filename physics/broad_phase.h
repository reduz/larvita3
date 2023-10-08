//
// C++ Interface: broad_phase
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef BROAD_PHASE_H
#define BROAD_PHASE_H

#include "physics/physics_body.h"
#include "physics/narrow_pair.h"

/**
	@author Juan Linietsky <reduzio@gmail.com>
*/
class BroadPhase : public SignalTarget {
	
protected:
	static BroadPhase* (*create_func)();
public:

	Signal<Method1 < NarrowPair* > > narrow_pair_created_signal;

	virtual VolumeSeparatorManager* get_volume_separator_manager()=0;

	virtual void register_body(PhysicsBody *p_body)=0;
	virtual void remove_body(PhysicsBody *p_body)=0;
	
	virtual List<NarrowPair*>* get_narrow_pair_list()=0;
	
	virtual void pre_process(float p_time)=0;
	virtual void process(float p_time)=0;
	
	virtual PhysicsBody * intersect_segment(const Vector3& p_from,const Vector3& p_to,Vector3& r_clip,Vector3& r_normal,Uint32 p_flags) const=0;
		
	struct ContactData {
		
		Vector3 dir;
		PhysicsBody *against;	
	};
		
	virtual void intersect_volume_and_fetch_contacts(IRef<Volume> p_volume,const Matrix4& p_transform,const Vector3& p_motion,ContactData * p_contacts,int *p_contact_count,int p_contact_max,Uint32 p_flags=PhysicsBody::FLAGS_ALL)=0;
	
	static BroadPhase* create();
	
	
	BroadPhase();
	virtual ~BroadPhase();

};

#endif
