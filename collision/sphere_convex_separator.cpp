//
// C++ Implementation: sphere_convex_separator
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "sphere_convex_separator.h"
#include "convex_polygon_volume.h"
#include "sphere_volume.h"
#include "contact_solver.h"

bool SphereConvexSeparator::handles( const Volume& p_A, const Volume& p_B ) const {

	return (p_A.get_volume_type()==Volume::CONVEX_POLYGON && p_B.get_volume_type()==Volume::SPHERE) || (p_A.get_volume_type()==Volume::SPHERE && p_B.get_volume_type()==Volume::CONVEX_POLYGON);
	
}

void SphereConvexSeparator::separate( const Volume& p_A,const Matrix4& p_A_transform, const Volume& p_B, const Matrix4 &p_B_transform, VolumeContactHandler *p_contact_handler) const {
	
	bool swap;
	
	const ConvexPolygonVolume *A;
	const SphereVolume *B;
	const Matrix4 * A_transform;
	const Matrix4 * B_transform;

	if (p_A.get_volume_type()==Volume::CONVEX_POLYGON && p_B.get_volume_type()==Volume::SPHERE) {
	
		A=SAFE_CAST<const ConvexPolygonVolume*>(&p_A);
		B=SAFE_CAST<const SphereVolume*>(&p_B);
		A_transform=&p_A_transform;
		B_transform=&p_B_transform;
		swap=false;
	} else {
	
		A=SAFE_CAST<const ConvexPolygonVolume*>(&p_B);
		B=SAFE_CAST<const SphereVolume*>(&p_A);
		A_transform=&p_B_transform;
		B_transform=&p_A_transform;
		swap=true;
	
	}
	
	float best_depth=1e15;
	Vector3 best_axis;
	

	/* SIMPLE SAT (SEPARATING AXIS THEOREM) */
	
	// test faces first
	for (int i=0;i<A->get_face_count();i++) {
	
		Vector3 axis=A->get_face_plane(i).normal;
		A_transform->transform_no_translation(axis);
		
		float minA,maxA,minB,maxB;
		
		A->project_range(axis,*A_transform,minA,maxA);
		B->project_range(axis,*B_transform,minB,maxB);
			
//		float bdold=best_depth;
		
		if (!test_projection(axis,best_depth,best_axis,minA,maxA,minB,maxB))
			return;
					
	//	if (bdold!=best_depth)
	//		printf("best is face %i\n",i);
		
	}


	// test edges second
	for(int i=0;i<A->get_edge_count();i++) {

	
		Vector3 v1=A->get_vertex(A->get_edge_v1(i));
		A_transform->transform(v1);
		Vector3 v2=A->get_vertex(A->get_edge_v2(i));
		A_transform->transform(v2);
		Vector3 v3=B_transform->get_translation();
		
		Vector3 n1=v2-v1;
		Vector3 n2=v2-v3;
		
		Vector3 axis = Vector3::cross(Vector3::cross( n1, n2 ),n1);
		
		if (axis.squared_length()<0.0001)
			continue; // coplanar
			
		axis.normalize();
			
		float minA,maxA,minB,maxB;
		
		A->project_range(axis,*A_transform,minA,maxA);
		B->project_range(axis,*B_transform,minB,maxB);
		
		float bdold=best_depth;
		
		if (!test_projection(axis,best_depth,best_axis,minA,maxA,minB,maxB))
			return;
		
	//	if (bdold!=best_depth)
	//		printf("best is edge\n");
	}
	
	// test vertices last
	for(int i=0;i<A->get_vertex_count();i++) {

	
		Vector3 v1=A->get_vertex(i);
		A_transform->transform(v1);
		Vector3 v2=B_transform->get_translation();
		
		Vector3 axis = v2-v1;
		
		if (axis.squared_length()<0.0001)
			continue; // coplanar
			
		axis.normalize();
			
		float minA,maxA,minB,maxB;
		
		A->project_range(axis,*A_transform,minA,maxA);
		B->project_range(axis,*B_transform,minB,maxB);
		
		float bdold=best_depth;
		
		if (!test_projection(axis,best_depth,best_axis,minA,maxA,minB,maxB))
			return;
		
	//	if (bdold!=best_depth)
	//		printf("best is vertex\n");
		
	}

	ERR_FAIL_COND(best_depth==1e15); //nothing found?
	
	Vector3 supports_A[Volume::MAX_SUPPORTS];
	int support_count_A;
	A->get_support(-best_axis,*A_transform,supports_A,&support_count_A,Volume::MAX_SUPPORTS);
	
	/*
	//printf("A POS: %f,%f,%f\n",A_transform->get_translation().x,A_transform->get_translation().y,A_transform->get_translation().z);
	printf("axis: %f,%f,%f\n",best_axis.x,best_axis.y,best_axis.z);
	printf("depth: %f\n",best_depth);
	printf("verts %i\n",support_count_A);
	
	for (int i=0;i<support_count_A;i++) {
		
		printf("%i- %f,%f,%f\n",i,supports_A[i].x,supports_A[i].y,supports_A[i].z);
	}
	*/
	Vector3 supports_B[Volume::MAX_SUPPORTS];
	int support_count_B;
	B->get_support(best_axis,*B_transform,supports_B,&support_count_B,Volume::MAX_SUPPORTS);

	//printf("B POS: %f,%f,%f\n",*B_transform.get_translation().x,*B_transform.get_translation().y,*B_transform.get_translation().z);
//	printf("Axis: %f,%f,%f\n",best_axis.x,best_axis.y,best_axis.z);

/*
	for (int i=0;i<support_count_B;i++)
		CollisionDebugger::singleton->plot_contact(supports_B[i]);
	for (int i=0;i<support_count_A;i++)
		CollisionDebugger::singleton->plot_contact(supports_A[i]);*/
	//CollisionDebugger::singleton->plot_contact(p_contact.B);

	if (swap) {
		ContactSolver::solve(supports_B,support_count_B,supports_A,support_count_A,p_contact_handler);
	} else {
		ContactSolver::solve(supports_A,support_count_A,supports_B,support_count_B,p_contact_handler);
	}
	

}


