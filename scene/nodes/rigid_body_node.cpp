//
// C++ Implementation: rigid_body_node
//
// Description:
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "rigid_body_node.h"
#include "scene/main/scene_main_loop.h"
#include "physics/physics.h"


void RigidBodyNode::_iapi_set(const String& p_path, const Variant& p_value) {

	if (p_path=="volume" && p_value.get_type()==Variant::IOBJ) {

		volume=p_value.get_IOBJ();
	}
}
void RigidBodyNode::_iapi_get(const String& p_path,Variant& p_ret_value) {

	if (p_path=="volume") {

		p_ret_value=IObjRef(volume);
	}

}
void RigidBodyNode::_iapi_get_property_list( List<PropertyInfo>* p_list ) {

	p_list->push_back( IAPI::PropertyInfo(Variant::IOBJ,"volume"));
}
void RigidBodyNode::_iapi_get_property_hint( const String& p_path, PropertyHint& p_hint ) {

	if (p_path=="volume") {

		p_hint=IAPI::PropertyHint( IAPI::PropertyHint::HINT_IOBJ_TYPE , "Volume" );
	}
}


void RigidBodyNode::_iapi_get_method_list( List<MethodInfo>* p_list ) {

	p_list->push_back(MethodInfo("show_collision"));

}


void RigidBodyNode::set_friction(float p_friction) {

	friction=p_friction;
	if (rb)
		rb->set_friction(p_friction);
}
float RigidBodyNode::get_friction() const {

	return friction;
}
void RigidBodyNode::set_bounce(float p_bounce) {

	bounce=p_bounce;
	if (rb)
		rb->set_bounce(p_bounce);
}
float RigidBodyNode::get_bounce() const {

	return bounce;
}


IRef<Volume> RigidBodyNode::get_volume() const {

	return volume;
}

void RigidBodyNode::set_mass(float p_mass) {

	if (rb)
		rb->set_mass(p_mass);
	mass=p_mass;
}
float RigidBodyNode::get_mass() const {

	return mass;
}


void RigidBodyNode::set_volume(IRef<Volume> p_volume) {


	if (rb)
		memdelete(rb);

	rb = memnew( RigidBody(p_volume,mass ) );
	rb->set_friction( friction );
	rb->set_bounce( bounce );

	volume=p_volume;

	SceneMainLoop::get_singleton()->get_physics()->register_body( rb );

	if (get_owner() && rb)
		rb->set_owner( get_owner() );

}

void RigidBodyNode::process(Uint32 p_type, float p_time) {

	if (!rb)
		return;

	set_global_matrix(rb->get_transform());

}

void RigidBodyNode::event(EventType p_type) {

	switch(p_type) {

		case EVENT_MATRIX_CHANGED: {

			if (!rb)
				break;
			rb->set_transform( get_global_matrix() );
		} break;
		default: {}
	}
}



RigidBodyNode::RigidBodyNode() {

	set_signal.connect( this, &RigidBodyNode::_iapi_set );
	get_signal.connect( this, &RigidBodyNode::_iapi_get );
	get_property_list_signal.connect( this, &RigidBodyNode::_iapi_get_property_list );
	get_property_hint_signal.connect( this, &RigidBodyNode::_iapi_get_property_hint );

	get_method_list_signal.connect( this, &RigidBodyNode::_iapi_get_method_list );

	event_signal.connect(this,&RigidBodyNode::event);
	process_signal.connect( this, &RigidBodyNode::process );
	friction=1.0;
	bounce=0.0;
	rb=NULL;

	mass=1;
	//set_matrix_mode( MATRIX_MODE_GLOBAL );
	set_process_flags( MODE_FLAG_GAME | CALLBACK_FLAG_POST);

}

RigidBodyNode::~RigidBodyNode() {

	if (rb)
		memdelete(rb);
}

