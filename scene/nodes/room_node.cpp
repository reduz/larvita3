//
// C++ Implementation: room_node
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "room_node.h"
#include "render/renderer.h"
#include "geometry_tools.h"

/** IAPI proxy */
void RoomNode::_iapi_set(const String& p_path, const Variant& p_value) {
	
}
void RoomNode::_iapi_get(const String& p_path,Variant& p_ret_value) {
	
}
void RoomNode::_iapi_get_property_list( List<PropertyInfo>* p_list ) {
	
}
void RoomNode::_iapi_get_property_hint( const String& p_path, PropertyHint& p_hint ) {
	
}

void RoomNode::find_visibility_data(IRef<Node> p_node,DVector<Face3>& p_faces) {
		
	if (p_node->is_type("VisualNode")) {
		
		IRef<VisualNode> vnode=p_node;
		
		ERR_FAIL_COND(vnode.is_null());
		
		DVector<Face3> addup=vnode->get_visible_geometry();
		if (addup.size()) {
		
			Matrix4 transform_to_local=p_node->get_global_matrix() * get_global_matrix().inverse();
		
			int addup_size=addup.size();
			int addup_from=p_faces.size();
			
			p_faces.resize( p_faces.size() + addup.size() );
			
			addup.read_lock();
			const Face3 * read_ptr=addup.read();
			
			p_faces.write_lock();
			Face3* write_ptr=p_faces.write();
			
			for (int i=0;i<addup_size;i++) {
				
				for (int j=0;j<3;j++) {
					Vector3 v=read_ptr[i].vertex[j];
					transform_to_local.transform(v);
					write_ptr[i+addup_from].vertex[j]=v;
				}
			}
				
			p_faces.write_unlock();
			addup.read_unlock();						
		}
		
	}
	
	for (int i=0;i<p_node->get_child_count();i++) {
		
		find_visibility_data(p_node->get_child_by_index(i),p_faces);
	}
}

void RoomNode::_iapi_get_method_list( List<MethodInfo>* p_list ) {
	
	p_list->push_back( MethodInfo("generate_visibility") );
}
void RoomNode::_iapi_call(const String& p_name, const List<Variant>& p_params,Variant& r_ret) {

	if (p_name=="generate_visibility") {
		
		DVector<Face3> faces;
		find_visibility_data(this,faces);
		
		faces = GeometryTools::wrap_geometry(faces);
		
		if (faces.size()) {
			
			AABB new_aabb;
			new_aabb.pos=faces[0].vertex[0];
			
			for (int i=0;i<faces.size();i++)
				for (int j=0;j<3;j++)
					new_aabb.expand_to(faces[i].vertex[j]);

			set_AABB(new_aabb);
		}
		
		visibility_debug=faces;
	}
}

void RoomNode::draw(const SpatialRenderData& p_data) {
	
	if (visibility_debug.size()==0)
		return;
	Renderer *r=Renderer::get_singleton();
	
	r->render_set_matrix( Renderer::MATRIX_WORLD, get_global_matrix() );
	
	r->render_primitive_set_material( r->get_default_material() );
	
	
	int visibility_debug_size=visibility_debug.size();
	visibility_debug.read_lock();
	const Face3* visibility_debug_ptr = visibility_debug.read();
	
	for (int i=0;i<visibility_debug_size;i++) {
		
/*		Vector3 normals[3];
		normals[0]=visibility_debug_ptr[i].get_plane().normal;
		normals[1]=normals[0];
		normals[2]=normals[0];
		Color color[3];
		color[0]=Color(80,80,180,150);
		color[1]=color[0];		
		color[2]=color[0];*/
		
		Vector3 line[2];
		line[0]=visibility_debug_ptr[i].vertex[0];
		line[1]=visibility_debug_ptr[i].vertex[1];		
		r->render_primitive(2,line,0,0,0,0);
		line[0]=visibility_debug_ptr[i].vertex[1];
		line[1]=visibility_debug_ptr[i].vertex[2];		
		r->render_primitive(2,line,0,0,0,0);
		line[0]=visibility_debug_ptr[i].vertex[2];
		line[1]=visibility_debug_ptr[i].vertex[0];		
		r->render_primitive(2,line,0,0,0,0);
	}
	
	visibility_debug.read_unlock();

}

RoomNode::RoomNode() {
	
		
	set_signal.connect( this, &RoomNode::_iapi_set );
	get_signal.connect( this, &RoomNode::_iapi_get );
	get_property_list_signal.connect( this, &RoomNode::_iapi_get_property_list );
	get_property_hint_signal.connect( this, &RoomNode::_iapi_get_property_hint );
	
	get_method_list_signal.connect( this, &RoomNode::_iapi_get_method_list );
	call_signal.connect( this, &RoomNode::_iapi_call );
	
//	event_signal.connect(this,&RoomNode::event);	
}


RoomNode::~RoomNode() {
	
}


