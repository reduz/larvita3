//
// C++ Implementation: gjk_separator
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "gjk_separator.h"
#include "contact_solver.h"

bool GJK_Separator::handles( const Volume& p_A, const Volume& p_B ) const {
	
	return !p_A.is_concave() && !p_B.is_concave();
}

#define GJK_INSIMPLEX_EPSILON 0.0001f
#define GJK_INSIMPLEX_EPSILON2 (GJK_INSIMPLEX_EPSILON*GJK_INSIMPLEX_EPSILON)
#define MPR_COLLIDE_EPSILON 0.00001f

//#define printf(...)

class GJK_Solver {
	
	GJK_Debug *debug;
	enum {
		
		MAX_ITERATIONS=64
	};
	
	const Volume *A;
	const Volume *B;
	Matrix4 transform_A;
	Matrix4 transform_B;
	
	int order;
	bool success;
	
	struct Vertex {
		
		Vector3 pos;
		Vector3 ray;	
	};
	
	Vertex simplex[5];
	
	inline Vertex get_minkowski_support(const Vector3& p_ray_normal) {
		
		Vector3 sA,sB;
		int _c;
		A->get_support(p_ray_normal,transform_A,&sA,&_c,1);
		B->get_support(-p_ray_normal,transform_B,&sB,&_c,1);
		
		Vertex v;
		v.ray=p_ray_normal;
		// minkowski difference is A-B, this returns
		// the swept of one volume by the other in a requested direction
		v.pos=sA-sB; 
		
		return v;		
	}
	
	inline bool fetch_support(const Vector3& p_ray_normal) {
		
		// each new ray increases the simplex complexity, 
		// which then we will later attempt to simplify and reduce
		order++;
		simplex[order]=get_minkowski_support( p_ray_normal );
		printf("support for %f,%f,%f is %f,%f,%f\n",p_ray_normal.x,p_ray_normal.y,p_ray_normal.z,simplex[order].pos.x,simplex[order].pos.y,simplex[order].pos.z);
		//
		// we must check that the whole minkowski difference isn't
		// below the plane passing by the origin, (<>* .-> situation) 
		// with the ray normal,
		// because if so, then both shapes are not overlapping and
		// we have an early exit condition
		// the >0 comparision (instead of >=0) eliminates the "just touching" case.
		return (p_ray_normal.dot(simplex[order].pos)>0);
	}
	
	inline bool solve_simplex2(const Vector3& ao,const Vector3& ab,Vector3* r_ray_normal)
	{
		// a = simplex[1] , b = simplex[0]
		// ao = direction vector, a to origin
		// ab = direction vector, a to b
		
		if (ab.dot(ao)>=0) {
			
			// for the next ray we'll try to go
			// from the ab segment to the origin, since
			// it's evident that the origin is closer
			// to a point inside by ab segment, than to
			// either a or b themselves
			
			Vector3	cabo(Vector3::cross(ab,ao));
			
			if( cabo.squared_length() > GJK_INSIMPLEX_EPSILON2) {
				// next ray will go from the segment to the origin
				*r_ray_normal=Vector3::cross(cabo,ab); 
			} else {
				// if cabo.squared_length() is zero, it means that
				// both a, b and o are colinear. after fetch_ray (which 
				//discards the "not" case, 
				// this means the segment is passing by the origin
				return true; 
			}
		} else { 
			
			// we are just keeping a ( = simplex[1] ) because
			// it's closer to the origin than ab or b,
			// and then we'll try to approach the origin from it with
			// the next ray
			order=0;
			simplex[0]=simplex[1];
			*r_ray_normal=ao;
		}
		
		return false;
	}
	//
	inline bool solve_simplex3(const Vector3& ao,const Vector3& ab,const Vector3& ac,Vector3* r_ray_normal) {
	
		// a = simplex[2]
		// b = simplex[1]
		// c = simplex[0]
		
		return solve_simplex3a(ao,ab,ac,Vector3::cross(ab,ac),r_ray_normal);
	}
	//
	inline bool solve_simplex3a(const Vector3& ao,const Vector3& ab,const Vector3&  ac,const Vector3& cabc,Vector3* r_ray_normal) {
		
		// a = simplex[2]
		// b = simplex[1]
		// c = simplex[0]
		// cabc = triangle normal
		
		if ( Vector3::cross(cabc,ab).dot(ao)<-GJK_INSIMPLEX_EPSILON) {
			
			order=1;
			simplex[0]=simplex[1];
			simplex[1]=simplex[2];
			
			return solve_simplex2(ao,ab,r_ray_normal);	
		} else if ( Vector3::cross(cabc,ac).dot(ao)>+GJK_INSIMPLEX_EPSILON) { 
			
			order=1;
			simplex[1]=simplex[2];
			return solve_simplex2(ao,ac,r_ray_normal); 
		} else {
			
			
			float d = cabc.dot(ao);
			
			// check the distance from the triangle 
			// to the origin
			
			if( Math::absf(d)>GJK_INSIMPLEX_EPSILON) {
					
				// if not too little, throw a ray, depending
				// on triangle facing towards the origin
				
				if(d>0)	{ 
					*r_ray_normal=cabc; 
				} else {
					*r_ray_normal=-cabc;
					SWAP(simplex[0],simplex[1]); 
				}
				
				return false;
			} else {
				// triangle is right crossing the origin
				return true;
			}
		}
	}
	//
	inline bool solve_simplex4(const Vector3& ao,const Vector3& ab,const Vector3& ac,const Vector3& ad,Vector3* r_ray_normal) {
		Vector3 crs;
		if( (crs=Vector3::cross(ab,ac)).dot(ao)>GJK_INSIMPLEX_EPSILON) { 
			
			order=2;
			simplex[0]=simplex[1];
			simplex[1]=simplex[2];
			simplex[2]=simplex[3];
			return solve_simplex3a(ao,ab,ac,crs,r_ray_normal); 
		} else if ((crs=Vector3::cross(ac,ad)).dot(ao)>GJK_INSIMPLEX_EPSILON) {
			
			order=2;
			simplex[2]=simplex[3];
			
			return solve_simplex3a(ao,ac,ad,crs,r_ray_normal); 
			
		} else if((crs=Vector3::cross(ad,ab)).dot(ao)>GJK_INSIMPLEX_EPSILON) { 
			
			order=2;
			simplex[1]=simplex[0];
			simplex[0]=simplex[2];
			simplex[2]=simplex[3];
			
			return solve_simplex3a(ao,ad,ab,crs,r_ray_normal); 
			
		} else {
			
			return true;
		}
	}
	
	void compute_separation_axis(Vector3 * r_axis) {
				
		switch( order ) {
			
			case 0: {
				/* POINT */
				*r_axis = simplex[0].pos.get_normalized();
			} break;
			case 1: {
				/* SEGMENT */				
				// find closest point in segment
				Vector3 o=-simplex[0].pos;
				Vector3 n=simplex[1].pos-simplex[0].pos;
				float l =n.length();
				if (l<1e-10)
					*r_axis = simplex[0].pos.get_normalized(); // both points are the same, just give any
				n/=l;
					
				float d=n.dot(o);
	
				*r_axis = (simplex[0].pos+n*d).get_normalized();
				
			} break;
			case 2: {
				/* TRIANGLE */
				
				// find closest point in triangle
				/*
				printf("simplex\t %f,%f,%f\n\t\t%f,%f,%f\n\t\t,%f,%f,%f\n",
					simplex[0].pos.x,simplex[0].pos.y,simplex[0].pos.z,
					simplex[1].pos.x,simplex[1].pos.y,simplex[1].pos.z,
					simplex[2].pos.x,simplex[2].pos.y,simplex[2].pos.z );
    */
				Plane p = Plane( simplex[0].pos, simplex[1].pos, simplex[2].pos );
				
				*r_axis=p.normal;
				
			} break;
			default: {
				/* FULL SIMPLEX */
				
				
				float best_depth=1e15;
				Vector3 best_axis;
				
				// test point
				
				{
				
					float l = simplex[order].pos.length();
					if (l<1e-6) { // just touching, discard
						
						*r_axis=Vector3();
						return;
					}						
					printf("test order point, depth %f\n",l);
					if (l<best_depth) {
						best_depth=l;				best_axis=simplex[order].pos/l;
					}
				}
								
				// test edges
				
				for (int i=0;i<order;i++) {
						
					// find closest point in segment
					Vector3 o=-simplex[i].pos;
					Vector3 n=simplex[order].pos-simplex[i].pos;
					float l=n.length();
					Vector3 pos;
					
					if (l<1e-6) {
						
						pos=simplex[i].pos;
					} else {
						n/=l;
					
						float d=n.dot(o);

						pos=simplex[i].pos+n*d;
					}
					
					l = pos.length();
					if (l<1e-6) { // just touching, discard
					
						*r_axis=Vector3();
						return;
					}				
							
					printf("test edge %i:%i, depth %f\n",i,order,l);
					
					if (l<best_depth) {
						best_depth=l;		
						best_axis=pos/l;
					}							
				}
				
				// test triangles 
				
				for (int i=0;i<order;i++) {
					
					for (int j=i+1;j<order;j++) {
						
						Plane p = Plane( simplex[i].pos, simplex[j].pos, simplex[order].pos );
						
						float d = Math::absf(p.d);
						
						if (d<1e-6) {
							// just touching, discard
							*r_axis=Vector3();
							return; 
						}
						
						printf("test triangle %i:%i:%i, depth %f\n",i,j,order,d);
						
						if ( d<best_depth ) {
							best_depth=d;
							best_axis=p.normal;
						}
					}
				}
				
				printf("best depth was %f\n",best_depth);
				*r_axis=best_axis;
				
				/*
				// find closest point in triangle
				printf("simplex\t %f,%f,%f\n\t\t%f,%f,%f\n\t\t,%f,%f,%f\n",
				       simplex[1].pos.x,simplex[1].pos.y,simplex[1].pos.z,
					   simplex[2].pos.x,simplex[2].pos.y,simplex[2].pos.z,
    					simplex[3].pos.x,simplex[3].pos.y,simplex[3].pos.z );
    
				Plane p = Plane( simplex[1].pos, simplex[2].pos, simplex[3].pos );
				
				*r_axis=p.normal;*/
			} break;
			
		}
	}
	
	//	
public:	
	
	bool search_origin(Vector3 *r_axis,const Vector3& p_first_ray=Vector3(0,1,0)) {
		
		if (debug) {
			
			debug->steps.clear();
		}
		
		order=-1;
		success=true;
		Vector3 ray_normal=p_first_ray;
		
		fetch_support(ray_normal);
		
		if (debug) {
					
			GJK_Debug::Step gkdstep;
			gkdstep.ray=ray_normal;			
			gkdstep.support=simplex[order].pos;			
			for (int i=0;i<5;i++)
				gkdstep.simplex[i]=simplex[i].pos;
					
			gkdstep.order=order;
			debug->steps.push_back(gkdstep);
		}
		
		ray_normal = -simplex[0].pos;
		
		for(int i=0;i<MAX_ITERATIONS;i++) {
			
			float ray_len=ray_normal.length();
			ray_normal/=ray_len>0?ray_len:1;
			printf("iteration %i, order %i\n",i,order);
			
			if(fetch_support(ray_normal)) {
				bool found=false;
				
				GJK_Debug::Step gkdstep;
				
				if (debug) {
					
					gkdstep.ray=ray_normal;			
					gkdstep.support=simplex[order].pos;			
				}
				
				switch(order) {
					case 1: {
						found=solve_simplex2(-simplex[1].pos,simplex[0].pos-simplex[1].pos,&ray_normal);
					} break;
					case 2: {
						found=solve_simplex3(-simplex[2].pos,simplex[1].pos-simplex[2].pos,simplex[0].pos-simplex[2].pos,&ray_normal);
					} break;
					case 3: {	
						found=solve_simplex4(-simplex[3].pos,simplex[2].pos-simplex[3].pos,simplex[1].pos-simplex[3].pos,simplex[0].pos-simplex[3].pos,&ray_normal);
					} break;
				}
				
				if (debug) {
					
					for (int i=0;i<5;i++)
						gkdstep.simplex[i]=simplex[i].pos;
					
					gkdstep.order=order;
					
					debug->steps.push_back(gkdstep);
				}
							
				if (found) {
					compute_separation_axis(r_axis);
					printf("GJK found at order %i, axis %f,%f,%f\n",order,r_axis->x,r_axis->y,r_axis->z);
					return true;
				}
				
			} else {				
				return false;
			}
		}
		// went past all iterations
		success=false;
		ERR_FAIL_COND_V(!success,false);
		return false;
		
	}	
	
	GJK_Solver( const Volume& p_A,const Matrix4& p_A_transform, const Volume& p_B, const Matrix4 &p_B_transform,GJK_Debug *_debug ) {
		
		A=&p_A;
		B=&p_B;		
		transform_A=p_A_transform;
		transform_B=p_B_transform;
		debug=_debug;
	}
};


class XC_Solver { // xenocollide solver
	
	const Volume *A;
	const Volume *B;
	Matrix4 transform_A;
	Matrix4 transform_B;

	
	MPR_Debug *mpr_debug;
	
public:		

	bool collide(Vector3* r_separation_axis) {
	
		int supportCount = 0;
	
		// v0 = center of Minkowski sum
		Vector3 v0A = A->get_center(transform_A);
		Vector3 v0B = B->get_center(transform_B);
		Vector3 v0 = v0A - v0B;
		
	
		// Avoid case where centers overlap -- any direction is fine in this case
		if (v0.is_almost_zero()) 
			v0 = Vector3(0.00001f, 0, 0);
	
		// v1 = support in direction of origin
		Vector3 n = -v0;
		n.normalize();
		supportCount++;
		int _c;
		Vector3 v1A,v1B;
		A->get_support(n,transform_A,&v1A,&_c,1);
		B->get_support(-n,transform_B,&v1B,&_c,1);
		Vector3 v1 = v1A-v1B;
		
		if (v1.dot(n) <= 0) {
			if (r_separation_axis)
				*r_separation_axis=n;
			return false;
		}
	
		// v2 - support perpendicular to v1,v0
		n = Vector3::cross(v1,v0);
		n.normalize();
		if (n.is_almost_zero()) {
			
			n = v1 - v0;
			n.normalize();
			if (r_separation_axis)
				*r_separation_axis=n;
			return true;
		}
		
		supportCount++;
		Vector3 v2A,v2B;
		A->get_support(n,transform_A,&v2A,&_c,1);
		B->get_support(-n,transform_B,&v2B,&_c,1);
		Vector3 v2 = v2A-v2B;

		if (v2.dot(n) <= 0) {
			if (r_separation_axis)
				*r_separation_axis=n;
			return false;
		}
	
		// Determine whether origin is on + or - side of plane (v1,v0,v2)
		n = Vector3::cross( (v1 - v0) , (v2 - v0) );
		n.normalize();
		real_t dist = n.dot(v0);
	
		ERR_FAIL_COND_V( n==Vector3(),false );
	
		// If the origin is on the - side of the plane, reverse the direction of the plane
		if (dist > 0) {
			SWAP(v1, v2);
			SWAP(v1A, v2A);
			SWAP(v1B, v2B);
			n = -n;
		}
	
		
		// Phase One: Identify a portal
	
		while (1) {
			// Obtain the support point in a direction perpendicular to the existing plane
			// Note: This point is guaranteed to lie off the plane
			supportCount++;
			
			Vector3 v3A,v3B;
			A->get_support(n,transform_A,&v3A,&_c,1);
			B->get_support(-n,transform_B,&v3B,&_c,1);
			Vector3 v3 = v3A-v3B;
			

			if (v3.dot(n) <= 0) {
				if (r_separation_axis)
					*r_separation_axis=n;
				return false;
			}
			
			// If origin is outside (v1,v0,v3), then eliminate v2 and loop
			if (Vector3::cross(v1,v3).dot(v0) < 0)	{
				v2 = v3;
				v2A = v3A;
				v2B = v3B;
				n = Vector3::cross( (v1 - v0) , (v3 - v0) );
				n.normalize();
				continue;
			}
	
			// If origin is outside (v3,v0,v2), then eliminate v1 and loop
			if ( Vector3::cross( v3 , v2 ).dot(v0) < 0)
			{
				v1 = v3;
				v1A = v3A;
				v1B = v3B;
				n = Vector3::cross((v3 - v0) , (v2 - v0));
				n.normalize();
				continue;
			}
	
			bool hit = false;
	
			// Phase Two: Refine the portal
	
			int phase2 = 0;
	
			// We are now inside of a wedge...
			while (1) {
				phase2++;
	
				// Compute normal of the wedge face
				n = Vector3::cross( (v2 - v1) , (v3 - v1) );
	
				// Can this happen???  Can it be handled more cleanly?
				ERR_FAIL_COND_V( n==Vector3(), true );
	
				n.normalize();
	
				// Compute distance from origin to wedge face
				real_t d = n.dot(v1);
	
				// If the origin is inside the wedge, we have a hit
				if (d >= 0 && !hit) {
	
					if (r_separation_axis)
						*r_separation_axis=n;
	
					
					// Compute the barycentric coordinates of the origin
					/*
					real_t b0 = Vector3::cross( v1 , v2).dot( v3 );
					real_t b1 = Vector3::cross( v3 , v2).dot( v0 );
					real_t b2 = Vector3::cross( v0 , v1).dot( v3 );
					real_t b3 = Vector3::cross( v2 , v1).dot( v0 );
	
					real_t sum = b0 + b1 + b2 + b3;
	
					if (sum <= 0)
					{
	
	
						b0 = 0;
						b1 = Vector3::cross( v2 % v3 ).dot(n);
						b2 = Vector3::cross( v3 % v1 ).dot(n);
						b3 = Vector3::cross( v1 % v2 ).dot(n);
	
						sum = b1 + b2 + b3;
					}
	
					real_t inv = 1.0f / sum;
	
					if (point1)
{
						Vector3 p1 = (b0 * v01 + b1 * v11 + b2 * v21 + b3 * v31) * inv;
						*point1 = p1;
					}
	
					if (point2)
					{
						Vector3 p2 = (b0 * v02 + b1 * v12 + b2 * v22 + b3 * v32) * inv;
						*point2 = p2;
					}
					*/
					// HIT!!!
					hit = true;
				}
	
				// Find the support point in the direction of the wedge face
				supportCount++;
				Vector3 v4A,v4B;
				A->get_support(n,transform_A,&v4A,&_c,1);
				B->get_support(-n,transform_B,&v4B,&_c,1);
				Vector3 v4 = v4A-v4B;
				
	
				real_t delta = (v4 - v3).dot(n);
				real_t separation = -(v4.dot(n));
	
				// If the boundary is thin enough or the origin is outside the support plane for the newly discovered vertex, then we can terminate
				if ( delta <= MPR_COLLIDE_EPSILON || separation >= 0 /*|| phase2 > 300*/ )
				{
					if (r_separation_axis)
						*r_separation_axis=n;
					/*
					// MISS!!! (We didn't move closer)
					static int maxPhase2 = 0;
					static int maxCallCount = 0;
					static real_t avg = 0.0f;
					static int hitCount = 0;
	
					static real_t avgSupportCount = 0.0f;
	
					if (hit)
					{
						hitCount++;
	
						avg = (avg * (hitCount-1) + phase2) / hitCount;
						avgSupportCount = (avgSupportCount * (hitCount-1) + supportCount) / hitCount;
						gAvgSupportCount = avgSupportCount;
	
						if (phase2 > maxPhase2)
						{
							maxPhase2 = phase2;
							gMaxPhase2 = maxPhase2;
							maxCallCount = callCount;
						}
					}
					*/
					return hit;
				}
	
				// Compute the tetrahedron dividing face (v4,v0,v1)
				real_t d1 = Vector3::cross( v4 , v1).dot(v0);
	
				// Compute the tetrahedron dividing face (v4,v0,v2)
				real_t d2 = Vector3::cross( v4 , v2).dot(v0);
	
				// Compute the tetrahedron dividing face (v4,v0,v3)
				real_t d3 = Vector3::cross( v4 , v3).dot(v0);
	
				if (d1 < 0)
				{
					if (d2 < 0)
					{
						// Inside d1 & inside d2 ==> eliminate v1
						v1 = v4;
						v1A = v4A;
						v1B = v4B;
					}
					else
					{
						// Inside d1 & outside d2 ==> eliminate v3
						v3 = v4;
						v3A = v4A;
						v3B = v4B;
					}
				}
				else
				{
					if (d3 < 0)
					{
						// Outside d1 & inside d3 ==> eliminate v2
						v2 = v4;
						v2A = v4A;
						v2B = v4B;
					}
					else
					{
						// Outside d1 & outside d3 ==> eliminate v1
						v1 = v4;
						v1A = v4A;
						v1B = v4B;
					}
				}
			}
		}
	}
	
	XC_Solver( const Volume& p_A,const Matrix4& p_A_transform, const Volume& p_B, const Matrix4 &p_B_transform,MPR_Debug *p_mpr_debug) {
		
		A=&p_A;
		B=&p_B;		
		transform_A=p_A_transform;
		transform_B=p_B_transform;
		mpr_debug=p_mpr_debug;
	}
	
};


//

void GJK_Separator::separate( const Volume& p_A,const Matrix4& p_A_transform, const Volume& p_B, const Matrix4 &p_B_transform, VolumeContactHandler *p_contact_handler) const {
	
	//printf("A is %i, B is %i\n",p_A.get_volume_type(),p_B.get_volume_type());
	//GJK_Solver solver( p_A,p_A_transform, p_B, p_B_transform, debug );
	XC_Solver xc_solver( p_A,p_A_transform, p_B, p_B_transform, mpr_debug );
	
	if (mpr_debug) {
			
		mpr_debug->ray=Vector3();
	}
	
	Vector3 sep_axis;
	//if (solver.search_origin(&sep_axis)) {
	
	if (xc_solver.collide(&sep_axis)) {
		//printf("xc determined collision\n");	
		
		sep_axis.normalize();
		//printf("XC separation axis %f,%f,%f\n",sep_axis.x,sep_axis.y,sep_axis.z);
		
		if (mpr_debug) {
			
			mpr_debug->ray=sep_axis;
		}
			
		if (sep_axis.squared_length() < 1e-6 )
			return; // too small, probably a just-touching case, ignore
		
		float best_depth=1e15;
		Vector3 best_axis;
		
		float minA,maxA,minB,maxB;
		
		p_A.project_range(sep_axis,p_A_transform,minA,maxA);
		p_B.project_range(sep_axis,p_B_transform,minB,maxB);
		
		
		if (!test_projection(sep_axis,best_depth,best_axis,minA,maxA,minB,maxB)) {
		//	printf("projection determined nocollision, axis %f,%f,%f, best depth %f\n",sep_axis.x,sep_axis.y,sep_axis.z,best_depth);
			return; // nothing happened
		}
		
		Vector3 supports_A[Volume::MAX_SUPPORTS];
		int support_count_A;
		p_A.get_support(-best_axis,p_A_transform,supports_A,&support_count_A,Volume::MAX_SUPPORTS);
		
		Vector3 supports_B[Volume::MAX_SUPPORTS];
		int support_count_B;
		p_B.get_support(best_axis,p_B_transform,supports_B,&support_count_B,Volume::MAX_SUPPORTS);
		
		/*
		for (int i=0;i<support_count_A;i++)
			printf("%i- support A %f,%f,%f\n",i,supports_A[i].x,supports_A[i].y,supports_A[i].z);
		
		for (int i=0;i<support_count_B;i++)
			printf("%i- support B %f,%f,%f\n",i,supports_B[i].x,supports_B[i].y,supports_B[i].z);
		*/
		if (p_contact_handler)
			ContactSolver::solve(supports_A,support_count_A,supports_B,support_count_B,p_contact_handler);
				
	}
}


