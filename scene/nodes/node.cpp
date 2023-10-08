//
// C++ Implementation: node
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "node.h"

#include "scene/main/scene_main_loop.h"
#include "performance_metrics.h"
IRef<Node> Node::take_child_private(String p_name,bool p_check_errors) {

	if (node_cache.on_delete)
		return IRef<Node>();

	ChildList::Iterator *I=child_list.begin();
	
	while(I) {
	
		if (I->get()->get_name() == p_name )
			break;
		I=I->next();
	}
	
	ERR_FAIL_COND_V(!I,IRef<Node>()); //child not found
	
	IRef<Node> node = I->get();
	
	child_list.erase(I); // remove from childs
	node->_set_inside_scene(false);
	
	return node;

}

void Node::_set(const String& p_path, const Variant& p_value) {

	
	if (p_path == "name") {
		
		set_name(p_value);
		
	} else if (p_path=="matrix/local") {
		set_local_matrix(p_value.get_matrix4());
		property_changed_signal.call("matrix/global");
		property_changed_signal.call("matrix/position");

	} else if (p_path=="matrix/global") {
		
		set_global_matrix(p_value.get_matrix4());
		property_changed_signal.call("matrix/local");	
		property_changed_signal.call("matrix/position");
		
	} else if (p_path=="matrix/pose") {
		
		set_pose_matrix(p_value.get_matrix4());
		property_changed_signal.call("matrix/global");
		property_changed_signal.call("matrix/position");
		property_changed_signal.call("matrix/local");
		
	} else if (p_path=="matrix/global_mode") {
		
		set_matrix_mode( p_value.get_bool()?MATRIX_MODE_GLOBAL:MATRIX_MODE_LOCAL );
		
	} else if (p_path == "matrix/position") {
		
		Matrix4 mat = get_global_matrix();
		mat.set_translation(p_value);
		set_global_matrix(mat);
		property_changed_signal.call("matrix/global");
		property_changed_signal.call("matrix/local");
		
	} else if (p_path=="process_flags") {
		set_process_flags(p_value.get_int());
	}

}
void Node::_get(const String& p_path,Variant& p_ret_value) {

	if (p_path == "name") {
		
		p_ret_value = get_name();
		
	} else if (p_path=="matrix/local")
		p_ret_value=get_local_matrix();
	else if (p_path=="matrix/global")
		p_ret_value=get_global_matrix();
	else if (p_path=="matrix/pose")
		p_ret_value=get_pose_matrix();
	else if (p_path=="matrix/global_mode")
		p_ret_value=(get_matrix_mode()==MATRIX_MODE_GLOBAL);
	else if (p_path == "matrix/position")
		p_ret_value=get_global_matrix().get_translation();
	else if (p_path=="process_flags")
		p_ret_value=get_process_flags();
		
}

void Node::_get_property_list( List<PropertyInfo>* p_list ) {

	p_list->push_back( IAPI::PropertyInfo( Variant::STRING, "name" ) );
	p_list->push_back( IAPI::PropertyInfo( Variant::INT, "process_flags") );

	p_list->push_back( PropertyInfo( Variant::BOOL, "matrix/global_mode" ) );
	p_list->push_back( PropertyInfo( Variant::MATRIX4, "matrix/global", PropertyInfo::USAGE_GENERAL|PropertyInfo::USAGE_EDITING ) );
	p_list->push_back( PropertyInfo( Variant::MATRIX4, "matrix/pose", PropertyInfo::USAGE_GENERAL|PropertyInfo::USAGE_EDITING ) );
	p_list->push_back( PropertyInfo( Variant::VECTOR3, "matrix/position",PropertyInfo::USAGE_GENERAL|PropertyInfo::USAGE_EDITING ) );
	p_list->push_back( PropertyInfo( Variant::MATRIX4, "matrix/local" ) );
	
}
void Node::_get_property_hint( const String& p_path, PropertyHint& p_hint ) {

	if (p_path=="process_flags") {
	
		p_hint = PropertyHint( PropertyHint::HINT_FLAGS,"Enabled in Game,Enabled in Pause,Enabled in Editor,Normal Callback,Physics Callback,Post Callback");
	
	}
}

const Node::ChildList* Node::get_child_list() const {

	return &child_list;
}



void Node::set_dirty_matrix_cache() {

	/* you can't set as dirty something that is already dirty */
	
	node_cache.matrix_dirty=true;

	/* for this algorithm to be fast, the whole tree must be set dirty */
	ChildList::Iterator *I=child_list.begin();

	while (I) {

		I->get()->set_dirty_matrix_cache();
		I=I->next();
	}
	
//	if (node_cache.matrix_dirty)
//		return;
	
	event_signal.call(EVENT_MATRIX_CHANGED);
}

void Node::_set_inside_scene(bool p_inside) {


	if (node_cache.inside_scene==p_inside)
		return;

	node_cache.inside_scene=p_inside;
	
	ChildList::Iterator *I=child_list.begin();


	while (I) {

		I->get()->_set_inside_scene(p_inside);
		I=I->next();
	}

	if (p_inside) {
	
		event_signal.call( EVENT_ENTER_SCENE);
		SceneMainLoop::get_singleton()->node_enter_scene_signal.call( IRef<Node>( this ) );

	} else {
	
		event_signal.call( EVENT_EXIT_SCENE);
		SceneMainLoop::get_singleton()->node_exit_scene_signal.call( IRef<Node>( this ) );
	}

	_update_process_flags();
}

void Node::event_call_tree(EventType p_event) {

	event_signal.call(p_event);

	ChildList::Iterator *I=child_list.begin();

	while (I) {

		I->get()->event_call_tree(p_event);
		I=I->next();
	}
}


bool Node::add_child(IRef<Node> p_child) {

	/* Child already has a parent! */
	ERR_FAIL_COND_V( !p_child->get_parent().is_null(),true );
	/* Attempt to add itself as p_child */
	ERR_FAIL_COND_V( p_child==this,true );
	/* Children with empty name not allowed */
	//ERR_FAIL_COND_V( p_child->get_name()=="" , true );
	/* Children with path separators (/) now allowed */
	ERR_FAIL_COND_V( p_child->get_name().find("/")>=0 , true );

	if (p_child->get_name() == "") {
		p_child->set_name("Node");
	};
	
	int count = 0;
	String cname = p_child->get_name();
	String count_name = cname;
	while(has_child(count_name)) {

		count_name = cname + "." + String::num(count++);
	};

	if (count_name != cname) {
		p_child->set_name(count_name);
	};


	child_list.push_back( p_child );

	p_child->data.parent=WeakRef<Node>(this);
	
	p_child->set_dirty_matrix_cache();

	p_child->_set_inside_scene(node_cache.inside_scene);
	
	return false;

}


void Node::remove_child(IRef<Node> p_child) {

	take_child(p_child);
}

IRef<Node> Node::get_node(String p_path) {

	if (p_path.length()==0)
		return IRef<Node>(this);

	if (p_path[0]=='/') {
	
		/* Handle Absolute Pathh */
		WeakRef<Node> root=this;

		while (root->data.parent) {

			root=root->data.parent;
		}
		
		return root->get_node(p_path.right(0));
	}

	
	String child;
	String child_path;
	
	int slash_pos=p_path.find("/");
	
	if (slash_pos>=0) {
		child=p_path.left(slash_pos);
		child_path=p_path.right(slash_pos);
	} else {
	
		child=p_path;
	}
			
	IRef<Node> child_node = get_child( child );

	// lacking a child
	if (child_node.is_null())
		return child_node;
	
	if (child_path.length()) {
	
		return child_node->get_node(child_path);
	} else {
		return child_node;
	}

	return IRef<Node>(); //unreachable code
}

bool Node::has_child(String p_name) {

	ChildList::Iterator *I=child_list.begin();

	while (I) {

		if (I->get()->get_name()==p_name)
			return true;
		I=I->next();
	}	
	
	return false;
}

IRef<Node> Node::get_child(String p_name) {

	if (p_name == ".") return this;
	if (p_name == "..") return data.parent.get_iref();
	if (p_name == "~") return data.owner.get_iref();

	ChildList::Iterator *I=child_list.begin();
	
	while (I) {

		if (I->get()->get_name()==p_name) {
			
			return I->get();
		};
		I=I->next();
	}	
	
	return IRef<Node>();

}

IRef<Node> Node::get_parent() {

	return data.parent;
}

IRef<Node> Node::take_child(String p_name) {

	IRef<Node>child = get_child(p_name);
	ERR_FAIL_COND_V(!child,IRef<Node>());
	return take_child(child);

}

IRef<Node> Node::take_child(IRef<Node> p_node) {

	if (child_list.erase(p_node)) {
		p_node->data.parent = WeakRef<Node>();
		p_node->data.owner = WeakRef<Node>();
		p_node->_set_inside_scene(false);
		return p_node;
	}
	return IRef<Node>();
}


int Node::get_child_count() const {

	return child_list.size();
}
IRef<Node> Node::get_child_by_index(int p_index) {


	ChildList::Iterator *I=child_list.begin();
	int idx=0;
	while (I) {

		if (idx==p_index)
			return I->get();
		I=I->next();
		idx++;
	}	

	return IRef<Node>();
}

	
void Node::set_owner(IRef<Node> p_owner) {

	ERR_FAIL_COND(!p_owner->is_type("SceneNode"));
	data.owner=p_owner;
	event_signal.call(EVENT_OWNER_CHANGED);
}
IRef<Node> Node::get_owner() const {

	return data.owner;
}

bool Node::inside_owner_scene() const {

	if (!data.owner) 
		return false;

	WeakRef<Node> node= data.parent;
	while (!node.is_null()) {

		if (node.ptr() == data.owner.ptr()) 
			return true;

		//if (node->get_type() == "SceneNode") return false;
		if (node->data.owner.ptr() != data.owner.ptr()) return false;
		node = node->data.parent;
	};

	return false;
}


bool Node::has_valid_owner() const {

	if (data.owner.is_null()) 
		return false;

	WeakRef<Node> node = data.parent;
	while (node) {

		if (node.ptr() == data.owner.ptr()) 
			return true;
/*		if (node->get_type() == TYPE_SCENE) return false;
		if (node->data.owner != data.owner) return false; */
		node = node->data.parent;
	};

	return false;
}


void Node::set_local_matrix(const Matrix4& p_matrix) {

	PerformanceMetrics::begin_area( PerformanceMetrics::SECTION_PROCESS, "set_local_matrix");
	
		
	if (!data.parent.is_null() && data.matrix_mode==MATRIX_MODE_GLOBAL) {
		data.matrix = data.parent->get_global_matrix();
		data.matrix.set_as_inverse4x3();

		data.matrix=data.matrix * p_matrix;
	} else {
		data.matrix=p_matrix;
	}
	
	set_dirty_matrix_cache();
	PerformanceMetrics::end_area( PerformanceMetrics::SECTION_PROCESS, "set_local_matrix");
	
}


bool Node::is_inside_scene_tree() const {

	return node_cache.inside_scene;	
};

void Node::set_global_matrix(const Matrix4& p_matrix) {

	if (!data.parent.is_null() && data.matrix_mode==MATRIX_MODE_LOCAL) {

		Matrix4 local= data.parent->get_global_matrix();
		local.set_as_inverse4x3();
		local = local * p_matrix;
		data.matrix = local;

	} else {

		data.matrix=p_matrix;
	}
	
	set_dirty_matrix_cache();	
}

void Node::set_pose_matrix(const Matrix4& p_matrix) {

	PerformanceMetrics::begin_area( PerformanceMetrics::SECTION_PROCESS, "set_pose_matrix");
	
	data.pose_matrix=p_matrix;
	node_cache.has_pose_matrix=!p_matrix.is_identity();
	set_dirty_matrix_cache();

	PerformanceMetrics::end_area( PerformanceMetrics::SECTION_PROCESS, "set_pose_matrix");
	
		
}

Matrix4 Node::get_global_matrix() {

		
	if (data.matrix_mode == MATRIX_MODE_GLOBAL) {
		
		if (node_cache.has_pose_matrix)
			return data.matrix * data.pose_matrix;
		else
			return data.matrix;
		
	} 
			
	if (node_cache.matrix_dirty) {
		
		if (data.parent.is_null()) {
			
			node_cache.matrix=data.matrix;
		} else {
			
			node_cache.matrix=data.parent->get_global_matrix() * data.matrix;			
		}
		if (node_cache.has_pose_matrix) {
			
			node_cache.matrix = node_cache.matrix * data.pose_matrix;
		}
		
		node_cache.matrix_dirty=false;
	}
		
	return node_cache.matrix;

		
}
	
Matrix4 Node::get_local_matrix() {

	if (data.matrix_mode == MATRIX_MODE_LOCAL) {
		
		//if (node_cache.has_pose_matrix)
		//	return data.matrix * data.pose_matrix;
		//else
			return data.matrix;
		
	} 
	
	if (node_cache.matrix_dirty) {
		
		if (data.parent.is_null()) {
		
			node_cache.matrix=data.matrix;
		} else {
			
			node_cache.matrix=data.parent->get_global_matrix();
			node_cache.matrix.set_as_inverse4x3();
		
			node_cache.matrix=node_cache.matrix * data.matrix;
		}
		
		/*
		if (node_cache.has_pose_matrix) {
			
			node_cache.matrix = node_cache.matrix * data.pose_matrix;
		}
		*/
		
		node_cache.matrix_dirty=false;
	}
	
	return node_cache.matrix;

}

void Node::set_matrix_mode(MatrixMode p_mode) {

	Matrix4 global=get_global_matrix();
	data.matrix_mode=p_mode;
	set_global_matrix(global);	

}
Node::MatrixMode Node::get_matrix_mode() const {

	return data.matrix_mode;
}

String Node::get_name() const {

	return data.name;
}
void Node::set_name(String p_name) {

	/* for now, you can't change the name of a node if it's inside the tree */
	//ERR_FAIL_COND(data.parent);
	/* path separators (/) not allowed in name */
	ERR_FAIL_COND(p_name.find("/")>=0);

	if (!data.parent || !data.parent->has_child(p_name)) {

		data.name=p_name;
	};

}

String Node::get_node_path(IRef<Node> p_node, IRef<Node> p_root, bool p_include_root) {

	String path;

	IRef<Node> parent;

	while (p_node) {

		if (p_node == p_root && !p_include_root) break;
		path = "/"+p_node->get_name()+path;
		if (p_node == p_root) break;
		parent = p_node->data.parent;
		if (p_node == p_root) break;
		p_node = parent;
	};
	return path;
};


void Node::_process_normal(float p_time) {
	
	if (SceneMainLoop::get_singleton()->get_process_flags()&data.process_flags)
		process_signal.call(CALLBACK_FLAG_NORMAL,p_time);	
}
void Node::_process_physics(float p_time) {
	
	if (SceneMainLoop::get_singleton()->get_process_flags()&data.process_flags)
		process_signal.call(CALLBACK_FLAG_PHYSICS,p_time);	
	
}
void Node::_process_post(float p_time) {
	
	if (SceneMainLoop::get_singleton()->get_process_flags()&data.process_flags)
		process_signal.call(CALLBACK_FLAG_POST,p_time);	
}



void Node::set_process_flags(Uint32 p_flags) {

	data.process_flags = p_flags;
	_update_process_flags();
}

Uint32 Node::get_process_flags() const {
	
	return data.process_flags;
}

void Node::set_process_callback_mode(Uint32 p_mode) {

	set_process_flags( (data.process_flags & MODE_FLAG_ALL) | (p_mode & CALLBACK_FLAG_ALL) );
};

Uint32 Node::get_process_callback_mode() const {

	return data.process_flags & CALLBACK_FLAG_ALL;
};

void Node::_update_process_flags() {
	
	SceneMainLoop* sml = SceneMainLoop::get_singleton();
	sml->frame_process_signal.disconnect( this, &Node::_process_normal);
	sml->physics_process_signal.disconnect( this, &Node::_process_physics );
	sml->post_frame_process_signal.disconnect( this, &Node::_process_post );
	
	if (!data.process_flags || !is_inside_scene_tree())
		return;
	
	if (data.process_flags&CALLBACK_FLAG_NORMAL)
		sml->frame_process_signal.connect( this, &Node::_process_normal );
	if (data.process_flags&CALLBACK_FLAG_PHYSICS)	
		sml->physics_process_signal.connect( this, &Node::_process_physics );
	if (data.process_flags&CALLBACK_FLAG_POST)	
		sml->post_frame_process_signal.connect( this, &Node::_process_post );

	
}


Node::Node() {

	data.matrix_mode=MATRIX_MODE_LOCAL;
	data.process_flags = 0;
	node_cache.has_pose_matrix=false;
	node_cache.matrix_dirty=true;
	node_cache.on_delete=false;
	node_cache.inside_scene=false;
	set_signal.connect( this, &Node::_set );
	get_signal.connect( this, &Node::_get );
	get_property_list_signal.connect( this, &Node::_get_property_list );
	get_property_hint_signal.connect( this, &Node::_get_property_hint );
		
}

Node::~Node() {

	

	
	node_cache.on_delete=true;

	event_signal.disconnect( this ); // disconnect signals to this
	
	child_list.clear();

	node_cache.on_delete=false;
}


