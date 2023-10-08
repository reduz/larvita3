
//
// C++ Implementation: collision_node
//
// Description: 
//
//
// Author: Juan Linietsky <red@lunatea>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "collision_node.h"
#include "scene/main/scene_main_loop.h"
#include "physics/physics.h"
#include "collision/convex_polygon_volume.h"
#include "render/renderer.h"

void CollisionNode::draw(const SpatialRenderData& p_data) {
	
	Renderer::get_singleton()->render_set_matrix( Renderer::MATRIX_WORLD, get_global_matrix() );
	
	if (volume.is_null())
		return;
	
	if (volume->get_type()=="ConvexPolygonVolume") {
		
		IRef<ConvexPolygonVolume> cpn = volume;
		ConvexPolygonVolume *c = cpn.ptr();
		
		Renderer::get_singleton()->render_primitive_set_material( Renderer::get_singleton()->get_indicator_material() );
		
		for (int i=0;i<c->get_face_count();i++) {
			
			int ic=c->get_face_index_count(i);
			if (ic<3)
				continue;
			Vector3 va[3];
			Vector3 normal=c->get_face_plane(i).normal;
			Vector3 na[3]={ normal,normal,normal };
#define _VOL_COL Color(60,100,150,100)
			Color ca[3]={ _VOL_COL,_VOL_COL,_VOL_COL};
			va[0]=c->get_vertex( c->get_face_index(i,0 ) );
			for (int j=1;j<(ic-1);j++) {
				
				va[1]=c->get_vertex( c->get_face_index(i,j ) );
				va[2]=c->get_vertex( c->get_face_index(i,j+1 ) );
				
				Renderer::get_singleton()->render_primitive(3,va,na,0,ca);
			}
		}
	}
	if (volume->get_type()=="TriangleMeshVolume") {
		
	};
}


void CollisionNode::_iapi_set(const String& p_path, const Variant& p_value) {

	if (p_path=="volume" && p_value.get_type()==Variant::IOBJ) {
		
		set_volume(p_value.get_IOBJ());

	} else if (p_path == "user_data") {
		
		user_data = p_value;
		if (sb)
			sb->set_user_data(user_data);
		
	} if (p_path=="flags" && (p_value.get_type()==Variant::INT || p_value.get_type() == Variant::REAL)) {
		flags=p_value;
		if (sb) {
			sb->set_extra_flags( flags );
		}
	} if (p_path=="friction" && p_value.get_type()==Variant::REAL) {		
		friction=p_value;
		if (sb)
			sb->set_friction(friction);
	} if (p_path=="collidable" && p_value.get_type()==Variant::BOOL) {		
		collidable=p_value.get_bool();
		if (sb)
			sb->set_collidable(collidable);
	} if (p_path=="teleports" && p_value.get_type()==Variant::BOOL) {		
		teleports=p_value.get_bool();
		
	} if (p_path=="display") {
		
		bool display=p_value.get_bool();
		
		if (visual_debug) {
			
			if (!display) {
				
				//hide (delete)
				Renderer::get_singleton()->get_spatial_indexer()->remove_spatial_object(visual_debug);
				visual_debug=NULL;
			}			
		} else {
			
			if (display) {
				//show (create
				visual_debug=Renderer::get_singleton()->get_spatial_indexer()->create_spatial_object();
				
				ERR_FAIL_COND(!visual_debug);
				
				visual_debug->set_draw_callback( Method1<const SpatialRenderData&>( this, &CollisionNode::draw ) );
				visual_debug->set_owner( this );
						
			}
		}
	}

}
void CollisionNode::_iapi_get(const String& p_path,Variant& p_ret_value) {

	if (p_path=="volume") {
		
		p_ret_value=IObjRef(volume);
	} if (p_path == "user_data") {
		
		p_ret_value = user_data;
		
	} if (p_path=="flags") {
		p_ret_value=flags;
	} if (p_path=="friction") {
		
		p_ret_value=friction;
	} if (p_path=="collidable") {
		
		p_ret_value=collidable;
	} if (p_path=="teleports") {
		
		p_ret_value=teleports;
	} if (p_path=="display") {
		
		p_ret_value = visual_debug?true:false;
	}
}

void CollisionNode::_iapi_get_property_list( List<PropertyInfo>* p_list ) {

	p_list->push_back( IAPI::PropertyInfo(Variant::IOBJ,"volume"));
	p_list->push_back( IAPI::PropertyInfo(Variant::STRING,"user_data"));
	p_list->push_back( IAPI::PropertyInfo(Variant::INT,"flags"));
	p_list->push_back( IAPI::PropertyInfo(Variant::REAL,"friction"));
	p_list->push_back( IAPI::PropertyInfo(Variant::BOOL,"collidable"));
	p_list->push_back( IAPI::PropertyInfo(Variant::BOOL,"teleports"));
	p_list->push_back( IAPI::PropertyInfo(Variant::BOOL,"display", IAPI::PropertyInfo::USAGE_EDITING));
}
void CollisionNode::_iapi_get_property_hint( const String& p_path, PropertyHint& p_hint ) {

	if (p_path=="volume") {
	
		p_hint=IAPI::PropertyHint( IAPI::PropertyHint::HINT_IOBJ_TYPE , "Volume" );
	}
	if (p_path=="flags") {
		p_hint = IAPI::PropertyHint( IAPI::PropertyHint::HINT_FLAGS, "Static,,Dynamic,Influence" );
	};
}


void CollisionNode::set_friction(float p_friction) {

	friction=p_friction;
	if (sb)
		sb->set_friction(p_friction);	
}
float CollisionNode::get_friction() const {

	return friction;
}
void CollisionNode::set_bounce(float p_bounce) {

	bounce=p_bounce;
	if (sb)
		sb->set_bounce(p_bounce);	
}
float CollisionNode::get_bounce() const {

	return bounce;
}


IRef<Volume> CollisionNode::get_volume() const {

	return volume;
}



void CollisionNode::set_volume(IRef<Volume> p_volume) {

	if (sb) {
		memdelete(sb);
		sb=NULL;
	}
		
	volume=p_volume;
	if (volume.is_null()) 
		return;
	sb = memnew( StaticBody(p_volume ) );
	sb->set_friction( friction );
	sb->set_bounce( bounce );
	sb->set_collidable( collidable );
	sb->set_user_data( user_data );
		
	if (is_inside_scene_tree() && SceneMainLoop::get_singleton())
		SceneMainLoop::get_singleton()->get_physics()->register_body( sb );
	
	if (get_owner() && sb)
		sb->set_owner( get_owner() );			
		
	if (visual_debug)
		visual_debug->set_AABB( volume->compute_AABB( get_global_matrix() ) );
	sb->set_extra_flags(flags);
	
}

void CollisionNode::event(EventType p_type) {

	switch(p_type) {
	
		case EVENT_MATRIX_CHANGED: {
		
			if (sb) {
				if (teleports)
					sb->teleport( get_global_matrix() );
				else
					sb->set_transform( get_global_matrix() );
					
				if (visual_debug)
					visual_debug->set_AABB( volume->compute_AABB( get_global_matrix() ) );
			}
			

		} break;
		case EVENT_ENTER_SCENE: {
		
		
			if (sb) {
				SceneMainLoop::get_singleton()->get_physics()->register_body( sb );
			}
			
			if (visual_debug) {
				visual_debug->set_AABB( volume->compute_AABB( get_global_matrix() ) );
			}
		
		} break;
		case EVENT_EXIT_SCENE: {
		
			if (sb) {
				SceneMainLoop::get_singleton()->get_physics()->remove_body( sb );
			}
			
			if (visual_debug) {
				visual_debug->set_AABB( AABB() );
			}
		
		
		} break;
		default: {}
	}
}



CollisionNode::CollisionNode() {

	set_signal.connect( this, &CollisionNode::_iapi_set );
	get_signal.connect( this, &CollisionNode::_iapi_get );
	get_property_list_signal.connect( this, &CollisionNode::_iapi_get_property_list );
	get_property_hint_signal.connect( this, &CollisionNode::_iapi_get_property_hint );

	event_signal.connect(this,&CollisionNode::event);

	teleports = false;
	collidable = true;	
	friction=1.0;
	bounce=0.0;
	sb=NULL;
	flags=PhysicsBody::FLAG_STATIC;
	visual_debug=NULL;
}

CollisionNode::~CollisionNode() {

	if (sb) 
		memdelete(sb);
	if (visual_debug)
		Renderer::get_singleton()->get_spatial_indexer()->remove_spatial_object(visual_debug);

}


