//
// C++ Interface: vector3
//
// Description:
//
//
// Author: red,,, <red@hororo>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef VECTOR3_H
#define VECTOR3_H


#include "typedefs.h"
#include "math_defs.h"

/**
 * 3-D Vector class
 */

SCRIPT_BIND_BEGIN;

class Vector3 {
public:


	real_t x,y,z; /// components
	
	inline real_t get_x() const { return x; } /** get x as real_t. the fixed point version also returns it as real_t */
	inline real_t get_y() const { return y; } /** get y as real_t. the fixed point version also returns it as real_t */
	inline real_t get_z() const { return z; } /** get z as real_t. the fixed point version also returns it as real_t */
	
	inline void set_x(const real_t& p_x) { x=p_x; } /** set x from a real_t, the fixed point version also sets from real_t */
	inline void set_y(const real_t& p_y) { y=p_y; } /** set y from a real_t, the fixed point version also sets from real_t */
	inline void set_z(const real_t& p_z) { z=p_z; } /** set z from a real_t, the fixed point version also sets from real_t */
	
	const Vector3& set(real_t p_x, real_t p_y,  real_t p_z); /** set x,y,z as real_ts, fixed point version also sets from real_t */
	const Vector3& set(const Vector3& p_v1); /** set as a vector, the same as operator= */
	void set(int p_axis,real_t p_val);

	/**
	* ///< informative functions, are used to retrieve useful data about the instantiated class
	*/
	bool is_zero() const; /** Vector3 is (0,0,0) */
	bool cmp(const Vector3 &p_v1) const; /** compare a Vector3, return true if equal, same as operator== */
	
	
	/**
		* ///< math functions, these functions results in changing the instantiated class based on equations
		*/
	real_t distance_to(const Vector3 &p_v1) const; /** as a point, return the distance to another point */
	_FORCE_INLINE_ real_t squared_distance_to(const Vector3 &p_v1) const; /** as a point, return the distance to another point, but omit the square root, great for comparisons */
	real_t length() const; /** vector length */
	real_t squared_length() const; /** vector length, with square root omitted */

	const Vector3 &set_as_normal_of(const Vector3 &vector_A, const Vector3 &vector_B, const Vector3 &vector_C); /** set as normal of 3 points */

	void zero(); /** clear it, becomes (0,0,0) */

	const Vector3 &normalize(); /** make the vector legnth 1 */
	bool is_normalized() const; /** vector has length 1 (or veeeery near), check math_types.h for precision */
	Vector3 get_normalized() const; /** get normalized version */
	
	Vector3 reflect(const Vector3& p_normal) const;
	Vector3 slide(const Vector3& p_normal) const;
	// -- Cross Product -- //
	_FORCE_INLINE_ static Vector3 cross(const Vector3 &p_v1, const Vector3 &p_v2); /** return the cross product of two vectors */
	const Vector3 &set_as_cross_of(const Vector3 &p_v1, const Vector3 &p_v2); /** set as cross product of two other vectors */
	const Vector3 &set_as_cross_with(const Vector3 &p_v); /** cross with another vector */
	
	// -- Dot Product -- //
	_FORCE_INLINE_ real_t dot(const Vector3 &p_v1) const; /** return dot product with another vector */

	const Vector3 &neg(); /** negated vector -Vector() */
	const Vector3 &abs(); /** abs() x,y and z  */
	const Vector3 &cartesianize(); /** map to 2D */

	const Vector3 &min(const Vector3 &p_v1); /** compare to other vector, only get minimums */
	const Vector3 &max(const Vector3 &p_v1); /** compare to other vector, only get maximums */

	const Vector3 &cubic(const Vector3 &A, const Vector3 &TA, const Vector3 &TB, const Vector3 &B, real_t t); /** cubic interpolate with 4 vectors */
	
	// -- Addition / Substraction -- //
	const Vector3 &add(const Vector3 &p_v1);


	const Vector3 &sub(const Vector3 &p_v1);


	const Vector3 &adjust_min_bounds();

	
	/* OPERATOR OVERLOADING */
	

	_FORCE_INLINE_ Vector3 operator+(const Vector3 &p_v1) const;

	_FORCE_INLINE_ Vector3 operator-(const Vector3 &p_v1) const;
	_FORCE_INLINE_ Vector3 operator-() const;

	_FORCE_INLINE_ bool operator <(const Vector3 &p_v1) const;

	_FORCE_INLINE_ Vector3 operator*(const Vector3 &p_v1) const; /**< multiply components, NOT crossproduct **/
	_FORCE_INLINE_ Vector3 operator/(const Vector3 &p_v1) const; /**< divide components, NOT crossdivide **/
	_FORCE_INLINE_ Vector3 operator*(const real_t &rvalue) const;

	_FORCE_INLINE_ bool operator==(const Vector3 &p_v1) const;



	_FORCE_INLINE_ bool operator>(const Vector3 &p_v1) const;
	_FORCE_INLINE_ void operator*=(real_t r_value);
	_FORCE_INLINE_ void operator/=(real_t r_value);
	_FORCE_INLINE_ bool operator!=(const Vector3 &p_v1) const;
	_FORCE_INLINE_ void operator-=(const Vector3 &p_v1);
	_FORCE_INLINE_ void operator+=(const Vector3 &p_v1);




	_FORCE_INLINE_ Vector3 operator/(const real_t &rvalue) const;

	/**
	 * ///< transformation functions, these functions results in changing the instantiated class based on equations
	 */


	/** linear interpolation */
	 static _FORCE_INLINE_  Vector3 lerp(const Vector3& p_a,const Vector3& p_b,float p_c);
	 

	 bool is_colinear_with(const Vector3 &p_vector) const; /** return colinearity */

	 real_t angle_with(const Vector3 &p_vector) const; /** return angle with another vector */
	 real_t cos_with(const Vector3& p_vector) const; /** cosine with another vector */
	 Vector3 project(const Vector3& p_direction) const; /** project by other vector */
	 Vector3 get_orthogonal_vector() const;

	 void rotate(const Vector3& p_pivot_normal,real_t p_phi);
	 
	 inline bool is_almost_zero() const { return squared_length() < CMP_EPSILON2; }
	 void snap(real_t p_snap); ///< snap to a precision, with rounding to avoid numerical error in some computations
	 
	/**
	 * ///< Constructors/Destructor
	 */
	 _FORCE_INLINE_ Vector3() {  x=0; y=0; z=0;  };
	 _FORCE_INLINE_ Vector3(real_t p_x, real_t p_y, real_t p_z) { x=p_x; y=p_y; z=p_z; }; /** create from real_ts, fixed point also has this */
	// ~Vector3() {printf("destroying %p\n", this);};

	///< This is useful for iterating
	/*real_t * array();*/

};

SCRIPT_BIND_END;



inline Vector3 Vector3::operator +(const Vector3 &p_v1) const {

	return Vector3(x+p_v1.x, y+p_v1.y, z+p_v1.z);
}



 inline Vector3 Vector3::operator-(const Vector3 &p_v1) const {

	 return Vector3(x-p_v1.x, y-p_v1.y, z-p_v1.z);

 }


 inline Vector3 Vector3::operator-() const {

	 return Vector3(-x,-y,-z);
 }



 inline bool Vector3::operator <(const Vector3 &p_v1) const {

	 return (( x < p_v1.x ) && ( y < p_v1.y ) && ( z < p_v1.z ));
 }





 inline Vector3 Vector3::operator *(const Vector3 &p_v1) const {

	 return Vector3(x*p_v1.x,y*p_v1.y,z*p_v1.z);

 }


 inline Vector3 Vector3::operator *(const real_t &rvalue) const {

	 return Vector3(x*rvalue,
			y*rvalue,
   z*rvalue);
 }




 inline bool Vector3::operator >(const Vector3 &p_v1) const {

	 return (( x > p_v1.x ) && ( y > p_v1.y ) && ( z > p_v1.z ));
 }


 inline void Vector3::operator *=(real_t r_value) {

	 x = x*r_value;
	 y = y*r_value;
	 z = z*r_value;

 }


inline void Vector3::operator /=(real_t r_value) {

	 x = x/r_value;
	 y = y/r_value;
	 z = z/r_value;
}

inline void Vector3::operator -=(const Vector3 &p_v1) {

	x -= p_v1.x;
	y -= p_v1.y;
	z -= p_v1.z;
}


 inline void Vector3::operator +=(const Vector3 &p_v1) {

	 x += p_v1.x;
	 y += p_v1.y;
	 z += p_v1.z;
 }





 inline Vector3 Vector3::operator /(const Vector3 &p_v1) const {

	 return Vector3(x/p_v1.x,y/p_v1.y,z/p_v1.z);

 }


inline Vector3 Vector3::operator /(const real_t &rvalue) const {

	 return Vector3((x/rvalue),
			 (y/rvalue),
			  (z/rvalue));
 }

inline bool Vector3::operator ==(const Vector3 &p_v1) const {

	 return ((x==p_v1.x) && (y==p_v1.y) && (z==p_v1.z));
 }


inline bool Vector3::operator !=(const Vector3 &p_v1) const {

	 return ((x!=p_v1.x) || (y!=p_v1.y) || (z!=p_v1.z));
}

inline Vector3 Vector3::cross(const Vector3 &p_v1, const Vector3 &p_v2) {
	
	real_t x =  (p_v1.y * p_v2.z) - (p_v1.z * p_v2.y);
	real_t y =  (p_v1.z * p_v2.x) - (p_v1.x * p_v2.z);
	real_t z =  (p_v1.x * p_v2.y) - (p_v1.y * p_v2.x);
	
	return Vector3(x,y,z);
	
}

inline real_t Vector3::dot(const Vector3 &p_v) const {

	 return (x * p_v.x) + (y * p_v.y) + (z * p_v.z);
}
inline Vector3 Vector3::lerp(const Vector3& p_a,const Vector3& p_b,float p_c) {
		
	Vector3 res=p_a;
	
	res.x+= (p_c * (p_b.x-p_a.x));
	res.y+= (p_c * (p_b.y-p_a.y));
	res.z+= (p_c * (p_b.z-p_a.z));
	return res;
}

inline real_t Vector3::squared_distance_to(const Vector3 &p_v1) const {

	real_t x2= (p_v1.x - x)*(p_v1.x - x) ;
	real_t y2= (p_v1.y - y)*(p_v1.y - y) ;
	real_t z2= (p_v1.z - z)*(p_v1.z - z) ;

	return x2+y2+z2;
}


#endif


