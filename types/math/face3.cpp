//
// C++ Implementation: face3d
//
// Description: 
//
//
// Author: Juan Linietsky <reduz@codenix.com>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "face3.h"
#include <stdio.h>
#include "geometry_tools.h"

int Face3::split_by_plane(const Plane& p_plane,Face3 p_res[3],bool p_is_over[3],double _epsilon) const {

	ERR_FAIL_COND_V(!has_area(_epsilon),0);


        Vector3 above[4];
	int above_count=0;
	
	Vector3 below[4];
	int below_count=0;

	for (int i=0;i<3;i++) {

		if (p_plane.has_point( vertex[i], _epsilon )) { // point is in plane
				
			ERR_FAIL_COND_V(above_count>=4,0);
			above[above_count++]=vertex[i];
			ERR_FAIL_COND_V(below_count>=4,0);
			below[below_count++]=vertex[i];
			
		} else {
			
			if (p_plane.is_point_over( vertex[i])) {
				//Point is over			
				ERR_FAIL_COND_V(above_count>=4,0);
				above[above_count++]=vertex[i];

			} else {
				//Point is under
				ERR_FAIL_COND_V(below_count>=4,0);
				below[below_count++]=vertex[i];
			}
			
			/* Check for Intersection between this and the next vertex*/
			
			Vector3 inters;
			Plane::IntersectResult ires=p_plane.get_intersection_with_segment( vertex[i],vertex[(i+1)%3],&inters,_epsilon);
		
			if (ires==Plane::INTERSECT_NONE)
				continue; // no intersection, nothing wrong
			
			/* Intersection goes to both */
			ERR_FAIL_COND_V(above_count>=4,0);
			above[above_count++]=inters;
			ERR_FAIL_COND_V(below_count>=4,0);
			below[below_count++]=inters;
		}
	}
	
	int polygons_created=0;
	
	ERR_FAIL_COND_V( above_count>=4 && below_count>=4 , 0 ); //bug in the algo
	
	if (above_count>=3) {
		
		p_res[polygons_created]=Face3( above[0], above[1], above[2] );
		p_is_over[polygons_created]=true;
		polygons_created++;
		
		if (above_count==4) {
		
			p_res[polygons_created]=Face3( above[2], above[3], above[0] );
			p_is_over[polygons_created]=true;
			polygons_created++;
		
		}
	} 
	
	if (below_count>=3) {
		
		p_res[polygons_created]=Face3( below[0], below[1], below[2] );
		p_is_over[polygons_created]=false;
		polygons_created++;
		
		if (below_count==4) {
		
			p_res[polygons_created]=Face3( below[2], below[3], below[0] );
			p_is_over[polygons_created]=false;
			polygons_created++;
		
		}
	} 
	
	return polygons_created;
}



bool Face3::intersects_ray(const Vector3& p_from,const Vector3& p_dir,Vector3 * p_intersection) {
			
	return GeometryTools::ray_intersects_triangle(p_from,p_dir,vertex[0],vertex[1],vertex[2],p_intersection);

}

bool Face3::intersects_segment(const Vector3& p_from,const Vector3& p_dir,Vector3 * p_intersection) {
			
	return GeometryTools::segment_intersects_triangle(p_from,p_dir,vertex[0],vertex[1],vertex[2],p_intersection);

}


bool Face3::has_area(double _epsilon) const {
	
	Vector3 v1=vertex[2]-vertex[0];
	Vector3 v2=vertex[1]-vertex[0];
	
	v1.normalize();
	v2.normalize();
	Vector3 perp;
	perp.set_as_cross_of( v1, v2 );

	return (perp.length()>_epsilon);
}

Face3::Side Face3::get_side_of(const Face3& p_face,double _epsilon,ClockDirection p_clock_dir) const {

	int over=0,under=0;
	
	Plane plane=get_plane(p_clock_dir);
	
	for (int i=0;i<3;i++) {
		
		const Vector3 &v=p_face.vertex[i];
		
		if (plane.has_point(v,_epsilon)) //coplanar, dont bother
			continue;
		
		if (plane.is_point_over(v))
			over++;
		else
			under++;
		
	}
	
	if ( over > 0 && under == 0 )
		return SIDE_OVER;
	else if (under > 0 && over ==0 )
		return SIDE_UNDER;
	else if (under ==0 && over == 0)
		return SIDE_COPLANAR;
	else
		return SIDE_SPANNING;

}


Face3::Face3(const Vector3 &p_v1,const Vector3 &p_v2,const Vector3 &p_v3) {

	
	vertex[0]=p_v1;
	vertex[1]=p_v2;
	vertex[2]=p_v3;
}

Plane Face3::get_plane(ClockDirection p_dir) const {

	return Plane( vertex[0], vertex[1], vertex[2] , p_dir );

}

Vector3 Face3::get_median_point() const {

	return (vertex[0] + vertex[1] + vertex[2])/3.0;
}

ClockDirection Face3::get_clock_dir() const {

	
	Vector3 normal=Vector3::cross(vertex[0]-vertex[1], vertex[0]-vertex[2]);
	//printf("normal is %g,%g,%g x %g,%g,%g- wtfu is %g\n",tofloat(normal.x),tofloat(normal.y),tofloat(normal.z),tofloat(vertex[0].x),tofloat(vertex[0].y),tofloat(vertex[0].z),tofloat( normal.dot( vertex[0] ) ) );
	return ( normal.dot( vertex[0] ) >= 0 ) ? CLOCKWISE : COUNTERCLOCKWISE;

}

bool Face3::intersects_aabb(const AABB& p_aabb) const {
	
	/** TEST PLANE **/
	if (!p_aabb.intersects_plane( get_plane() ))
		return false;
	
	/** TEST FACE AXIS */
	
#define TEST_AXIS(m_ax)\
	{\
		float aabb_min=p_aabb.pos.m_ax;\
		float aabb_max=p_aabb.pos.m_ax+p_aabb.size.m_ax;\
		float tri_min,tri_max;\
		for (int i=0;i<3;i++) {\
			if (i==0 || vertex[i].m_ax > tri_max)\
				tri_max=vertex[i].m_ax;\
			if (i==0 || vertex[i].m_ax < tri_min)\
				tri_min=vertex[i].m_ax;\
		}\
\
		if (tri_max<aabb_min || aabb_max<tri_min)\
			return false;\
	}
	
	TEST_AXIS(x);
	TEST_AXIS(y);
	TEST_AXIS(z);
	
	/** TEST ALL EDGES **/
	
	Vector3 edge_norms[3]={ 		
		vertex[0]-vertex[1],
		vertex[1]-vertex[2],
		vertex[2]-vertex[0],
	};
	
	for (int i=0;i<12;i++) {
		
		Vector3 from,to;
		p_aabb.get_edge(i,from,to);
		Vector3 e1=from-to;
		for (int j=0;j<3;j++) {
			Vector3 e2=edge_norms[i];
			
			Vector3 axis=Vector3::cross( e1, e2 );
				
			if (axis.squared_length()<0.0001)
				continue; // coplanar
			axis.normalize();
					
			float minA,maxA,minB,maxB;		
			p_aabb.project_range(axis,minA,maxA);
			project_range(axis,Matrix4(),minB,maxB);
			
			if (maxA<minB || maxB<minA)
				return false;
		}
	}
	return true;
	
}

void Face3::project_range(const Vector3& p_normal,const Matrix4& p_transform,float& r_min, float& r_max) const {

	for (int i=0;i<3;i++) {
	
		Vector3 v=vertex[i];
		p_transform.transform(v);
		float d=p_normal.dot(v);
		
		if (i==0 || d > r_max)
			r_max=d;
			
		if (i==0 || d < r_min)
			r_min=d;			
	}
}



void Face3::get_support(const Vector3& p_normal,const Matrix4& p_transform,Vector3 *p_vertices,int* p_count,int p_max) const {
	
#define _FACE_IS_VALID_SUPPORT_TRESHOLD 0.95
#define _EDGE_IS_VALID_SUPPORT_TRESHOLD 0.05

	if (p_max<=0)
		return;

	Vector3 n=p_normal;
	p_transform.inverse_transform_no_translation(n);

	if (p_max>1) {
		/** TEST FACE AS SUPPORT **/
		if (get_plane().normal.dot(n) > _FACE_IS_VALID_SUPPORT_TRESHOLD) {
			
			*p_count=MIN(3,p_max);
		
			for (int i=0;i<*p_count;i++) {
		
				p_transform.transform_copy(vertex[i],p_vertices[i]);
			}
			
			return;
			
		}
	}

	/** FIND SUPPORT VERTEX **/
	
	int vert_support_idx=-1;
	float support_max;

	for (int i=0;i<3;i++) {
	
		float d=n.dot(vertex[i]);
		
		if (i==0 || d > support_max) {
			support_max=d;
			vert_support_idx=i;
		}
	}		

	if (p_max==1) {
		
		p_transform.transform_copy(vertex[vert_support_idx],p_vertices[0]);
		return;		
	}
	/** TEST EDGES AS SUPPORT **/
	int best_edge_idx=-1;
	float best_edge_dot=1e10;
	
	for (int i=0;i<3;i++) {

		if (i!=vert_support_idx && ((i+1)%3)!=vert_support_idx)
			continue;
		
	// check if edge is valid as a support
		float dot=(vertex[i]-vertex[(i+1)%3]).get_normalized().dot(n);
		dot=ABS(dot);

		if (dot > _EDGE_IS_VALID_SUPPORT_TRESHOLD || dot>best_edge_dot )
			continue;
				
		best_edge_dot=dot;
		best_edge_idx=i;
	}
	
	if (best_edge_idx!=-1) {
				
		*p_count=2;

		p_transform.transform_copy(vertex[best_edge_idx],p_vertices[0]);
		p_transform.transform_copy(vertex[(best_edge_idx+1)%3],p_vertices[1]);		
		return;			
	}
	
	
	*p_count=1;
	p_transform.transform_copy(vertex[vert_support_idx],p_vertices[0]);		
	
}
	

Face3::Face3()
{
}


Face3::~Face3()
{
}


