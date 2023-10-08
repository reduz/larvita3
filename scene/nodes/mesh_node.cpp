//
// C++ Implementation: mesh
//
// Description:
//
//
// Author: Juan Linietsky <reduz@codenix.com>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "mesh_node.h"
#include "vector3.h"
#include "scene/main/scene_main_loop.h"
#include "render/renderer.h"
#include "scene/nodes/skeleton_node.h"

#ifndef _NO_TOOLS

#include "scene/nodes/collision_node.h"
#include "collision/convex_polygon_volume.h"
#include "collision/triangle_mesh_volume.h"
#include "geometry_tools.h"
#include "render/surface_tool.h"
#endif

void MeshNode::draw(const SpatialRenderData& p_data) {
	
	about_to_draw_signal.call();
	
	if (use_post_matrix) {
	
		Renderer::get_singleton()->render_set_matrix( Renderer::MATRIX_WORLD, get_global_matrix() * post_matrix );
		
	} else {
	
		Renderer::get_singleton()->render_set_matrix( Renderer::MATRIX_WORLD, get_global_matrix() );
	}
	
	
	
	IRef<Skeleton> sk;
	
	
	if (use_skeleton_node && !skeleton_node.is_null()) {
		
		sk=skeleton_node->get_skeleton();
	} else {
		
		sk=skeleton;
	}
	
	Renderer::get_singleton()->render_mesh( mesh, sk, extra_material );
};

void MeshNode::event(EventType p_type) {

	switch(p_type) {
				
		case EVENT_ENTER_SCENE: {
			
			if (use_skeleton_node)
				fetch_skeleton();
		} break;
		default :{};
	}

}


void MeshNode::set_mesh(IRef<Mesh> p_mesh) {

	mesh = p_mesh;
	if (mesh.is_null())
		set_AABB( AABB() );
	else
		set_AABB( mesh->get_AABB() );
};

void MeshNode::set_skeleton(IRef<Skeleton> p_skeleton) {

	skeleton=p_skeleton;
}


IRef<Mesh> MeshNode::get_mesh() {

	return mesh;
}
IRef<Skeleton> MeshNode::get_skeleton() {
	
	return skeleton;
}

void MeshNode::_iapi_set(const String& p_path, const Variant& p_value) {

	if (p_path=="mesh") {
	
		set_mesh(p_value.get_IOBJ());
	} else if (p_path=="skeleton") {
	
		set_skeleton(p_value.get_IOBJ());
	} else if (p_path == "use_skeleton_node") {
	
		set_use_skeleton_node(p_value);
	} 
}
void MeshNode::_iapi_get(const String& p_path,Variant& p_ret_value) {

	if (p_path=="mesh") {
	
		p_ret_value=IObjRef(get_mesh());
	} else if (p_path=="skeleton") {
	
		p_ret_value=IObjRef(get_skeleton());
	} else if (p_path == "use_skeleton_node") {
	
		p_ret_value = use_skeleton_node;	
	} 
	
}
void MeshNode::_iapi_get_property_list( List<PropertyInfo>* p_list ) {

	p_list->push_back( IAPI::PropertyInfo( Variant::IOBJ, "mesh" ) );
	p_list->push_back( IAPI::PropertyInfo( Variant::IOBJ, "skeleton", PropertyInfo::USAGE_EDITING ) ); // don't want it to persist
	p_list->push_back( IAPI::PropertyInfo( Variant::BOOL, "use_skeleton_node" ) );
	
}

void MeshNode::_iapi_get_property_hint( const String& p_path, PropertyHint& p_hint ) {

	if (p_path=="mesh") {
	
		p_hint=PropertyHint( PropertyHint::HINT_IOBJ_TYPE, "Mesh" );
	} else if (p_path=="skeleton") {
	
		p_hint=PropertyHint( PropertyHint::HINT_IOBJ_TYPE, "Skeleton" );
		
	} 	
}

void MeshNode::_iapi_get_method_list( List<MethodInfo>* p_list ) {

#ifndef _NO_TOOLS
	p_list->push_back(MethodInfo("make_collision_nodes"));
	p_list->push_back(MethodInfo("apply_global_matrix"));
	p_list->push_back(MethodInfo("force_make_trimesh_node"));
	p_list->push_back(MethodInfo("flip_faces"));
	p_list->push_back(MethodInfo("merge_surfaces"));
	p_list->push_back(MethodInfo("generate_tangents"));
	p_list->push_back(MethodInfo("generate_tangents_noindex"));
	IAPI::MethodInfo::ParamInfo pi(Variant::REAL, "radius", PropertyHint(PropertyHint::HINT_RANGE, "0,128,0.001", 1));
	MethodInfo m("create_sphere");
	m.parameters.push_back(pi);
	p_list->push_back(m);
#endif
}

static void apply_matrix_to_mesh(IRef<Mesh> p_mesh, Matrix4 p_mat) {

	for (int i=0; i<p_mesh->get_surface_count(); i++) {
	
		IRef<Surface> surf = p_mesh->get_surface(i);
		
		{
			Variant v = surf->get_array(Surface::ARRAY_VERTEX);
			for (int j=0; j<v.size()/3; j++) {
			
				Vector3 vert(v.array_get_real(j*3 + 0), v.array_get_real(j*3 + 1), v.array_get_real(j*3 + 2));
				p_mat.transform(vert);
				v.array_set_real(j*3 + 0, vert.x);
				v.array_set_real(j*3 + 1, vert.y);
				v.array_set_real(j*3 + 2, vert.z);
			};
			surf->set_array(Surface::ARRAY_VERTEX, v);
		};
		
		if (!(surf->get_format() & Surface::FORMAT_ARRAY_NORMAL)) {
		
			continue;
		};
		
		{
			Variant n = surf->get_array(Surface::ARRAY_NORMAL);
			for (int j=0; j<n.size()/3; j++) {
			
				Vector3 norm(n.array_get_real(j*3 + 0), n.array_get_real(j*3 + 1), n.array_get_real(j*3 + 2));
				p_mat.transform_no_translation(norm);
				norm.normalize();
				n.array_set_real(j*3 + 0, norm.x);
				n.array_set_real(j*3 + 1, norm.y);
				n.array_set_real(j*3 + 2, norm.z);
				
			};
			surf->set_array(Surface::ARRAY_NORMAL, n);
		};
	};
};

void MeshNode::_iapi_call(const String& p_name, const List<Variant>& p_params,Variant& r_ret) {

#ifndef _NO_TOOLS

	if (!mesh)
		return;

	if (p_name=="make_collision_nodes") { // make_physics too i guess
	
		printf("makeing nodes\n");
		DVector< Face3> faces = mesh->get_faces();
		
		printf("faces %i\n",faces.size());
		
		DVector< DVector<Face3> > obj_list = GeometryTools::separate_objects( faces );
		
		printf("objects %i\n",obj_list.size());
		for (int i=0;i<obj_list.size();i++) {
			
			DVector<Face3> obj = obj_list[i]; // go copy on write!
						
			Volume *volume=NULL;
			if (ConvexPolygonVolume::is_geometry_convex(obj)) {
				
				ConvexPolygonVolume *cpv = memnew( ConvexPolygonVolume( obj ) );
				
				if (!cpv->is_valid()) {
					
					memdelete(cpv);
				} else {
					
					volume=cpv;
				}
			} else {
				// will use trimesh
				TriangleMeshVolume *trimesh = memnew( TriangleMeshVolume( obj ) );
				//if (!trimesh->is_
				volume=trimesh;
				
			}
			
			if (!volume)
				continue;
			IRef<Volume> volumeref=IRef<Volume>( volume );
			
			String vname=get_name() + "_col_" + String::num(i);
			IRef<CollisionNode> cn = IRef<CollisionNode>( memnew( CollisionNode ) );
			cn->set_name( vname );
			cn->set_owner( get_owner() );
			cn->set_volume( volumeref );
			add_child( cn );		
			printf("added collision node\n");
		}
		
	}
	
	if (p_name == "force_make_trimesh_node") {

		printf("makeing nodes\n");
		DVector< Face3> faces = mesh->get_faces();
		
		printf("faces %i\n",faces.size());

		TriangleMeshVolume *trimesh = memnew( TriangleMeshVolume( faces ) );
		//if (!trimesh->is_
		Volume *volume=NULL;
		volume=trimesh;
		ERR_FAIL_COND(!volume);
		
		IRef<Volume> volumeref=IRef<Volume>( volume );
		
		String vname=get_name() + "_trimesh_col";
		IRef<CollisionNode> cn = IRef<CollisionNode>( memnew( CollisionNode ) );
		cn->set_name( vname );
		cn->set_owner( get_owner() );
		cn->set_volume( volumeref );
		add_child( cn );
		((IAPI*)cn.operator->())->set("display", true);
		printf("added collision node\n");
	};
	
	if (p_name == "apply_global_matrix") {
		
		ERR_FAIL_COND(!mesh);
		Matrix4 mat = get_global_matrix();
		apply_matrix_to_mesh(mesh, mat);
		set_global_matrix(Matrix4());
		set_AABB( mesh->get_AABB() );
	};
	
	if (p_name=="flip_faces") { // make_physics too i guess
	
		for (int i=0;i<mesh->get_surface_count();i++) {
		
			IRef<Surface> surf = mesh->get_surface(i);
			if (surf->get_primitive_type()!=Surface::PRIMITIVE_TRIANGLES)
				continue;
				
			if (surf->get_format()&Surface::FORMAT_ARRAY_INDEX) {
			
				Variant v=surf->get_array( Surface::ARRAY_INDEX);
				
				for (int j=0;j<(v.size()/3);j++) {
				
					int aux=v.array_get_int(j*3);
					v.array_set_int( j*3 , v.array_get_int( j*3 +1 ) );
					v.array_set_int( j*3+1 , aux );					
				}
				
				surf->set_array( Surface::ARRAY_INDEX, v);
			} else {
			
				Variant v=surf->get_array( Surface::ARRAY_VERTEX);
				
				for (int j=0;j<(v.size()/9);j++) {
				
				 	Vector3 aux;
					aux.x=v.array_get_real(j*9+0);
					aux.y=v.array_get_real(j*9+1);
					aux.z=v.array_get_real(j*9+2);
					
					v.array_set_real( j*9+0, v.array_get_real( j*9+3 ) );
					v.array_set_real( j*9+1, v.array_get_real( j*9+4 ) );
					v.array_set_real( j*9+2, v.array_get_real( j*9+5 ) );
					
					v.array_set_real( j*9+3, aux.x );
					v.array_set_real( j*9+4, aux.y );
					v.array_set_real( j*9+5, aux.z );
				}
			
				surf->set_array( Surface::ARRAY_VERTEX,v );
			}
		}
	}
	
	if (p_name=="merge_surfaces") { // make_physics too i guess
	
	
		int total_count=0;
		unsigned int last_format=0;
		IRef<Material> material;
		
		for (int i=0;i<mesh->get_surface_count();i++) {
		
			IRef<Surface> surf = mesh->get_surface(i);
			if (surf->get_primitive_type()!=Surface::PRIMITIVE_TRIANGLES)
				return; // make sure all is triangles
				
			if (i==0) {
				last_format=surf->get_format();
				material=surf->get_material();
			}
				
			if (last_format!=surf->get_format())
				return; // must all be same format
				
			if (surf->get_format()&Surface::FORMAT_ARRAY_INDEX) {
			
				total_count+=surf->get_index_array_len();
			} else {
			
				total_count+=surf->get_array_len();			
			}
			
			last_format=surf->get_format();
		}
		
		last_format&=~Surface::FORMAT_ARRAY_INDEX;
		
		
		
		SurfaceTool newsurf(Surface::PRIMITIVE_TRIANGLES,last_format);
	
		for (int i=0;i<mesh->get_surface_count();i++) {
		
			IRef<Surface> surf = mesh->get_surface(i);
								
			Variant indices;
			int len;
			if (surf->get_format()&Surface::FORMAT_ARRAY_INDEX) {
			
				indices=surf->get_array(Surface::ARRAY_INDEX);
				len=surf->get_index_array_len();
			} else {
			
				len=surf->get_array_len();
			}
			
			Variant vertices=surf->get_array( Surface::ARRAY_VERTEX );
			Variant normals;
			Variant colors;
			Variant uv0s;
			
			if (surf->get_format()&Surface::FORMAT_ARRAY_NORMAL) {
			
				normals=surf->get_array( Surface::ARRAY_NORMAL );
			}
			
			if (surf->get_format()&Surface::FORMAT_ARRAY_COLOR) {
			
				colors=surf->get_array( Surface::ARRAY_COLOR );
			}
			if (surf->get_format()&Surface::FORMAT_ARRAY_TEX_UV_0) {
			
				uv0s=surf->get_array( Surface::ARRAY_TEX_UV_0 );
			}
			
			for (int j=0;j<len;j++) {
			
				int vi=(indices.get_type()==Variant::NIL)?j:indices.array_get_int(j);
				
				if (uv0s.get_type()!=Variant::NIL)
					newsurf.add_uv(0,Vector3( uv0s.array_get_real(vi*2+0),uv0s.array_get_real(vi*2+1),0 ) );
				
				if (colors.get_type()!=Variant::NIL) {
					Color col;
					col.from_32( colors.array_get_int(vi));
					newsurf.add_color( col );
				}
				
				if (normals.get_type()!=Variant::NIL)
					newsurf.add_normal(Vector3( normals.array_get_real(vi*3+0),normals.array_get_real(vi*3+1),normals.array_get_real(vi*3+2) ) );
				
				newsurf.add_vertex(Vector3( vertices.array_get_real(vi*3+0),vertices.array_get_real(vi*3+1),vertices.array_get_real(vi*3+2) ) );				
			}
		}
		
		IRef<Surface> s = newsurf.commit();
		
		s->set_material(material);
		
		mesh->set_surface_count(0);
		mesh->add_surface(s);
	}

	if (p_name == "generate_tangents") {

		generate_tangents(true);
	}
	if (p_name == "generate_tangents_noindex") {

		generate_tangents(false);
	}
	
	if (p_name=="create_sphere") {
		
		mesh = Renderer::get_singleton()->make_sphere(8, 8, p_params[0]);
	};
	
#endif
}

void MeshNode::fetch_skeleton() {
	
	
	IRef<Node> parent = get_parent();
	
	while (!parent.is_null()) {
		if (parent->is_type("SkeletonNode")) {
			
			skeleton_node=parent;
			return;	
		}
		
		parent = parent->get_parent();
	}
	
	use_skeleton_node=false;
	property_changed_signal.call("use_skeleton_node");
	ERR_FAIL();
}


void MeshNode::set_use_skeleton_node(bool p_enabled) {
	
	use_skeleton_node=p_enabled;
	if (is_inside_scene_tree() && use_skeleton_node)
		fetch_skeleton();
}

DVector<Face3> MeshNode::get_visible_geometry() const {

	if (mesh.is_null())
		return DVector<Face3>();
		
	return mesh->get_faces();
}


void MeshNode::generate_tangents(bool p_noindex) {

	if (mesh.is_null())
		return;
		
	IRef<Mesh> new_mesh;
	new_mesh = Renderer::get_singleton()->create_mesh();
	
	for (int i=0;i<mesh->get_surface_count();i++) {
		
		SurfaceTool surftool( mesh->get_surface( i ) );
		
		surftool.generate_tangents();
		if (!p_noindex)
			surftool.index();
		IRef<Surface> surf = surftool.commit();
		printf("add surface?");
		new_mesh->add_surface(surf);
		printf("add surface end");
	}
	
	mesh=new_mesh;
}

MeshNode::MeshNode() {

	use_skeleton_node=false;
	
	set_signal.connect( this, &MeshNode::_iapi_set );
	get_signal.connect( this, &MeshNode::_iapi_get );
	get_property_list_signal.connect( this, &MeshNode::_iapi_get_property_list );
	get_property_hint_signal.connect( this, &MeshNode::_iapi_get_property_hint );
	
	get_method_list_signal.connect( this, &MeshNode::_iapi_get_method_list );
	call_signal.connect( this, &MeshNode::_iapi_call );
	
	event_signal.connect(this,&MeshNode::event);
}


MeshNode::~MeshNode() {
	

}

