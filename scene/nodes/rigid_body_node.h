//
// C++ Interface: rigid_body_node
//
// Description:
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef RIGID_BODY_NODE_H
#define RIGID_BODY_NODE_H

/**
	@author Juan Linietsky <reduzio@gmail.com>
*/

#include "scene/nodes/node.h"
#include "physics/rigid_body.h"

class RigidBodyNode : public Node {

	IAPI_TYPE( RigidBodyNode, Node );
	IAPI_INSTANCE_FUNC( RigidBodyNode );

private:

	IRef<Volume> volume;
	RigidBody *rb;
	void process(Uint32 p_type, float p_time);
	void event(EventType p_type);
	float friction;
	float bounce;
	float mass;

	void _iapi_set(const String& p_path, const Variant& p_value);
	void _iapi_get(const String& p_path,Variant& p_ret_value);
	void _iapi_get_property_list( List<PropertyInfo>* p_list );
	void _iapi_get_property_hint( const String& p_path, PropertyHint& p_hint );

	void _iapi_get_method_list( List<MethodInfo>* p_list );

public:

	void set_volume(IRef<Volume> p_volume);
	IRef<Volume> get_volume() const;

	RigidBody* get_rigid_body() { return rb; };

	void set_mass(float p_mass);
	float get_mass() const;

	void set_friction(float p_friction);
	float get_friction() const;
	void set_bounce(float p_bounce);
	float get_bounce() const;

	RigidBodyNode();
	~RigidBodyNode();
};


#endif
