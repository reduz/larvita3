//
// C++ Implementation: convex_separator
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "convex_separator.h"
#include "convex_polygon_volume.h"
#include "contact_solver.h"

bool ConvexSeparator::handles( const Volume& p_A, const Volume& p_B ) const {

	return p_A.get_volume_type()==Volume::CONVEX_POLYGON && p_B.get_volume_type()==Volume::CONVEX_POLYGON;
}
void ConvexSeparator::separate( const Volume& p_A,const Matrix4& p_A_transform, const Volume& p_B, const Matrix4 &p_B_transform, VolumeContactHandler *p_contact_handler) const {
	
	const ConvexPolygonVolume *A=SAFE_CAST<const ConvexPolygonVolume*>(&p_A);
	const ConvexPolygonVolume *B=SAFE_CAST<const ConvexPolygonVolume*>(&p_B);
	
	float best_depth=1e15;
	Vector3 best_axis;
	
	/* SIMPLE SAT (SEPARATING AXIS THEOREM) */
	
	for (int i=0;i<A->get_face_count();i++) {
	
		Vector3 axis=A->get_face_plane(i).normal;
		p_A_transform.transform_no_translation(axis);
		
		float minA,maxA,minB,maxB;
		
		A->project_range(axis,p_A_transform,minA,maxA);
		B->project_range(axis,p_B_transform,minB,maxB);
		
		if (!test_projection(axis,best_depth,best_axis,minA,maxA,minB,maxB))
			return;

			
	}
	
	for (int i=0;i<B->get_face_count();i++) {
	
		Vector3 axis=B->get_face_plane(i).normal;
		p_B_transform.transform_no_translation(axis);
		axis=-axis;
		
		float minA,maxA,minB,maxB;
		
		A->project_range(axis,p_A_transform,minA,maxA);
		B->project_range(axis,p_B_transform,minB,maxB);
		
		if (!test_projection(axis,best_depth,best_axis,minA,maxA,minB,maxB))
			return;

	}


	for(int i=0;i<A->get_edge_count();i++) {

		for(int j=0;j<B->get_edge_count();j++) {
		
			Vector3 e1=A->get_edge_normal(i);
			p_A_transform.transform_no_translation(e1);
			Vector3 e2=B->get_edge_normal(j);
			p_B_transform.transform_no_translation(e2);
		
			Vector3 axis=Vector3::cross( e1, e2 );
			
			if (axis.squared_length()<0.0001)
				continue; // coplanar
			axis.normalize();
				
			float minA,maxA,minB,maxB;
			
			A->project_range(axis,p_A_transform,minA,maxA);
			B->project_range(axis,p_B_transform,minB,maxB);
			
			if (!test_projection(axis,best_depth,best_axis,minA,maxA,minB,maxB))
				return;
		

		}
	}
	
	ERR_FAIL_COND(best_depth==1e15); //nothing found?
	
	Vector3 supports_A[Volume::MAX_SUPPORTS];
	int support_count_A;
	A->get_support(-best_axis,p_A_transform,supports_A,&support_count_A,Volume::MAX_SUPPORTS);
	
	/*
	printf("best axis: %f,%f,%f (%f depth)\n",best_axis.x,best_axis.y,best_axis.z,best_depth);
	
	for (int i=0;i<support_count_A;i++) {
	
		printf("A support %i: %f,%f,%f\n",i,supports_A[i].x,supports_A[i].y,supports_A[i].z);
	}
	*/
	//printf("A POS: %f,%f,%f\n",p_A_transform.get_translation().x,p_A_transform.get_translation().y,p_A_transform.get_translation().z);
	//printf("support A: %f,%f,%f\n",-best_axis.x,-best_axis.y,-best_axis.z);
	
	Vector3 supports_B[Volume::MAX_SUPPORTS];
	int support_count_B;
	B->get_support(best_axis,p_B_transform,supports_B,&support_count_B,Volume::MAX_SUPPORTS);
/*
	for (int i=0;i<support_count_B;i++) {
	
		printf("B support %i: %f,%f,%f\n",i,supports_B[i].x,supports_B[i].y,supports_B[i].z);
	}
*/
	//printf("B POS: %f,%f,%f\n",p_B_transform.get_translation().x,p_B_transform.get_translation().y,p_B_transform.get_translation().z);
//	printf("Axis: %f,%f,%f\n",best_axis.x,best_axis.y,best_axis.z);

/*
	for (int i=0;i<support_count_B;i++)
		CollisionDebugger::singleton->plot_contact(supports_B[i]);
	for (int i=0;i<support_count_A;i++)
		CollisionDebugger::singleton->plot_contact(supports_A[i]);*/
	//CollisionDebugger::singleton->plot_contact(p_contact.B);

	ContactSolver::solve(supports_A,support_count_A,supports_B,support_count_B,p_contact_handler);
	

}


