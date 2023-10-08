
//
// C++ Interface: quat
//
// Description:
//
//
// Author: Juan Linietsky <red@lunatea>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef QUAT_H
#define QUAT_H

/**
 * @file    Quat.h
 * @brief   Quat class definition
 * @date    Mon Sep 30 17:40:46 ART 2002
 * @version $Id: mquat.h,v 1.16 2003/03/18 20:46:57 marto Exp $
 *
 * Suggestions:
 * @TODO fix direction and creating stuff, it's opposite of matrix
 */


#include "math_defs.h"


/**
 * @class Quat
 * @brief Quat hypercomplex number class
 *
 * Responsibilities:
 *		* Manages the X,Y,Z,W quaternion elements and implements 3d math functions
 *
 * Collaborations:
 *
 * Interface:
 *		* Implements 3d math functions specific for quaternion usage
 *
 */

#include "vector3.h"

class Matrix4;


class Quat {

	real_t project_to_sphere(real_t r, real_t x, real_t y);

public:


	///< class main elements
	real_t x,y,z,w;

	real_t get_x() const { return x; };
	real_t get_y() const { return y; };
	real_t get_z() const { return z; };
	real_t get_w() const { return w; };

	/**
	* ///< these functions are used to assign values to an already instantiated Vector2D class
		*/
	Quat *set(real_t p_x, real_t p_y, real_t p_z, real_t p_w) {
		x = p_x; y = p_y; z = p_z; w = p_w;
		return this;
	}
	Quat *set(const Quat &p_q1) {
		x = p_q1.x; y = p_q1.y; z = p_q1.z; w = p_q1.w;
		return this;
	}

	Quat *set(const Vector3 &p_axis, real_t p_angle);
	Quat *set(const Matrix4 &p_m1);


	/**
	* ///< informative functions, are used to retrieve useful data about the instantiated class
	*/
	bool is_zero() const;
	bool is_identity() const;
	bool cmp(const Quat &p_q1) const;
	bool operator ==(const Quat &p_q1) const { return cmp(p_q1); }

	real_t squared() const;
	real_t length() const;
	real_t dot(const Quat &p_q1) const;

	/**
	* ///< math functions, these functions results in changing the instantiated class based on equations
	*/
	Quat &add(const Quat &p_q1);
	Quat &add(const Quat &p_q1, const Quat &p_q2);
	Quat operator +(const Quat &p_q1);
	Quat operator +(real_t p_k);

	Quat &sub(const Quat &p_q1);
	Quat &sub(const Quat &p_q1, const Quat &p_q2);
	Quat operator -(const Quat &p_q1);
	Quat operator -(real_t p_k);
	Quat operator -() const;


	void operator -=(const Quat &p_q1);
	void operator -=(real_t p_k);
	void operator *=(const Quat &p_q1);
	void operator *=(real_t p_k);
	bool operator !=(const Quat &p_q1) const { return !cmp(p_q1); }
	void operator =(const Quat &p_q1) { set(p_q1); }
	void operator =(const Matrix4 &p_m1)   { set(p_m1); }
	void operator +=(const Quat &p_q1);
	void operator +=(real_t p_k);

	Quat &mul(const Quat &p_q1);
	Quat &mul(const Quat &p_q1, const Quat &p_q2);
	Quat operator *(const Quat &p_q1);
	Quat operator *(real_t p_k);

	Quat &scalar(real_t p_k);
	Quat &abs();
	Quat &neg();
	Quat &cnj();
	Quat &set_as_inverse();
	Quat inverse();
	Quat &ln();
	Quat &exp();

	Quat &normalize();
	Quat get_negated() const;
	/**
	* ///< transformation functions, these functions results in changing the instantiated class based on equations
	*/
	Quat &load_zero();
	Quat &load_identity();

	Quat &blend(const Quat &p_q1, const Quat &p_q2, real_t d);
	Quat &blend(const Quat &p_q1, real_t p_d) { return blend(*this, p_q1, p_d); }
	Quat &slerp(const Quat &p_q1, const Quat &p_q2, real_t t);
	Quat &slerp(const Quat &p_q1, real_t p_t) { return slerp(*this, p_q1, p_t); }


	void transform(Vector3 &p_v1) const;

	void get_axes(Vector3 *p_axe1=NULL,Vector3 *p_axe2=NULL,Vector3 *p_axe3=NULL) const;
	void set_from_axes(const Vector3 *p_axe1,const Vector3 *p_axe2,	const Vector3 *p_axe3);

	/**
	* ///< Constructors/Destructor
	*/
	Quat() { load_identity(); };
	Quat(real_t p_x, real_t p_y, real_t p_z, real_t p_w) { set(p_x,p_y,p_z,p_w); };
	Quat(const Quat &p_q1) { set(p_q1); };
	Quat(const Vector3 &p_axis, real_t p_angle) { set(p_axis, p_angle); };
	Quat(const Matrix4 &p_m1) { set(p_m1); };
	~Quat() { };

	void get_euler_angles(real_t* rot_x, real_t* rot_y, real_t* rot_z) const;
};


#endif
