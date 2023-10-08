//
// C++ Implementation: primitive_list_node
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "primitive_list_node.h"
#include "scene/main/scene_main_loop.h"

void PrimitiveListNode::set_material(IRef<Material> p_material) {
	
	material=p_material;
}
IRef<Material> PrimitiveListNode::get_material() const {
	
	return material;
}

void PrimitiveListNode::_iapi_set(const String& p_path, const Variant& p_value) {
	
	if (p_path=="material")
		material=p_value.get_IOBJ();
	
}
void PrimitiveListNode::_iapi_get(const String& p_path,Variant& p_ret_value) {
	
	if (p_path=="material")
		p_ret_value=IObjRef(material);
	
}
void PrimitiveListNode::_iapi_get_property_list( List<PropertyInfo>* p_list ) {
	
	p_list->push_back( PropertyInfo( Variant::IOBJ, "material" ));
	
}
void PrimitiveListNode::_iapi_get_property_hint( const String& p_path, PropertyHint& p_hint ) {
	
	if (p_path=="material")
		p_hint=PropertyHint( PropertyHint::HINT_IOBJ_TYPE, "Material" );
}


void PrimitiveListNode::add_primitive(const Primitive& p_primitive) {

	primitive_list.push_back(p_primitive);
	regenerate_static_volume();
}

void PrimitiveListNode::regenerate_static_volume() {

	List<Primitive>::Iterator *I=primitive_list.begin();
	AABB static_volume=AABB();
	
	while (I) {
	
		if (I==primitive_list.begin()) {
			
			static_volume.pos=I->get().vertices[0];
			static_volume.size=static_volume.pos;
		}
		
		for (int i=0;i<I->get().points;i++) {
			
			static_volume.expand_to( I->get().vertices[i] );
		}
		I=I->next();
	}
	
	set_AABB(static_volume);

}
void PrimitiveListNode::clear_primitive_list() {

	primitive_list.clear();
	regenerate_static_volume();
}


void PrimitiveListNode::draw(const SpatialRenderData& p_render_data) {

	Renderer *r=Renderer::get_singleton();
	r->render_set_matrix( Renderer::MATRIX_WORLD, get_global_matrix() );
	
	if (!material.is_null() ) {
	
		r->render_primitive_set_material( material );
	} else{
	
		r->render_primitive_set_material( IRef<Material>() );
	}
	
	List<Primitive>::Iterator *I=primitive_list.begin();
	
	while (I) {
	
		const Primitive& p=I->get();
		
		if (!p.texture.is_null())
			r->render_primitive_change_texture(p.texture);
			
		r->render_primitive(p.points, p.vertices, p.use_normals?p.normals:0, NULL, p.use_colors?p.colors:NULL, p.texture?p.uvs:0);
					
		I=I->next();
	}	
}

PrimitiveListNode::PrimitiveListNode() {
	
		
	set_signal.connect( this, &PrimitiveListNode::_iapi_set );
	get_signal.connect( this, &PrimitiveListNode::_iapi_get );
	get_property_list_signal.connect( this, &PrimitiveListNode::_iapi_get_property_list );
	get_property_hint_signal.connect( this, &PrimitiveListNode::_iapi_get_property_hint );
}

