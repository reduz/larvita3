#include "skeleton_node.h"

#include "error_macros.h"

#include "scene/main/scene_main_loop.h"
#include "render/renderer.h"


void SkeletonNode::update_skeleton() {
	
	if (skeleton.is_null()) {
		
		skeleton=Renderer::get_singleton()->create_skeleton();
	}
	
	if (skeleton->get_bone_count()!=bones.size()) {
		
		if (bones.size())
			skeleton->create( bones.size() );
	}
}

void SkeletonNode::process_callback(Uint32 p_mode, float p_time) {
	process();
}

void SkeletonNode::set_bone_parent(int p_bone, int p_parent) {

	ERR_FAIL_INDEX(p_bone, bones.size());
	if (p_parent != -1) {
		ERR_FAIL_INDEX(p_parent, bones.size());
	};
	ERR_FAIL_COND(p_parent >= p_bone);
	
	
	bones[p_bone].parent = p_parent;
	
	if (p_parent != -1) {
		bones[p_bone].rest_global_inverse = (bones[p_parent].rest_global_inverse.inverse() * bones[p_bone].rest).inverse();
	} else {
		bones[p_bone].rest_global_inverse = bones[p_bone].rest.inverse();
	};

	skeleton_dirty=true;
}

int SkeletonNode::add_bone(int p_idx, String p_name, Matrix4 p_matrix, int p_parent) {

	ERR_FAIL_COND_V(get_bone_id(p_name) != -1, -1);

	ERR_FAIL_COND_V(bones.size() == 0 && p_parent != -1, -1);
	if (p_parent != -1) {
		ERR_FAIL_INDEX_V(p_parent, bones.size(), -1);
	};

	Bone bone;
	bone.rest = p_matrix;
	bone.parent = p_parent;
	bone.on=true;
	bone.name = p_name;

	
	if (p_parent != -1) {
		bone.rest_global_inverse = (bones[p_parent].rest_global_inverse.inverse() * p_matrix).inverse();
	} else {
		bone.rest_global_inverse = p_matrix.inverse();
	};
	
	bones[p_idx] = bone;
	

	skeleton_dirty=true;
	
	return p_idx;
}

int SkeletonNode::get_bone_parent(int p_bone) {

	ERR_FAIL_INDEX_V(p_bone, bones.size(), -1);

	int parent=bones[p_bone].parent;
	
	return parent;
}


void SkeletonNode::bind_child_to_bone(String p_child, String p_bone) {

	//	ERR_FAIL_COND(p_child.find("/") < 0);

	IRef<Node> node = get_child(p_child);
	ERR_FAIL_COND(!node);

	int bone = get_bone_id(p_bone);
	ERR_FAIL_COND(bone == -1);

	bind_child_to_bone(node, bone);
	skeleton_dirty=true;
	
};

void SkeletonNode::bind_child_to_bone(IRef<Node> p_child, int p_bone) {

	ERR_FAIL_COND(!has_child(p_child->get_name()));
	ERR_FAIL_INDEX(p_bone, bones.size());

	BoundChild bound;
	bound.child = p_child;
	bound.bone = p_bone;

	bound_childs.push_back(bound);
	skeleton_dirty=true;
	
};

int SkeletonNode::get_bone_id(String p_name) {

	int id=-1;
	for (int i=0; i<bones.size(); i++) {

		if (bones[i].name == p_name) {
			id=i;
			break;
		}
	};
	
	
	return id;

};


void SkeletonNode::set_bone_name(int p_bone, String p_name) {

	ERR_FAIL_INDEX(p_bone, bones.size());
	
	bones[p_bone].name=p_name;

};


void SkeletonNode::set_bone_rest(int p_bone, const Matrix4& p_mat) {

	ERR_FAIL_INDEX(p_bone, bones.size());


	bones[p_bone].rest=p_mat;
	skeleton_dirty=true;
};

void SkeletonNode::set_bone_pose(int p_bone, const Matrix4& p_pose) {

	ERR_FAIL_INDEX(p_bone, bones.size());

	bones[p_bone].pose=p_pose;
	skeleton_dirty=true;
	
};

Matrix4 SkeletonNode::get_bone_final(int p_bone) {

	ERR_FAIL_INDEX_V(p_bone, bones.size(), Matrix4());
	
	Matrix4 global = get_global_matrix() * bones[p_bone].skel_global;
	
	return global;
};

Matrix4 SkeletonNode::get_bone_pose(int p_bone) {

	ERR_FAIL_INDEX_V(p_bone, bones.size(), Matrix4());

	Matrix4 pose = bones[p_bone].pose;
	
	return pose;
};

Matrix4 SkeletonNode::get_bone_rest(int p_bone) {

	Matrix4 rest = bones[p_bone].rest;
	
	return rest;

};

String SkeletonNode::get_bone_name(int p_bone) {

	ERR_FAIL_INDEX_V(p_bone, bones.size(), "");
	
	String name;
	name=bones[p_bone].name;
	
	return name;
};

void SkeletonNode::set_bone_enabled(int p_bone,bool p_enabled) {
	
	ERR_FAIL_INDEX(p_bone, bones.size());
	
	bones[p_bone].on=p_enabled;
	
}
bool SkeletonNode::is_bone_enabled(int p_bone) const {
	
	ERR_FAIL_INDEX_V(p_bone, bones.size(), false);
	return bones[p_bone].on;
	
}


void SkeletonNode::draw(const SpatialRenderData& p_data) {


	if (draw_material.is_null()) {
	
		draw_material=Renderer::get_singleton()->create_material();
		draw_material->set("flags/visible",true);
		draw_material->set("flags/double_sided",true);
		draw_material->set("flags/invert_normals",false);
		draw_material->set("flags/shaded",false);
		draw_material->set("flags/draw_z",false);
		draw_material->set("flags/test_z",false);
		draw_material->set("flags/wireframe",false);
		draw_material->set("layer",1);
	
		draw_material->set("vars/transparency",0.0f);
		draw_material->set("vars/shininess",0.0f);
		draw_material->set("vars/line_thickness",1.0f);
	}
	
	Renderer::get_singleton()->render_set_matrix( Renderer::MATRIX_WORLD, get_global_matrix() );
	
	Renderer::get_singleton()->render_primitive_set_material( draw_material );
	
	for (int i=0;i<bones.size();i++) {
	
		Vector3 points[2];
		points[0] = bones[i].skel_global.get_translation();
		points[1] = (bones[i].parent>=0) ? bones[ bones[i].parent ].skel_global.get_translation() : points[0];
		
		Color colors[2]={Color(255,100,255), Color(255,100,255) };
		
		Renderer::get_singleton()->render_primitive(2,points,0,0,colors);
		Vector3 crossx[]={
		points[0]+Vector3(0.02,0,0), points[0]+Vector3(-0.02,0,0) };
		Renderer::get_singleton()->render_primitive(2,crossx,0,0,colors);
		
		Vector3 crossy[]={
		points[0]+Vector3(0,0.02,0), points[0]+Vector3(0,-0.02,0) };
		Renderer::get_singleton()->render_primitive(2,crossy,0,0,colors);
		
		Vector3 crossz[]={
		points[0]+Vector3(0,0,0.02), points[0]+Vector3(0,0,-0.02) };
		Renderer::get_singleton()->render_primitive(2,crossz,0,0,colors);		
	}
	
}

void SkeletonNode::event(EventType p_type) {

	switch(p_type) {
	
		case EVENT_MATRIX_CHANGED: {
			if (visual_debug) {
			
				AABB aabb( Vector3(-1,-1,-1),Vector3(2,2,2) );
				get_global_matrix().transform_aabb(aabb);
				visual_debug->set_AABB( aabb );	
			}
						

		} break;
		case EVENT_ENTER_SCENE: {
			
			if (visual_debug) {
				AABB aabb( Vector3(-1,-1,-1),Vector3(2,2,2) );
				get_global_matrix().transform_aabb(aabb);
				visual_debug->set_AABB( aabb );	
			}
		
		} break;
		case EVENT_EXIT_SCENE: {
					
			if (visual_debug) {
				visual_debug->set_AABB( AABB() );
			}
		
		
		} break;
		default: {}
	}
}
void SkeletonNode::process() {
	
	
	if (!skeleton_dirty)
		return;
	
	update_skeleton();
	
	int bone_count = bones.size();
	
	for (int i=0; i<bone_count; i++) {

		int parent = bones[i].parent;
		if (parent >= 0) {

			bones[i].skel_global = bones[i].on?( bones[parent].skel_global * (bones[i].rest * bones[i].pose)):(bones[parent].skel_global*bones[i].rest);

		} else {
			bones[i].skel_global = bones[i].on?(bones[i].rest * bones[i].pose):bones[i].rest;
		};

		skeleton->set_bone_matrix( i, bones[i].skel_global * bones[i].rest_global_inverse );

	};
	for(int i = 0 ; i < bound_childs.size() ; i++) {
		
		int bone = bound_childs[i].bone;
		ERR_FAIL_INDEX(bone,bone_count);
		ERR_FAIL_COND(bound_childs[i].child.is_null());
		bound_childs[i].child->set_pose_matrix(bones[bone].skel_global);
	}
	
	skeleton_dirty=false;
	
};

void SkeletonNode::_iapi_set(const String& p_path, const Variant& p_value) {

	if (p_path == "bone_count") {
	
		set_bone_count(p_value);
	
	} else if (p_path.find("bones[") == 0) {
	
		int openpos = p_path.find("[")+1;
		int closepos = p_path.find("]");
		ERR_FAIL_COND(closepos==-1);
		int sep = p_path.find("/")+1;
		ERR_FAIL_COND(sep==-1);
	
		int bone=p_path.substr( openpos, closepos-openpos ).to_int();
		String var=p_path.substr( sep, p_path.length() - sep );
		
		ERR_FAIL_COND(var.empty());
		ERR_FAIL_INDEX(bone, get_bone_count() );
		
		if (var == "name") {
			
			set_bone_name(bone, p_value);
		} else if (var == "rest") {
		
			set_bone_rest(bone, p_value);
		
		} else if (var == "pose") {
		
			set_bone_pose(bone, p_value);
		
		} else if (var == "parent_idx") {
		
			set_bone_parent(bone, p_value);
		} else if (var == "on") {
			
			set_bone_enabled(bone,p_value);
		}

	} else if (p_path=="display") {
		
		display=p_value.get_bool();
		
		if (visual_debug) {
			
			if (!display) {
				
				//hide (delete)
				Renderer::get_singleton()->get_spatial_indexer()->remove_spatial_object(visual_debug);
				visual_debug=NULL;
			}			
		} else {
			
			if (display) {
				//show (create
				visual_debug=Renderer::get_singleton()->get_spatial_indexer()->create_spatial_object();
				
				ERR_FAIL_COND(!visual_debug);
				
				visual_debug->set_draw_callback( Method1<const SpatialRenderData&>( this, &SkeletonNode::draw ) );
				visual_debug->set_owner( this );		
				
				AABB aabb( Vector3(-1,-1,-1),Vector3(2,2,2) );
				get_global_matrix().transform_aabb(aabb);
				visual_debug->set_AABB( aabb );	
				
			}
		}
		
	}
};

void SkeletonNode::set_bone_count(unsigned int p_count) {

	bones.resize(p_count);
	skeleton=Renderer::get_singleton()->create_skeleton();

};

void SkeletonNode::_iapi_get(const String& p_path,Variant& p_ret_value) {

	if (p_path == "bone_count") {

		p_ret_value = get_bone_count();
	
	} else if (p_path.find("bones[") == 0) {
	
		int openpos = p_path.find("[")+1;
		int closepos = p_path.find("]");
		ERR_FAIL_COND(closepos==-1);
		int sep = p_path.find("/")+1;
		ERR_FAIL_COND(sep==-1);
	
		int bone=p_path.substr( openpos, closepos-openpos ).to_int();
		String var=p_path.substr( sep, p_path.length() - sep );
		
		ERR_FAIL_COND(var.empty());
		ERR_FAIL_INDEX(bone, get_bone_count() );

		if (var == "name") {
			
			p_ret_value = get_bone_name(bone);

		} else if (var == "rest") {
		
			p_ret_value = get_bone_rest(bone);
		
		} else if (var == "pose") {
		
			p_ret_value = get_bone_pose(bone);
		
		} else if (var == "parent_idx") {
		
			p_ret_value = get_bone_parent(bone);
		} else if (var == "on") {
			
			p_ret_value=is_bone_enabled(bone);
		}
	} else if (p_path=="display") {
	
		p_ret_value=display;
	}
};

void SkeletonNode::_iapi_get_property_list( List<PropertyInfo>* p_list ) {

	p_list->push_back( IAPI::PropertyInfo( Variant::INT, "bone_count" ) );
	String pref = "bones[";
	for (int i=0; i<bones.size(); i++) {
	
		p_list->push_back( IAPI::PropertyInfo( Variant::STRING, pref + String::num(i) + "]/name" ) );
		p_list->push_back( IAPI::PropertyInfo( Variant::MATRIX4, pref + String::num(i) + "]/pose" ) );
		p_list->push_back( IAPI::PropertyInfo( Variant::MATRIX4, pref + String::num(i) + "]/rest" ) );
		p_list->push_back( IAPI::PropertyInfo( Variant::INT, pref + String::num(i) + "]/parent_idx" ) );
		p_list->push_back( IAPI::PropertyInfo( Variant::BOOL, pref + String::num(i) + "]/on" ) );
	};
	
	p_list->push_back( IAPI::PropertyInfo( Variant::BOOL, "display" ) );
	
};

void SkeletonNode::_iapi_get_property_hint( const String& p_path, PropertyHint& p_hint ) {
	
	if (p_path.find("parent_idx")!=-1)
		p_hint=PropertyHint( PropertyHint::HINT_RANGE,"-1,256" );
};

void SkeletonNode::_iapi_call(const String& p_name, const List<Variant>& p_params,Variant& r_ret) {

	if (p_name=="clear_pose") {

		for (int i=0; i<bones.size(); i++) {

			set_bone_pose(i, Matrix4());
		};

	};
};

void SkeletonNode::_iapi_get_method_list( List<MethodInfo>* p_list ) {

	p_list->push_back(MethodInfo("clear_pose"));
};


SkeletonNode::SkeletonNode() {

	skeleton_dirty=true;
	set_signal.connect( this, &SkeletonNode::_iapi_set );
	get_signal.connect( this, &SkeletonNode::_iapi_get );
	get_property_list_signal.connect( this, &SkeletonNode::_iapi_get_property_list );
	get_property_hint_signal.connect( this, &SkeletonNode::_iapi_get_property_hint );
	event_signal.connect( this, &SkeletonNode::event );

	get_method_list_signal.connect( this, &SkeletonNode::_iapi_get_method_list );
	call_signal.connect( this, &SkeletonNode::_iapi_call );

	set_process_flags( MODE_FLAG_ALL|CALLBACK_FLAG_POST);
	
	process_signal.connect( this, &SkeletonNode::process_callback );
	
	visual_debug=NULL;
	display=false;
		
};

SkeletonNode::~SkeletonNode() {

};

