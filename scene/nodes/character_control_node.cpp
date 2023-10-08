//
// C++ Implementation: character_control_node
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "character_control_node.h"
#include "scene/main/scene_main_loop.h"



void CharacterControlBodyNode::_iapi_set(const String& p_path, const Variant& p_value) {

	if (p_path=="volume" && p_value.get_type()==Variant::IOBJ) {
		
		volume=p_value.get_IOBJ();
	}
}
void CharacterControlBodyNode::_iapi_get(const String& p_path,Variant& p_ret_value) {

	if (p_path=="volume") {
		
		p_ret_value=IObjRef(volume);
	}

}
void CharacterControlBodyNode::_iapi_get_property_list( List<PropertyInfo>* p_list ) {

	p_list->push_back( IAPI::PropertyInfo(Variant::IOBJ,"volume"));
}
void CharacterControlBodyNode::_iapi_get_property_hint( const String& p_path, PropertyHint& p_hint ) {

	if (p_path=="volume") {
	
		p_hint=IAPI::PropertyHint( IAPI::PropertyHint::HINT_IOBJ_TYPE , "Volume" );
	}
}


void CharacterControlBodyNode::_iapi_get_method_list( List<MethodInfo>* p_list ) {

	
}


void CharacterControlBodyNode::set_friction(float p_friction) {

	friction=p_friction;

}
float CharacterControlBodyNode::get_friction() const {

	return friction;
}
void CharacterControlBodyNode::set_bounce(float p_bounce) {

	bounce=p_bounce;
}
float CharacterControlBodyNode::get_bounce() const {

	return bounce;
}


IRef<Volume> CharacterControlBodyNode::get_volume() const {

	return volume;
}

void CharacterControlBodyNode::set_mass(float p_mass) {
	

	mass=p_mass;
}
float CharacterControlBodyNode::get_mass() const {
	
	return mass;
}


void CharacterControlBodyNode::set_volume(IRef<Volume> p_volume) {
		
	volume=p_volume;
	
}


/******************/


bool CharacterControlBodyNode::collision_test() {
	/*
	//set_global_matrix(rb->get_transform());
	BroadPhase::ContactData cresult[MAX_RESULT_CONTACTS];
	
	bool collision=false;
	
	Vector3 touch_normal;
	float max_depth2;
	
	
	for(int i=0;i<MAX_RECOVER_ATTEMPTS;i++) {
	
		
		SceneMainLoop::get_singleton()->get_physics()->intersect_volume_and_fetch_contacts( volume, get_global_matrix(), Vector3(),  cresult, &ccount, MAX_RESULT_CONTACTS );
				
		if (ccount==0)
			break;
		
		int ccount=0;
		max_depth2=0;
		touch_normal=Vector3();
		
		for (int j=0;j<ccount;j++) {
			
			if (cresult[j].dir.squared_length() > max_depth2 ) {
				
				max_depth2=cresult[j].dir.squared_length();
				touch_normal=cresult[j].dir;
			}
			
			pos+=cresult[j].dir;
		}
		
		collision=true;
	}
	
	if (!collision)
		return false; // no collision
	
	touch_normal.normalize();
	
	*/
	return true;
}

void CharacterControlBodyNode::process(Uint32 p_type, float p_time) {
				
	if (volume.is_null())
		return;

	
}

void CharacterControlBodyNode::event(EventType p_type) {

	switch(p_type) {
	
		case EVENT_MATRIX_CHANGED: {
		
		} break;
		default: {}
	}
}



CharacterControlBodyNode::CharacterControlBodyNode() {

	set_signal.connect( this, &CharacterControlBodyNode::_iapi_set );
	get_signal.connect( this, &CharacterControlBodyNode::_iapi_get );
	get_property_list_signal.connect( this, &CharacterControlBodyNode::_iapi_get_property_list );
	get_property_hint_signal.connect( this, &CharacterControlBodyNode::_iapi_get_property_hint );

	get_method_list_signal.connect( this, &CharacterControlBodyNode::_iapi_get_method_list );
	
	event_signal.connect(this,&CharacterControlBodyNode::event);

	friction=1.0;
	bounce=0.0;

	mass=1;
	set_matrix_mode( MATRIX_MODE_GLOBAL );
	set_process_flags( MODE_FLAG_GAME | CALLBACK_FLAG_POST);

}

CharacterControlBodyNode::~CharacterControlBodyNode() {


}

