//
// C++ Interface: room_node
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef ROOM_NODE_H
#define ROOM_NODE_H
#include "scene/nodes/visual_node.h"

/**
	@author Juan Linietsky <reduzio@gmail.com>
*/
class RoomNode : public VisualNode {
	
	IAPI_TYPE( RoomNode, VisualNode );
	
	DVector<Face3> visibility_debug;
	
	void event(EventType p_type);
	
	/** IAPI proxy */
	void _iapi_set(const String& p_path, const Variant& p_value);
	void _iapi_get(const String& p_path,Variant& p_ret_value);
	void _iapi_get_property_list( List<PropertyInfo>* p_list );
	void _iapi_get_property_hint( const String& p_path, PropertyHint& p_hint );
	
	void _iapi_get_method_list( List<MethodInfo>* p_list );
	void _iapi_call(const String& p_name, const List<Variant>& p_params,Variant& r_ret);
	
	void find_visibility_data(IRef<Node> p_node,DVector<Face3>& p_faces);
	
protected:
	virtual void draw(const SpatialRenderData& p_data);			
public:
	RoomNode();	
	~RoomNode();

};

#endif
