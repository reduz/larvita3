//
// C++ Implementation: concave_separator
//
// Description: 
//
//
// Author: ,,, <red@lunatea>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "concave_separator.h"
#include "contact_solver.h"
#include "geometry_tools.h"

#include "print_string.h"


void ConcaveSeparator::triangle_callback_generic(const Face3& p_triangle,const Vector3& p_normal) {
	
	triangle_vol.set_face( p_triangle );
	if (swap) 
		gjk.separate( *volume_generic, transform_generic, triangle_vol , Matrix4(), contact_handler );
	else 
		gjk.separate( triangle_vol, Matrix4(), *volume_generic,transform_generic, contact_handler );
	
}


inline bool ConcaveSeparator::triangle_callback_sphere_intersection_test(const Face3& p_triangle,const Vector3& p_normal,Vector3& r_triangle_contact,Vector3& r_sphere_contact) const {

	/** 1st) TEST PLANE **/

	float d=p_normal.dot(sphere_pos)-p_normal.dot(p_triangle.vertex[0]);
	
	if (d > sphere_radius || d < -sphere_radius) // not touching the plane of the face, return
		return false;
		
	Vector3 contact=sphere_pos - (p_normal*d);
		
	/** 2nd) TEST INSIDE TRIANGLE **/
	
	
	if (GeometryTools::point_in_projected_triangle(contact,p_triangle.vertex[0],p_triangle.vertex[1],p_triangle.vertex[2])) {
		r_triangle_contact=contact;
		r_sphere_contact=sphere_pos-p_normal*sphere_radius;
		//printf("solved inside triangle\n");
		return true;			
	}
	
	/** 3rd TEST INSIDE EDGE CYLINDERS **/
			
	const Vector3 verts[4]={p_triangle.vertex[0],p_triangle.vertex[1],p_triangle.vertex[2],p_triangle.vertex[0]}; // for() friendly
						
	for (int i=0;i<3;i++) {
	
		// check edge cylinder
		
		Vector3 n1=verts[i]-verts[i+1];
		Vector3 n2=sphere_pos-verts[i+1];
		
		///@TODO i could discard by range here to make the algorithm quicker? dunno..
		
		// check point within cylinder radius
		Vector3 axis = Vector3::cross(Vector3::cross( n1, n2 ),n1);
		axis.normalize(); // ugh
		
		float ad=axis.dot(n2);
		
		if (ABS(ad)>sphere_radius) {
			// no chance with this edge, too far away			
			continue;
		}			
					
		// check point within edge capsule cylinder
		/** 4th TEST INSIDE EDGE POINTS **/
		
		float sphere_at = n1.dot(n2);
					
		if (sphere_at>=0 && sphere_at<n1.dot(n1)) {
		
			r_triangle_contact=sphere_pos-axis*(axis.dot(n2));
			r_sphere_contact=sphere_pos-axis*sphere_radius;
			// point inside here
			//printf("solved inside edge\n");
			return true;
		}
								
		float r2=sphere_radius*sphere_radius; 
								
		if (n2.squared_length()<r2) {
		
			Vector3 n=(sphere_pos-verts[i+1]).get_normalized();
			
			//r_triangle_contact=verts[i+1]+n*sphere_radius;sphere_pos+axis*(sphere_radius-axis.dot(n2));
			r_triangle_contact=verts[i+1];
			r_sphere_contact=sphere_pos-n*sphere_radius;
			//printf("solved inside point segment 1\n");		
			return true;
		}
								
		if (n2.squared_distance_to(n1)<r2) {
			Vector3 n=(sphere_pos-verts[i]).get_normalized();
			
			//r_triangle_contact=verts[i]+n*sphere_radius;sphere_pos+axis*(sphere_radius-axis.dot(n2));
			r_triangle_contact=verts[i];
			r_sphere_contact=sphere_pos-n*sphere_radius;
			//printf("solved inside point segment 1\n");		
			return true;
		}
								
		break; // It's pointless to continue at this point, so save some cpu cycles
	}
			
	return false;
}

void ConcaveSeparator::triangle_callback_sphere(const Face3& p_triangle,const Vector3& p_normal) {


	
	VolumeContact c;
	
	// c.A triangle, c.B sphere	
	if (!triangle_callback_sphere_intersection_test(p_triangle,p_normal,c.A,c.B))
		return;
		
	//printf("normal is %f,%f,%f\n",p_normal.x,p_normal.y,p_normal.z);
	//printf("shere at %f,%f,%f\n",sphere_pos.x,sphere_pos.y,sphere_pos.z);
	//printf("A (tri) %f,%f,%f\n",c.A.x,c.A.y,c.A.z);
	//printf("B (sph) %f,%f,%f\n",c.B.x,c.B.y,c.B.z);
	
	if (swap)
		c.invert();
		
	contact_handler->add_contact(c,true);
		
	
}
void ConcaveSeparator::triangle_callback_convex_polygon(const Face3& p_triangle,const Vector3& p_normal) {
	
	float B_min,B_max;
	float A_triminmax=p_normal.dot(p_triangle.vertex[0]);
	
	B_convex_polygon->project_range(p_normal,convex_transform,B_min,B_max);
		
	if (B_min>=A_triminmax || ((B_min+(B_max-B_min)/2.0))<A_triminmax) {
		//printf("plane validated separation\n");
		return;		
	}
	
	Vector3 best_axis=p_normal;
	real_t best_depth=B_min-A_triminmax;
	
	
	// test if faces of B separate the triangle
	for (int i=0;i<B_convex_polygon->get_face_count();i++) {
	
		Vector3 axis=B_convex_polygon->get_face_plane(i).normal;
		convex_transform.transform_no_translation(axis);
		Vector3 point=B_convex_polygon->get_vertex( B_convex_polygon->get_face_index(i,0));
		convex_transform.transform(point);
		
		Plane p(point,axis);
		real_t min_d=1e15;
		
		for (int j=0;j<3;j++) {
			
			float d=p.distance_to_point(p_triangle.vertex[j]);
			if (d<min_d)
				min_d=d;
		}
		
		if (min_d>=0) { // nothing up
			//printf("face validated separation\n");
			return;
		}
		
		if (min_d>best_depth) {
			
			best_depth=min_d;
			best_axis=-axis;
		}
	}	
	
	// test edges

	Vector3 triangle_edge_normals[3]={ 
		p_triangle.vertex[0]-p_triangle.vertex[1],
		p_triangle.vertex[1]-p_triangle.vertex[2],
		p_triangle.vertex[2]-p_triangle.vertex[0] };
		

	for(int i=0;i<B_convex_polygon->get_edge_count();i++) {
		
		for (int j=0;j<3;j++) {
			
			Vector3 e1=triangle_edge_normals[j];
			Vector3 e2=B_convex_polygon->get_edge_normal(i);
			convex_transform.transform_no_translation(e2);
			
			Vector3 axis=Vector3::cross( e1, e2 );
				
			if (axis.squared_length()<0.0001)
				continue; // coplanar
			
			axis.normalize();
			
			real_t A_min,A_max;
			
			p_triangle.project_range(axis,Matrix4(),A_min,A_max);
			B_convex_polygon->project_range(axis,convex_transform,B_min,B_max);
			/*if (!test_projection(axis,best_depth,best_axis,A_max,A_max,B_min,B_max))
				return;
			continue;*/
			if (A_max<B_min || B_max<A_min) {
				return;							
			}
			real_t depth=B_min-A_max;
			
			//printf("depth %f\n",depth);
			if (depth > best_depth) {
				best_depth=depth;
				best_axis=axis;
			}
		}		

	}	
	
	Vector3 supports_A[Volume::MAX_SUPPORTS];
	int support_count_A;
	p_triangle.get_support(best_axis,Matrix4(),supports_A,&support_count_A,Volume::MAX_SUPPORTS);
	
	Vector3 supports_B[Volume::MAX_SUPPORTS];
	int support_count_B;
	B_convex_polygon->get_support(-best_axis,convex_transform,supports_B,&support_count_B,Volume::MAX_SUPPORTS);
	/*
	printf("collided it seems, bestaxis: %f,%f,%f, depth %f, supports A %i supports B %i\n",best_axis.x,best_axis.y,best_axis.z,best_depth,support_count_A,support_count_B);
	
	for(int i=0;i<support_count_A;i++) 
		printf("support A %i: %f,%f,%f\n",i,supports_A[i].x,supports_A[i].y,supports_A[i].z);
	
	for(int i=0;i<support_count_B;i++) 
		printf("support B %i: %f,%f,%f\n",i,supports_B[i].x,supports_B[i].y,supports_B[i].z);
	*/
	if (swap) { 
		
		ContactSolver::solve(supports_B,support_count_B,supports_A,support_count_A,contact_handler);	
		
	} else {
		
		ContactSolver::solve(supports_A,support_count_A,supports_B,support_count_B,contact_handler);	
	}

//	ContactSolver::solve(supports_A,support_count_A,supports_B,support_count_B,p_contact_handler);	
}


void ConcaveSeparator::triangle_callback_pill(const Face3& p_triangle,const Vector3& p_normal) {
	 
	Vector3 normal = p_triangle.get_plane().normal;
	float B_min,B_max;
	float A_triminmax=normal.dot(p_triangle.vertex[0]);
	
	B_pill->project_range(normal,pill_transform,B_min,B_max);
		
	if (B_min>=A_triminmax || ((B_min+(B_max-B_min)/2.0))<A_triminmax) {
		//printf("plane validated separation\n");
		return;		
	}
		
	//printf("triangle:\t%g,%g,%g\n\t%g,%g,%g\n\t%g,%g,%g\n",	       p_triangle.vertex[0].x,p_triangle.vertex[0].y,p_triangle.vertex[0].z,		p_triangle.vertex[1].x,p_triangle.vertex[1].y,p_triangle.vertex[1].z, 	p_triangle.vertex[2].x,p_triangle.vertex[2].y,p_triangle.vertex[2].z );
	
 float best_depth=1e15;
	Vector3 best_axis;
	
	if (!test_projection(normal,best_depth,best_axis,A_triminmax,A_triminmax,B_min,B_max))
		return;
	
	//String solved_using="pill_height-triangle";
	//printf("triangle best depth %f\n",best_depth);		
	
	/* SIMPLE SAT (SEPARATING AXIS THEOREM) */
	
	Vector3 triangle_edge_normals[3]={ 
		p_triangle.vertex[0]-p_triangle.vertex[1],
		p_triangle.vertex[1]-p_triangle.vertex[2],
		p_triangle.vertex[2]-p_triangle.vertex[0] };

	Vector3 pill_up=Vector3(0,B_pill->get_height()/2.0 ,0);
	pill_transform.transform_no_translation( pill_up );

	// test edges <-> cylinder
	for(int i=0;i<3;i++) {
			
		/*
		Vector3 n1=triangle_edge_normals[i];
		Vector3 n2=pill_up-p_triangle.vertex[i];
		
		Vector3 axis = Vector3::cross(Vector3::cross( n1, n2 ),n1);
		*/
		Vector3 axis = Vector3::cross( triangle_edge_normals[i], pill_up );
		axis.normalize();
		if (axis.squared_length()<0.00001)
			continue; // coplanar
						
		float minA,maxA,minB,maxB;
		
		p_triangle.project_range(axis,Matrix4(),minA,maxA);
		B_pill->project_range(axis,pill_transform,minB,maxB);
		
		float bdold=best_depth;
		
		if (!test_projection(axis,best_depth,best_axis,minA,maxA,minB,maxB))
			return;
		
		//if (bdold!=best_depth)
		//	solved_using="side - edge "+String::num(i);
	}
	
	// test points <-> cylinder
	for(int i=0;i<3;i++) {
			
		
		Vector3 n1=pill_up;
		Vector3 n2=p_triangle.vertex[i]-pill_transform.get_translation();
		
		Vector3 axis = Vector3::cross(Vector3::cross( n1, n2 ),n1);
		axis.normalize();
		if (axis.squared_length()<0.00001)
	  		continue; // coplanar
						
		float minA,maxA,minB,maxB;
		
		p_triangle.project_range(axis,Matrix4(),minA,maxA);
		B_pill->project_range(axis,pill_transform,minB,maxB);
		
		float bdold=best_depth;
		
		if (!test_projection(axis,best_depth,best_axis,minA,maxA,minB,maxB))
			return;
		
		//if (bdold!=best_depth)
		//	solved_using="side - point "+String::num(i);
	}
	
	
	for (int s=0;s<2;s++) {
	
		Vector3 center=pill_transform.get_translation()+((s==0)?pill_up:-pill_up);
		
	// test edges second
		for(int i=0;i<3;i++) {

			Vector3 v1=p_triangle.vertex[i];
			Vector3 v2=p_triangle.vertex[(i+1)%3];
			Vector3 v3=center;
		
			Vector3 n1=v2-v1;
			Vector3 n2=v2-v3;
		
			Vector3 axis = Vector3::cross(Vector3::cross( n1, n2 ),n1);
		
			axis.normalize();
			
			if (axis.squared_length()<0.0001)
				continue; // coplanar
			
			
			float minA,maxA,minB,maxB;
		
			p_triangle.project_range(axis,Matrix4(),minA,maxA);
			B_pill->project_range(axis,pill_transform,minB,maxB);
		
			float bdold=best_depth;
		
			if (!test_projection(axis,best_depth,best_axis,minA,maxA,minB,maxB))
				return;
		
			//if (bdold!=best_depth)
			//	solved_using="ball_"+String((s==0)?"up":"down")+" - edge_"+String::num(i)+"\ne1:  "+Variant(v1).get_string()+"\ne2: "+Variant(v2).get_string()+"\n,c: "+Variant(v3).get_string();
		}
	
	// test vertices last
		for(int i=0;i<3;i++) {

	
			Vector3 v1=p_triangle.vertex[i];
			Vector3 v2=center;
		
			Vector3 axis = v2-v1;
		
			axis.normalize();
			
			if (axis.squared_length()<0.0001)
				continue; // coplanar
						
			float minA,maxA,minB,maxB;
		
			p_triangle.project_range(axis,Matrix4(),minA,maxA);
			B_pill->project_range(axis,pill_transform,minB,maxB);
		
			float bdold=best_depth;
		
			if (!test_projection(axis,best_depth,best_axis,minA,maxA,minB,maxB))
				return;
		
			//if (bdold!=best_depth)
			//	solved_using="ball_"+String((s==0)?"up":"down")+" - point_"+String::num(i);
		
		}
		
	}

	ERR_FAIL_COND(best_depth==1e15); //nothing found?
	
	//print_line("Solved Pill: "+solved_using+", best axis: "+String::num(best_axis.x)+","+String::num(best_axis.y)+","+String::num(best_axis.z)+", best depth: %f\n"+String::num(best_depth)); 
	Vector3 supports_A[Volume::MAX_SUPPORTS];
	int support_count_A;
	p_triangle.get_support(-best_axis,Matrix4(),supports_A,&support_count_A,Volume::MAX_SUPPORTS);
	
	Vector3 supports_B[Volume::MAX_SUPPORTS];
	int support_count_B;
	B_pill->get_support(best_axis,pill_transform,supports_B,&support_count_B,Volume::MAX_SUPPORTS);
	
	if (swap) { 
		
		ContactSolver::solve(supports_B,support_count_B,supports_A,support_count_A,contact_handler);	
		
	} else {
		
		ContactSolver::solve(supports_A,support_count_A,supports_B,support_count_B,contact_handler);	
	}

//	ContactSolver::solve(supports_A,support_count_A,supports_B,support_count_B,p_contact_handler);	
}


bool ConcaveSeparator::handles( const Volume& p_A, const Volume& p_B ) const {

	return p_A.is_concave() || p_B.is_concave();
}

void ConcaveSeparator::separate( const Volume& p_A,const Matrix4& p_A_transform, const Volume& p_B, const Matrix4 &p_B_transform, VolumeContactHandler *p_contact_handler) const {

	ERR_FAIL_COND( !p_A.is_concave() && !p_B.is_concave());
	
	const Volume* A = &p_A;
	const Volume* B = &p_B;
	
	contact_handler=p_contact_handler;    
	
	if (A->is_concave() && B->is_concave()) {
		// hell is loose .. @TODO
	
	} else {
	
		const Matrix4 *A_transform;
		const Matrix4 *B_transform;
	
		if (B->is_concave()) {
		
			SWAP(A,B);
			swap=true; // for our simplification, A must be the concave one
			B_transform=&p_A_transform;
			A_transform=&p_B_transform;
		} else {
		
			swap=false;

			A_transform=&p_A_transform;
			B_transform=&p_B_transform;
		}
				
	
		
		switch (B->get_volume_type()) {
		
			case Volume::SPHERE: {
			
				B_sphere=static_cast<const SphereVolume*>(B);
				AABB src_aabb = B_sphere->compute_AABB( *B_transform );
				Method2<const Face3&,const Vector3&> m=Method2<const Face3&,const Vector3&>( const_cast<ConcaveSeparator*const>(this),&ConcaveSeparator::triangle_callback_sphere);
				sphere_pos=B_transform->get_translation();
				sphere_radius=B_sphere->get_radius();			
				A->intersect_AABB_with_faces(*A_transform,src_aabb,m);
			} break;
			case Volume::PILL: {
			
				//print_line("Solving Pill..");
				B_pill=static_cast<const PillVolume*>(B);
				pill_transform=*B_transform;		
				AABB src_aabb = B_pill->compute_AABB( *B_transform );
				Method2<const Face3&,const Vector3&> m=Method2<const Face3&,const Vector3&>( const_cast<ConcaveSeparator*const>(this),&ConcaveSeparator::triangle_callback_pill);
		
				A->intersect_AABB_with_faces(*A_transform,src_aabb,m);
			} break;
			case Volume::CONVEX_POLYGON: {
			
				B_convex_polygon=static_cast<const ConvexPolygonVolume*>(B);
				AABB src_aabb = B_convex_polygon->compute_AABB( *B_transform );
				convex_transform=*B_transform;
				
				Method2<const Face3&,const Vector3&> m=Method2<const Face3&,const Vector3&>( const_cast<ConcaveSeparator*const>(this),&ConcaveSeparator::triangle_callback_convex_polygon);
				
				A->intersect_AABB_with_faces(*A_transform,src_aabb,m);				
				
			} break;
			default: {
				
		  
				volume_generic=B;
				transform_generic=*B_transform;
		
				AABB src_aabb = B->compute_AABB( *B_transform );
		
				Method2<const Face3&,const Vector3&> m=Method2<const Face3&,const Vector3&>( const_cast<ConcaveSeparator*const>(this),&ConcaveSeparator::triangle_callback_generic);
			
				A->intersect_AABB_with_faces(*A_transform,src_aabb,m);
						
			}
		
		}
	}

}


