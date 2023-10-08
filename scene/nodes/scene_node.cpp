//
// C++ Implementation: scene_node
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "scene_node.h"
#include "iapi_persist.h"

IRef<IAPI> SceneNode::IAPI_DEFAULT_INSTANCE_FUNC(const String& p_type,const  IAPI::CreationParams& p_params) {
	
	IRef<SceneNode> scene_node;
	
	if (p_params.has("instance_from")) {
		/* Subscene */
		
		ObjID id = p_params.get("instance_from");

		scene_node = memnew(SceneNode);
		scene_node->__loading_instance_from = true;

		IRef<SceneNode> ref = IAPI_Persist::get_singleton()->load(id, scene_node);

		scene_node->__loading_instance_from = false;

		ERR_FAIL_COND_V(ref.is_null(),IRef<IAPI>());
		
		scene_node->set_scene_instance(true);
	} else {
				
		scene_node = memnew( SceneNode );
	}
	
	return scene_node;
}

ObjID SceneNode::get_instanced_from_ID() const {
	
	ERR_FAIL_COND_V( !instance, OBJ_INVALID_ID );
	return IAPI::get_ID();
};

ObjID SceneNode::get_ID() const {
	
	if (instance) {
		return OBJ_INVALID_ID; // built-in serialize
	} else
		return IAPI::get_ID(); // reference serialize
}
	/** IAPI proxy */
void SceneNode::_script_set(const String& p_path, const Variant& p_value) {

	if (!__loading_instance_from && p_path=="script") {
		if (p_value.get_type() == Variant::IOBJ)
			set_script(p_value.get_IOBJ());
		else
			set_script(IRef<IAPI>());
	} else if (p_path.find("_childs_/")==0) {
		/* convert to node */
		IRef<Node> node = p_value.get_IOBJ();
		ERR_FAIL_COND(node.is_null());
		
		String path=p_path.right(p_path.find("/"));
		String parent_path=path.left( path.find_last("/") );
		
		IRef<Node> parent_node;
		if (parent_path == "") {
			parent_node = this;
		} else {
			parent_node = get_node(parent_path);
		};

		ERR_FAIL_COND(parent_node.is_null());
		parent_node->add_child(node);
		node->set_owner(this);
		
	} else if (!__loading_instance_from && script_instance) {
		script_instance->set( p_path, p_value );	
	}
}
void SceneNode::_script_get(const String& p_path,Variant& p_ret_value) {

	if (p_path=="script") {
		p_ret_value=IObjRef(script);
	} else if (p_path=="instance_from") {
		p_ret_value=IAPI::get_ID();
	} else if (p_path.find("_childs_/")==0) {
		
		String path=p_path.right(p_path.find("/"));
		p_ret_value=IObjRef(get_node(path));
		
	} else if (script_instance) {
		Variant ret = script_instance->get( p_path );
		if (ret.get_type() != Variant::NIL)
			p_ret_value = ret;
	}
}
void SceneNode::_script_get_property_list( List<PropertyInfo>* p_list ) {
		
	if (instance) {
		/* for instanced scenes, just store where it was instanced from */
		 p_list->push_back(PropertyInfo(Variant::INT,"instance_from",PropertyInfo::USAGE_CREATION_PARAM) );
	} else {
		/* otherwise, just load the childs */
		List<FChildData> owned_childs;
		get_owned_childs(this,this,"",&owned_childs);

		List<FChildData>::Iterator *I=owned_childs.begin();
		
		while (I) {

			p_list->push_back( IAPI::PropertyInfo( Variant::IOBJ,"_childs_/"+I->get().path, PropertyInfo::USAGE_DATABASE ));
			I=I->next();
		}
	}
	
	p_list->push_back( IAPI::PropertyInfo( Variant::IOBJ, "script") );
	
	if (script_instance)
		script_instance->get_property_list( p_list );	
}
void SceneNode::_script_get_property_hint( const String& p_path, PropertyHint& p_hint ) {
	
	if (p_path=="script") {

		p_hint=IAPI::PropertyHint( IAPI::PropertyHint::HINT_IOBJ_TYPE,"Script" );
	} else if (script_instance) {
		PropertyHint hint = script_instance->get_property_hint( p_path );
			
		if (hint.type!=IAPI::PropertyHint::HINT_NONE)
			p_hint=hint;
	}	
}

void SceneNode::get_owned_childs(IRef<SceneNode> p_owner,IRef<Node> p_current,String p_path,List<FChildData> *p_list) {
	
	int childs=p_current->get_child_count();
	for (int i=0;i<childs;i++) {

		IRef<Node> child=p_current->get_child_by_index(i);

		String path=p_path;
		if (p_path.size()) {
			path = path+"/"+child->get_name();
		} else {
			path += child->get_name();
		}
		if (child->get_owner()==p_owner) {
			FChildData fcd;
			fcd.node=child;
			fcd.path=path;
			p_list->push_back(fcd);
		}
		get_owned_childs(p_owner,child,path,p_list);
	}	
}

void SceneNode::_script_call(const String& p_method, const List<Variant>& p_params,Variant& r_ret) {

	if (p_method == "set_script") {
		
		ERR_FAIL_COND( p_params.size() == 0 );
		String fname = p_params.begin()->get();
		ERR_FAIL_COND( fname == "" );
		
		script = IAPI_Persist::get_singleton()->instance_type("ScriptLua");
		ERR_FAIL_COND(!script);
		ERR_FAIL_COND( script->load(fname) != OK );
		set_script(script);
		
		return;
	};
	
	if (script_instance) {
		r_ret = script_instance->call( p_method, p_params );
	};
}
void SceneNode::_script_get_method_list(List<MethodInfo> *p_method_list) {

	IAPI::MethodInfo mi("set_script");
	mi.parameters.push_back(IAPI::MethodInfo::ParamInfo(Variant::STRING, "file", IAPI::PropertyHint(IAPI::PropertyHint::HINT_FILE, "*.lua")));
	p_method_list->push_back(mi);
	
	if (script_instance)
		script_instance->get_method_list( p_method_list );	
}

void SceneNode::set_script(IRef<Script> p_script) {
	
	script=p_script;
	if (script.is_null())
		script_instance.clear();
	else {
		script_instance=script->instance( IRef<SceneNode>( this ) );
		ERR_FAIL_COND(!script_instance);
		script_instance->construct();
	};
}
IRef<Script> SceneNode::get_script() {
	
	return script;
}
	
void SceneNode::set_scene_instance(bool p_active) {
	
	instance=p_active;
}
bool SceneNode::is_scene_instance() const {
	
	return instance;	
}

void SceneNode::_event(Node::EventType p_event) {

	if (!script_instance) return;

	List<Variant> params;
	params.push_back(p_event);
	script_instance->call("scene_event", params);
};
		
void SceneNode::_process(Uint32 process_type, float p_time) {

	if (!script_instance) return;

	List<Variant> params;
	params.push_back(process_type);
	params.push_back(p_time);
	script_instance->call("scene_process", params);
};
		
void SceneNode::save_node_shared_data(String p_base_path,WeakRef<IAPI> p_node) {

	ERR_FAIL_COND(p_node.is_null());
	List<PropertyInfo> pinfo;
	p_node->get_property_list(&pinfo);
	List<PropertyInfo>::Iterator *I=pinfo.begin();
	
	for(;I;I=I->next()) {
	
		if (I->get().type!=Variant::IOBJ)
			continue;
	
		Variant v=p_node->get( I->get().path );
		if (v.get_type()!=Variant::IOBJ)
			continue; // may be nil
	
		IAPIRef iapi=v.get_IOBJ();
		ERR_CONTINUE(iapi.is_null());		

		if (iapi->get_ID()==OBJ_INVALID_ID) {

			save_node_shared_data(p_base_path+"/"+I->get().path, iapi);
			
			// ask the iapi wether it wants to be presaved
			Variant res = iapi->call("needs_presave");
			if (res.get_bool()) {
			
				iapi->call("make_local_files");
				if (iapi->get_ID()==OBJ_INVALID_ID) {

					Error err = IAPI_Persist::get_singleton()->save(iapi,p_base_path+"/"+I->get().path);
					ERR_CONTINUE(err);
				};
			
			}
		} else {
		
			Error err =IAPI_Persist::get_singleton()->save(iapi);
			ERR_CONTINUE(err);
		}
	}
	
}
		
void SceneNode::presave_owned_shared_data(String p_base_path) {
	
	List<FChildData> owned_childs;
	get_owned_childs(this,this,"",&owned_childs);
	
	List<FChildData>::Iterator *I=owned_childs.begin();
	
	for(;I;I=I->next()) {
	
		save_node_shared_data(p_base_path,I->get().node.operator->());
	}
// 		
}
	
SceneNode::SceneNode(bool p_instance) {
	
	instance=p_instance;

	__loading_instance_from = false;

	set_signal.connect( this, &SceneNode::_script_set );
	get_signal.connect( this, &SceneNode::_script_get );
	get_property_list_signal.connect( this, &SceneNode::_script_get_property_list );
	get_property_hint_signal.connect( this, &SceneNode::_script_get_property_hint );

	call_signal.connect( this, &SceneNode::_script_call );
	get_method_list_signal.connect( this, &SceneNode::_script_get_method_list );

	event_signal.connect( this, &SceneNode::_event );
	process_signal.connect( this, &SceneNode::_process );
}


SceneNode::~SceneNode() {

}


