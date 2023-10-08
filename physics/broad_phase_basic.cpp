//
// C++ Implementation: broadphase_basic
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "broad_phase_basic.h"


void BroadPhaseBasic::set_as_default() {
	
	create_func=create_basic;	
}

BroadPhase* BroadPhaseBasic::create_basic() {
	
	return memnew( BroadPhaseBasic );	
}

void BroadPhaseBasic::register_body(PhysicsBody *p_body) {
	
	BodyData * bd = octree.create( p_body->get_aabb() );
	bd->body=p_body;
	body_list.push_back(bd);
	
	p_body->transform_changed.connect( Method( Method1<BodyData*>( this, &BroadPhaseBasic::body_transform_changed ), bd ) );

	
}
void BroadPhaseBasic::remove_body(PhysicsBody *p_body) {
	
		

	for (PairMap::iterator P = pair_map.begin() ; P!=pair_map.end();) {
	
		PairMap::iterator N=P;
		N++;
		if (P->first.A==p_body || P->first.B==p_body) {
			
			memdelete( P->second.pair );
			narrow_pair_list.erase( P->second.I );
			pair_map.erase(P);
		}
		
		P=N;
	}	
	
	BodyList::Iterator *I=body_list.begin();
			
	for(;I;I=I->next()) {
	
		if (I->get()->body==p_body) {
			
			
			octree.erase( I->get() );
			body_list.erase(I);
			
			break;
		}
	}
	
	p_body->transform_changed.clear();
	
}

List<NarrowPair*>* BroadPhaseBasic::get_narrow_pair_list() {
	
	return &narrow_pair_list;
}

void BroadPhaseBasic::body_transform_changed( BodyData* p_body ) {
		
	octree.move( p_body, p_body->body->get_aabb() );	
}

void BroadPhaseBasic::pre_process(float p_time) {

	for (BodyList::Iterator *I=body_list.begin();I;I=I->next()) {
			
		PhysicsBody *io = I->get()->body;
		if (io->get_inv_mass()==0.0)
			continue;
		if (io->is_pure_collision())
			continue;
		/* TODO check that point isn't inside an object 
		if (io->is_colliding())
			continue; // already touching something
		*/
		
		Vector3 vel = io->get_linear_velocity();
		float vel_v=vel.length();
		if (vel_v<CMP_EPSILON)
			continue;
		
		Vector3 vel_n=vel/vel_v;
		
		float min,max;
		io->get_volume()->project_range( vel_n, io->get_transform(), min,max );
		
		// check if it's moving more than 1/3 it's width in the given normal (so it can go thru objects ) 
		float max_allowed=(max-min)/3.0;
		
		if ( vel_v*p_time < max_allowed ) {
			
			continue; // all fine
		}
		//printf("MAY GO THRU OBJECTS\n");
		
		Vector3 segment_from;
		int _c;
		io->get_volume()->get_support( vel_n, io->get_transform(), &segment_from, &_c, 1 );
				
		Vector3 segment_to = segment_from + vel * p_time;
		
		segment_from+=vel_n * 0.01; // avoid colliding wit itself
		
		Vector3 clip,n;
		
		PhysicsBody * res = intersect_segment(segment_from,segment_to,clip,n,PhysicsBody::FLAG_DYNAMIC|PhysicsBody::FLAG_STATIC);
		
		if (!res)
			continue;
		if (n.dot(vel_n) >=0 )
			continue;
		
		ERR_CONTINUE(res == io );
		
		
		clip+=vel_n * 0.001; // embed the object a little so collision will fix it
		
		Vector3 back_vel = (clip - segment_to) / p_time;
		
		io->apply_impulse( Vector3(), back_vel / io->get_inv_mass() );
		/*
		printf("SHOULD BE CLIPPED TO %f,%f,%f\n",clip.x,clip.y,clip.z);
		
		printf("vel %f,%f,%f\n",vel.x,vel.y,vel.z);
		printf("back_back_vel %f,%f,%f\n",back_vel.x,back_vel.y,back_vel.z);
		*/
		vel = io->get_linear_velocity();
		Vector3 check = segment_from + vel * p_time;
		
		//printf("CHECK: %f,%f,%f\n",check.x,check.y,check.z);
		
	}
	
}
void BroadPhaseBasic::process(float p_time) {

	BodyData *neighbours[MAX_NEIGHBOURS];
	int neighbour_count;
	
	pass++;
	
	for (BodyList::Iterator *I=body_list.begin();I;I=I->next()) {
			
		PhysicsBody *io = I->get()->body;
		if (io->get_inv_mass()==0.0)
			continue;
		if (io->is_pure_collision())
			continue;
		
		
		neighbour_count = octree.cull_AABB( io->get_aabb(), neighbours,MAX_NEIGHBOURS );
		
		for (int i=0;i<neighbour_count;i++) {
			
			
			PhysicsBody *jo = neighbours[i]->body;
			
			if (io==jo)
				continue;
				
			/* both are collision masters, no collision */
			/*
			if (io->is_pure_collision() && jo->is_pure_collision()) {
				continue;
			}
				
			if (io->get_inv_mass()==0.0 && jo->get_inv_mass()==0.0) {

				continue; //pointless
			}
			*/	
			if (!io->is_pure_collision() && !jo->is_pure_collision() && !volume_separator.find_separator(*io->get_volume(),*jo->get_volume())) {

				continue;
			}
				
		
				
			if (pair_map.find( NarrowPairKey(io,jo) )==pair_map.end()) {
		
				//printf("%p(%i) and %p(%i) intersect, add\n",io,io->get_volume()->get_volume_type(),jo,jo->get_volume()->get_volume_type());
				PairData pd;
				pd.pair = memnew( NarrowPair(io,jo,&volume_separator) );
				narrow_pair_list.push_back(pd.pair);
				pd.I=narrow_pair_list.back();
				pair_map[NarrowPairKey(io,jo)]=pd;
				
				narrow_pair_created_signal.call(pd.pair);
				//printf("registering %s vs %s\n",io->get_volume()->get_type().ascii().get_data(), jo->get_volume()->get_type().ascii().get_data());
				//pair_map[ NarrowPairKey(io,jo)].collision_notify.set( this, &BroadPhase::collision_notified_callback );
			} 
			// valid pair
			pair_map[NarrowPairKey(io,jo)].last_pass=pass;
		}
	}		
	
	/* erase no longer useful pairs */
			
	
	PairMap::iterator pI = pair_map.begin();
	
	while(pI!=pair_map.end()) {
		
		PairMap::iterator pN=pI;
		pN++;
		
		if ( pI->second.last_pass!=pass && pI->first.A->get_aabb().intersects( pI->first.B->get_aabb() ) ) {
			
			narrow_pair_list.erase( pI->second.I );
			pair_map.erase( pI );
		}
		
		pI=pN;
	}
//	printf("%i collisions to resolve out of %i objects\n", pair_map.size(), body_list.size() );

}

PhysicsBody * BroadPhaseBasic::intersect_segment(const Vector3& p_from,const Vector3& p_to,Vector3& r_clip,Vector3& r_normal,Uint32 p_flags) const {
	
	
	BodyData *neighbours[MAX_NEIGHBOURS];
	int neighbour_count;
		
	
	neighbour_count = octree.cull_segment( p_from, p_to, neighbours, MAX_NEIGHBOURS );
	
	r_clip=p_to;
	PhysicsBody* result=NULL;
	
	for (int i=0;i<neighbour_count;i++) {
			
		
		Vector3 inters,normal;
					
		PhysicsBody *b = neighbours[i]->body;
		
		if (b->is_collidable() && b->get_flags()&p_flags && b->intersect_segment(p_from,p_to,inters,normal)) {
			
			if (p_from.squared_distance_to(inters) < p_from.squared_distance_to(r_clip)) {
				
				r_clip=inters;
				r_normal=normal;
				result=b;			
			}
		}		
	}	

	
	return result;	
}


class SimpleContactHandler : public VolumeContactHandler {
	
	PhysicsBody* current;
	BroadPhase::ContactData *contacts;
	int *contact_count;
	int contact_max;
public:	
	
	virtual void add_contact(const VolumeContact& p_contact,bool p_collided) {
		
		if (!p_collided)
			return;
		if (*contact_count>=contact_max)
			return;
		
		contacts[*contact_count].against=current;
		if (!current->is_pure_collision())
			contacts[*contact_count].dir=p_contact.A-p_contact.B;
		else
			contacts[*contact_count].dir=p_contact.B-p_contact.A;
			
		(*contact_count)++;
	}
	
	void set_current( PhysicsBody* p_current ) {
		
		current=p_current;
	}
	
	SimpleContactHandler(BroadPhase::ContactData *p_contacts,int  *p_contact_count,int p_contact_max) {
		
		contacts=p_contacts;
		contact_count=p_contact_count;
		contact_max=p_contact_max;
		*contact_count=0;
		current=NULL;
		
	}
};

void BroadPhaseBasic::intersect_volume_and_fetch_contacts(IRef<Volume> p_volume,const Matrix4& p_transform,const Vector3& p_motion,ContactData * p_contacts,int *p_contact_count,int p_contact_max,Uint32 p_flags) {
	
	SimpleContactHandler sch(p_contacts,p_contact_count,p_contact_max);
	
	const BodyList::Iterator *I=body_list.begin();
	
	AABB src_aabb = p_volume->compute_AABB( p_transform );
	for (;I;I=I->next()) {
		
		Vector3 inters,normal;
					
		PhysicsBody *b = I->get()->body;
		
		if (b->get_volume()==p_volume) {
			continue;
		}
		if (!b->is_collidable() || !(b->get_flags()&p_flags)) {
			continue;
		}
			
		if (!src_aabb.intersects( b->get_aabb() )) {
			continue;
		}
			

		sch.set_current(b);
		
		if (b->is_pure_collision()) {
			
			float frc=1.0; float bnc=0.0f;
			b->collide_volume(*p_volume.ptr(),p_transform,&sch,frc,bnc);
		} else {
			
			volume_separator.separate(*p_volume.ptr(),p_transform,*b->get_volume().ptr(),b->get_transform(),&sch);
		}
	}
	
}


BroadPhaseBasic::BroadPhaseBasic() {
	
	volume_separator.register_volume_separator(&plane_separator);
	volume_separator.register_volume_separator(&pill_convex_separator);
	volume_separator.register_volume_separator(&convex_separator);
	volume_separator.register_volume_separator(&sphere_separator);
	volume_separator.register_volume_separator(&sphere_convex_separator);
	volume_separator.register_volume_separator(&gjk_separator);
	volume_separator.register_volume_separator(&concave_separator);
	pass=0;
}


BroadPhaseBasic::~BroadPhaseBasic()
{
}


