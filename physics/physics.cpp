//
// C++ Implementation: physics
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "physics.h"

PhysicsWorldSettings &Physics::get_world_settings() {
	
	return world_settings;	
}


void Physics::register_body(PhysicsBody *p_body) {
	
	ERR_FAIL_COND( p_body->_ps ); // already registered
		
	body_list.push_back( p_body );
	broad_phase->register_body(p_body);
	p_body->_ps=this;
	
	
}

void Physics::remove_body(PhysicsBody *p_body) {
	
	ERR_FAIL_COND(!p_body->_ps);
	body_list.erase(p_body);
	p_body->_ps=NULL;
	
	broad_phase->remove_body(p_body);
	
}

void Physics::narrow_pair_created(NarrowPair* p_np) {
	
	p_np->collision_notify.set(this, &Physics::narrow_pair_collision);
};

void Physics::narrow_pair_collision(PhysicsBody* p_b1, PhysicsBody* p_b2, const Vector3& p_normal) {
	
	Collision col;
	col.body1 = p_b1;
	col.body2 = p_b2;
	col.normal = p_normal;
	
	collision_list.push_back(col);
};

void Physics::process(float p_time)  {
			
						
	List<NarrowPair*>* pair_list=broad_phase->get_narrow_pair_list();

	for (List<NarrowPair*>::Iterator *P = pair_list->begin() ; P ; P=P->next()) {
	
		P->get()->process( p_time );
	}
	
	BodyList::Iterator *I=body_list.begin();
		
	for (;I;I=I->next()) {
				
		if (!I->get()->can_process())
			continue;
		I->get()->integrate_forces( p_time );
	}
	
	broad_phase->pre_process(p_time);
	
//	for (;I;I=I->next()) {
//		
//		I->get()->_colliding=false;
//	}
	

	for (int i=0;i<iterations;i++) {
		
		for (List<NarrowPair*>::Iterator *P = pair_list->begin() ; P ; P=P->next()) {
	
			P->get()->solve();
		}
	}

	I=body_list.begin();

	for (;I;I=I->next()) {
		if (!I->get()->can_process())
			continue;
		I->get()->integrate_velocities( p_time );
	}

	List<Collision>::Iterator* J = collision_list.begin();
	for (;J;J=J->next()) {
		
		IRef<IAPI> body1;
		body1 = J->get().body1->get_owner();
		IRef<IAPI> body2;
		body2 = J->get().body2->get_owner();
		
		if (body1) {

			List<Variant> params;
			params.push_back(-J->get().normal);
			params.push_back(IObjRef(body2));
			params.push_back(J->get().body2->get_flags());
			params.push_back(J->get().body1->get_user_data());
			params.push_back(J->get().body2->get_user_data());
			body1->call("collision_notify", params);
		};
		if (body2) {

			List<Variant> params;
			params.push_back(J->get().normal);
			params.push_back(IObjRef(body1));
			params.push_back(J->get().body1->get_flags());
			params.push_back(J->get().body2->get_user_data());
			params.push_back(J->get().body1->get_user_data());
			body2->call("collision_notify", params);
		};
	};
	collision_list.clear();
	
	broad_phase->process(p_time);	
	
}

PhysicsBody * Physics::intersect_segment(const Vector3& p_from,const Vector3& p_to,Vector3& r_clip,Vector3& r_normal,Uint32 p_flags) const  {
	
	return broad_phase->intersect_segment(p_from,p_to,r_clip,r_normal,p_flags);
}

void Physics::intersect_volume_and_fetch_contacts(IRef<Volume> p_volume,const Matrix4& p_transform,const Vector3& p_motion,BroadPhase::ContactData * p_contacts,int *p_contact_count,int p_contact_max,Uint32 p_flags) {
	
	broad_phase->intersect_volume_and_fetch_contacts(p_volume,p_transform,p_motion,p_contacts,p_contact_count,p_contact_max,p_flags);
}

Physics::Physics(int p_iterations) {
	
	broad_phase=BroadPhase::create();
	ERR_FAIL_COND(!broad_phase);
	iterations=p_iterations;
	
	broad_phase->narrow_pair_created_signal.connect(this, &Physics::narrow_pair_created);	
}


Physics::~Physics() {
	
	for(BodyList::Iterator *I=body_list.begin();I;I=I->next()) {
	
		broad_phase->remove_body(I->get());
		I->get()->_ps=NULL;
	}
	
	if (broad_phase)
		memdelete(broad_phase);	
}


