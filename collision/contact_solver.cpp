//
// C++ Implementation: contact_solver
//
// Description:
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "contact_solver.h"
#include "polygon_clipper.h"

/* helper used by a few solvers */
static Vector3 get_closest_point_to_segment(const Vector3& p_point, const Vector3 *p_segment) {

	Vector3 p=p_point-p_segment[0];
	Vector3 n=p_segment[1]-p_segment[0];
	float l =n.length();
	if (l<1e-10)
		return p_segment[0]; // both points are the same, just give any
	n/=l;

	float d=n.dot(p);

	if (d<=0.0)
		return p_segment[0]; // before first point
	else if (d>=l)
		return p_segment[1]; // after first point
	else
		return p_segment[0]+n*d; // inside


}


static void solve_point_edge(const Vector3& p_pointA, const Vector3 *p_edgeB,VolumeContactHandler *p_contact_handler,bool p_swap) {


	Vector3 closest = get_closest_point_to_segment( p_pointA, p_edgeB );

	VolumeContact vc;
	vc.A=p_pointA;
	vc.B=closest;
	if (p_swap)
		vc.invert();
		/*
	if (p_swap) {
		printf("A closest point to edge is %f,%f,%f\n",vc.A.x,vc.A.y,vc.A.z);
		printf("B point is %f,%f,%f\n",vc.B.x,vc.B.y,vc.B.z);
	} else {
		printf("A point is %f,%f,%f\n",vc.A.x,vc.A.y,vc.A.z);
		printf("B closest point to edge is %f,%f,%f\n",vc.B.x,vc.B.y,vc.B.z);
	}
*/
	p_contact_handler->add_contact(vc,true);

}

static void solve_point_poly(const Vector3& p_pointA, const Vector3 *p_polyB,VolumeContactHandler *p_contact_handler,bool p_swap) {


	/* polygons are always clockwise, which is waht the plane uses as default */
	Plane plane_B=Plane(p_polyB[0],p_polyB[1],p_polyB[2]);

	//printf("POINT-POLY: %f,%f,%f - POLY: %f,%f,%f : %f\n",p_pointA.x,p_pointA.y,p_pointA.z,plane_B.normal.x,plane_B.normal.y,plane_B.normal.z,plane_B.d);
	Vector3 closest = p_pointA - plane_B.normal * plane_B.distance_to_point( p_pointA );

	VolumeContact vc;
	vc.A=p_pointA;
	vc.B=closest;

	if (p_swap)
		vc.invert();

	p_contact_handler->add_contact(vc,true);

}

static void solve_edge_edge(const Vector3 *p_edgeA, const Vector3 *p_edgeB,VolumeContactHandler *p_contact_handler) {


	Vector3 eAv=p_edgeA[1]-p_edgeA[0];
	Vector3 eBv=p_edgeB[1]-p_edgeB[0];

	Vector3 c=Vector3::cross(Vector3::cross(eAv,eBv),eBv);

	if ( ABS(eAv.dot(c) )<CMP_EPSILON ) {

#ifndef WINDOWS_ENABLED
#warning, must handle this case
#endif
		//ERR_PRINT("Unhandled");
		return;

	} else {


		float d = (c.dot( p_edgeB[0] ) - p_edgeA[0].dot(c))/eAv.dot(c);
		if (d<0.0)
			d=0.0;
		if (d>1.0)
			d=1.0;

		VolumeContact vc;
		vc.A=p_edgeA[0]+eAv*d;
		vc.B=get_closest_point_to_segment(vc.A, p_edgeB);
		/*
		printf("vc.A point to edge A is %f,%f,%f\n",vc.A.x,vc.A.y,vc.A.z);
		printf("vc.B point to edge B is %f,%f,%f\n",vc.B.x,vc.B.y,vc.B.z);
		*/
		p_contact_handler->add_contact(vc,true);
	}


}



static void solve_poly_poly(const Vector3 *p_polyA,int p_A_count,const Vector3 *p_polyB,int p_B_count,VolumeContactHandler *p_contact_handler,bool p_swap) {

	/* CLIP A BY B */

	static PolygonClipper clipper;

	ERR_FAIL_COND( p_B_count < 3 ); // couldn't clip anything otherwise
	ERR_FAIL_COND( p_A_count < 2 ); // A can be 2 minimum since it's going to be clipped. This makes it work for edges

	clipper.clear();

	// this always works because supports are sorted clockwise
	Plane plane_B=Plane(p_polyB[0],p_polyB[1],p_polyB[2]);

	/* Add points of A into clipper */
	for (int i=0;i<p_A_count;i++) {

		clipper.add_point(p_polyA[i]);

	}

	/* Make planes for B and use them for clipping */

	int prev=p_B_count-1;

	for (int i=0;i<p_B_count;i++) {


		Vector3 pb1=p_polyB[i];
		Vector3 pb2=p_polyB[prev];

		Plane p( pb1, Vector3::cross(plane_B.normal,pb1-pb2).get_normalized());
		clipper.clip(p);

		prev=i;
	}

	/* Collect points and turn them into contacts */

	for (int i=0;i<clipper.get_point_count();i++) {


		VolumeContact vc;
		vc.A=clipper.get_point(i);
		vc.B=vc.A - plane_B.normal*plane_B.distance_to_point(vc.A);

		if (plane_B.distance_to_point(vc.A)>0.00001)
			continue;

		if (p_swap)
			vc.invert();

		p_contact_handler->add_contact(vc,true);

	}
}

static void solve_edge_poly(const Vector3 *p_edgeA,const Vector3 *p_polyB,int p_B_count,VolumeContactHandler *p_contact_handler,bool p_swap) {

	solve_poly_poly(p_edgeA,2,p_polyB,p_B_count,p_contact_handler,p_swap);
}


void ContactSolver::solve(const Vector3 * p_A_points,int p_A_count, const Vector3 * p_B_points,int p_B_count,VolumeContactHandler *p_contact_handler) {


//	printf("A: %i , B: %i\n",p_A_count,p_B_count);
	/*
	printf("\t-A: \n");
	for (int i=0;i<p_A_count;i++)
		printf("\t\t%f,%f,%f\n",p_A_points[i].x,p_A_points[i].y,p_A_points[i].z);
	printf("\t-B: \n");
	for (int i=0;i<p_B_count;i++)
		printf("\t\t%f,%f,%f\n",p_B_points[i].x,p_B_points[i].y,p_B_points[i].z);
	*/
	ERR_FAIL_COND(p_A_count<=0);
	ERR_FAIL_COND(p_B_count<=0);

	if (p_A_count==1 && p_B_count==1) {
		/* point VS point, simple matter */
		p_contact_handler->add_contact( VolumeContact( *p_A_points, *p_B_points ),true );

	} else if (p_A_count==1 && p_B_count==2) {
		/* point-edge */
		solve_point_edge(*p_A_points,p_B_points,p_contact_handler,false);
	} else if (p_A_count==2 && p_B_count==1) {
		/* point-edge,swapped */
		solve_point_edge(*p_B_points,p_A_points,p_contact_handler,true);
	} else if (p_A_count==1 && p_B_count>2) {
		/* point-poly */
		solve_point_poly(*p_A_points,p_B_points,p_contact_handler,false);
	} else if (p_A_count>2 && p_B_count==1) {
		/* point-poly,swapped */
		solve_point_poly(*p_B_points,p_A_points,p_contact_handler,true);
	} else if (p_A_count==2 && p_B_count==2) {
		/* edge-edge */
		solve_edge_edge(p_A_points,p_B_points,p_contact_handler);
	} else if (p_A_count==2 && p_B_count>2) {
		/* edge-poly */
		solve_edge_poly(p_A_points,p_B_points,p_B_count,p_contact_handler,false);
	} else if (p_A_count>2 && p_B_count==2) {
		/* edge-poly,swapped */
		solve_edge_poly(p_B_points,p_A_points,p_A_count,p_contact_handler,true);
	} else if (p_A_count>2 && p_B_count>2) {
		/* poly-poly */
		solve_poly_poly(p_A_points,p_A_count,p_B_points,p_B_count,p_contact_handler,false);
	} else {

		ERR_PRINT("BUG TRAP");
	}

}




