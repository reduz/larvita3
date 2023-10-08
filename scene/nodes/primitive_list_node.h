//
// C++ Interface: primitive_list_node
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef PRIMITIVE_LIST_NODE_H
#define PRIMITIVE_LIST_NODE_H

#include "render/renderer.h"
#include "scene/nodes/visual_node.h"

/**
This node is useful for drawing a list of primitives. It is used mainly for debugging spatial stuff.

	@author Juan Linietsky <reduzio@gmail.com>
*/
class PrimitiveListNode : public VisualNode {

	IAPI_TYPE( PrimitiveListNode, VisualNode );
	IAPI_INSTANCE_FUNC( PrimitiveListNode );
public:
	
	struct Primitive {
	
		int points;
		Vector3 vertices[4];
		bool use_colors;
		Color colors[4];
		/* normals not necesary */
		bool use_normals;
		Vector3 normals[4];
		float uvs[8];
		IRef<Texture> texture;
		
		Primitive() {
			use_normals=false;
			use_colors=false;
			points=0;
		}
	};
	
	
private:	
	
	List<Primitive> primitive_list;

	void regenerate_static_volume();
	IRef<Material> material;
	
	virtual void draw(const SpatialRenderData& p_render_data); ///< Visual nodes can render,
	
	/** IAPI proxy */
	void _iapi_set(const String& p_path, const Variant& p_value);
	void _iapi_get(const String& p_path,Variant& p_ret_value);
	void _iapi_get_property_list( List<PropertyInfo>* p_list );
	void _iapi_get_property_hint( const String& p_path, PropertyHint& p_hint );
	
public:

	void set_material(IRef<Material> p_material);
	IRef<Material> get_material() const;
	void add_primitive(const Primitive& p_primitive);
	void clear_primitive_list();

	PrimitiveListNode();
	~PrimitiveListNode() {};

};

#endif
