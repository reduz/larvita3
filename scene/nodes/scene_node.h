//
// C++ Interface: scene_node
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef SCENE_NODE_H
#define SCENE_NODE_H

#include "scene/nodes/node.h"
#include "script/script.h"
/**
	@author Juan Linietsky <reduzio@gmail.com>
*/
class SceneNode : public Node {
	
	IAPI_TYPE( SceneNode, Node );

	bool __loading_instance_from;
	

	IRef<Script> script;
	IRef<ScriptInstance> script_instance;
	bool instance; // real scene or instanced scene
	/** IAPI proxy */
	void _script_set(const String& p_path, const Variant& p_value);
	void _script_get(const String& p_path,Variant& p_ret_value);
	void _script_get_property_list( List<PropertyInfo>* p_list );
	void _script_get_property_hint( const String& p_path, PropertyHint& p_hint );
	
	void _script_call(const String& p_method, const List<Variant>& p_params,Variant& r_ret);
	void _script_get_method_list(List<MethodInfo> *p_method_list);

	struct FChildData {
		
		WeakRef<Node> node;
		String path;
	};
	
	void get_owned_childs(IRef<SceneNode> p_owner,IRef<Node> p_current,String p_path,List<FChildData> *p_list);	

	void _event(Node::EventType p_event);
	void _process(Uint32 process_time, float p_time);
	
	void save_node_shared_data(String p_base_path,WeakRef<IAPI> p_node);
		
public:	

	ObjID get_instanced_from_ID() const;
	ObjID get_ID() const;	

	static IRef<IAPI> IAPI_DEFAULT_INSTANCE_FUNC(const String& p_type,const  IAPI::CreationParams& p_params);
	
	void set_script(IRef<Script> p_script);
	IRef<Script> get_script();
		
	void set_scene_instance(bool p_active);
	bool is_scene_instance() const;
	
	/**
	 * This is a helper that saves to DB every child/far child of types that would
	 * benefit from sharing between multiple instances of rhis scene.
	 * It is primarily used by the editor.
	 */
	
	void presave_owned_shared_data(String p_base_path="");
	
	SceneNode(bool p_instance=false);
	~SceneNode();

};

#endif
