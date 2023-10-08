//
// C++ Interface: character_control_node
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef CHARACTER_CONTROL_NODE_H
#define CHARACTER_CONTROL_NODE_H

#include "scene/nodes/node.h"
#include "scene/nodes/node.h"
#include "scene/main/scene_main_loop.h"

class CharacterControlBodyNode : public Node {

	
	IAPI_TYPE( CharacterControlBodyNode, Node );
	IAPI_INSTANCE_FUNC( CharacterControlBodyNode );

private:	
	
	enum {
		
		MAX_RESULT_CONTACTS=64,
		MAX_RECOVER_ATTEMPTS=4
	};
	
	IRef<Volume> volume;

	void process(Uint32 p_type, float p_time);
	void event(EventType p_type);
	float friction;
	float bounce;
	float mass;
	
	Vector3 pos;
	
	void _iapi_set(const String& p_path, const Variant& p_value);
	void _iapi_get(const String& p_path,Variant& p_ret_value);
	void _iapi_get_property_list( List<PropertyInfo>* p_list );
	void _iapi_get_property_hint( const String& p_path, PropertyHint& p_hint );
	
	void _iapi_get_method_list( List<MethodInfo>* p_list );
	
	bool collision_test();
	
public:	
	
	void set_volume(IRef<Volume> p_volume);
	IRef<Volume> get_volume() const;
	
	void set_mass(float p_mass);
	float get_mass() const;
	
	void set_friction(float p_friction);
	float get_friction() const;
	void set_bounce(float p_bounce);
	float get_bounce() const;
	
	CharacterControlBodyNode();
	~CharacterControlBodyNode();
};


#endif
