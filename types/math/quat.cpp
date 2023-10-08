//
// C++ Implementation: quat
//
// Description:
//
//
// Author: Juan Linietsky <red@lunatea>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "quat.h"
#include "matrix4.h"
#include "math_funcs.h"

real_t Quat::project_to_sphere(real_t r, real_t x, real_t y)
{
	real_t d, t, z;

	d = Math::sqrt(x*x + y*y);
	if (d < r * 0.70710678118654752440) {    // forcing it to be real_t
				// Inside sphere
		z = Math::sqrt(r*r - d*d);
	} else {
				// On hyperbola
		t = r / 1.41421356237309504880; // forcing it to be real_t
		z = t*t / d;
	}

	return z;
}




/**
 * ///< these functions are used to assign values to an already instantiated Vector2D class
 */

/**
 * brief: generates a quaternion complex number by a Vector3 and an angle
 *
 * long:
 *
 * @param const Vector3 &p_axis <source Vector3 class>
 * @param real_t p_angle <source angle real_t number>
 * @return self to be used as a function parameter
 */
Quat *Quat::set(const Vector3 &p_axis, real_t p_angle)
{
	real_t l = p_axis.length();

	if( l < EPSILON)
		load_identity();
	else {
		real_t omega = -0.5 * p_angle;
		real_t s = Math::sin(omega) / l;
		x = s * p_axis.x;
		y = s * p_axis.y;
		z = s * p_axis.z;
		w = Math::cos(omega);
	}

	return this;
}

/**
 * brief: generates a quaternion complex number out of a 4 dimensional matrix
 *
 * long:   Standard matrix - quaternion conversion. The matrix is supposed to
 *		  have its last column and row equal to (0,0,0,1) otherwise, the result
 *		  is undefined.
 *
 * @param const Vector3 &p_axis <source Vector3 class>
 * @param real_t p_angle <source angle real_t number>
 * @return self to be used as a function parameter
 */

Quat *Quat::set(const Matrix4 &p_m1) {
	real_t tr,s;
	int i,j,k;
	static int nxt[3] = {1,2,0};

	real_t *elements[4] = { &x, &y, &z, &w};

	tr = p_m1.elements[0][0] + p_m1.elements[1][1] + p_m1.elements[2][2];
	if (tr > 0.0) {
		s = Math::sqrt(tr + 1);
		w = s * 0.5;
		s = 0.5 / s;
		x = (p_m1.elements[1][2] - p_m1.elements[2][1]) * s;
		y = (p_m1.elements[2][0] - p_m1.elements[0][2]) * s;
		z = (p_m1.elements[0][1] - p_m1.elements[1][0]) * s;
	}
	else {
		i = 0;
		if (p_m1.elements[1][1] > p_m1.elements[0][0]) i = 1;
		if (p_m1.elements[2][2] > p_m1.elements[i][i]) i = 2;
		j = nxt[i];
		k = nxt[j];
		s = Math::sqrt((p_m1.elements[i][i]-(p_m1.elements[j][j]+p_m1.elements[k][k]))+1);

		*elements[i] = s * 0.5;

		if (Math::absd(s)<EPSILON) {
			load_identity();
		}
		else {
			s = 0.5 /s;
			w = (p_m1.elements[j][k] - p_m1.elements[k][j]) * s;

			*elements[j] = (p_m1.elements[i][j] + p_m1.elements[j][i]) * s;
			*elements[k] = (p_m1.elements[i][k] + p_m1.elements[k][i]) * s;
		}
	}

	return this;
}




/**
 * ///< informative functions, are used to retrieve useful data about the instantiated class
 */

/**
 * brief: returns if the class elements are 0
 *
 * @return true/false
 */
bool Quat::is_zero() const
{
	if( !x && !y && !z && !w )
		return true;
	return false;
}

/**
 * brief: returns if the x,y,z class elements are 0 and w is 1
 *
 * @return true/false
 */
bool Quat::is_identity() const
{
	if( !x && !y && !z && w==1 )
		return true;
	return false;
}

/**
 * brief: compares self elements with p_q1 and returns true/false
 *
 * long:
 *
 * @param const Quat &p_q1 <source class to be compared with>
 * @return true/false
 */
bool Quat::cmp(const Quat &p_q1) const
{
	if( (p_q1.x == this->x) && (p_q1.y == this->y) && (p_q1.z == this->z) && (p_q1.w == this->w) )
		return true;
	return false;
}

/**
 * brief:
 *
 * @return real_t
 */
real_t Quat::squared() const
{
	return (x*x)+(y*y)+(z*z)+(w*w);
}

/**
 * brief: return the length of the quaternion
 *
 * @return real_t containing the length
 */
real_t Quat::length() const
{
	return Math::sqrt( (x*x)+(y*y)+(z*z)+(w*w) );
}

/**
 * brief: computes the dot product of self with p_q1
 *
 * @return real_t containing the dot product
 */
real_t Quat::dot(const Quat &p_q1) const
{
	return (x*p_q1.x)+(y*p_q1.y)+(z*p_q1.z)+(w*p_q1.w);
}



/**
 * ///< math functions, these functions results in changing the instantiated class based on equations
 */

/**
 * brief: adds the passed Quat to self, overloaded with + and +=
 *
 * @param const Quat &p_m1 <source Quat you want to add>
 * @return some function returns self to be used as parameters to other functions or to be constructed as equations
 */
Quat &Quat::add(const Quat &p_q1)
{
	this->operator +=(p_q1);
	return *this;
}

Quat &Quat::add(const Quat &p_q1, const Quat &p_q2)
{
		// add p_q1 + p_q2 and assign it to self
	this->set(p_q1.x+p_q2.x, p_q1.y+p_q2.y, p_q1.z+p_q2.z, p_q1.w+p_q2.w);

		// return self
	return *this;
}

Quat Quat::operator +(const Quat &p_q1) {
	return Quat(x+p_q1.x, y+p_q1.y, z+p_q1.z, w+p_q1.w);
}

Quat Quat::operator +(real_t p_k) {
	return Quat(x+p_k, y+p_k, z+p_k, w+p_k);
}

void Quat::operator +=(const Quat &p_q1) {
	x += p_q1.x;
	y += p_q1.y;
	z += p_q1.z;
	w += p_q1.w;

}

void Quat::operator +=(real_t p_k)
{
	x += p_k;
	y += p_k;
	z += p_k;
	w += p_k;
}


/**
 * Subtracts the passed Quat to self, overloaded with - and -=
 *
 * @param const Quat &p_m1 <source Quat you want to sub>
 * @return some function returns self to be used as parameters to other functions or to be constructed as equations
 */

Quat &Quat::sub(const Quat &p_q1)
{
	this->operator -=(p_q1);
	return *this;
}

Quat &Quat::sub(const Quat &p_q1, const Quat &p_q2)
{
		// add p_q1 - p_q2 and assign it to self
	this->set(p_q1.x-p_q2.x, p_q1.y-p_q2.y, p_q1.z-p_q2.z, p_q1.w-p_q2.w);

		// return self
	return *this;
}

Quat Quat::operator -(const Quat &p_q1)
{
	Quat new_quaternion(x-p_q1.x, y-p_q1.y, z-p_q1.z, w-p_q1.w);
	return new_quaternion;
}

Quat Quat::operator -(real_t p_k)
{
	Quat new_quaternion(x-p_k, y-p_k, z-p_k, w-p_k);
	return new_quaternion;
}


Quat Quat::get_negated() const {

	return -*this;
}

Quat Quat::operator -() const {

	Quat new_quat;
	real_t dist=1.0/length();
	new_quat.w=w*dist;
	new_quat.x=x*-dist;
	new_quat.y=y*-dist;
	new_quat.z=z*-dist;
	new_quat.normalize();
	return new_quat;

}

void Quat::operator -=(const Quat &p_q1)
{
	x -= p_q1.x;
	y -= p_q1.y;
	z -= p_q1.z;
	w -= p_q1.w;

	return;
}

void Quat::operator -=(real_t p_k)
{
	x -= p_k;
	y -= p_k;
	z -= p_k;
	w -= p_k;
}

/**
 * brief: subtracts the passed Quat to self, overloaded with - and -=
 *
 * @param const Quat &p_m1 <source Quat you want to sub>
 * @return some function returns self to be used as parameters to other functions or to be constructed as equations
 */
Quat &Quat::mul(const Quat &p_q1)
{
	this->operator *=(p_q1);
	return *this;
}

Quat &Quat::mul(const Quat &p_q1, const Quat &p_q2)
{
	x = (p_q1.w * p_q2.x) + (p_q1.x * p_q2.w) + (p_q1.y * p_q2.z) - (p_q1.z * p_q2.y);
	y = (p_q1.w * p_q2.y) + (p_q1.y * p_q2.w) + (p_q1.z * p_q2.x) - (p_q1.x * p_q2.z);
	z = (p_q1.w * p_q2.z) + (p_q1.z * p_q2.w) + (p_q1.x * p_q2.y) - (p_q1.y * p_q2.x);
	w = (p_q1.w * p_q2.w) - (p_q1.x * p_q2.x) - (p_q1.y * p_q2.y) - (p_q1.z * p_q2.z);

	return *this;
}

Quat Quat::operator *(const Quat &p_q1)
{
	Quat new_quaternion(
				  (w * p_q1.x) + (x * p_q1.w) + (y * p_q1.z) - (z * p_q1.y),
				   (w * p_q1.y) + (y * p_q1.w) + (z * p_q1.x) - (x * p_q1.z),
				    (w * p_q1.z) + (z * p_q1.w) + (x * p_q1.y) - (y * p_q1.x),
				     (w * p_q1.w) - (x * p_q1.x) - (y * p_q1.y) - (z * p_q1.z) );
	return new_quaternion;

}

Quat Quat::operator *(real_t p_k)
{
	Quat new_quaternion(x*p_k, y*p_k, z*p_k, w*p_k);
	return new_quaternion;
}

void Quat::operator *=(const Quat &p_q1)
{
	x = (w * p_q1.x) + (x * p_q1.w) + (y * p_q1.z) - (z * p_q1.y);
	y = (w * p_q1.y) + (y * p_q1.w) + (z * p_q1.x) - (x * p_q1.z);
	z = (w * p_q1.z) + (z * p_q1.w) + (x * p_q1.y) - (y * p_q1.x);
	w = (w * p_q1.w) - (x * p_q1.x) - (y * p_q1.y) - (z * p_q1.z);
}

void Quat::operator *=(real_t p_k)
{
	x *= p_k;
	y *= p_k;
	z *= p_k;
	w *= p_k;
}

/**
 * brief:
 *
 * @param real_t p_k <>
 * @return self
 */
Quat &Quat::scalar(real_t p_k)
{
	this->operator *(p_k);
	return *this;
}

/**
 * brief: computes the absolute value of each quaternion element
 *
 * @return self
 */
Quat &Quat::abs()
{
	x = Math::absd(x);
	y = Math::absd(y);
	z = Math::absd(z);
	w = Math::absd(w);
	return *this;
}

/**
 * brief: computes the negative value of each quaternion element
 *
 * @return self
 */
Quat &Quat::neg()
{
	x = -x;
	y = -y;
	z = -z;
	w = -w;
	return *this;
}

/**
 * brief: computes the quaternion conjugate (not so sure about this)
 *
 * @return self
 */
Quat &Quat::cnj()
{
	x = -x;
	y = -y;
	z = -z;
	return *this;
}

/**
 * Set the quaternion as the inverse of itself
 *
 * @return self
 */
Quat &Quat::set_as_inverse()
{
	real_t l = length();

	if( Math::absd(l) < EPSILON)
		load_identity();
	else {
		real_t c = 1.0 / l;
		x *= -c;
		y *= -c;
		z *= -c;
		w *=  c;
	}

	return *this;
}


/**
 * Return a the inverse of itself.
 *
 * @return self
 */
Quat Quat::inverse()
{
	Quat inverseq;

	real_t l = length();

	if( Math::absd(l) < EPSILON)
		inverseq.load_identity();
	else {
		real_t c = 1.0 / l;
		inverseq.x = x * -c;
		inverseq.y = y * -c;
		inverseq.z = z * -c;
		inverseq.w = w *  c;
	}

	return inverseq;
}



/**
 * brief:
 *
 * @return self
 */
Quat &Quat::ln()
{
	real_t c;
	real_t s  = Math::sqrt((x*x)+(y*y)+(z*z));
	real_t om = Math::atan2(s, w);
	if( Math::absd(s) < EPSILON )
		c = 0.0;
	else
		c = om / s;

	x = x * c;
	y = y * c;
	z = z * c;
	w = 0.0;

	return *this;
}

Quat &Quat::exp()
{
	real_t sinom;
	real_t om = Math::sqrt((x*x)+(y*y)+(z*z));

	if( Math::absd(om) < EPSILON)
		sinom = 1.0;
	else
		sinom = Math::sin(om) / om;

	x *= sinom;
	y *= sinom;
	z *= sinom;
	w  = Math::cos(om);

	return *this;
}

Quat &Quat::normalize()
{
	real_t l = length();

	if (Math::absd(l)<EPSILON)
		load_identity();
	else {
//				real_t c = 1.0 / l;

//				x *= c;
//				y *= c;
//				z *= c;
//				w *= c;

		x /= l;
		y /= l;
		z /= l;
		w /= l;

	}

	return *this;
}


/**
 * ///< transformation functions, these functions results in changing the instantiated class based on equations
 */

/**
 * brief: sets all the class elements to 0
 */
Quat &Quat::load_zero()
{
	x = y = z = w = 0.0;
	return *this;
}
/**
 * brief: sets x,y,z class elements to 0 and w element to 1
 */
Quat &Quat::load_identity()
{
	x = y = z = 0.0;
	w = 1.0;
	return *this;
}


Quat &Quat::blend(const Quat &p_q1, const Quat &p_q2, real_t d)
{
	real_t inv_d;
	bool  flip = false;

	real_t norm = p_q1.x * p_q2.x + p_q1.y * p_q2.y + p_q1.z * p_q2.z + p_q1.w * p_q2.w;

	if( norm < 0.0 ) {
		norm = -norm;
		flip = true;
	}

	if( 1.0 - norm < 0.000001f ) {
		inv_d = 1.0 - d;
	} else {
		real_t theta = Math::acos(norm);

		real_t s = 1.0 / Math::sin(theta);

		inv_d = Math::sin((1.0 - d) * theta) * s;
		d = Math::sin(d * theta) * s;
	}

	if ( flip )
		d = -d;

	this->x = inv_d * p_q1.x + d * p_q2.x;
	this->y = inv_d * p_q1.y + d * p_q2.y;
	this->z = inv_d * p_q1.z + d * p_q2.z;
	this->w = inv_d * p_q1.w + d * p_q2.w;

	return *this;
}

Quat &Quat::slerp(const Quat &p_q1, const Quat &p_q2, real_t t)
{


	real_t         to1[4];
	real_t        omega, cosom, sinom, scale0, scale1;


	// calc cosine
	cosom = p_q1.x * p_q2.x + p_q1.y * p_q2.y + p_q1.z * p_q2.z
			+ p_q1.w * p_q2.w;


	// adjust signs (if necessary)
	if ( cosom <0.0 ) {
		cosom = -cosom; to1[0] = - p_q2.x;
		to1[1] = - p_q2.y;
		to1[2] = - p_q2.z;
		to1[3] = - p_q2.w;
	} else  {
		to1[0] = p_q2.x;
		to1[1] = p_q2.y;
		to1[2] = p_q2.z;
		to1[3] = p_q2.w;
	}


	// calculate coefficients

	if ( (1.0 - cosom) > EPSILON ) {
		// standard case (slerp)
		omega = Math::acos(cosom);
		sinom = Math::sin(omega);
		scale0 = Math::sin((1.0 - t) * omega) / sinom;
		scale1 = Math::sin(t * omega) / sinom;
	} else {
		// "from" and "to" quaternions are very close
		//  ... so we can do a linear interpolation
		scale0 = 1.0 - t;
		scale1 = t;
	}
	// calculate final values
	this->x = scale0 * p_q1.x + scale1 * to1[0];
	this->y = scale0 * p_q1.y + scale1 * to1[1];
	this->z = scale0 * p_q1.z + scale1 * to1[2];
	this->w = scale0 * p_q1.w + scale1 * to1[3];


	return *this;
	/*

	real_t om, sinom;
	real_t sp, sq;
	Quat qq;

	real_t l = (p_q1.x * p_q2.x) + (p_q1.y * p_q2.y) + (p_q1.z * p_q2.z) + (p_q1.w * p_q2.w);

	if( (1.0+l) > EPSILON ) {
	if( Math::absd(l) > 1.0 )
	l /= Math::absd(l);

	om = Math::acos(l);
	sinom = Math::sin(om);
	if( Math::absd(sinom) > EPSILON ) {
	sp = Math::sin( (1.0-t) * om ) / sinom;
	sq = Math::sin( t * om ) / sinom;
} else {
	sp = 1.0 - t;
	sq = t;
}

	x = (sp * p_q1.x) + (sq * p_q2.x);
	y = (sp * p_q1.y) + (sq * p_q2.y);
	z = (sp * p_q1.z) + (sq * p_q2.z);
	w = (sp * p_q1.w) + (sq * p_q2.w);
} else {
	qq.x = -p_q1.y;
	qq.y =  p_q1.x;
	qq.z = -p_q1.w;
	qq.w =  p_q1.z;
	sp = Math::sin( (1.0-t) * HALFPI );
	sq = Math::sin( t*HALFPI );

	x = (sp * p_q1.x) + (sq * qq.x);
	y = (sp * p_q1.y) + (sq * qq.y);
	z = (sp * p_q1.z) + (sq * qq.z);
	w = (sp * p_q1.w) + (sq * qq.w);
}

	return *this;
	*/
}




/**
 *  Given two rotations, e1 and e2, expressed as quaternion rotations,
 *   figure out the equivalent single rotation and return it.
 * // @todo change name.
 * long description.
 *
 * @param par1 <desc>
 * @param par2 <desc>
 * @return
 */
/*
Quat Quat::nahuel_add_quat(const Quat &p_q2) const {
	Quat t1, t2, t3, tf;

	t1.x = x * p_q2.w;
	t1.y = y * p_q2.w;
	t1.z = z * p_q2.w;

	t2.x = p_q2.x * w;
	t2.y = p_q2.y * w;
	t2.z = p_q2.z * w;

	Vector3 cross; // ver cross
	cross.set_as_cross_of(Vector3(p_q2.x, p_q2.y, p_q2.z), Vector3(x, y, z));

	t3.x = cross.x;
	t3.y = cross.y;
	t3.z = cross.z;


	tf.x = t1.x + t2.x + t3.x;
	tf.y = t1.y + t2.y + t3.y;
	tf.z = t1.z + t2.z + t3.z;


	Vector3 tmp1(x, y, z), tmp2(p_q2.x, p_q2.y, p_q2.z);

	tf.w = w * p_q2.w - tmp1.dot(tmp2);

	tf.normalize();

	return tf; // Note, the return is by value.

}
 */




/**
 * Rotate a vector3d using the quaterniong (the vector is changed)
 *
 * @param p_v1 the vector that you want to transform.
 */
void Quat::transform(Vector3 &p_v1) const {
	Matrix4 tmp;
	tmp.load_rotate(*this);
	tmp.transform(p_v1);
}


void Quat::get_axes(Vector3 *p_axe1, Vector3 *p_axe2, Vector3 *p_axe3) const {
	/*
	Matrix4 tmp;
	Vector4D tmpv;

	tmp.load_rotate(*this);

	if(p_axe1 != NULL) {
		tmpv = tmp.get_col(0);
		p_axe1->x = tmpv.x;
		p_axe1->y = tmpv.y;
		p_axe1->z = tmpv.z;
	}


	if(p_axe2 != NULL) {
		tmpv = tmp.get_col(1);
		p_axe2->x = tmpv.x;
		p_axe2->y = tmpv.y;
		p_axe2->z = tmpv.z;
	}


	if(p_axe3 != NULL) {
		tmpv = tmp.get_col(2);
		p_axe3->x = tmpv.x;
		p_axe3->y = tmpv.y;
		p_axe3->z = tmpv.z;
	}
*/
}





void Quat::set_from_axes(const Vector3 *p_axe1, const Vector3 *p_axe2, const Vector3 *p_axe3) {
	Matrix4 tmp;
	tmp.load_identity();

/*	tmp.set_col(0, *p_axe1);
	tmp.set_col(1, *p_axe2);
	tmp.set_col(2, *p_axe3); */

	set(tmp);
}

void Quat::get_euler_angles(real_t* rot_x, real_t* rot_y, real_t* rot_z) const
{
	real_t sq_x = x * x;
	real_t sq_y = y * y;
	real_t sq_z = z * z;
	real_t sq_w = w * w;

	*rot_x = Math::atan2(2 * (y * z + x * w),(-sq_x - sq_y + sq_z + sq_w));
	*rot_y = Math::asin(-2 * (x * z - y * w));
	*rot_z = Math::atan2(2 * (x * y + z * w),(sq_x - sq_y - sq_z + sq_w));
}
