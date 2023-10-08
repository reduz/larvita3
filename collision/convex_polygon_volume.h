//
// C++ Interface: convex_polygon_volume
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef CONVEX_POLYGON_VOLUME_H
#define CONVEX_POLYGON_VOLUME_H

#include "collision/volume.h"
#include "face3.h"
#include "dvector.h"

/**
	@author Juan Linietsky <reduzio@gmail.com>
*/
class ConvexPolygonVolume : public Volume {
	
	IAPI_TYPE(ConvexPolygonVolume,Volume);

	struct Face {
	
		int *indices;
		int index_count;
		mutable Plane p;	
	};
	
	Face *faces;
	int face_count;
	
	
	struct Edge {
	
		int indices[2];
		mutable Vector3 normal_cache; // used for caching
		Edge() { indices[0]=0; indices[1]=0; }
	};
	
	Edge *edges;
	int edge_count;
	
	Vector3 *vertices;
	int vertex_count;
	
	void create_from_generated_hull(const DVector<Face3>& p_faces);
	void create_from_planes(const Plane * p_planes,int p_plane_count);
	
	void regenerate_face_plane_caches() const;
	void regenerate_edge_normal_caches() const;
	
	mutable bool face_plane_caches_dirty;
	mutable bool edge_normal_caches_dirty;
public:
	
	static IRef<IAPI> IAPI_DEFAULT_INSTANCE_FUNC(const String& p_type,const  IAPI::CreationParams& p_params);	
	
	virtual void set(String p_path, const Variant& p_value); 
	virtual Variant get(String p_path) const; 
	virtual void get_property_list( List<PropertyInfo> *p_list ) const; 	
	Variant call(String p_method, const List<Variant>& p_params=List<Variant>());
	
	
	virtual Type get_volume_type() const { return CONVEX_POLYGON; }
	
	virtual void project_range(const Vector3& p_normal,const Matrix4& p_transform,float& r_min, float& r_max) const; 
	virtual void get_support(const Vector3& p_normal,const Matrix4& p_transform,Vector3 *p_vertices,int* p_count,int p_max) const;
	
	virtual Vector3 get_local_inertia(float p_mass) const;
		
	
	virtual AABB compute_AABB( const Matrix4& p_transform ) const;
	
	virtual Vector3 get_center( const Matrix4& p_transform) const;
	
	/** CONVEX OBJECT API */
		
	int get_face_count() const;
	Plane get_face_plane(int p_which) const;
	int get_face_index_count(int p_which) const;
	int get_face_index(int p_which,int p_index) const;
	void set_face_index(int p_which,int p_index,int p_vertex);
	
	int get_edge_count() const;
	int get_edge_v1(int p_which) const;
	int get_edge_v2(int p_which) const;
	Vector3 get_edge_normal(int p_which) const;
	void set_edge_v1(int p_which, int p_v);
	void set_edge_v2(int p_which, int p_v);
	
	int get_vertex_count() const;
	Vector3 get_vertex(int p_which) const;
	void set_vertex(int p_which,const Vector3& p_vertex);
	
	static bool is_geometry_convex(const DVector<Face3>& p_faces);
	static bool is_geometry_closed(const DVector<Face3>& p_faces);
		
	bool is_valid() const;
	
	bool intersect_segment(const Matrix4& p_transform,const Vector3& p_from,const Vector3& p_to,Vector3& r_clip,Vector3& r_normal) const;
	
	
	/**  Create but don't setup values
	  */
	ConvexPolygonVolume(int p_face_count,const int *p_face_indices_count,int p_edge_count,int p_vertex_count); 
	
	/**  Create from faces. 
	  * If the object is convex, things are easy, otherwise, it gets convexhulled
	  */
	ConvexPolygonVolume(const DVector<Face3>& p_faces);
	
	/**  Create from planes. 
	  * Just pass the planes and the vertices will be generated, as well as unnecesary planes
	  * removed. This is more CPU Intensive.
	  */
	ConvexPolygonVolume(const Plane* p_planes,int p_plane_count);
	
	/**  Create a box, from a half-widths vector.
	  */
	ConvexPolygonVolume(const Vector3& p_half_widths); 
	
	/** Create sphere of radius 1, given lats and lons, use scale to make it bigger
	 */
	ConvexPolygonVolume(int p_lats, int p_lons,float p_radius); 
	 
	/** Create cylinder, given lons, scale it
	 */	 
	ConvexPolygonVolume(int p_lons,float p_radius,float p_height);	
	~ConvexPolygonVolume();	
};

#endif
