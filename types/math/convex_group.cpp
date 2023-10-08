//
// C++ Implementation: convex_group
//
// Description:
//
//
// Author: Juan Linietsky <reduz@codenix.com>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "convex_group.h"

#include "variant.h"

bool ConvexGroup::intersects_with_aabb(const AABB& p_aabb) const {

	//cache all points to check against!
	Vector3 points[8] = {
		p_aabb.pos+Vector3( 0		 ,0		, 0		),
		p_aabb.pos+Vector3( 0		 ,0		, p_aabb.size.z	),
		p_aabb.pos+Vector3( 0		 ,p_aabb.size.y	, 0		),
		p_aabb.pos+Vector3( 0		 ,p_aabb.size.y	, p_aabb.size.z	),
		p_aabb.pos+Vector3(p_aabb.size.x,0		, 0		),
		p_aabb.pos+Vector3(p_aabb.size.x,0		, p_aabb.size.z	),
		p_aabb.pos+Vector3(p_aabb.size.x,p_aabb.size.y	, 0		),
		p_aabb.pos+Vector3(p_aabb.size.x,p_aabb.size.y	, p_aabb.size.z	),
	};



	read_lock();
	const Plane *planes=read();

	for (int i=0;i<size();i++) { //for each plane

		const Plane & plane=planes[i];
		bool all_points_over=true;
		//test if it has all points over!

		for (int j=0;j<8;j++) {


			if (!plane.is_point_over( points[j] )) {

				all_points_over=false;
				break;
			}

		}

		if (all_points_over) {

			read_unlock();
			return false;
		}
	}

	read_unlock();

	return true;
}

bool ConvexGroup::intersects(Vector3* p_points, int p_count) const {
	
	read_lock();
	const Plane *planes=read();

	for (int i=0;i<size();i++) { //for each plane

		const Plane & plane=planes[i];
		bool all_points_over=true;
		//test if it has all points over!

		for (int j=0;j<p_count;j++) {


			if (!plane.is_point_over( p_points[j] )) {

				all_points_over=false;
				break;
			}

		}

		if (all_points_over) {

			read_unlock();
			return false;
		}
	}

	read_unlock();

	return true;
};


void ConvexGroup::create_from_projection(const Matrix4& p_proj_matrix) {

	clear();
	/** Fast Plane Extraction from combined modelview/projection matrices.
	 * References:
	 * http://www.markmorley.com/opengl/frustumculling.html
	 * http://www2.ravensoft.com/users/ggribb/plane%20extraction.pdf
	 */
	 
	
	const real_t * matrix = (const real_t*)p_proj_matrix.elements;
			
	resize(6);
	
	Plane new_plane;

	///////--- Near Plane ---///////
	new_plane=Plane(matrix[ 3] + matrix[ 2],
		      matrix[ 7] + matrix[ 6],
		      matrix[11] + matrix[10],
		      matrix[15] + matrix[14]);
	
	new_plane.normal=-new_plane.normal;
	new_plane.normalize();

	set(0,new_plane);

	///////--- Far Plane ---///////
	new_plane=Plane(matrix[ 3] - matrix[ 2],
		      matrix[ 7] - matrix[ 6],
		      matrix[11] - matrix[10],
		      matrix[15] - matrix[14]);

	new_plane.normal=-new_plane.normal;
	new_plane.normalize();

	set(1,new_plane);


	///////--- Left Plane ---///////
	new_plane=Plane(matrix[ 3] + matrix[ 0],
		      matrix[ 7] + matrix[ 4],
		      matrix[11] + matrix[ 8],
		      matrix[15] + matrix[12]);
		      
	new_plane.normal=-new_plane.normal;
	new_plane.normalize();

	set(2,new_plane);


	///////--- Top Plane ---///////
	new_plane=Plane(matrix[ 3] - matrix[ 1],
		      matrix[ 7] - matrix[ 5],
		      matrix[11] - matrix[ 9],
		      matrix[15] - matrix[13]);
		      
	
	new_plane.normal=-new_plane.normal;
	new_plane.normalize();

	set(3,new_plane);


	///////--- Right Plane ---///////
	new_plane=Plane(matrix[ 3] - matrix[ 0],
		      matrix[ 7] - matrix[ 4],
		      matrix[11] - matrix[ 8],
		      matrix[15] - matrix[12]);

	
	new_plane.normal=-new_plane.normal;
	new_plane.normalize();
	
	set(4,new_plane);


	///////--- Bottom Plane ---///////
	new_plane=Plane(matrix[ 3] + matrix[ 1],
		      matrix[ 7] + matrix[ 5],
		      matrix[11] + matrix[ 9],
		      matrix[15] + matrix[13]);

	
	new_plane.normal=-new_plane.normal;
	new_plane.normalize();

	set(5,new_plane);
	

}
