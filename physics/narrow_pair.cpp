//
// C++ Implementation: narrow_pair
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "narrow_pair.h"
#include "quat.h"
#include "os/memory.h"

#include "collision/volume_separator.h"

bool NarrowPair::accumulate_impulses=true;
bool NarrowPair::split_impulses=false;
bool NarrowPair::position_correction=true;
bool NarrowPair::warm_start=false;

#define ALLOWED_PENETRATION 0.001
// bullet uses this as threshold for breaking contacts
#define CONTACT_BREAKING_THRESHOLD 0.002
#define CONTACT_BREAKING_THRESHOLD_SQR (CONTACT_BREAKING_THRESHOLD*CONTACT_BREAKING_THRESHOLD)


void NarrowPair::add_contact(const VolumeContact& p_contact,bool p_collided) {

	if (!p_collided) 
		return;
//	A->_colliding=true;
//	B->_colliding=true;
	

	/* ADDING A CONTACT */
	
	
	VolumeContact contact_local=p_contact;;
	A->get_transform().inverse_transform(contact_local.A);
	B->get_transform().inverse_transform(contact_local.B);
	
	// 1st , check that we already have it in the new contacts
	
	
	for (int i=0;i<contact_count;i++) {
	
//		ContactData &cd=contacts[i];
		
		Vector3 rel_A = contacts[i].contact_local.A - contact_local.A;
		Vector3 rel_B = contacts[i].contact_local.B - contact_local.B;
		
		float sqr_dist_relA=rel_A.squared_length();
		float sqr_dist_relB=rel_B.squared_length();
		
		if (sqr_dist_relA<CONTACT_BREAKING_THRESHOLD_SQR && sqr_dist_relB<CONTACT_BREAKING_THRESHOLD_SQR)
			return; // pointless
			
	}
	
	//printf("adding contact %f,%f,%f\n",(p_contact.A-p_contact.B).x,(p_contact.A-p_contact.B).y,(p_contact.A-p_contact.B).z);
	
	// 2nd , add it
	
	int contact_pos=-1;
	
	if (contact_count<MAX_CONTACTS) {
		// room for contacts exists
		contact_pos=contact_count++;
		
	} else {
		/* no more room for contacts, try to see if replacing is worth */
		//printf("OUT OF CONTACTS\n");
		Vector3 accum;
		int deepest_idx=-1;
		float deepest_depth=0;
		
		// step 1, calculate the accum and the deepest contact 
		for(int i=0;i<MAX_CONTACTS;i++) {
		
			if (i==0) {
			
				deepest_idx=0;
				deepest_depth=contacts[0].depth;
			
			}
		
			accum+=contacts[i].contact_middle_point;
		}
		
		// step 2 calculate center of mass
		Vector3 cofmass = accum/contact_count;
		
		// step 3, calculate the closest pair of points
		// TODO@ find a faster way to do this :(
		
		int closest_idx_i=-1;
		int closest_idx_j=-1;
		float closest_dist=1e20;
		
		for (int i=0;i<MAX_CONTACTS;i++) {
		
			Vector3 ci=(i==deepest_idx)?p_contact.middle_point():contacts[i].contact_middle_point;
		
			for (int j=(i+1);j<MAX_CONTACTS;j++) {
			
				Vector3 cj=(j==deepest_idx)?p_contact.middle_point():contacts[j].contact_middle_point;
				
				
				float dist=ci.squared_distance_to(cj);
				if (dist<closest_dist) {
				
					closest_idx_i=i;
					closest_idx_j=j;
					closest_dist=dist;
				}
			}
			
		}
		
		// step 4, calculate which one of them is closest to the center of mass
		
		float dist_to_center_i=cofmass.squared_distance_to( 
			(closest_idx_i==deepest_idx)?p_contact.middle_point():
			contacts[closest_idx_i].contact_middle_point );
		
		float dist_to_center_j=cofmass.squared_distance_to( 
			(closest_idx_j==deepest_idx)?p_contact.middle_point():
			contacts[closest_idx_j].contact_middle_point );
		
		
		
		if ( dist_to_center_i < dist_to_center_j ) {
		
			if (closest_idx_i==deepest_idx) // new point must be discarded
				return;
				
			contact_pos=closest_idx_i;
		} else {
		
			if (closest_idx_j==deepest_idx) // new point must be discarded
				return;
				
			contact_pos=closest_idx_j;
		
		}
					
	}
	

	if (CollisionDebugger::singleton) {
		CollisionDebugger::singleton->plot_contact(p_contact.A);
		CollisionDebugger::singleton->plot_contact(p_contact.B);
	};
	
	VolumeContact contact=p_contact;
	if (invert_contacts)
		contact.invert();
		
	ContactData &cd=contacts[contact_pos];
	
	
	cd.contact=contact;
	cd.depth=(cd.contact.B-cd.contact.A).length();
	cd.contact_middle_point=cd.contact.middle_point();
	cd.contact_local=contact_local;
	
	cd.acc_tangent_impulse=0;
	cd.acc_normal_impulse=0;
	
	cd.mass_normal=0;
	cd.mass_tangent=0;
	cd.tangent=Vector3();
	cd.normal=Vector3();
	cd.contact=contact;
	cd.lifetime=0;
		

}



void NarrowPair::process(float p_time) {

		
	current_friction=1.0;

	/* process collision */
	
	old_contact_count=contact_count;
	contact_count=0;
	SWAP(contacts,old_contacts);
	
	if (A->is_pure_collision()) {
		
		float f=B->get_friction(),b=0;
		
		A->collide_volume( *B->get_volume().ptr(), B->get_transform(),this,f,b );
		current_friction=f;
		
	} else if (B->is_pure_collision()) {
	
		float f=A->get_friction(),b=0;
		invert_contacts=true;
		B->collide_volume( *A->get_volume().ptr(), A->get_transform(),this,f,b );
		invert_contacts=false;
		current_friction=f;
		
	} else if (separator) {
	
		separator->separate(*A->get_volume(),A->get_transform(),*B->get_volume(),B->get_transform(),this);
		current_friction = A->get_friction() * B->get_friction(); //Math::sqrt((A->get_friction() * A->get_friction()) + (B->get_friction() * B->get_friction()));
	} else {
		return; /* dont do anything */
	}
		
	
	/* process contacts*/
	
	float bias_factor = split_impulses ? 0.8f : 0.2f;
	bias_factor = position_correction ? bias_factor : 0.0f;

	Matrix4 transform_A=A->get_transform();
	Matrix4 transform_B=B->get_transform();


	timestep_cache=p_time;

	for (int i=0;i<contact_count;i++)
	{
	
		ContactData &c=contacts[i];
		
		Vector3 contact_A = c.contact.A - transform_A.get_translation();
		Vector3 contact_B = c.contact.B - transform_B.get_translation();

		Vector3 n = (c.contact.A-c.contact.B).get_normalized();
		c.normal=n;
		//printf("contact normal %f,%f,%f, depth %f, \n",c.normal.x,c.normal.y,c.normal.z,(c.contact.A-c.contact.B).length());
		
		//printf("contact %i normal %f,%f,%f\n",i,n.x,n.y,n.z);
		
	 	//float num = -1.0 * vel_rel.dot( n );
	 	float denom = ( A->get_inv_mass() + B->get_inv_mass() );
	 	
		Vector3 inertia_A = Vector3::cross( contact_A, n );
		A->get_inv_inertia_tensor().transform( inertia_A );
			
		Vector3 inertia_B = Vector3::cross( contact_B, n );
		B->get_inv_inertia_tensor().transform( inertia_B );
		 	
		denom += n.dot(Vector3::cross( inertia_A, contact_A )) + n.dot(Vector3::cross( inertia_B, contact_B ));
	 	
		c.mass_normal=1.0/denom;
		
		A->notify_contact(*B, contact_B, n);
		B->notify_contact(*A, contact_A, -n);
		
		if (A->get_owner() || B->get_owner()) {
			collision_notify.call(A,B,n);
		}

	}

}


void NarrowPair::solve() {

	if (!A->is_collidable() || !B->is_collidable())
		return; //not collidable, don't solve

	for (int i=0;i<contact_count;i++)
	{
	
		ContactData  &c =contacts[i];


		Vector3 contact_A = c.contact.A - A->get_transform().get_translation();
		Vector3 contact_B = c.contact.B - B->get_transform().get_translation();
	
		Vector3 vel = B->get_velocity_at_point(contact_B) - A->get_velocity_at_point(contact_A);		
		
		float bias=0.35;		
		bias=bias * (1.0/timestep_cache) * (c.depth-0.0001);
				
		float j = (-vel.dot(c.normal)+bias) * c.mass_normal;
		
		//printf("j is %f\n",j);
		
		if (accumulate_impulses) {

			float acc_imp_old = c.acc_normal_impulse;
			c.acc_normal_impulse=acc_imp_old+j;
			if (c.acc_normal_impulse<0)
				c.acc_normal_impulse=0;
			j=c.acc_normal_impulse-acc_imp_old;
		}
			
		
		A->apply_impulse( contact_A, c.normal * -j );
		B->apply_impulse( contact_B, c.normal * j );

		if (j==0)
			continue;
		
		// friction
		
		float friction=current_friction;
		
		if (ABS(friction)<CMP_EPSILON)
			continue;

		vel = B->get_velocity_at_point(contact_B) - A->get_velocity_at_point(contact_A);
					  
		float rel_vel = c.normal.dot(vel);


		Vector3 lat_vel = vel - c.normal * rel_vel;
		float lat_rel_vel = lat_vel.length();


		
		if (lat_rel_vel > CMP_EPSILON) {
			
			
			lat_vel /= lat_rel_vel;
			
			Vector3 temp1 = Vector3::cross( contact_A, lat_vel );
			A->get_inv_inertia_tensor().transform( temp1 );
				
			Vector3 temp2 = Vector3::cross( contact_B, lat_vel );
			B->get_inv_inertia_tensor().transform( temp2 );
			
			
			float friction_impulse = -lat_rel_vel /
				(A->get_inv_mass() + B->get_inv_mass() + lat_vel.dot(Vector3::cross(temp1,contact_A) + Vector3::cross(temp2,contact_B)));
			float normal_impulse = j * friction;

				
			if (accumulate_impulses) {
			
				float clampt=current_friction * c.acc_normal_impulse;
				
				float old_tangent_imp=c.acc_tangent_impulse;
				c.acc_tangent_impulse = old_tangent_imp + friction_impulse;
				if (c.acc_tangent_impulse < -clampt)
					c.acc_tangent_impulse=-clampt;
				if (c.acc_tangent_impulse > clampt)
					c.acc_tangent_impulse=clampt;
					
				
				friction_impulse = c.acc_tangent_impulse - old_tangent_imp;
				
				
			} else {
			
				if ( friction_impulse > normal_impulse)
					friction_impulse=normal_impulse;
					
				if ( friction_impulse < -normal_impulse)
					friction_impulse=-normal_impulse;
			
			}
														
			A->apply_impulse(contact_A,lat_vel * -friction_impulse);
			B->apply_impulse(contact_B,lat_vel * friction_impulse);
		}
	}

			
	
}

PhysicsBody *NarrowPair::get_A() {

	return A;
}
PhysicsBody *NarrowPair::get_B() {

	return B;
}

NarrowPair::NarrowPair() {

	A=NULL;
	B=NULL;
	contacts=NULL;
	old_contacts=NULL;
	contact_count=0;
	separator=NULL;
}

NarrowPair::NarrowPair(PhysicsBody *p_A,PhysicsBody *p_B,VolumeSeparatorManager *p_separator_manager) {

	A=p_A;
	B=p_B;
	contacts=memnew_arr(ContactData,MAX_CONTACTS);
	old_contacts=memnew_arr(ContactData,MAX_CONTACTS);
	
	separator=NULL;
	
	if (p_A->is_pure_collision() && p_B->is_pure_collision()) {
		/* can't have two pure collisions in a pair*/
		ERR_PRINT("BUG: Narrow Pair of Pure Collisions!");
	} else if (!p_A->is_pure_collision() && !p_B->is_pure_collision()) {
		/* none is pure collision, needs separator */
		
		separator=p_separator_manager->find_separator(*p_A->get_volume(),*p_B->get_volume());
		if (!separator) {
			
			ERR_PRINT("BUG: No Separator for NarrowPair ");
			
		}
	}

	
	/** Swapping them is important to retain the sorting order of the pairs, so they
	    are always separated in the same order, A shold always be less than B */
	    
	if (B<A) {
		
		SWAP(A,B); 
	}
	contact_count=0;
	old_contact_count=0;
	invert_contacts=false;
	
	
}


NarrowPair::~NarrowPair() {

}


