//
// C++ Interface: visual_node
//
// Description: 
//
//
// Author: ,,, <red@lunatea>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef VISUAL_NODE_H
#define VISUAL_NODE_H


#include "scene/nodes/node.h"
#include "render/spatial_indexer.h"
#include "face3.h"

/**
	@author ,,, <red@lunatea>
*/
class VisualNode : public Node {

	IAPI_TYPE( VisualNode, Node );
	
	SpatialObject *so;
	AABB aabb;
	bool visible;
	
	void event(EventType p_type);
	
	void _draw(const SpatialRenderData& p_data);	

	/** IAPI proxy */
	void _iapi_set(const String& p_path, const Variant& p_value);
	void _iapi_get(const String& p_path,Variant& p_ret_value);
	void _iapi_get_property_list( List<PropertyInfo>* p_list );
	void _iapi_get_property_hint( const String& p_path, PropertyHint& p_hint );
		
	void _iapi_init();
		
	void update_spatial_object();
	
protected:
	
	void set_AABB(const AABB& p_AABB);
	virtual void draw(const SpatialRenderData& p_data)=0;

	SpatialObject* get_spatial_object();

	IRef<IAPI> extra_material;

	bool use_post_matrix;
	Matrix4 post_matrix;
	
public:

	AABB get_local_AABB() const;

	bool is_visible() const;
	void set_visible(bool p_visible);

	const Matrix4& get_post_matrix() const;
	void set_post_matrix(const Matrix4& p_post_matrix);
	
 	void set_extra_material(IRef<IAPI> p_mat);
	IRef<IAPI> get_extra_material() const;
	
	virtual DVector<Face3> get_visible_geometry() const;

	VisualNode();	
	~VisualNode();

};

#endif
