
#include "camera_node.h"
#include "scene/main/scene_main_loop.h"
#include "render/renderer.h"
#include "render/spatial_indexer.h"
/* basic handlers for iapi */
void CameraNode::_set(const String& p_path, const Variant& p_value) {

	if (p_path=="projection_matrix") {
	
		set_projection_matrix( p_value.get_matrix4() );
	} 
}

void CameraNode::_get(const String& p_path,Variant& p_ret_value) {

	if (p_path=="projection_matrix") {
	
		p_ret_value = get_projection_matrix();
	} 
}

void CameraNode::_get_property_list( List<PropertyInfo>* p_list ) {

	p_list->push_back( PropertyInfo( Variant::MATRIX4, "projection_matrix" ) );
}

void CameraNode::_call(const String& p_method, const List<Variant>& p_params,Variant& p_ret) {

	if (p_method=="set_as_current")
		set_as_current();
	
}
void CameraNode::_get_method_list(List<MethodInfo> *p_method_list) {

	MethodInfo minfo;
	
	minfo.name="set_as_current";
	p_method_list->push_back( minfo );
}


void CameraNode::render() {
	
	Renderer *r = Renderer::get_singleton();
	
	
	r->render_set_matrix( Renderer::MATRIX_PROJECTION, projection_matrix );
	r->render_set_matrix( Renderer::MATRIX_CAMERA, get_global_matrix() );
		
	r->get_spatial_indexer()->render();
		
}

void CameraNode::set_as_current() {
	
	SceneMainLoop::get_singleton()->set_current_camera( this );
		
}

Matrix4 CameraNode::get_projection_matrix() {

	
	return projection_matrix;
};

void CameraNode::set_projection_matrix(const Matrix4& p_mat) {

	projection_matrix = p_mat;

};


CameraNode::CameraNode() {

	set_signal.connect( this, &CameraNode::_set );
	get_signal.connect( this, &CameraNode::_get );
	get_property_list_signal.connect( this, &CameraNode::_get_property_list );
	call_signal.connect( this, &CameraNode::_call );
	get_method_list_signal.connect( this, &CameraNode::_get_method_list );	
	
	projection_matrix.set_projection(60.0,4.0/3.0,0.1, 100.0 );	

}
CameraNode::~CameraNode() {
		
		
}






