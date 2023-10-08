//
// C++ Interface: triangle_mesh_volume
//
// Description: 
//
//
// Author: ,,, <red@lunatea>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef TRIANGLE_MESH_VOLUME_H
#define TRIANGLE_MESH_VOLUME_H


#include "collision/volume.h"
#include "face3.h"
#include "vector.h"
#include "dvector.h"

/**
	@author ,,, <red@lunatea>
*/
class TriangleMeshVolume : public Volume {
	
	IAPI_TYPE( TriangleMeshVolume, Volume );

	enum {
	
		EMPTY_NODE=-1,
		EMPTY_TRIANGLE=-1
	};

	struct Triangle {
	
		Vector3 normal;
		int indices[3];
	};

	struct Node {
	
		int left_node;
		int right_node;
		int triangle;
		AABB aabb;
		
		Node() { left_node=right_node=EMPTY_NODE; triangle=EMPTY_TRIANGLE; }
	};

	DVector<Vector3> vertices;
	DVector<Triangle> triangles;
	DVector<Node> aabb_tree;	

	mutable const Vector3 * vertices_cache;
	mutable int vertices_cache_size;
	mutable const Triangle * triangles_cache;
	mutable int triangles_cache_size;
	mutable const Node * aabb_tree_cache;	
	mutable int aabb_tree_cache_size;
	

	int root_idx;

	int add_triangle(const Face3& p_triangle);
	int split_faces(const DVector<Face3> &p_faces);

	void intersect_AABB_with_faces_find(const Matrix4& p_transform,const AABB& p_AABB, Method2<const Face3&,const Vector3&>& p_face_callback,int p_node) const;
	bool intersect_segment_find(const Vector3& p_from,const Vector3& p_to,Vector3& r_clip,Vector3& r_cross,real_t& closest,int p_node) const ;
	
	bool configured;

//	TriangleMeshVolume(int p_vertex_count,int p_triangle_count,int p_aabb_node_count);

public:
	
	static IRef<IAPI> IAPI_DEFAULT_INSTANCE_FUNC(const String& p_type,const  IAPI::CreationParams& p_params);	
	
	virtual void set(String p_path, const Variant& p_value); 
	virtual Variant get(String p_path) const; 
	virtual void get_property_list( List<PropertyInfo> *p_list ) const; 	
	
	virtual bool is_concave() const { return true; }
	
	virtual Type get_volume_type() const;
	
	virtual void project_range(const Vector3& p_normal,const Matrix4& p_transform,float& r_min, float& r_max) const ; ///< project this volume to the given normal axis, creating a segment. r_min contains the lowest projected value, and r_max the highest.
	virtual void get_support(const Vector3& p_normal,const Matrix4& p_transform,Vector3 *p_vertices,int* p_count,int p_max) const; ///< Get the support points given a certain direction, if more than one, they are returned in clockwise order. if p_count == SUPPORT_TYPE_CIRCLE, then the support is a circle (two vertices, pos and radius).
	
	virtual bool intersect_segment(const Matrix4& p_transform,const Vector3& p_from,const Vector3& p_to,Vector3& r_clip,Vector3& r_normal) const ;
					
	virtual Vector3 get_local_inertia(float p_mass) const;
	
	virtual AABB compute_AABB( const Matrix4& p_transform ) const;
	
	virtual Vector3 get_center( const Matrix4& p_transform) const;
	
	/* Used by separator */
	
	virtual void intersect_AABB_with_faces(const Matrix4& p_transform,const AABB& p_AABB, Method2<const Face3&,const Vector3&>& p_face_callback) const;
	
	
	TriangleMeshVolume();
	TriangleMeshVolume(const DVector<Face3> &p_faces);
	~TriangleMeshVolume();

};

#endif
