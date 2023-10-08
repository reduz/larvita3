//
// C++ Interface: broadphase_basic
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef BROADPHASE_BASIC_H
#define BROADPHASE_BASIC_H

#include "physics/broad_phase.h"
#include <map>

#include "collision/sphere_separator.h"
#include "collision/plane_separator.h"
#include "collision/convex_separator.h"
#include "collision/sphere_convex_separator.h"
#include "collision/convex_pill_separator.h"
#include "collision/concave_separator.h"
#include "collision/gjk_separator.h"
#include "octree.h"
/**
	@author Juan Linietsky <reduzio@gmail.com>
*/

class BroadPhaseBasic : public BroadPhase {
		
	static BroadPhase* create_basic();
	enum {
		
		MAX_NEIGHBOURS=128
	};
	
	struct BodyData {
		
		PhysicsBody* body;
		BodyData() { body=NULL; }
	};
	
	mutable Octree<BodyData> octree;
	
	struct PairData {
		
		int last_pass;
		NarrowPair *pair;
		List<NarrowPair*>::Iterator *I;
		PairData() { pair=NULL; I=NULL; };
	};
	
	typedef List<BodyData*> BodyList;
	BodyList body_list;
	
	typedef std::map<NarrowPairKey,PairData> PairMap;	
	PairMap pair_map;
	
	List<NarrowPair*> narrow_pair_list;
	
	VolumeSeparatorManager volume_separator;
	SphereSeparator sphere_separator;
	PlaneSeparator plane_separator;
	ConvexSeparator convex_separator; // reference implementation
	SphereConvexSeparator sphere_convex_separator; // reference 
	PillConvexSeparator pill_convex_separator; // reference implementation
	ConcaveSeparator concave_separator; // reference implementation
	GJK_Separator gjk_separator;
	
	int pass;
	void body_transform_changed( BodyData* p_body );
public:

	VolumeSeparatorManager* get_volume_separator_manager() { return &volume_separator; };

	virtual void register_body(PhysicsBody *p_body);
	virtual void remove_body(PhysicsBody *p_body);
	
	virtual List<NarrowPair*>* get_narrow_pair_list();
	
	virtual void pre_process(float p_time);
	virtual void process(float p_time);
	
	virtual PhysicsBody * intersect_segment(const Vector3& p_from,const Vector3& p_to,Vector3& r_clip,Vector3& r_normal,Uint32 p_flags) const;
	
	virtual void intersect_volume_and_fetch_contacts(IRef<Volume> p_volume,const Matrix4& p_transform, const Vector3& p_motion,ContactData * p_contacts,int *p_contact_count,int p_contact_max,Uint32 p_flags=PhysicsBody::FLAGS_ALL);
		
	static void set_as_default();		
	
	BroadPhaseBasic();
	~BroadPhaseBasic();

};

#endif
