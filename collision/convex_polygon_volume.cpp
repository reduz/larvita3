//
// C++ Implementation: convex_polygon_volume
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "convex_polygon_volume.h"

IRef<IAPI> ConvexPolygonVolume::IAPI_DEFAULT_INSTANCE_FUNC(const String& p_type,const  IAPI::CreationParams& p_params) {
	
	ERR_FAIL_COND_V( !p_params.has("face_count"), IRef<IAPI>() );
	ERR_FAIL_COND_V( !p_params.has("face_indices"), IRef<IAPI>() );
	ERR_FAIL_COND_V( !p_params.has("vertex_count"), IRef<IAPI>() );
	ERR_FAIL_COND_V( !p_params.has("edge_count"), IRef<IAPI>() );
	
	int face_count = p_params.get("face_count");
	int edge_count = p_params.get("edge_count");
	int vertex_count = p_params.get("vertex_count");
	ERR_FAIL_COND_V(p_params.get("face_indices").get_type()!=Variant::INT_ARRAY,IRef<IAPI>());
		
	DVector<int> indices = p_params.get("face_indices").get_int_array();
	ERR_FAIL_COND_V( indices.size()==0, IRef<IAPI>() );
	
	indices.read_lock();
	
	ConvexPolygonVolume *cvolume = memnew( ConvexPolygonVolume( face_count, indices.read(), edge_count, vertex_count ) );
		
	indices.read_unlock();
	
	if (!cvolume->is_valid()) {
		
		memdelete( cvolume );
		ERR_PRINT("Invalid Convex volume");
		return IRef<IAPI>();
	}

	return cvolume;
}


void ConvexPolygonVolume::set(String p_path, const Variant& p_value) {
	
	if (p_path.find("faces/")==0) {
						
		int index=p_path.get_slice("/",1).to_int();
		ERR_FAIL_INDEX(index,get_face_count());
		String what=p_path.get_slice("/",2);
		/*if (what=="plane") {
			// ignore, not needed anymore	
			set_face_plane(index,p_value.get_plane());	

		} else*/ if (what=="indices") {
				
			ERR_FAIL_COND(p_value.get_type()!=Variant::INT_ARRAY);
			ERR_FAIL_COND(p_value.size()!=get_face_index_count(index));
			for(int i=0;i<p_value.size();i++)
				set_face_index(index,i,p_value.array_get_int(i));

		}
			
	} else if (p_path.find("edges/")==0) {
			
		int index=p_path.get_slice("/",1).to_int();
		ERR_FAIL_INDEX(index,get_edge_count());
		ERR_FAIL_COND(p_value.get_type()!=Variant::INT_ARRAY);
		ERR_FAIL_COND(p_value.size()!=2);
			
		set_edge_v1(index,p_value.array_get_int(0));
		set_edge_v2(index,p_value.array_get_int(1));
			
	} else if (p_path.find("verts")==0) {
						
		int index=p_path.get_slice("/",1).to_int();
		ERR_FAIL_INDEX(index,get_vertex_count());
		set_vertex(index,p_value.get_vector3());

	}
	
}
Variant ConvexPolygonVolume::get(String p_path) const {
	
	if (p_path=="face_count")
		return face_count;
	if (p_path=="edge_count")
		return edge_count;
	if (p_path=="vertex_count")
		return vertex_count;
	if (p_path=="face_indices") {
		
		Variant v( Variant::INT_ARRAY, face_count );
		for (int i=0;i<face_count;i++)
			v.array_set_int(i,faces[i].index_count);
		return v;
	}
			
	if (p_path.find("faces/")==0) {
								
		int index=p_path.get_slice("/",1).to_int();
		ERR_FAIL_INDEX_V(index,get_face_count(),Variant());
		String what=p_path.get_slice("/",2);
		if (what=="plane") {
			// used for debug only, doesn't get saved anyway
			return get_face_plane(index);	

		} else if (what=="indices") {
						
			Variant indices(Variant::INT_ARRAY,get_face_index_count(index));
						
			for(int i=0;i<indices.size();i++)
				indices.array_set_int(i,get_face_index(index,i));
			return indices;
		}
					
	} else if (p_path.find("edges/")==0) {
					
		int index=p_path.get_slice("/",1).to_int();
		ERR_FAIL_INDEX_V(index,get_edge_count(),Variant());
					
		Variant edge( Variant::INT_ARRAY,2 );
		edge.array_set_int(0,get_edge_v1(index));
		edge.array_set_int(1,get_edge_v2(index));
					
		return edge;
	} else if (p_path.find("verts/")==0) {
								
		int index=p_path.get_slice("/",1).to_int();
		ERR_FAIL_INDEX_V(index,get_vertex_count(),Variant());
		return get_vertex(index);

	}
				
	if (p_path=="face_count")
		return get_face_count();
	if (p_path=="face_indices") {
					
		Variant v;
		v.create( Variant::INT_ARRAY, get_face_count() );
		for (int i=0;i<get_face_count();i++) {
			v.array_set(i,get_face_index_count(i));			 
		}
		return v;
	}
	if (p_path=="vertex_count")
		return get_vertex_count();
	if (p_path=="edge_count")
		return get_edge_count();
				
		
	return Variant();
}

void ConvexPolygonVolume::get_property_list( List<PropertyInfo> *p_list ) const {
	
						
	p_list->push_back( PropertyInfo( Variant::INT, "face_count", PropertyInfo::USAGE_CREATION_PARAM ));
	p_list->push_back( PropertyInfo( Variant::INT, "vertex_count", PropertyInfo::USAGE_CREATION_PARAM ));
	p_list->push_back( PropertyInfo( Variant::INT, "edge_count", PropertyInfo::USAGE_CREATION_PARAM ));
	p_list->push_back( PropertyInfo( Variant::INT_ARRAY, "face_indices", PropertyInfo::USAGE_CREATION_PARAM ));
	
	for (int i=0;i<get_face_count();i++) {
		String path="faces/"+String::num(i)+"/";
		p_list->push_back( IAPI::PropertyInfo(Variant::PLANE,path+"plane",PropertyInfo::USAGE_EDITING));
		p_list->push_back( IAPI::PropertyInfo(Variant::INT_ARRAY,path+"indices"));
	}					
				
	for (int i=0;i<get_edge_count();i++) {
		String path="edges/"+String::num(i);
		p_list->push_back( IAPI::PropertyInfo(Variant::INT_ARRAY,path) );
	}
						
	for (int i=0;i<get_vertex_count();i++) {
		String path="verts/"+String::num(i);
		p_list->push_back( IAPI::PropertyInfo(Variant::VECTOR3,path) );
	}
				
}

Variant ConvexPolygonVolume::call(String p_method, const List<Variant>& p_params) {

	if (p_method=="needs_presave")
		return true;
		
	return Variant();
}

Vector3 ConvexPolygonVolume::get_center( const Matrix4& p_transform) const {
	
	if (vertex_count==0)
		return p_transform.get_translation();
	
	AABB aabb;
	aabb.pos=vertices[1];
	
	for (int i=1;i<vertex_count;i++) {
	
		aabb.expand_to( vertices[i] );
	}
	
	Vector3 result = aabb.pos + aabb.size * 0.5;
	
	p_transform.transform( result );
	
	return result;
}


void ConvexPolygonVolume::project_range(const Vector3& p_normal,const Matrix4& p_transform,float& r_min, float& r_max) const {


	for (int i=0;i<vertex_count;i++) {
	
		Vector3 v=vertices[i];
		p_transform.transform(v);
		float d=p_normal.dot(v);
		
		if (i==0 || d > r_max)
			r_max=d;
			
		if (i==0 || d < r_min)
			r_min=d;			
	}
}

/* dot product tuning, dp is prefererd because it's resolution independent */
#define _FACE_IS_VALID_SUPPORT_TRESHOLD 0.98
#define _EDGE_IS_VALID_SUPPORT_TRESHOLD 0.02

void ConvexPolygonVolume::get_support(const Vector3& p_normal,const Matrix4& p_transform,Vector3 *p_vertices,int* p_count,int p_max) const {

	if (p_max<=0)
		return;

	if (face_plane_caches_dirty)
		 regenerate_face_plane_caches();
	if (edge_normal_caches_dirty)
		regenerate_edge_normal_caches();

	Vector3 n=p_normal;
	p_transform.inverse_transform_no_translation(n);
	
	int vert_support_idx=-1;
	float support_max;

	for (int i=0;i<vertex_count;i++) {
	
		float d=n.dot(vertices[i]);
		
		if (i==0 || d > support_max) {
			support_max=d;
			vert_support_idx=i;
		}
	}

	ERR_FAIL_COND(vert_support_idx==-1);

	if (p_max==1) {
		// simple optimization, if asked for one support, return the vertex
		p_transform.transform_copy(vertices[vert_support_idx],*p_vertices);
		*p_count=1;
		return;
	}
	
	/** TEST FACES AS SUPPORT **/

	float best_face_dot=-2.0;
	int best_face_idx=-1;

	for (int i=0;i<face_count;i++) {
	
		Face& f=faces[i];
	
		// check if face is valid as a support
		float dot=f.p.normal.dot(n);
		if (dot < _FACE_IS_VALID_SUPPORT_TRESHOLD || dot<best_face_dot )
			continue;
	
		//now check that face HAS the support
		bool has_support=false;
		
		for (int j=0;j<f.index_count;j++) {
		
			if (f.indices[j]==vert_support_idx) {
			
				has_support=true;
				break;
			}
		}
	
		if (!has_support)
			continue;
			
		best_face_dot=dot;
		best_face_idx=i;
	}

	if (best_face_idx!=-1) {
	
		Face& f=faces[best_face_idx];
	
		*p_count=MIN(f.index_count,p_max);
		
		for (int i=0;i<*p_count;i++) {
		
			p_transform.transform_copy(vertices[f.indices[i]],p_vertices[i]);
		}
		
		return;
		
	}

	/** TEST EDGES AS SUPPORT **/


	float best_edge_dot=1e10;
	int best_edge_idx=-1;

	for (int i=0;i<edge_count;i++) {
	
		Edge& e=edges[i];
	
		// check if edge has the support
		if (e.indices[0]!=vert_support_idx && e.indices[1]!=vert_support_idx)
			continue;
	
		// check if edge is valid as a support
		float dot=ABS(e.normal_cache.dot(n));
		
		if (dot > _EDGE_IS_VALID_SUPPORT_TRESHOLD || dot>best_edge_dot )
			continue;
	
		best_edge_dot=dot;
		best_edge_idx=i;
	}

	if (best_edge_idx!=-1) {
	
		Edge& e=edges[best_edge_idx];
	
		*p_count=MIN(2,p_max);
		
		for (int i=0;i<*p_count;i++)
			p_transform.transform_copy(vertices[e.indices[i]],p_vertices[i]);
		
		return;
		
	}

	*p_count=1;
	p_transform.transform_copy(vertices[vert_support_idx],p_vertices[0]);		

#if 0
	return;
	

	/* Test Faces */
	
	if (face_plane_caches_dirty)
		 regenerate_face_plane_caches();
	if (edge_normal_caches_dirty)
		regenerate_edge_normal_caches();
	
	float dottest=-2.0; // impossibly bad dot
	int best_face=-1;
	
	for (int i=0;i<face_count;i++) {
	
		Vector3 n = faces[i].p.normal;
		p_transform.transform_no_translation(n);
		float dot = n.dot(p_normal);
		
//		printf("n %i - %f,%f,%f - dot %f\n",i,n.x,n.y,n.z,dot);
		if (dot > dottest && faces[i].index_count>2) {
			dottest=dot;
			best_face=i;
		}
	}
	
	if (dottest > _FACE_IS_VALID_SUPPORT_TRESHOLD ) {
			
		*p_count=MIN(p_max,faces[best_face].index_count);
		
		for (int i=0;i<*p_count;i++) {
		
			p_vertices[i]=vertices[faces[best_face].indices[i]];
			p_transform.transform(p_vertices[i]);
		}
	
		return; // found face support
	}
	
	/* Test Edges and Points*/
	
	int best_edge=-1;
	int best_point=-1;
	float dottest_edge=-1e10;
	float dottest_point=-1e10;
	
	for (int i=0;i<edge_count;i++) {
	

		Vector3 e0=vertices[edges[i].indices[0]];
		Vector3 e1=vertices[edges[i].indices[1]];
		p_transform.transform(e0);
		p_transform.transform(e1);
		bool test_edge=false;
	
		float dot1=p_normal.dot( e0 );
		
		if (dot1 >= dottest_point) {
		
			dottest_point=dot1;
			best_point=edges[i].indices[0];
			test_edge=true;

		}
		
		float dot2=p_normal.dot( e1 );
		
		if (dot2 >= dottest_point) {
		
			dottest_point=dot2;
			best_point=edges[i].indices[1];
			test_edge=true;

		}
			
		if (!test_edge) //little optimization
			continue;
			
		Vector3 edge_normal=edges[i].normal_cache;
		p_transform.transform_no_translation(edge_normal);		
		
	
		if ( ABS(edge_normal.dot( p_normal )) > _EDGE_IS_VALID_SUPPORT_TRESHOLD )
			continue; //not useful edge
			
		float edge_dot=(dot1>dot2)?dot1:dot2;
		
		if (edge_dot > dottest_edge ) {
		
			dottest_edge = edge_dot;
			best_edge=i;
		}
	
	}
	
	if (best_edge>=0 && dottest_edge==dottest_point) {
	
		/* support is edge */
		
		if (p_max<2) {
			*p_count=0;
			ERR_FAIL_COND(p_max<2);
		}
		
		*p_count=2;
		
		for (int i=0;i<2;i++) {
		
			p_vertices[i]=vertices[edges[best_edge].indices[i]];
			p_transform.transform(p_vertices[i]);
		}
		
		return;
		
	} else if (best_point >=0 ) {
	
		if (p_max<1) {
			*p_count=0;
			ERR_FAIL_COND(p_max<2);
		}
	
		p_vertices[0]=vertices[best_point];
		p_transform.transform(p_vertices[0]);
		*p_count=1;
		return;
	
	}
	
	*p_count=0;

	ERR_PRINT("Couldn't get support?");
#endif
	
}
bool ConvexPolygonVolume::intersect_segment(const Matrix4& p_transform,const Vector3& p_from,const Vector3& p_to,Vector3& r_clip,Vector3& r_normal) const {
	
	
	if (face_plane_caches_dirty)
		 regenerate_face_plane_caches();
	if (edge_normal_caches_dirty)
		regenerate_edge_normal_caches();
	
	Vector3 from=p_from;
	Vector3 to=p_to;
	
	r_normal=Vector3();
	r_clip=from;
	
	for (int i=0;i<face_count;i++) {
	
		Plane p=faces[i].p;
		p_transform.transform(p);
		int side_from,side_to;
		
		side_from = p.has_point(from)?0:(p.is_point_over(from)?1:-1);
		side_to = p.has_point(to)?0:(p.is_point_over(to)?1:-1);
		
		if (side_from==side_to) {
			
			if (side_from>=0) {
				return false; // no intersection
			}
			// if both -1, ignore
		} else {
			// intersection			
			
			Vector3 inters;
			Plane::IntersectResult ires =  p.get_intersection_with_segment(from,to,&inters);
			if (ires!=Plane::INTERSECT_POINT)
				continue;
			
			if (side_from==1) {
				from=inters;
				r_clip=from;		
				r_normal=p.normal;
			}
			
			if (side_to==1)
				to=inters;
		}
	}
	return true
			
			
			
			;	
	
	/*
	Vector3 dir = p_to-p_from;
	
	for (int i=0;i<face_count;i++) {
		
	Vector3 n = faces[i].p.normal;
	p_transform.transform_no_translation(n);
				
	if ( dir.dot(n) <= CMP_EPSILON ) // going in the opposite direction
	continue;		
	if ( (n.dot(p_from) * n.dot( p_to )) <= CMP_EPSILON )
	continue; // not intersecting plane
		
	Vector3 v=vertices[faces[i].indices[0]];
	p_transform.transform(v);
		
	float d = n.dot( v );
	float t = - (n.dot(p_from) - d) / dir.dot(n);
		
	Vector3 inters = p_to + dir * t;
		
		/// now check against triangle faces 
		
		
	Vector3 prev_cross;
	bool inside_poly=true;
		
	for (int j=0;j<(faces[i].index_count-1);j++) {
			
			
	Vector3 v1=vertices[faces[i].indices[j]];
	p_transform.transform(v1);
	v1-=inters;
	Vector3 v2=vertices[faces[i].indices[j+1]];
	p_transform.transform(v2);
	v2-=inters;
						
	Vector3 cross = Vector3::cross( v1, v2 );
			
	if (cross.dot( prev_cross ) < 0 ) {
				
	inside_poly=false;
	break;
}
			
	prev_cross=cross;
}
		
	if (inside_poly) {
			
	r_clip=inters;
	r_normal=n;
	return true;			
}   				   
}
	return false; */
}


Vector3 ConvexPolygonVolume::get_local_inertia(float p_mass) const {


	AABB aabb=compute_AABB(Matrix4());
	
	Vector3 itmp = aabb.size;
	
	itmp.x*=itmp.x;
	itmp.y*=itmp.y;
	itmp.z*=itmp.z;
	
	return (Vector3(itmp.y+itmp.z,itmp.x+itmp.z,itmp.x+itmp.y)*p_mass)*0.0833333;
	

}
	

/** CONVEX OBJECT API */

AABB ConvexPolygonVolume::compute_AABB( const Matrix4& p_transform ) const {

		
	AABB aabb;
	
	for (int i=0;i<vertex_count;i++) {
	
		Vector3 v=vertices[i];
		
		p_transform.transform(v);
	
		if (i==0) {
		
			aabb.pos=v;
		} else {
		
			aabb.expand_to( v );
		}
	}

	return aabb;
}
	
	
int ConvexPolygonVolume::get_face_count() const { 

	return face_count; 
}

void ConvexPolygonVolume::regenerate_face_plane_caches() const {

	
	for (int i=0;i<face_count;i++) {

		Plane avg;
		
		ERR_FAIL_INDEX( faces[i].indices[0], vertex_count );
		ERR_FAIL_INDEX( faces[i].indices[1], vertex_count );
		ERR_CONTINUE(faces[i].index_count<3); // wtf
		
		for (int j=2;j<faces[i].index_count;j++) {
		
		
			ERR_FAIL_INDEX( faces[i].indices[j], vertex_count );
			
			Plane p( vertices[ faces[i].indices[0] ], vertices[ faces[i].indices[j-1] ], vertices[ faces[i].indices[j] ] );
			
			avg.normal+=p.normal;
			avg.d+=p.d;
		}
		
		avg.normal/=(real_t)faces[i].index_count-2;
		avg.d/=(real_t)faces[i].index_count-2;
		
		faces[i].p=avg;
	}
	
	face_plane_caches_dirty=false;
}
	
Plane ConvexPolygonVolume::get_face_plane(int p_which) const {

	ERR_FAIL_INDEX_V(p_which,face_count,Plane());
	
	if (face_plane_caches_dirty)
		regenerate_face_plane_caches();

	return faces[p_which].p;
}
	
int ConvexPolygonVolume::get_edge_count() const {

	return edge_count;
}
int ConvexPolygonVolume::get_edge_v1(int p_which) const {

	ERR_FAIL_INDEX_V(p_which,edge_count,-1);

	return edges[p_which].indices[0];
}
int ConvexPolygonVolume::get_edge_v2(int p_which) const {
	
	ERR_FAIL_INDEX_V(p_which,edge_count,-1);

	return edges[p_which].indices[1];

}

void ConvexPolygonVolume::regenerate_edge_normal_caches() const {

	for (int i=0;i<edge_count;i++) {
		
		ERR_FAIL_INDEX( edges[i].indices[0], vertex_count );
		ERR_FAIL_INDEX( edges[i].indices[1], vertex_count );
		edges[i].normal_cache=(vertices[edges[i].indices[0]]-vertices[edges[i].indices[1]]).get_normalized();
	
	}

	edge_normal_caches_dirty=false;
}

Vector3 ConvexPolygonVolume::get_edge_normal(int p_which) const {

	if (edge_normal_caches_dirty)
		regenerate_edge_normal_caches();
		
	return edges[p_which].normal_cache;
}

void ConvexPolygonVolume::set_edge_v1(int p_which, int p_vertex) {

	ERR_FAIL_INDEX(p_which,edge_count);
	ERR_FAIL_INDEX(p_vertex,vertex_count);

	edges[p_which].indices[0]=p_vertex;
		
	edge_normal_caches_dirty=true;

	edges[p_which].normal_cache=(vertices[edges[p_which].indices[0]]-vertices[edges[p_which].indices[1]]).get_normalized();
	

}
void ConvexPolygonVolume::set_edge_v2(int p_which, int p_vertex) {

	ERR_FAIL_INDEX(p_which,edge_count);
	ERR_FAIL_INDEX(p_vertex,vertex_count);
	
	edges[p_which].indices[1]=p_vertex;
		
	edge_normal_caches_dirty=true;
		
	edges[p_which].normal_cache=(vertices[edges[p_which].indices[0]]-vertices[edges[p_which].indices[1]]).get_normalized();

}
	
	
int ConvexPolygonVolume::get_vertex_count() const {

	return vertex_count;
}
		
	
Vector3 ConvexPolygonVolume::get_vertex(int p_which) const {

	ERR_FAIL_INDEX_V(p_which,vertex_count,Vector3());
	return vertices[p_which];
}
	
	
void ConvexPolygonVolume::set_vertex(int p_which,const Vector3& p_vertex) {
	
	ERR_FAIL_INDEX(p_which,vertex_count);
	vertices[p_which]=p_vertex;
			
	edge_normal_caches_dirty=true;
	face_plane_caches_dirty=true;
	
}

int ConvexPolygonVolume::get_face_index_count(int p_which) const {

	ERR_FAIL_INDEX_V(p_which,face_count,-1);
	
	return faces[p_which].index_count;
}

int ConvexPolygonVolume::get_face_index(int p_which,int p_index) const {

	ERR_FAIL_INDEX_V(p_which,face_count,-1);
	ERR_FAIL_INDEX_V(p_index,faces[p_which].index_count,-1);
	
	return faces[p_which].indices[p_index];

}
void ConvexPolygonVolume::set_face_index(int p_which,int p_index,int p_vertex){

	ERR_FAIL_INDEX(p_which,face_count);
	ERR_FAIL_INDEX(p_index,faces[p_which].index_count);
	ERR_FAIL_INDEX(p_vertex,vertex_count);
	
	faces[p_which].indices[p_index]=p_vertex;

}


bool ConvexPolygonVolume::is_valid() const {

	return faces && vertices && edges;
}


ConvexPolygonVolume::ConvexPolygonVolume(int p_face_count,const int *p_face_indices_count,int p_edge_count,int p_vertex_count) {

	ERR_FAIL_COND(p_face_count<3);
	ERR_FAIL_COND(p_edge_count<3);
	ERR_FAIL_COND(p_vertex_count<3);

	faces = memnew_arr( Face, p_face_count );
	for (int i=0;i<p_face_count;i++) {
	
		faces[i].index_count=p_face_indices_count[i];
		faces[i].indices = memnew_arr( int, p_face_indices_count[i] );
		
	}
	face_count=p_face_count;
	
	edges = memnew_arr( Edge, p_edge_count );
	edge_count=p_edge_count;
	vertices = memnew_arr( Vector3, p_vertex_count );
	vertex_count=p_vertex_count;	
	
}

#define _MIN_VERTEX_DISTANCE_TRESHOLD 0.0001

bool ConvexPolygonVolume::is_geometry_closed(const DVector<Face3>& p_faces) {

	p_faces.read_lock();
	const Face3 *faces_ptr = p_faces.read();
	
	for (int i=0;i<p_faces.size();i++) {

		Vector3 v[4]={faces_ptr[i].vertex[0],faces_ptr[i].vertex[1],faces_ptr[i].vertex[2],faces_ptr[i].vertex[0]};
		

		for (int vi=0;vi<3;vi++) {
		
			Vector3 v1=v[vi];
			Vector3 v2=v[vi+1];
		
			int match=0;
		
			for (int j=0;j<p_faces.size();j++) {
		
				if (j==i)
					continue;
					
				Vector3 w[4]={faces_ptr[j].vertex[0],faces_ptr[j].vertex[2],faces_ptr[j].vertex[1],faces_ptr[j].vertex[0]};
		
				for (int wj=0;wj<3;wj++) {
		
					Vector3 w1=w[wj];
					Vector3 w2=w[wj+1];
					if (w1==v1 && // vertices pre-snapped for equal comparison
					       w2==v2 ) {
					    
						match++;
									       }

				}
			}
			
			if (match!=1) {
			
				printf("match is %i, not closed\n",match);
				p_faces.read_unlock();
				return false;
			}
		}
	}

	printf("closed\n");
	p_faces.read_unlock();
	return true;
}


bool ConvexPolygonVolume::is_geometry_convex(const DVector<Face3>& p_faces) {
	
	p_faces.read_lock();
	const Face3 *faces_ptr = p_faces.read();
	
	for (int i=0;i<p_faces.size();i++) {
		
		Plane p = faces_ptr[i].get_plane();;
		
		for (int j=0;j<p_faces.size();j++) {
		
			for (int k=0;k<3;k++) {
			
				if (p.distance_to_point(faces_ptr[j].vertex[k]) > CMP_POINT_IN_PLANE_EPSILON) {
					
					p_faces.read_unlock();
					return false;
				}
			}
		}
	}
	p_faces.read_unlock();
	
	return true;
	
}


void ConvexPolygonVolume::create_from_generated_hull(const DVector<Face3>& p_faces) {
	
	edges=0;
	faces=0;
	vertices=0;
	edge_count=0;
	face_count=0;
	vertex_count=0;
	
	DVector<Vector3> aux_vertices;
	
	p_faces.read_lock();
	const Face3 *faces_ptr = p_faces.read();
	
	for (int i=0;i<p_faces.size();i++) {
		
		for (int j=0;j<3;j++) {
			
			Vector3 v= faces_ptr[i].vertex[j];
			
			bool exists=false;
			aux_vertices.read_lock();
			const Vector3 *aux_vertices_ptr=aux_vertices.read();
			for (int k=0;k<aux_vertices.size();k++) {
				
				if (v.distance_to(aux_vertices_ptr[k])<CMP_POINT_IN_PLANE_EPSILON) {
					
					exists=true;
					break;
				}
			}
			
			aux_vertices.read_unlock();
			
			if (!exists)
				aux_vertices.push_back(v);
		}
	}
	
	printf("found %i unique vertices\n",aux_vertices.size());
	DVector<Plane> valid_planes;
	
	aux_vertices.read_lock();
	const Vector3 *aux_vertices_ptr=aux_vertices.read();
	
	/* SLOW, VERY SLOW... OMFG SLOW... I'm so lazy :P.. will optimize sometime, 
	runs offline anyway... */
	for (int i=0;i<aux_vertices.size();i++) {
			
		for (int j=i+1;j<aux_vertices.size();j++) {
				
			for (int k=j+1;k<aux_vertices.size();k++) {
		
				Plane p(aux_vertices_ptr[i],aux_vertices_ptr[j],aux_vertices_ptr[k]);
				
				if (p.normal.length()<0.8) // test invalid plane, jic
					continue;
				int over=0,under=0;
				
				for (int l=0;l<aux_vertices.size();l++) {
					
/*					printf("plane %s against vertex %s\n",Variant(p).get_string().ascii().get_data(),
					       Variant(aux_vertices_ptr[l]).get_string().ascii().get_data());*/
					Vector3 v=aux_vertices_ptr[l];
					if (p.has_point(v))
						continue;
					else if (p.is_point_over(v))
						over++;
					else
						under++;
					
				}
				
				//printf("plane %s has %i over, %i under\n",Variant(p).get_string().ascii().get_data(),over,under);
				ERR_CONTINUE(over==0 && under==0); //bug?
				if (over>0 && under>0)
					continue;
				
				if (over) //negate plane if all points are over, so it points out
					p=-p;
					
				bool exists=false;
				valid_planes.read_lock();
				const Plane *valid_planes_ptr=valid_planes.read();
				
				for (int m=0;m<valid_planes.size();m++) {
					
					Plane q=valid_planes_ptr[m];
					if ( (q.normal.dot( p.normal ) > 0.999 ) && ABS(q.d-p.d)<1e-5) {
						
						exists=true;
						break;
					}
				}
				
				valid_planes.read_unlock();
				
				if (!exists)
					valid_planes.push_back(p);
					
			}
		}		
	}
	
	aux_vertices.read_unlock();
	
	ERR_FAIL_COND(valid_planes.size()==0);
	
	valid_planes.read_lock();
	
	create_from_planes(valid_planes.read(),valid_planes.size());
	
	valid_planes.read_unlock();

	
}
	
#define _VERTEX_SNAP 0.0001

ConvexPolygonVolume::ConvexPolygonVolume(const DVector<Face3>& p_faces) {


	DVector<Face3> snapped_faces;
	
	for(int i=0;i<p_faces.size();i++) {
	
		Face3 f=p_faces[i];
		
		for (int j=0;j<3;j++) {
		
			f.vertex[j].snap(_VERTEX_SNAP);
		}	
		snapped_faces.push_back(f);
	}
	

	if (!is_geometry_convex(snapped_faces) || !is_geometry_closed(snapped_faces)) {
		
		create_from_generated_hull(snapped_faces);
		return;
	}
	
	DVector<Plane> planes_arr;
	
	/* Add Planes from Group */
	
	for (int i=0;i<snapped_faces.size();i++) {
	
		Face3 f = snapped_faces[i];
		Plane p=f.get_plane();
		
		bool plane_unique=true;

		for (int j=0;j<planes_arr.size();j++) {
		
			Plane q = planes_arr[j];
			
			/* discard coplanar */
			if ( p.is_almost_like(q) ) {
			
				plane_unique=false;
				break;
			}
								
		}
		
		if (plane_unique)
			planes_arr.push_back(p);
		
	}
	
	planes_arr.read_lock();
	create_from_planes( planes_arr.read(), planes_arr.size() );
	planes_arr.read_unlock();
}


/**  Create from planes. 
 * Just pass the planes and the vertices will be generated, as well as unnecesary planes
 * removed. This is quite a lot more CPU and memory Intensive.
 */
	

void ConvexPolygonVolume::create_from_planes(const Plane * p_planes,int p_plane_count) {

	faces=0;
	edges=0;
	vertices=0;
	face_count=0;
	vertex_count=0;
	edge_count=0;
	
	/* create all vertices first */
	
	DVector<Vector3> vertices_temp;

	
	for (int i=0;i<p_plane_count;i++) {
	
		Plane pi=p_planes[i];
		for (int j=i+1;j<p_plane_count;j++) {
		
			Plane pj=p_planes[j];

			for (int k=j+1;k<p_plane_count;k++) {

				Plane pk=p_planes[k];
				
				Vector3 v;
				if (!pi.get_intersection_with_planes(pj,pk,&v))
					continue;
				vertices_temp.push_back(v);
				
			}
		}
	}



	/* validate vertices */

	DVector<Vector3> valid_vertices;
	
	for (int i=0;i<vertices_temp.size();i++) {
	
		Vector3 v = vertices_temp[i];
		bool point_valid=true;
		for (int j=0;j<p_plane_count;j++) {
		
			Plane p=p_planes[j];
			float d = p.distance_to_point(v);
			
			if (d>CMP_POINT_IN_PLANE_EPSILON) {
				
				point_valid=false;
				break;
			} else if (d>0) {
				/* don't let the vertex be more than the plane */
				v -= p.normal * d;
			}
		}
		
		if (!point_valid)
			continue;
		
		
		valid_vertices.read_lock();
		const Vector3 *valid_vertices_ptr=valid_vertices.read();
		for (int j=0;j<valid_vertices.size();j++) {
			
			if ( ABS(v.distance_to( valid_vertices_ptr[j]) )< _MIN_VERTEX_DISTANCE_TRESHOLD ) {
				
				point_valid=false;
				break;				
			}
		}
		valid_vertices.read_unlock();
		
		if (!point_valid)
			continue;
		valid_vertices.push_back(v);
	}
	
	ERR_FAIL_COND(valid_vertices.size()==0);
	
	/* validate planes */
	DVector<Plane> valid_planes;

	for (int i=0;i<p_plane_count;i++) {

		Plane p=p_planes[i];
		bool valid_plane=false;
		
		for (int j=0;j<valid_vertices.size();j++) {
		
			/* if the plane is toching a vertex, the plane is valid */
			/* maybe i should expand itto a plane having more than one vertex? */
			
			if (Math::absd(p.distance_to_point(valid_vertices[j]))<CMP_POINT_IN_PLANE_EPSILON) {
			
				valid_plane=true;
				break;
			}
		}
		
		if (valid_plane) {
			valid_planes.push_back(p);
			
		}

	}
	
	ERR_FAIL_COND(valid_planes.size()==0);
	
	/* VERTICES FIRST */
	
	vertices=memnew_arr( Vector3, valid_vertices.size() );
	
	for (int i=0;i<valid_vertices.size();i++)
		vertices[i]=valid_vertices[i];
		
	vertex_count=valid_vertices.size();
	
	
	/* FACES SECOND */
	faces=memnew_arr( Face, valid_planes.size() );
	
	for (int i=0;i<valid_planes.size();i++) {
	
		faces[i].p=valid_planes[i];
		faces[i].index_count=0;
		faces[i].indices=NULL;
		
		/* find the indices */
		DVector<int> indices;
		Vector3 center;
		
		for (int j=0;j<vertex_count;j++) {
		
			if (ABS(faces[i].p.distance_to_point(vertices[j]))<CMP_POINT_IN_PLANE_EPSILON) {
				indices.push_back(j);
				center+=vertices[j];
			}
		}
					
		/* sort them */
		
		if (indices.size()==0)
			printf("plane %i\n",i);
		ERR_CONTINUE(indices.size()==0);
		
		if (indices.size()==1) {
			//weird, may happen?
			faces[i].indices=memnew_arr( int, 1 );
			faces[i].indices[0]=indices[0];
			faces[i].index_count=1;
			continue;
		}
		
		center/=indices.size(); //not the best point ever, but should work
		
		// create binormal and tangent vector
		Vector3 binormal=Vector3::cross( faces[i].p.normal, center-vertices[  indices[0] ] ); // just take any point as ref
		binormal.normalize();
		Vector3 tangent=Vector3::cross( faces[i].p.normal, binormal );
		tangent.normalize();
		
		
		// bubblysort!
		
		while (true) {
		
			bool sorted=true;
		
			for (int j=0;j<(indices.size()-1);j++) {
			
				float x=binormal.dot( vertices[ indices[j] ]-center );
				float y=tangent.dot( vertices[ indices[j] ]-center );
				float phi=Math::atan2(y,x);
			
				float nx=binormal.dot( vertices[ indices[j+1] ]-center );
				float ny=tangent.dot( vertices[ indices[j+1] ]-center );
				float nphi=Math::atan2(ny,nx);
			
				if (phi<nphi) {
				
					int aux=indices[j];
					indices.set(j,indices[j+1]);
					indices.set(j+1,aux);
					sorted=false;
				}
			}
			
			if (sorted)
				break;
		}
		
		faces[i].index_count=indices.size();
		faces[i].indices = memnew_arr( int, indices.size() );
		
		for (int j=0;j<indices.size();j++)
			faces[i].indices[j]=indices[j];
		
	}
	
	
	face_count=valid_planes.size();
	
	/* EDGES LAST */
	
	/* this approach is kind of crappy, but faster, wont let me clean unneeded edges
	DVector<Edge> tmpedges;
	
	for (int i=0;i<face_count;i++) {
		
	int prev=faces[i].index_count-1;
	for (int j=0;j<faces[i].index_count;j++) {

	int v1=faces[i].indices[prev];
	int v2=faces[i].indices[j];
	prev=j;
	bool exists=false;
	tmpedges.lock();
			
	for (int k=0;k<tmpedges.size();k++) {
				
	if (
	(tmpedges(k).indices[0]==v1 &&
	tmpedges(k).indices[1]==v2) || 
	(tmpedges(k).indices[0]==v2 &&
	tmpedges(k).indices[1]==v1) ) {
					
	exists=true;
	break;		
}
}
			
	tmpedges.unlock();
			
	if (exists)
	continue;
			
	Edge e;
	e.indices[0]=v1;
	e.indices[1]=v2;
	e.normal_cache=(vertices[v1]-vertices[v2]).get_normalized();
	tmpedges.push_back(e);
}
}
	
	*/
	
	DVector<Edge> tmpedges;
	
	for (int i=0;i<vertex_count;i++) {
		// test all combinations of vertices, and see if they share two planes
		Vector3 vi=vertices[i];
		for (int j=i+1;j<vertex_count;j++) {
	
			Vector3 vj=vertices[j];
			
			bool is_edge=false;
			
			for (int k=0;k<valid_planes.size();k++) {
			
				Plane pk=valid_planes[k];
				for (int l=k+1;l<valid_planes.size();l++) {
				
					Plane pl=valid_planes[l];

					float ang = pl.normal.dot(pk.normal);
// I could use a lot less, like 0.5, but then should check for vertices separatedly
// from the edges in get_support
#define _PLANE_TOLERANCE 0.999
					if (ang > _PLANE_TOLERANCE )
						continue;
					
					if (pk.has_point( vi ) && pk.has_point( vj ) && pl.has_point( vi ) && pl.has_point( vj )) {
						is_edge=true;
						break;
					}
					
				}
				
				if (is_edge)
					break;
			}
			
			if (!is_edge)
				continue;
			Edge e;
			e.indices[0]=i;
			e.indices[1]=j;
			e.normal_cache=(vertices[i]-vertices[j]).get_normalized();

			tmpedges.push_back(e);
		}
	}
		
	
	ERR_FAIL_COND( tmpedges.size() < 1 );
	
	edges = memnew_arr( Edge, tmpedges.size() );
	for (int i=0;i<tmpedges.size();i++)
		edges[i]=tmpedges[i];
		
	edge_count=tmpedges.size();

	/* done! */
	edge_normal_caches_dirty=true;
	face_plane_caches_dirty=true;

}
	
ConvexPolygonVolume::ConvexPolygonVolume(const Plane* p_planes,int p_plane_count) {


	create_from_planes(p_planes,p_plane_count);
	
}

/**  Create a box, from a half-widths vector.
 */
ConvexPolygonVolume::ConvexPolygonVolume(const Vector3& p_half_widths) {

		
	Plane * planes = memnew_arr( Plane, 6 );
		
	planes[0]=Plane(0,1,0,p_half_widths.x);
	planes[1]=Plane(0,-1,0,p_half_widths.x);
	planes[2]=Plane(1,0,0,p_half_widths.y);
	planes[3]=Plane(-1,0,0,p_half_widths.y);
	planes[4]=Plane(0,0,1,p_half_widths.z);
	planes[5]=Plane(0,0,-1,p_half_widths.z);	
	
	create_from_planes(planes,6);
	
	memdelete_arr(planes);
	edge_normal_caches_dirty=true;
	face_plane_caches_dirty=true;
	
}

/** Create sphere, given lats and lons
 */
ConvexPolygonVolume::ConvexPolygonVolume(int p_lats, int p_lons,float p_radius) {

	edge_normal_caches_dirty=true;
	face_plane_caches_dirty=true;

	
}
	
/** Create cylinder, given lons, radius and height.
 */
	
ConvexPolygonVolume::ConvexPolygonVolume(int p_lons,float p_radius,float p_height) {


	DVector<Plane> p;
	
	for (int i=0;i<p_lons;i++) {
	
		Vector3 n=Vector3(Math::sin(i*(2.0*Math_PI)/p_lons),0,Math::cos(i*(2.0*Math_PI)/p_lons));
		p.push_back( Plane( n*p_radius, n ) );
	}
	
	p.push_back( Plane( 0, 1, 0, p_height ) );
	p.push_back( Plane( 0, -1, 0, p_height ) );
	
	p.read_lock();
	create_from_planes( p.read(), p.size() );
	p.read_unlock();
	edge_normal_caches_dirty=true;
	face_plane_caches_dirty=true;
		
}


ConvexPolygonVolume::~ConvexPolygonVolume() {

	
	if (faces) {
		for (int i=0;i<face_count;i++) {
		
			if (faces[i].indices)
				memdelete_arr( faces[i].indices );
		}
		memdelete_arr(faces);
	}
	
	if (edges)
		memdelete_arr(edges);
	
	if (vertices)
		memdelete_arr(vertices);
}


