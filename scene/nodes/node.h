//
// C++ Interface: node
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef NODE_H
#define NODE_H

#include "iapi_layer.h"
#include "iref.h"

class Node : public IAPI_Layer {

	IAPI_TYPE( Node, IAPI_Layer );
public:
	/**
	 * Type of Events that nodes can receive from the base class.
	 * Nodes emit notifications, (either for any of the inherited 
	 * classes or anyone who wants them). Signalling system is used
	 * instead of inheritance for greater flexibility.
	 * To receive events, connect event_signal.
	 */
	
	enum EventType {
	
		EVENT_MATRIX_CHANGED, ///< Received when the matrix of the node, or a parent node changes
		
		EVENT_DELETED, ///< Received when node is deleted, even though it will not work if connected to itself for obvious reasons (called from ~Node) this may be a dangerous event, use with care.
  		EVENT_OWNER_CHANGED, ///< Owner changed 	
		EVENT_CONFIGURE, ///< this is called after the whole tree has been loaded, in case that some nodes need configuration at that stage (generally when they need other (parent) nodes
		EVENT_ENTER_SCENE,
		EVENT_EXIT_SCENE,
	};
	
	enum ProcessFlags {
		
		MODE_FLAG_GAME=1,
  		MODE_FLAG_PAUSE=2,
		MODE_FLAG_EDITOR=4,
  		MODE_FLAG_ALL=1|2|4,
		CALLBACK_FLAG_NORMAL=8,
		CALLBACK_FLAG_PHYSICS=16,
		CALLBACK_FLAG_POST=32,
		CALLBACK_FLAG_ALL=8|16|32,
	};
	
	enum MatrixMode {
		MATRIX_MODE_LOCAL, ///< Matrix is stored in local coordinades, so set_matrix() is faster than set_global_matrix()
		MATRIX_MODE_GLOBAL, ///< Matrix is stored in global coordinades, so set_global_matrix() is faste than set_matrix()		
	};
private:
friend class SceneMainLoop;	

	struct Data {

		String name; //Nodes have a name, used for identification
		MatrixMode matrix_mode;
		Matrix4 matrix; //Regular Matrix
		Matrix4 pose_matrix; //Pose matrix, this can be either enabled/disabled, will be post multiplied to the regular matrix. Has many uses
		WeakRef<Node> parent;

		WeakRef<Node> owner;		
		
		Uint32 process_flags;
	} data;

	struct NodeCaches {
		Matrix4 matrix;
		bool matrix_dirty; //matrix is dirty!
		bool has_pose_matrix;
		bool on_delete; //true while being deleted
		bool inside_scene;

	} node_cache;


	typedef List< IRef<Node> > ChildList;
	ChildList child_list; // list of child nodes


	/* basic handlers for iapi */
	void _set(const String& p_path, const Variant& p_value);
	void _get(const String& p_path,Variant& p_ret_value);
	void _get_property_list( List<PropertyInfo>* p_list );
	void _get_property_hint( const String& p_path, PropertyHint& p_hint );

	IRef<Node> take_child_private(String p_name,bool p_check_errors); ///< take child used internally
	void set_dirty_matrix_cache(); ///< set global matrix cache as dirty for this and all subtree childs
	void event_call_tree(EventType p_event); ///< call an event for all the subtree
	void _set_inside_scene(bool p_inside);

	void _process_normal(float p_time);
	void _process_physics(float p_time);
	void _process_post(float p_time);
	
	void _update_process_flags();
	
protected:

	const ChildList * get_child_list() const;
	
public:
	
	Signal< Method1< EventType> > event_signal; ///< Nodes can generate events, to receive them connect to this signal. Check EventType for events.
	Signal< Method2<Uint32,float> > process_signal; ///< Nodes can generate events, to receive them connect to this signal. Check EventType for events.

	/**
	 * Return the path of a node, either relative or absolute.
	 * @param p_node note to get the path from
	 * @param p_root reference root node, tree root if null
	 * @param include_root wether root must be included on the resulting path
	 */

	static String get_node_path(IRef<Node> p_node, IRef<Node> p_root = IRef<Node>(), bool include_root = false);
	
	virtual bool add_child(IRef<Node> p_child); ///< Add Children, return true if p_child can't be added. Children memory will be managed by this node (and will be deleted when this one is deleted).

	virtual void remove_child(IRef<Node> p_child); ///< Remove a p_child, and delete it

	IRef<Node> get_node(String p_name); ///< Get node, relative to current, paths are accepted, delimited by "/", and from root if "/" is found at the begining. If no paths are specified, works the same as get_child()

	bool has_child(String p_name); ///< Check if it has a children node by name

	IRef<Node> get_child(String p_name); ///< Get a children node by name

	IRef<Node> get_parent(); ///< return the parent to this node

	IRef<Node> take_child(String p_name); ///< By name, Take a p_child from this node (and from the tree). The children is returned and the ex-parent assumes no more responsibility on deleting it. 

	IRef<Node> take_child(IRef<Node> p_node); ///< By pointer, Take a p_child from this node (and from the tree). The children is returned and the ex-parent assumes no more responsibility on deleting it. 


	int get_child_count() const; ///< return the amount of childs in this node
	IRef<Node> get_child_by_index(int p_index); ///< get a child by index

	
	void set_owner(IRef<Node> p_owner); ///< When loading and saving a scene, only nodes with declared ownership to a scene are saved. (the scene editing tool assigns ownerships, scripts don't need to assign it to the nodes they create. This way also, scenes can have children spawned through sub-instantiated scenes. p_owner must be a SceneNode
	IRef<Node> get_owner() const; ///< Return the ownership of this node.
	bool has_valid_owner() const; ///< Check if the owner is a parent/grandparent
	bool inside_owner_scene() const;

	bool is_inside_scene_tree() const;


	Matrix4 get_local_matrix(); ///< Get the node matrix, each node has a matrix with is relative to it's parent, unless specified to use a toplevel matrix (nonrelative to the parent)
	void set_local_matrix(const Matrix4& p_matrix); ///< Set the node matrix, each node has a matrix with is relative to it's parent, unless specified to use a toplevel matrix (nonrelative to the parent)

	void set_global_matrix(const Matrix4& p_matrix); ///< Set a matrix globally
	
	Matrix4 get_global_matrix(); ///< return the global matrix,  ( Pose matrix IS included )
	
	void set_matrix_mode(MatrixMode p_mode); 
	MatrixMode get_matrix_mode() const;
	
	inline bool has_pose_matrix() { return node_cache.has_pose_matrix; }
	inline const Matrix4& get_pose_matrix() { return data.pose_matrix; } ///< Return the pose matrix
	void set_pose_matrix(const Matrix4& p_matrix); ///< Setting identiy disables the pose matrx
	


	void set_process_flags(Uint32 p_flags);
	Uint32 get_process_flags() const;
	
	void set_process_callback_mode(Uint32 p_mode);
	Uint32 get_process_callback_mode() const;
	
	virtual String get_name() const;
	void set_name(String p_name);
		
	
	Node();
	virtual ~Node();

};

#endif
