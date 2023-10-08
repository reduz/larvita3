//
// C++ Interface: collision_node
//
// Description: 
//
//
// Author: Juan Linietsky <red@lunatea>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef COLLISION_NODE_H
#define COLLISION_NODE_H


#include "scene/nodes/node.h"
#include "physics/static_body.h"
#include "render/spatial_indexer.h"
/**
	@author Juan Linietsky <reduz@codenix.com>
*/

class CollisionNode : public Node {
	
	IAPI_TYPE( CollisionNode, Node );
	IAPI_INSTANCE_FUNC( CollisionNode );

private:	
	
	IRef<Volume> volume;
	StaticBody *sb;
	void event(EventType p_type);
	float friction;
	float bounce;
	Uint32 flags;
	bool collidable;
	bool teleports;
	Variant user_data;
	
	SpatialObject *visual_debug;
	
	void draw(const SpatialRenderData& p_data);
	
	void _iapi_set(const String& p_path, const Variant& p_value);
	void _iapi_get(const String& p_path,Variant& p_ret_value);
	void _iapi_get_property_list( List<PropertyInfo>* p_list );
	void _iapi_get_property_hint( const String& p_path, PropertyHint& p_hint );
	
	
public:	
	
	StaticBody* get_static_body() { return sb; };
	
	void set_volume(IRef<Volume> p_volume);
	IRef<Volume> get_volume() const;
	
	void set_friction(float p_friction);
	float get_friction() const;
	void set_bounce(float p_bounce);
	float get_bounce() const;
	
	CollisionNode();
	~CollisionNode();
};


#endif
