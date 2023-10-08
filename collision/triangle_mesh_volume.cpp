//
// C++ Implementation: triangle_mesh_volume
//
// Description: 
//
//
// Author: ,,, <red@lunatea>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "triangle_mesh_volume.h"
#include "sort.h"
#include "geometry_tools.h"

/*
TriangleMeshVolume::TriangleMeshVolume(int p_vertex_count,int p_triangle_count,int p_aabb_node_count) {

	

}
*/
IRef<IAPI> TriangleMeshVolume::IAPI_DEFAULT_INSTANCE_FUNC(const String& p_type,const  IAPI::CreationParams& p_params) {

	TriangleMeshVolume *tms;
	
	tms=memnew( TriangleMeshVolume );
	return tms;
}

void TriangleMeshVolume::set(String p_path, const Variant& p_value) {

	if (p_path=="triangles") {
		
		aabb_tree.clear();
		triangles.clear();
		vertices.clear();
		root_idx=-1;
		
		DVector<Face3> faces;
		int tricount=p_value.size() / 9;
		faces.resize( tricount );
				
		faces.write_lock();
		Face3 *faces_ptr = faces.write();
				
		
		for (int i=0;i<tricount;i++) {
			
			
			
			for (int j=0;j<3;j++) {
			
				Vector3 &v = faces_ptr[i].vertex[j];
				
				v.x = p_value.array_get_real( i*9 + j*3 + 0);
				v.y = p_value.array_get_real( i*9 + j*3 + 1);
				v.z = p_value.array_get_real( i*9 + j*3 + 2);
			}
		}
		
		faces.write_unlock();
		
		root_idx=split_faces(faces);
	
		configured=true;
		
	}

}

Variant TriangleMeshVolume::get(String p_path) const {

	
	if (!configured)
		return Variant();
	
	if (p_path=="triangles") {
		
		int tricount=triangles.size();
		Variant tris( Variant::REAL_ARRAY, tricount * 3 * 3 );
				
		
		for (int i=0;i<tricount;i++) {
			
			Triangle t = triangles[i];
			
			for (int j=0;j<3;j++) {
			
				Vector3 v = vertices[ t.indices[j] ];
				
				tris.array_set_real( i*9 + j*3 + 0, v.x );
				tris.array_set_real( i*9 + j*3 + 1, v.y );
				tris.array_set_real( i*9 + j*3 + 2, v.z );
			}
		}
		
		return tris;
	}
	return Variant();
}

void TriangleMeshVolume::get_property_list( List<PropertyInfo> *p_list ) const {

	p_list->push_back( PropertyInfo( Variant::REAL_ARRAY, "triangles" ) );
}



Volume::Type TriangleMeshVolume::get_volume_type() const {

	return TRIANGLE_MESH;
}

void TriangleMeshVolume::project_range(const Vector3& p_normal,const Matrix4& p_transform,float& r_min, float& r_max) const {

	/* Although range is used mainly in convex meshes, it may be useful to tes the object insde, for example, a BSP tree */
	ERR_FAIL_COND( root_idx==-1);
	ERR_FAIL_COND( !configured);

	vertices_cache_size=vertices.size();
	vertices.read_lock();
	vertices_cache=vertices.read();

	for (int i=0;i<vertices_cache_size;i++) {
	
		Vector3 v=vertices[i];
		p_transform.transform(v);
		float d=p_normal.dot(v);
		
		if (i==0 || d > r_max)
			r_max=d;
			
		if (i==0 || d < r_min)
			r_min=d;			
	}
	
	vertices.read_unlock();

}

#define _FACE_IS_VALID_SUPPORT_TRESHOLD 0.99
#define _EDGE_IS_VALID_SUPPORT_TRESHOLD 0.01

void TriangleMeshVolume::get_support(const Vector3& p_normal,const Matrix4& p_transform,Vector3 *p_vertices,int* p_count,int p_max) const {

	ERR_FAIL_COND( !configured );
	ERR_FAIL_COND( root_idx==-1);

	/* Support is meant for convex shapes, but a simple vertex is given anyway */

	vertices_cache_size=vertices.size();
	vertices.read_lock();
	vertices_cache=vertices.read();
		
	/* Test Faces */
	
	// find support vertex
	Vector3 base_support;
	float maxd;
	
	for (int i=0;i<vertices_cache_size;i++) {
	
		Vector3 v=vertices[i];
		p_transform.transform(v);
		float d=p_normal.dot(v);
		
		if (i==0 || d > maxd) {
			maxd=d;
			base_support=v;
		}
	}
	
	*p_vertices=base_support;
	*p_count=1;

	vertices.read_unlock();

}

bool TriangleMeshVolume::intersect_segment_find(const Vector3& p_from,const Vector3& p_to,Vector3& r_clip,Vector3& r_cross,real_t& closest,int p_node) const {

	ERR_FAIL_COND_V( !configured, false );
	ERR_FAIL_INDEX_V( p_node,aabb_tree_cache_size, false );
	
	const Node &node=aabb_tree_cache[p_node];
	Vector3 c,n;
	
	if (!node.aabb.intersect_segment(p_from,p_to,c,n))
		return false;
		
	if (node.triangle!=EMPTY_TRIANGLE) {
	
		ERR_FAIL_INDEX_V(node.triangle,triangles_cache_size,false);
		
		const Triangle &tr=triangles_cache[node.triangle];
		ERR_FAIL_INDEX_V(tr.indices[0],vertices_cache_size,false);
		const Vector3 &v0=vertices_cache[ tr.indices[0] ];
		ERR_FAIL_INDEX_V(tr.indices[1],vertices_cache_size,false);
		const Vector3 &v1=vertices_cache[ tr.indices[1] ];
		ERR_FAIL_INDEX_V(tr.indices[2],vertices_cache_size,false);
		const Vector3 &v2=vertices_cache[ tr.indices[2] ];
		
		Vector3 res;
		bool inters=GeometryTools::segment_intersects_triangle(p_from,p_to,v0,v1,v2,&res);
		
		if (!inters)
			return false;
			
		float dist_sqr=p_from.squared_distance_to( res );
		if (dist_sqr<closest) {
		
			closest=dist_sqr;
			r_cross=tr.normal;
			r_clip=res;
		}
			
		return true;
	}	
	
	bool res_left = (node.left_node!=EMPTY_NODE)?intersect_segment_find(p_from,p_to,r_clip,r_cross,closest,node.left_node):false;
	bool res_right = (node.right_node!=EMPTY_NODE)?intersect_segment_find(p_from,p_to,r_clip,r_cross,closest,node.right_node):false;
	
	return (res_left || res_right);
}


bool TriangleMeshVolume::intersect_segment(const Matrix4& p_transform,const Vector3& p_from,const Vector3& p_to,Vector3& r_clip,Vector3& r_normal) const {

	ERR_FAIL_COND_V( !configured, false );
	ERR_FAIL_COND_V( root_idx==-1, false);
	ERR_FAIL_COND_V( !configured, false );
	
	Vector3 from=p_from;
	Vector3 to=p_to;
	
	vertices_cache_size=vertices.size();
	vertices.read_lock();
	vertices_cache=vertices.read();
	
	triangles_cache_size=triangles.size();
	triangles.read_lock();
	triangles_cache=triangles.read();
	
	aabb_tree_cache_size=aabb_tree.size();
	aabb_tree.read_lock();
	aabb_tree_cache=aabb_tree.read();	
	
	p_transform.inverse_transform(from);
	p_transform.inverse_transform(to);
	
	real_t closest=1e20;
	bool res = intersect_segment_find(from,to,r_clip,r_normal,closest,root_idx);
	
	if (res) {
		p_transform.transform( r_clip );
		p_transform.transform_no_translation( r_normal );
	}
	
	vertices.read_unlock();
	triangles.read_unlock();
	aabb_tree.read_unlock();

	return res;

}
		
Vector3 TriangleMeshVolume::get_local_inertia(float p_mass) const {

	ERR_FAIL_COND_V( !configured, Vector3() );
	AABB aabb=compute_AABB(Matrix4());
	
	Vector3 itmp = aabb.size;
	
	itmp.x*=itmp.x;
	itmp.y*=itmp.y;
	itmp.z*=itmp.z;
	
	return (Vector3(itmp.y+itmp.z,itmp.x+itmp.z,itmp.x+itmp.y)*p_mass)*0.0833333;

}


AABB TriangleMeshVolume::compute_AABB( const Matrix4& p_transform ) const {

	ERR_FAIL_COND_V( !configured, AABB() );
	ERR_FAIL_COND_V( aabb_tree.size()==0, AABB() );
	
	AABB aabb=aabb_tree[ root_idx ].aabb;
	p_transform.transform_aabb(aabb);
	return aabb;
}

void TriangleMeshVolume::intersect_AABB_with_faces_find(const Matrix4& p_transform,const AABB& p_AABB, Method2<const Face3&,const Vector3&>& p_face_callback,int p_node) const {

	ERR_FAIL_COND( !configured );	
	ERR_FAIL_INDEX( p_node,aabb_tree_cache_size);
	
	const Node &node=aabb_tree_cache[p_node];
		
	if (!node.aabb.intersects(p_AABB))
		return;
		
	if (node.triangle!=EMPTY_TRIANGLE) {
	
		ERR_FAIL_INDEX(node.triangle,triangles_cache_size);
		Face3 face_res;
		
		const Triangle &tr=triangles_cache[node.triangle];
		
		ERR_FAIL_INDEX(tr.indices[0],vertices_cache_size);
		p_transform.transform_copy(vertices_cache[ tr.indices[0] ],face_res.vertex[0]);
		ERR_FAIL_INDEX(tr.indices[1],vertices_cache_size);
		p_transform.transform_copy(vertices_cache[ tr.indices[1] ],face_res.vertex[1]);
		ERR_FAIL_INDEX(tr.indices[2],vertices_cache_size);
		p_transform.transform_copy(vertices_cache[ tr.indices[2] ],face_res.vertex[2]);
		
		Vector3 normal=tr.normal;
		p_transform.transform_no_translation(normal);
		
		p_face_callback.call(face_res,normal);
		
	} else {
	
		if (node.left_node!=EMPTY_NODE)
			intersect_AABB_with_faces_find(p_transform,p_AABB, p_face_callback,node.left_node);
		if (node.right_node!=EMPTY_NODE)
			intersect_AABB_with_faces_find(p_transform,p_AABB, p_face_callback,node.right_node);
	
	}	
}

void TriangleMeshVolume::intersect_AABB_with_faces(const Matrix4& p_transform,const AABB& p_AABB, Method2<const Face3&,const Vector3&>& p_face_callback) const {

	ERR_FAIL_COND( !configured );
	
	ERR_FAIL_COND( root_idx==-1);
	ERR_FAIL_COND( !configured );
	
	vertices_cache_size=vertices.size();
	vertices.read_lock();
	vertices_cache=vertices.read();
	
	triangles_cache_size=triangles.size();
	triangles.read_lock();
	triangles_cache=triangles.read();
	
	aabb_tree_cache_size=aabb_tree.size();
	aabb_tree.read_lock();
	aabb_tree_cache=aabb_tree.read();	

	AABB src_aabb=p_AABB;
	

	p_transform.inverse_transform_aabb(src_aabb);
	
	intersect_AABB_with_faces_find(p_transform,src_aabb,p_face_callback,root_idx);
	
	vertices.read_unlock();
	triangles.read_unlock();
	aabb_tree.read_unlock();

}


/******* CREATION FUNCS *******/

#define _TRIANGLE_VERTEX_SNAP 0.0001


struct _TriangleMeshVolume_AABBSort {

	int index;
	float z;
	
	bool operator<(const _TriangleMeshVolume_AABBSort& p_cmp) const { 
		return z<p_cmp.z;
	}
};
	
	
int TriangleMeshVolume::add_triangle(const Face3& p_triangle) {

	Triangle triangle;
	

	// fetch indices
	for (int i=0;i<3;i++) {
	
		Vector3 v=p_triangle.vertex[i];
	
		int vertices_size=vertices.size();
		vertices.read_lock();
		const Vector3*vertices_ptr = vertices.read();
		
		int idx=-1;
		
		for (int j=0;j<vertices_size;j++) {
		
			if (vertices_ptr[j]==v) {
			
				idx=j;
				break;
			}
				
		}
		
		vertices.read_unlock();
		
		if (idx==-1) {
		
			triangle.indices[i]=vertices.size();
			vertices.push_back( v );
		} else {
		
			triangle.indices[i]=idx;
		}
	}
	
	triangle.normal=Vector3::cross( p_triangle.vertex[0]-p_triangle.vertex[2], p_triangle.vertex[0] - p_triangle.vertex[1] ).get_normalized();
	
	int triangle_idx=triangles.size();
	triangles.push_back( triangle );
	
	Node node;
	node.triangle=triangle_idx;
	node.aabb=p_triangle.get_aabb();
	
	aabb_tree.push_back(node);
	return aabb_tree.size() -1;
}

int TriangleMeshVolume::split_faces(const DVector<Face3> &p_faces) {
	

	ERR_FAIL_COND_V(p_faces.size()==0,-1);
	
	if (p_faces.size()==1) {
	
		return add_triangle(p_faces[0]);
	}
	
	int face_count=p_faces.size();
	p_faces.read_lock();
	const Face3 * faces=p_faces.read();
	
	AABB global_aabb;
	
	
	for (int i=0;i<face_count;i++) {
	
		for (int j=0;j<3;j++) {
		
			if ( i==0 && j==0 )
				global_aabb.pos=faces[i].vertex[j];
			else
				global_aabb.expand_to(faces[i].vertex[j]);
		}
	}


	int longest_axis=global_aabb.get_longest_axis_index();

	Vector< _TriangleMeshVolume_AABBSort > sorted_triangles;
	sorted_triangles.resize(face_count);
	
	for (int i=0;i<face_count;i++) {
	
		_TriangleMeshVolume_AABBSort aabbs;
		aabbs.index=i;
		switch(longest_axis) {
			case 0: aabbs.z=faces[i].get_median_point().x; break;
			case 1: aabbs.z=faces[i].get_median_point().y; break;
			case 2: aabbs.z=faces[i].get_median_point().z; break;
			default: ERR_CONTINUE( ERR_BUG );
		}
		
		sorted_triangles[i]=aabbs;
	}

	SortArray<_TriangleMeshVolume_AABBSort> array_sort;
	
	array_sort.sort(&sorted_triangles[0],face_count);
	
	DVector<Face3> left_array;
	DVector<Face3> right_array;
	
	for (int i=0;i<face_count;i++) {
	
		if (i < face_count/2) {
		
			left_array.push_back( faces[ sorted_triangles[i].index ] );
		} else {
		
			right_array.push_back( faces[ sorted_triangles[i].index ] );		
		}
	}
						
	p_faces.read_unlock();

	Node node;
	
	node.left_node=split_faces(left_array);
	node.right_node=split_faces(right_array);
	node.triangle=EMPTY_TRIANGLE;
	node.aabb=global_aabb;
	
	aabb_tree.push_back( node );
	
	return aabb_tree.size() - 1;
}

Vector3 TriangleMeshVolume::get_center( const Matrix4& p_transform) const {
	
	return p_transform.get_translation();
}


TriangleMeshVolume::TriangleMeshVolume() {
	configured=false;
}

TriangleMeshVolume::TriangleMeshVolume(const DVector<Face3> &p_faces) {

	configured=false;
	
	if (p_faces.size())
		root_idx=split_faces(p_faces);
	
	configured=true;
}


TriangleMeshVolume::~TriangleMeshVolume() {

}


