 //
// C++ Interface: narrow_pair
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef NARROW_PAIR_H
#define NARROW_PAIR_H

/**
	@author Juan Linietsky <reduzio@gmail.com>
*/

#include "physics/physics_body.h"
#include "collision/volume_separator.h"
#include "signals/signals.h"

struct NarrowPairKey {

	PhysicsBody *A;
	PhysicsBody *B;

	inline bool operator<(const NarrowPairKey& p_pair) const {
	
		if (A < p_pair.A)
			return true;
		if (A > p_pair.A)
			return false;
			
		return (B < p_pair.B );
	}

	NarrowPairKey(PhysicsBody *p_A,PhysicsBody *p_B) {
		
		A=p_A;
		B=p_B;
		if (A>B)
			SWAP(A,B);
	}
};

class NarrowPair : public VolumeContactHandler {

	enum {
		MAX_CONTACTS=8
	};
		
		
	
		
	struct ContactData {
	
		VolumeContact contact;
		VolumeContact contact_local;
		Vector3 contact_middle_point;
		
		float depth;
		
		Vector3 normal;
		Vector3 tangent;
		/* computed per contact */
		float acc_normal_impulse;
		float acc_tangent_impulse;
		float mass_normal;
		float mass_tangent;
		
		int lifetime;
		
		ContactData() { acc_normal_impulse=0; acc_tangent_impulse=0;  lifetime=0; }
	};
	
	
	int contact_count;
	int old_contact_count;
	
	ContactData *contacts;
	ContactData *old_contacts;
	
	float current_friction;

	PhysicsBody *A;
	PhysicsBody *B;
	
	static bool accumulate_impulses;
	static bool split_impulses;
	static bool position_correction;
	static bool warm_start;
	
	VolumeSeparator *separator;
	
	
	void add_contact(const VolumeContact& p_contact,bool p_collided);
	bool invert_contacts;
	
	float timestep_cache;
	
public:
	
	Method3< PhysicsBody*, PhysicsBody*,const Vector3&> collision_notify;

	/**
	 * NarrowPairs must be sortable to warrant the processing order
	 */
	
	PhysicsBody *get_A();
	PhysicsBody *get_B();
	

	void process(float p_time);
	void solve();
	
	NarrowPair();
	
	NarrowPair(PhysicsBody *p_A,PhysicsBody *p_B,VolumeSeparatorManager *p_separator_manager);
	~NarrowPair();

};

#endif
