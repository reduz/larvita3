//
// C++ Interface: mesh
//
// Description:
//
//
// Author: Juan Linietsky <reduz@codenix.com>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef SCENEMESH_H
#define SCENEMESH_H

#include "scene/nodes/visual_node.h"
#include "render/spatial_indexer.h"
#include "render/renderer.h"
#include "scene/nodes/skeleton_node.h"

/**
	@author Juan Linietsky <reduz@codenix.com>
*/


class MeshNode : public VisualNode {

	IAPI_TYPE( MeshNode, VisualNode );
	IAPI_INSTANCE_FUNC( MeshNode );

	WeakRef<SkeletonNode> skeleton_node;
	
	void draw(const SpatialRenderData& p_data);
private:

	bool use_skeleton_node;

	IRef<Mesh> mesh;
	IRef<Skeleton> skeleton; //in case it uses a skeleton
		
	void event(EventType p_type);
	
	/** IAPI proxy */
	void _iapi_set(const String& p_path, const Variant& p_value);
	void _iapi_get(const String& p_path,Variant& p_ret_value);
	void _iapi_get_property_list( List<PropertyInfo>* p_list );
	void _iapi_get_property_hint( const String& p_path, PropertyHint& p_hint );
	
	void _iapi_get_method_list( List<MethodInfo>* p_list );
	void _iapi_call(const String& p_name, const List<Variant>& p_params,Variant& r_ret);
	
	void fetch_skeleton();
public: 

	
	Signal<> about_to_draw_signal;
	
	void set_mesh(IRef<Mesh> p_mesh);
	IRef<Mesh> get_mesh();
	
	void set_skeleton(IRef<Skeleton> p_skeleton);
	IRef<Skeleton> get_skeleton();
	
	void set_use_skeleton_node(bool p_enabled);

	virtual DVector<Face3> get_visible_geometry() const;
	
	void generate_tangents(bool p_noindex);
	
	MeshNode();
	~MeshNode();

};

#endif
