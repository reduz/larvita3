#ifndef SKELETON_NODE
#define SKELETON_NODE

#include "node.h"

#include "types/dvector.h"
#include "render/skeleton.h"
#include "render/renderer.h"
#include "render/spatial_indexer.h"
#include "vector.h"

class SkeletonNode : public Node {

	IAPI_TYPE( SkeletonNode, Node );
	IAPI_INSTANCE_FUNC( SkeletonNode );

	struct Bone {

		String name;

		bool on;
		int parent;

		Matrix4 rest;
		Matrix4 pose;
		Matrix4 skel_global; // cache for the childs
		Matrix4 rest_global_inverse;
		
		Bone() { parent=-1; on=true; }
	};

	struct BoundChild {

		WeakRef<Node> child;
		int bone;
	};

	Vector<Bone> bones;
	Vector<BoundChild> bound_childs;

	IRef<Skeleton> skeleton;
	IRef<Material> draw_material;
		
	bool skeleton_dirty;
	
	void update_skeleton();

	bool display;
	SpatialObject *visual_debug;
	
	void draw(const SpatialRenderData& p_data);
	void event(EventType p_type);
	/** IAPI proxy */
	void _iapi_set(const String& p_path, const Variant& p_value);
	void _iapi_get(const String& p_path,Variant& p_ret_value);
	void _iapi_get_property_list( List<PropertyInfo>* p_list );
	void _iapi_get_property_hint( const String& p_path, PropertyHint& p_hint );
	void _iapi_call(const String& p_name, const List<Variant>& p_params,Variant& r_ret);
	void _iapi_get_method_list( List<MethodInfo>* p_list );
	void process_callback(Uint32 p_mode, float p_time);
public:

	inline const IRef<Skeleton>& get_skeleton() const { return skeleton; }
	
	int add_bone(int p_idx, String p_name, Matrix4 p_matrix, int p_parent=-1); ///< returns the index of the bone, or -1 on error
	int get_bone_id(String p_name);
	String get_bone_name(int p_bone);

	void set_bone_count(unsigned int p_count);
	int get_bone_count() { return bones.size(); };
	
	void set_bone_parent(int p_bone, int p_parent);
	int get_bone_parent(int p_bone);

	void set_bone_name(int p_bone, String p_name);
	
	void set_bone_rest(int p_bone, const Matrix4& p_mat);
	void set_bone_pose(int p_bone, const Matrix4& p_pose);
	
	Matrix4 get_bone_final(int p_bone);

	Matrix4 get_bone_pose(int p_bone);
	Matrix4 get_bone_rest(int p_bone);

	void bind_child_to_bone(String p_child, String p_bone);
	void bind_child_to_bone(IRef<Node> p_child, int p_bone);

	void set_bone_enabled(int p_bone,bool p_enabled);
	bool is_bone_enabled(int p_bone) const;
	
	void process();

	SkeletonNode();
	~SkeletonNode();

};

#endif

