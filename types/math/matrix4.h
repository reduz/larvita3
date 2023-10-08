//
// C++ Interface: matrix4
//
// Description:
//
//
// Author: red,,, <red@hororo>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef MATRIX4_H
#define MATRIX4_H

#include "typedefs.h"
#include "vector3.h"
#include "plane.h"
#include "rect3.h"
#include "math_funcs.h"

/**
 * Generic 4x4 Transform Matrix.
 * It is used for rotation+translation, or projection.
 */

SCRIPT_BIND_BEGIN;
class Quat;

class Matrix4 {
		/******************************************************/
		/* A Matrix4 is an array of sixteen 1.19.12 fracts    */
		/*     | m1 m5 m9  m13 |                              */
		/*     | m2 m6 m10 m14 |                              */
		/* M = | m3 m7 m11 m15 |                              */
		/*     | m4 m8 m12 m16 |                              */
		/*						      */
		/* Caution                                            */
		/* A Matrix4 is defined as follows:		      */
		/* M[i][j] where i are the columns and j are the rows */
		/* In C, i are rows and j columns. (in C, M[1][0]=m2) */
		/* This is the same as OpenGL format		      */
		/******************************************************/
public:


	SCRIPT_BIND_END;

	real_t elements[4][4];	    		///< the class main elements
	SCRIPT_BIND_BEGIN;

public:
	static const Matrix4 IDENTITY;


	real_t get(int column, int row) const { return elements[column][row];}
	void set(int column, int row,real_t p_v) { elements[column][row]=p_v;}

	/**
	* ///< these functions are used to assign values to an already instantiated Vector3 class
	*/
	const Matrix4& set(real_t _11, real_t _12, real_t _13, real_t _14,
		real_t _21, real_t _22, real_t _23, real_t _24,
		real_t _31, real_t _32, real_t _33, real_t _34,
		real_t _41, real_t _42, real_t _43, real_t _44);

	_FORCE_INLINE_ void set(const Matrix4 &p_m1) {
			elements[0][0] = p_m1.elements[0][0]; elements[0][1] = p_m1.elements[0][1]; elements[0][2] = p_m1.elements[0][2]; elements[0][3] = p_m1.elements[0][3];
			elements[1][0] = p_m1.elements[1][0]; elements[1][1] = p_m1.elements[1][1]; elements[1][2] = p_m1.elements[1][2]; elements[1][3] = p_m1.elements[1][3];
			elements[2][0] = p_m1.elements[2][0]; elements[2][1] = p_m1.elements[2][1]; elements[2][2] = p_m1.elements[2][2]; elements[2][3] = p_m1.elements[2][3];
			elements[3][0] = p_m1.elements[3][0]; elements[3][1] = p_m1.elements[3][1]; elements[3][2] = p_m1.elements[3][2]; elements[3][3] = p_m1.elements[3][3];
		
	}

	void set_col(int p_col, real_t _1, real_t _2, real_t _3, real_t _4);

	void transpose();

	/**
	* ///< informative functions, are used to retrieve useful data about the instantiated class
	*/
	bool is_zero() const;
	bool is_identity() const;
	bool cmp(const Matrix4 &p_m1) const;
//	void get_translation(Vector3 &p_v1) const;
	Vector3 get_translation() const;
	void set_translation(const Vector3& p_trans); ///< WARNING: this REPLACES the translation in the matrix, to translate properly, use translate()

	void clear_translation();
	void clear_rotation_scaling();

	/**
	* ///< math functions, these functions results in changing the instantiated class based on equations
	*/

	const Matrix4& set_lookat(const Vector3 &eye, const Vector3 &target, const Vector3 &up);
	const Matrix4& set_projection(real_t fov, real_t aspect, real_t z_near, real_t z_far);
	const Matrix4& set_orthogonal(real_t left, real_t right, real_t bottom, real_t top, real_t z_near, real_t z_far);
	const Matrix4& set_frustum(real_t left, real_t right, real_t bottom, real_t top, real_t z_near, real_t z_far);

	real_t get_z_near() const;
	real_t get_z_far() const;
	void get_viewport_size(real_t& r_width, real_t& r_height) const;



	bool set_as_inverse();
	Matrix4 inverse() const;

	Matrix4 set_as_inverse4x3();
	Matrix4 inverse4x3() const;

	void orthonormalize();

	Vector3 get_scale() const;

	const Matrix4& add(const Matrix4 &p_m1);
	const Matrix4& add(const Matrix4 &p_m1, const Matrix4 &p_m2);
	Matrix4 operator +(const Matrix4 &p_m1);

	const Matrix4& sub(const Matrix4 &p_m1);
	const Matrix4& sub(const Matrix4 &p_m1, const Matrix4 &p_m2);
	Matrix4 operator -(const Matrix4 &p_m1);

	const Matrix4& mul(const Matrix4 &p_m1);
	const Matrix4& mul(const Matrix4 &p_m1, const Matrix4 &p_m2);
	_FORCE_INLINE_ Matrix4 operator *(const Matrix4 &p_m1) const;

	bool operator ==(const Matrix4 &p_m1) const { return cmp(p_m1); }

SCRIPT_BIND_END;

	// operators not supported by tolua
	_FORCE_INLINE_ const real_t* operator[](int row) const { return elements[row]; } ///< overloaded [] to access elements[][] directly
	real_t* operator[](int row) { return elements[row]; } ///<
	bool operator !=(const Matrix4 &p_m1) const { return !cmp(p_m1); }
	void operator +=(const Matrix4 &p_m1);
	void operator -=(const Matrix4 &p_m1);
	_FORCE_INLINE_ void operator *=(const Matrix4 &p_m1);

SCRIPT_BIND_BEGIN;

public:
	/**
	* ///< transformation functions, these functions results in changing the instantiated class based on equations
	*/

	const Matrix4 &load_zero();
	const Matrix4 &load_identity();
	const Matrix4 &load_translate(real_t tx, real_t ty, real_t tz);
	const Matrix4 &load_translate(const Vector3 &p_vector);
	const Matrix4 &load_rotate_euler(const Vector3 &p_angles); ///< In X (yaw), Y (pitch), Z (roll) order

	const Matrix4 &rebuild_from_rotated_vector_and_roll(const Vector3& p_initial,const Vector3& p_final,real_t p_roll);

	const Matrix4 &translate(real_t tx, real_t ty, real_t tz);
	const Matrix4 &translate(const Vector3& T);
	const Matrix4 &translate_neg(const Vector3& T);
	const Matrix4 &load_scale(real_t sx, real_t sy, real_t sz);
	const Matrix4 &load_scale(const Vector3 &p_scale);
	const Matrix4 &scale(real_t sx, real_t sy, real_t sz);
	const Matrix4 &scale(const Vector3& S);

	const Matrix4 &rotate_x(real_t phi);
	const Matrix4 &rotate_y(real_t phi);
	const Matrix4 &rotate_z(real_t phi);
	const Matrix4 &load_rotate(const Quat &q);
	const Matrix4 &load_rotate(const Vector3 &p_axis, real_t p_phi);
	const Matrix4 &rotate(const Quat &q);
	const Matrix4 &rotate(const Vector3 &p_axis, real_t p_phi);
	const Matrix4 &rotate_euler(const Vector3 &p_angles);
	bool get_euler_rotation(real_t &pitch, real_t &yaw, real_t &roll);
	void set_euler_rotation(real_t pitch, real_t yaw, real_t roll);
	
	_FORCE_INLINE_ Vector3 xform(const Vector3 &p_v1) const;
	_FORCE_INLINE_ Vector3 xform_inv(const Vector3 &p_v1) const;
	_FORCE_INLINE_ Plane xform(const Plane &p_v1) const;
	_FORCE_INLINE_ Plane xform_inv(const Plane &p_v1) const;
	_FORCE_INLINE_ AABB xform(const AABB &p_v1) const;
	_FORCE_INLINE_ AABB xform_inv(const AABB &p_v1) const;


//	const Matrix4 &rotate_euler(real_t p_yaw, real_t p_pitch, real_t p_roll); -- sorry, just no.


	_FORCE_INLINE_ void transform(Vector3 &p_v1) const;
	_FORCE_INLINE_ void transform_copy(const Vector3 &p_src,Vector3 &r_dst) const;
	
	_FORCE_INLINE_ void transform_no_translation(Vector3 &p_v1) const;
	_FORCE_INLINE_ void transform_aabb(AABB &p_aabb) const;
	_FORCE_INLINE_ void transform_aabb_no_translation(AABB &p_aabb) const;
	_FORCE_INLINE_ void inverse_transform_aabb(AABB &p_aabb) const;

	void transform(Plane &p_plane) const;
	void inverse_transform(Plane &p_plane) const;

	_FORCE_INLINE_ void inverse_transform(Vector3 &p_v1) const;
	_FORCE_INLINE_ void inverse_transform_no_translation(Vector3 &p_v1) const;

    Vector3 get_x() const;
    Vector3 get_y() const;
    Vector3 get_z() const;

	void set_cylindrical_billboard(const Matrix4 &p_camera_matrix,
				const Matrix4 &p_transformation_matrix,
				Matrix4 *p_inverse_camera_matrix = NULL);

	void set_spherical_billboard(const Matrix4 &p_camera_matrix,
				const Matrix4 &p_transformation_matrix,
				Matrix4 *p_inverse_camera_matrix = NULL,
					Vector3 *p_scale_factor = NULL);

	/**
	* ///< Constructors/Destructor
	*/
	Matrix4(const Quat& p_quat);
	_FORCE_INLINE_ Matrix4() { 
		
		elements[0][0] = 1;  elements[1][0] = 0;  elements[2][0] = 0;  elements[3][0] = 0;
		elements[0][1] = 0;  elements[1][1] = 1;  elements[2][1] = 0;  elements[3][1] = 0;
		elements[0][2] = 0;  elements[1][2] = 0;  elements[2][2] = 1;  elements[3][2] = 0;
		elements[0][3] = 0;  elements[1][3] = 0;  elements[2][3] = 0;  elements[3][3] = 1;
	
	}; //{ zero(); };
	Matrix4(real_t _11, real_t _12, real_t _13, real_t _14,
			real_t _21, real_t _22, real_t _23, real_t _24,
			real_t _31, real_t _32, real_t _33, real_t _34,
			real_t _41, real_t _42, real_t _43, real_t _44) { set(_11,_12,_13,_14,_21,_22,_23,_24,_31,_32,_33,_34,_41,_42,_43,_44); }


};

SCRIPT_BIND_END;

#ifndef CUSTOM_MATRIX_TRANSFORM_H //this can be overriden with a specific verion for transforming a matrix

inline void Matrix4::transform(Vector3 &p_v1) const {
		real_t x = p_v1.x;
		real_t y = p_v1.y;
		real_t z = p_v1.z;
		p_v1.x= (elements[0][0]*x ) + ( elements[1][0]*y ) + ( elements[2][0]*z ) + elements[3][0];
		p_v1.y= (elements[0][1]*x ) + ( elements[1][1]*y ) + ( elements[2][1]*z ) + elements[3][1];
		p_v1.z= (elements[0][2]*x ) + ( elements[1][2]*y ) + ( elements[2][2]*z ) + elements[3][2];
}

inline Vector3 Matrix4::xform(const Vector3 &p_v1) const {
		real_t x = p_v1.x;
		real_t y = p_v1.y;
		real_t z = p_v1.z;
		return Vector3(
		 (elements[0][0]*x ) + ( elements[1][0]*y ) + ( elements[2][0]*z ) + elements[3][0],
		 (elements[0][1]*x ) + ( elements[1][1]*y ) + ( elements[2][1]*z ) + elements[3][1],
		(elements[0][2]*x ) + ( elements[1][2]*y ) + ( elements[2][2]*z ) + elements[3][2]
		);
}
inline Vector3 Matrix4::xform_inv(const Vector3 &p_v1) const {
	real_t x = p_v1.x-elements[3][0];
	real_t y = p_v1.y-elements[3][1];
	real_t z = p_v1.z-elements[3][2];
	return Vector3(
	 (elements[0][0]*x ) + ( elements[0][1]*y ) + ( elements[0][2]*z ),
	 (elements[1][0]*x ) + ( elements[1][1]*y ) + ( elements[1][2]*z ),
	 (elements[2][0]*x ) + ( elements[2][1]*y ) + ( elements[2][2]*z )
	 );
}

inline AABB Matrix4::xform(const AABB &p_v1) const {

	AABB aabb=p_v1;
	transform_aabb(aabb);
	return aabb;
}
inline AABB Matrix4::xform_inv(const AABB &p_v1) const {

	AABB aabb=p_v1;
	inverse_transform_aabb(aabb);
	return aabb;

}

inline Plane Matrix4::xform_inv(const Plane &p_v1) const {
	Plane p=p_v1;
	inverse_transform(p);
	return p;	
}

inline Plane Matrix4::xform(const Plane &p_v1) const {
	Plane p=p_v1;
	transform(p);
	return p;	
}


inline void Matrix4::transform_copy(const Vector3 &p_src,Vector3 &r_dst) const {
	
	r_dst.x= (elements[0][0]*p_src.x ) + ( elements[1][0]*p_src.y ) + ( elements[2][0]*p_src.z ) + elements[3][0];
	r_dst.y= (elements[0][1]*p_src.x ) + ( elements[1][1]*p_src.y ) + ( elements[2][1]*p_src.z ) + elements[3][1];
	r_dst.z= (elements[0][2]*p_src.x ) + ( elements[1][2]*p_src.y ) + ( elements[2][2]*p_src.z ) + elements[3][2];
	

}


inline void Matrix4::transform_no_translation(Vector3 &p_v1) const {

		real_t x = p_v1.x;
		real_t y = p_v1.y;
		real_t z = p_v1.z;
		p_v1.x= (elements[0][0]*x ) + ( elements[1][0]*y ) + ( elements[2][0]*z );
		p_v1.y= (elements[0][1]*x ) + ( elements[1][1]*y ) + ( elements[2][1]*z );
		p_v1.z= (elements[0][2]*x ) + ( elements[1][2]*y ) + ( elements[2][2]*z );
}

inline void Matrix4::inverse_transform(Vector3 &p_v1) const {
	real_t x = p_v1.x-elements[3][0];
	real_t y = p_v1.y-elements[3][1];
	real_t z = p_v1.z-elements[3][2];
	p_v1.x= (elements[0][0]*x ) + ( elements[0][1]*y ) + ( elements[0][2]*z );
	p_v1.y= (elements[1][0]*x ) + ( elements[1][1]*y ) + ( elements[1][2]*z );
	p_v1.z= (elements[2][0]*x ) + ( elements[2][1]*y ) + ( elements[2][2]*z );
}

inline void Matrix4::inverse_transform_no_translation(Vector3 &p_v1) const {
	real_t x = p_v1.x;
	real_t y = p_v1.y;
	real_t z = p_v1.z;
	p_v1.x= (elements[0][0]*x ) + ( elements[0][1]*y ) + ( elements[0][2]*z );
	p_v1.y= (elements[1][0]*x ) + ( elements[1][1]*y ) + ( elements[1][2]*z );
	p_v1.z= (elements[2][0]*x ) + ( elements[2][1]*y ) + ( elements[2][2]*z );
}

inline Matrix4 Matrix4::operator *(const Matrix4 &p_m1) const {
	Matrix4 new_matrix;

	for( int j = 0; j < 4; j++ ) {
		for( int i = 0; i < 4; i++ ) {
			real_t ab = 0;
			for( int k = 0; k < 4; k++ )
				ab += elements[k][i] * p_m1.elements[j][k];
			new_matrix.elements[j][i] = ab;
		}
	}

	return new_matrix;
}

inline void Matrix4::operator *=(const Matrix4 &p_m1)
{
	Matrix4 new_matrix;

	for( int j = 0; j < 4; j++ ) {
		for( int i = 0; i < 4; i++ ) {
			real_t ab = 0;
			for( int k = 0; k < 4; k++ )
				ab += elements[k][i] * p_m1.elements[j][k] ;
			new_matrix.elements[j][i] = ab;
		}
	}

	set(new_matrix);
}


inline void Matrix4::transform_aabb(AABB &p_aabb) const {
	
	
	/* define vertices */
	Vector3 vertices[8]={
			p_aabb.pos+Vector3(p_aabb.size.x,	p_aabb.size.y,	p_aabb.size.z),
			p_aabb.pos+Vector3(p_aabb.size.x,	p_aabb.size.y,	0),
			p_aabb.pos+Vector3(p_aabb.size.x,	0,		p_aabb.size.z),
			p_aabb.pos+Vector3(p_aabb.size.x,	0,		0),
			p_aabb.pos+Vector3(		0,	p_aabb.size.y,	p_aabb.size.z),
			p_aabb.pos+Vector3(		0,	p_aabb.size.y,	0),
			p_aabb.pos+Vector3(		0,	0,		p_aabb.size.z),
			p_aabb.pos+Vector3(		0,	0,		0)
	};
	
	/* transform vertices */	
	for (int i=0;i<8;i++) {
		transform(vertices[i]);
	}

	/* recreater AABB */

	Vector3 min=vertices[0],max=vertices[0];

	for (int i=1;i<8;i++) {
		
		if (vertices[i].x<min.x)
			min.x=vertices[i].x;
		if (vertices[i].y<min.y)
			min.y=vertices[i].y;
		if (vertices[i].z<min.z)
			min.z=vertices[i].z;
		
		if (vertices[i].x>max.x)
			max.x=vertices[i].x;
		if (vertices[i].y>max.y)
			max.y=vertices[i].y;
		if (vertices[i].z>max.z)
			max.z=vertices[i].z;
	}

	p_aabb.pos=min;
	p_aabb.size=max-min;
}


inline void Matrix4::transform_aabb_no_translation(AABB &p_aabb) const {
	
	/* define vertices */
	Vector3 vertices[8]={
			p_aabb.pos+Vector3(p_aabb.size.x,	p_aabb.size.y,	p_aabb.size.z),
			p_aabb.pos+Vector3(p_aabb.size.x,	p_aabb.size.y,	0),
			p_aabb.pos+Vector3(p_aabb.size.x,	0,		p_aabb.size.z),
			p_aabb.pos+Vector3(p_aabb.size.x,	0,		0),
			p_aabb.pos+Vector3(		0,	p_aabb.size.y,	p_aabb.size.z),
			p_aabb.pos+Vector3(		0,	p_aabb.size.y,	0),
			p_aabb.pos+Vector3(		0,	0,		p_aabb.size.z),
			p_aabb.pos+Vector3(		0,	0,		0)
	};
	
	/* transform vertices */	
	for (int i=0;i<8;i++) {
		transform_no_translation(vertices[i]);
	}

	/* recreater AABB */

	Vector3 min=vertices[0],max=vertices[0];

	for (int i=1;i<8;i++) {
		
		if (vertices[i].x<min.x)
			min.x=vertices[i].x;
		if (vertices[i].y<min.y)
			min.y=vertices[i].y;
		if (vertices[i].z<min.z)
			min.z=vertices[i].z;
		
		if (vertices[i].x>max.x)
			max.x=vertices[i].x;
		if (vertices[i].y>max.y)
			max.y=vertices[i].y;
		if (vertices[i].z>max.z)
			max.z=vertices[i].z;
	}

	p_aabb.pos=min;
	p_aabb.size=max-min;
	
}


inline void Matrix4::inverse_transform_aabb(AABB &p_aabb) const {
	
	
	/* define vertices */
	Vector3 vertices[8]={
			p_aabb.pos+Vector3(p_aabb.size.x,	p_aabb.size.y,	p_aabb.size.z),
			p_aabb.pos+Vector3(p_aabb.size.x,	p_aabb.size.y,	0),
			p_aabb.pos+Vector3(p_aabb.size.x,	0,		p_aabb.size.z),
			p_aabb.pos+Vector3(p_aabb.size.x,	0,		0),
			p_aabb.pos+Vector3(		0,	p_aabb.size.y,	p_aabb.size.z),
			p_aabb.pos+Vector3(		0,	p_aabb.size.y,	0),
			p_aabb.pos+Vector3(		0,	0,		p_aabb.size.z),
			p_aabb.pos+Vector3(		0,	0,		0)
	};
	
	/* transform vertices */	
	for (int i=0;i<8;i++) {
		inverse_transform(vertices[i]);
	}

	/* recreater AABB */

	Vector3 min=vertices[0],max=vertices[0];

	for (int i=1;i<8;i++) {
		
		if (vertices[i].x<min.x)
			min.x=vertices[i].x;
		if (vertices[i].y<min.y)
			min.y=vertices[i].y;
		if (vertices[i].z<min.z)
			min.z=vertices[i].z;
		
		if (vertices[i].x>max.x)
			max.x=vertices[i].x;
		if (vertices[i].y>max.y)
			max.y=vertices[i].y;
		if (vertices[i].z>max.z)
			max.z=vertices[i].z;
	}

	p_aabb.pos=min;
	p_aabb.size=max-min;
}

#else

//include custom file for matrix transformations
#include CUSTOM_MATRIX_TRANSFORM_H

#endif //custom matrix transform


#endif
