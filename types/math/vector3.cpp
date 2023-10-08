//
// C++ Implementation: vector3
//
// Description: 
//
//
// Author: red,,, <red@hororo>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "vector3.h"

#include "math_funcs.h"
/*
 real_t* Vector3::array() {

	return &x;	
}
*/

void Vector3::set(int p_axis,real_t p_val) {

	ERR_FAIL_INDEX(p_axis,3);
	
	if (p_axis==0)
		x=p_val;
	if (p_axis==1)
		y=p_val;
	if (p_axis==2)
		z=p_val;
}

 const Vector3& Vector3::set(real_t p_x, real_t p_y,  real_t p_z) {

	x=p_x;
	y=p_y;
	z=p_z;

	return *this;
}


 const Vector3& Vector3::set(const Vector3& p_v1) {

	x=p_v1.x;
	y=p_v1.y;
	z=p_v1.z;
	return *this;

}


/**
 * ///< informative functions, are used to retrieve useful data about the instantiated class
 */



 bool Vector3::is_zero() const  {

	return ((x==0) && (y==0) && (z==0));
}


 bool Vector3::cmp(const Vector3 &p_v1) const  {

	return ((x==p_v1.x) && (y==p_v1.y) && (z==p_v1.z));
}


/**
 * ///< math functions, these functions results in changing the instantiated class based on equations
 */


 real_t Vector3::distance_to(const Vector3 &p_v1) const {

	real_t x2= (p_v1.x - x)*(p_v1.x - x) ;
	real_t y2= (p_v1.y - y)*(p_v1.y - y) ;
	real_t z2= (p_v1.z - z)*(p_v1.z - z) ;

	return Math::sqrt( x2+y2+z2 );

}




 real_t Vector3::length() const {

	 /* To avoid overflows (may happen easily) this stays in 64 bits */
	real_t x2=x*x;
	real_t y2=y*y;
	real_t z2=z*z;
	
	return Math::sqrt(x2+y2+z2);

}

/* This is dangerous in fixed point, and only small values should be used */
 real_t Vector3::squared_length() const {
	
	real_t x2=x*x;
	real_t y2=y*y;
	real_t z2=z*z;
	
	return x2+y2+z2;
}

Vector3 Vector3::reflect(const Vector3& p_normal) const {

	return *this + (  p_normal * -(2 * dot(p_normal))); 
}

Vector3 Vector3::slide(const Vector3& p_normal) const {

	real_t l = length();
	
	if (l==0)
		return Vector3();
		
	Vector3 n = *this/l;
	
	Vector3 s = -Vector3::cross( Vector3::cross( n, p_normal ), p_normal );
	s*=l*(1.0-n.dot(-p_normal));

	return s;
}



 const Vector3 &Vector3::set_as_normal_of(const Vector3 &vector_A, const Vector3 &vector_B, const Vector3 &vector_C) {

	set_as_cross_of(vector_B - vector_A, vector_C - vector_A);

	return normalize();

}

Vector3 Vector3::get_normalized() const {

	Vector3 v=*this;
	v.normalize();
	return v;
}

bool Vector3::is_normalized() const {

	return Math::absf(squared_length()-1.0)<=CMP_NORMALIZE_TOLERANCE;	
}

 void Vector3::zero() {

	x=0;
	y=0;
	z=0;
}



 const Vector3 &Vector3::normalize() {

	real_t l = length();

	if (l==0.0f) {
		
		return *this; ///nothing to do here
	}
	
	x/=l;
	y/=l;
	z/=l;

	return *this;
}


// -- Cross Product -- //


 const Vector3 &Vector3::set_as_cross_of(const Vector3 &p_v1, const Vector3 &p_v2) {

	x =  (p_v1.y * p_v2.z) - (p_v1.z * p_v2.y);
	y =  (p_v1.z * p_v2.x) - (p_v1.x * p_v2.z);
	z =  (p_v1.x * p_v2.y) - (p_v1.y * p_v2.x);
	return *this;

}


 const Vector3 &Vector3::set_as_cross_with(const Vector3 &p_v) {

	real_t xt =  (y * p_v.z) - (z * p_v.y);
	real_t yt =  (z * p_v.x) - (x * p_v.z);
	real_t zt =  (x * p_v.y) - (y * p_v.x);
	x = xt;
	y = yt;
	z = zt;
	return *this;

}


// -- Dot Product -- //





 const Vector3 &Vector3::neg() {

	x = -x;
	y = -y;
	z = -z;

	return *this;

}


 const Vector3 &Vector3::abs() {

	x = Math::absf(x);
	y = Math::absf(y);
	z = Math::absf(z);
	return *this;
}


 

 const Vector3 &Vector3::cartesianize() {

	x/=z;
	y/=z;
	z=1.0f;
	
	return *this;

}


 const Vector3& Vector3::min(const Vector3 &p_v1) {

	if( (p_v1.x < x) ) x = p_v1.x;
	if( (p_v1.y < y) ) y = p_v1.y;
	if( (p_v1.z < z) ) z = p_v1.z;

	return *this;
}


 const Vector3& Vector3::max(const Vector3 &p_v1) {

	if( p_v1.x > x ) x = p_v1.x;
	if( p_v1.y > y ) y = p_v1.y;
	if( p_v1.z > z ) z = p_v1.z;

	return *this;
}


 const Vector3& Vector3::cubic(const Vector3 &A, const Vector3 &TA, const Vector3 &TB, const Vector3 &B, real_t t) {

	 
	real_t a =  2.0*t*t*t - 3.0*t*t + 1;
	real_t b = -2.0*t*t*t + 3.0*t*t;
	real_t c =    t*t*t - 2.0*t*t + t;
	real_t d =    t*t*t - t*t;

	x = (a * A.x) + (b *B.x) + (c * TA.x) + (d * TB.x);
	y = (a * A.y) + (b *B.y) + (c * TA.y) + (d * TB.y);
	z = (a * A.z) + (b *B.z) + (c * TA.z) + (d * TB.z);
	
	return *this;
}



// -- Addition / Substraction -- //

 const Vector3& Vector3::add(const Vector3 &p_v1) {

	this->operator +=(p_v1);
	return *this;

}


 const Vector3& Vector3::sub(const Vector3 &p_v1) {

	this->operator -=(p_v1);
	return *this;
}


 const Vector3& Vector3::adjust_min_bounds() {

	if (x<0) x=0;
	if (y<0) y=0;
	if (z<0) z=0;
	return *this;
}



/* OPERATOR OVERLOADING */




 
/**
 * ///< transformation functions, these functions results in changing the instantiated class based on equations
 */





 bool Vector3::is_colinear_with(const Vector3 &p_vector) const {
	return Vector3::cross(*this, p_vector).is_zero();
}



 real_t Vector3::angle_with(const Vector3 &p_vector) const {

	return Math::acos(this->dot(p_vector) / (this->length()*p_vector.length()));

}


 real_t Vector3::cos_with(const Vector3& p_vector) const {

	return (dot(p_vector) / (length()* p_vector.length()));
}

void Vector3::snap(real_t p_snap) {

	x+=p_snap/2.0;
	x-=Math::fmod(x,p_snap);
	y+=p_snap/2.0;
	y-=Math::fmod(y,p_snap);	
	z+=p_snap/2.0;
	z-=Math::fmod(z,p_snap);	
}
 void Vector3::rotate(const Vector3& p_pivot_normal,real_t p_phi) {
	 
	Vector3 tangent = Vector3::cross(*this,p_pivot_normal).get_normalized();	 
	Vector3 binormal = Vector3::cross(tangent,p_pivot_normal).get_normalized();	 
	
	real_t x = binormal.dot(*this);
	real_t y = tangent.dot(*this);
	     	
	real_t ang = Math::atan2(y,x);
	
	ang +=p_phi;
			
	*this= ((binormal * Math::cos(ang)) + (tangent * Math::sin(ang)))*length();		 
	 
 }


 Vector3 Vector3::project(const Vector3& p_direction) const {

	real_t dir_len, my_len, angle_cos;
	dir_len = p_direction.length();
	my_len = length();
	angle_cos = dot(p_direction) / (my_len * dir_len);
	Vector3 ret(p_direction);
	ret *= my_len * angle_cos / dir_len;

	
	return ret;

}


 Vector3 Vector3::get_orthogonal_vector() const {

	if (x == 0 && y == 0) {
		Vector3 ret(0,1.0,0);
		ret.set_as_cross_with(*this);
		return ret;
	} else {
		
		Vector3 ret(0,0,1.0);
		ret.set_as_cross_with(*this);
		return ret;
	}

	return *this;
}


