//
// C++ Implementation: visual_node
//
// Description: 
//
//
// Author: ,,, <red@lunatea>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "visual_node.h"
#include "render/renderer.h"

/** IAPI proxy */

void VisualNode::set_AABB(const AABB& p_AABB) {

	aabb=p_AABB;
	update_spatial_object();
}

void VisualNode::event(EventType p_type) {

	switch(p_type) {
		
		case EVENT_MATRIX_CHANGED: 
		case EVENT_ENTER_SCENE: 
			//update_spatial_object();
		case EVENT_EXIT_SCENE: {
			update_spatial_object();
		}; break;
		default: {}
	}

}

const Matrix4& VisualNode::get_post_matrix() const {

	return post_matrix;
};

void VisualNode::set_post_matrix(const Matrix4& p_post_matrix) {
	
	
	use_post_matrix = !p_post_matrix.is_identity();
	
	post_matrix=p_post_matrix;
};


bool VisualNode::is_visible() const {

	return visible;
}
void VisualNode::set_visible(bool p_visible) {

	visible=p_visible;
	update_spatial_object();
}

void VisualNode::set_extra_material(IRef<IAPI> p_mat) {

	extra_material = p_mat;
};

IRef<IAPI> VisualNode::get_extra_material() const {
	
	return extra_material;
};


void VisualNode::_iapi_set(const String& p_path, const Variant& p_value) {

	if (p_path=="visible")
		set_visible(p_value);
	
	if (p_path == "extra_material")
		set_extra_material(p_value.get_IOBJ());

}

void VisualNode::_iapi_get(const String& p_path,Variant& p_ret_value) {

	if (p_path=="visible")
		p_ret_value=is_visible();
	
	if (p_path == "extra_material")
		p_ret_value = IObjRef(extra_material);

}

void VisualNode::_iapi_get_property_list( List<PropertyInfo>* p_list ) {

	p_list->push_back( IAPI::PropertyInfo( Variant::BOOL, "visible" ) );
	p_list->push_back( IAPI::PropertyInfo( Variant::IOBJ, "extra_material" ) );
}

void VisualNode::_iapi_get_property_hint( const String& p_path, PropertyHint& p_hint ) {

	if (p_path=="extra_material")
		p_hint = PropertyHint( PropertyHint::HINT_IOBJ_TYPE, "Material" );
}

void VisualNode::update_spatial_object() {
	
	if (!visible || !is_inside_scene_tree()) {
	
		if (so) 
			so->set_AABB( AABB() );
	} else {
		AABB taabb=aabb; // transformed aabb
		get_global_matrix().transform_aabb( taabb );
		so->set_AABB( taabb );
		so->matrix = get_global_matrix();		
		//printf("update spatial object aabb %s\n",Variant(aabb).get_string().ascii().get_data());
	}
}

void VisualNode::_draw(const SpatialRenderData& p_data) {

	draw(p_data);
}

DVector<Face3> VisualNode::get_visible_geometry() const {

	return DVector<Face3>(); 
}

AABB VisualNode::get_local_AABB() const {

	return aabb;
}

SpatialObject* VisualNode::get_spatial_object() {
	return so;
};

void VisualNode::_iapi_init() {

	so = Renderer::get_singleton()->get_spatial_indexer()->create_spatial_object();
	ERR_FAIL_COND(!so);
	so->set_draw_callback( Method1<const SpatialRenderData&>( this, &VisualNode::draw ) );
	so->set_owner(this);


}

VisualNode::VisualNode() {

	visible=true;
	
	set_signal.connect( this, &VisualNode::_iapi_set );
	get_signal.connect( this, &VisualNode::_iapi_get );
	get_property_list_signal.connect( this, &VisualNode::_iapi_get_property_list );
	get_property_hint_signal.connect( this, &VisualNode::_iapi_get_property_hint );
	init_signal.connect( this, &VisualNode::_iapi_init );
	
	event_signal.connect(this,&VisualNode::event);
	
	so=NULL;
		
	use_post_matrix=false;
}


VisualNode::~VisualNode() {

	if (so)
		Renderer::get_singleton()->get_spatial_indexer()->remove_spatial_object(so);

}


