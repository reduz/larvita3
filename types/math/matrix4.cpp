//
// C++ Implementation: matrix4
//
// Description:
//
//
// Author: red,,, <red@hororo>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "matrix4.h"
#include "os/copymem.h"
#include "quat.h"


#define f_div(m_a,m_b) ((m_a)/(m_b))
#define f_mul(m_a,m_b) ((m_a)*(m_b))

const Matrix4 Matrix4::IDENTITY( 1, 0, 0, 0,
				   0, 1, 0, 0,
				   0, 0, 1, 0,
				   0, 0, 0, 1);

/**
 * these functions are used to assign values to an already instantiated Vector3 class
 */

/**
 * assigns the given values to self
 *
 * @param _?? <the value according to its position in the Matrix4>
 * @return this so it can be used as a parameter of a function
 */
const Matrix4& Matrix4::set(real_t _11, real_t _12, real_t _13, real_t _14,
			  real_t _21, real_t _22, real_t _23, real_t _24,
			  real_t _31, real_t _32, real_t _33, real_t _34,
			  real_t _41, real_t _42, real_t _43, real_t _44) {
		elements[0][0] = _11; elements[0][1] = _12; elements[0][2] = _13; elements[0][3] = _14;
		elements[1][0] = _21; elements[1][1] = _22; elements[1][2] = _23; elements[1][3] = _24;
		elements[2][0] = _31; elements[2][1] = _32; elements[2][2] = _33; elements[2][3] = _34;
		elements[3][0] = _41; elements[3][1] = _42; elements[3][2] = _43; elements[3][3] = _44;
		return *this;
}

void Matrix4::set_col(int col, real_t _1, real_t _2, real_t _3, real_t _4) {

	ERR_FAIL_INDEX(col,4);

	elements[col][0] = _1;
	elements[col][1] = _2;
	elements[col][2] = _3;
	elements[col][3] = _4;
};


/**
 * copies the elements of p_m1 onto self
 *
 * @param const Matrix4 &p_m1 <source Matrix4 to be copied of>
 * @return this so it can be used as a parameter of a function
 */


/**
 * informative functions, are used to retrieve useful data about the instantiated class
 */

/**
 * brief: returns if the class elements are 0
 *
 * long: zzzz
 *
 * @return true/false
 */
bool Matrix4::is_zero() const
{
		return ( !elements[0][0] && !elements[0][1] && !elements[0][2] && !elements[0][3] &&
				 !elements[1][0] && !elements[1][1] && !elements[1][2] && !elements[1][3] &&
				 !elements[2][0] && !elements[2][1] && !elements[2][2] && !elements[2][3] &&
				 !elements[3][0] && !elements[3][1] && !elements[3][2] && !elements[3][3] );
}

/**
 * brief: returns if the class is an identity matrix
 *
 * @return true/false
 */
bool Matrix4::is_identity() const
{
	return (  elements[0][0]==1.0 && !elements[0][1]    && !elements[0][2]    && !elements[0][3] &&
		 !elements[1][0]    &&  elements[1][1]==1.0 && !elements[1][2]    && !elements[1][3] &&
		 !elements[2][0]    && !elements[2][1]    &&  elements[2][2]==1.0 && !elements[2][3] &&
		 !elements[3][0]    && !elements[3][1]    && !elements[3][2]    &&  elements[3][3]==1.0 );
}

/**
 * brief: returns the matrix translation values into a Vector3
 *
 * @return Vector3 containing the matrix translation
 */
Vector3 Matrix4::get_translation() const
{
	return Vector3(elements[3][0], elements[3][1], elements[3][2]);
}

void Matrix4::set_translation(const Vector3& p_trans) {

	elements[3][0] = p_trans.x;
	elements[3][1] = p_trans.y;
	elements[3][2] = p_trans.z;
};


void Matrix4::clear_translation() {
	elements[3][0] = 0;
	elements[3][1] = 0;
	elements[3][2] = 0;
}

void Matrix4::clear_rotation_scaling() {

	for (int i=0;i<3;i++)
		for (int j=0;j<3;j++)
			if (i==j)
				elements[i][j] = 1;
			else
				elements[i][j] = 0;


}

/**
 * brief: compares self elements with p_m1 and returns true/false
 *
 * long:
 *
 * @param const Matrix4 &p_m1 <source class to be compared with>
 * @return true/false
 */
bool Matrix4::cmp(const Matrix4 &p_m1) const
{
	return ( (elements[0][0] == p_m1.elements[0][0]) && (elements[0][1] == p_m1.elements[0][1]) && (elements[0][2] == p_m1.elements[0][2]) && (elements[0][3] == p_m1.elements[0][3]) &&
		 (elements[1][0] == p_m1.elements[1][0]) && (elements[1][1] == p_m1.elements[1][1]) && (elements[1][2] == p_m1.elements[1][2]) && (elements[1][3] == p_m1.elements[1][3]) &&
		 (elements[2][0] == p_m1.elements[2][0]) && (elements[2][1] == p_m1.elements[2][1]) && (elements[2][2] == p_m1.elements[2][2]) && (elements[2][3] == p_m1.elements[2][3]) &&
		 (elements[3][0] == p_m1.elements[3][0]) && (elements[3][1] == p_m1.elements[3][1]) && (elements[3][2] == p_m1.elements[3][2]) && (elements[3][3] == p_m1.elements[3][3]) );

	///< @todo should I use memcmp()?
}


/**
 * ///< math functions, these functions results in changing the instantiated class based on equations
 */

/**
 * brief: creates a LH lookat matrix
 *
 *
 * @param Vector3 &eye the point where the camera is located
 * @param Vector3 &at  the point where the camera points to
 * @param Vector3 &up  a vector that rolls the camera (PASS IT NORMALIZED)
 * @return a reference to self
 * @todo see if this function works ok checking it against gluLookAt
 */
const Matrix4& Matrix4::set_lookat(const Vector3 &eye, const Vector3 &target, const Vector3 &up) {


	// Reference: MESA source code
	Vector3 v_x, v_y, v_z;

	/* Make rotation matrix */

	/* Z vector */
	v_z = eye - target;

	v_z.normalize();

	v_y = up;


	v_x.set_as_cross_of(v_y, v_z);

	/* Recompute Y = Z cross X */
	v_y.set_as_cross_of(v_z, v_x);

	v_x.normalize();
	v_y.normalize();


	elements[0][0] = v_x.x;  elements[1][0] = v_y.x;  elements[2][0] = v_z.x;  elements[3][0] = eye.x;
	elements[0][1] = v_x.y;  elements[1][1] = v_y.y;  elements[2][1] = v_z.y;  elements[3][1] = eye.y;
	elements[0][2] = v_x.z;  elements[1][2] = v_y.z;  elements[2][2] = v_z.z;  elements[3][2] = eye.z;
	elements[0][3] = 0;      elements[1][3] = 0;      elements[2][3] = 0;    elements[3][3] = 1.0;


	return *this;

}


/**
 * Creates a LH projected matrix (like gluPerspective)
 *
 * @note algorithm based on Mesa source code.
 *
 * @param real_t fovy   FOV angle in degrees
 * @param real_t aspect <aspect component of the matrix>
 * @param real_t z_near <z_near component of the matrix>
 * @param real_t z_far  <z_far  component of the matrix>
 * @return a reference to self
 */
const Matrix4& Matrix4::set_projection(real_t p_fovy_degrees, real_t p_aspect, real_t p_z_near, real_t p_z_far)
{

	real_t sine, cotangent, deltaZ;
	real_t radians = p_fovy_degrees / 2.0 * Math_PI / 180.0;

	deltaZ = p_z_far - p_z_near;
	sine = Math::sin(radians);

	if ((deltaZ == 0) || (sine == 0) || (p_aspect == 0)) {
		return *this;
	}
	cotangent = Math::cos(radians) / sine;

	load_identity();

	elements[0][0] = cotangent / p_aspect;
	elements[1][1] = cotangent;
	elements[2][2] = -(p_z_far + p_z_near) / deltaZ;
	elements[2][3] = -1;
	elements[3][2] = -2 * p_z_near * p_z_far / deltaZ;
	elements[3][3] = 0;

	return *this;

}


real_t Matrix4::get_z_near() const {

	const real_t * matrix = (const real_t*)this->elements;			
	Plane new_plane=Plane(matrix[ 3] + matrix[ 2],
	                matrix[ 7] + matrix[ 6],
	                matrix[11] + matrix[10],
	               -matrix[15] - matrix[14]);
	
	new_plane.normalize();
	return new_plane.d;
};

real_t Matrix4::get_z_far() const {

	const real_t * matrix = (const real_t*)this->elements;			
	Plane new_plane=Plane(matrix[ 3] - matrix[ 2],
	                matrix[ 7] - matrix[ 6],
	                matrix[11] - matrix[10],
	                matrix[15] - matrix[14]);
	
	new_plane.normal=-new_plane.normal;
	new_plane.normalize();
	
	return new_plane.d;
};

void Matrix4::get_viewport_size(real_t& r_width, real_t& r_height) const {

	const real_t * matrix = (const real_t*)this->elements;
	///////--- Near Plane ---///////
	Plane near_plane=Plane(matrix[ 3] + matrix[ 2],
	                matrix[ 7] + matrix[ 6],
	                matrix[11] + matrix[10],
			-matrix[15] - matrix[14]);
	near_plane.normalize();
		
	///////--- Right Plane ---///////
	Plane right_plane=Plane(matrix[ 3] - matrix[ 0],
	                matrix[ 7] - matrix[ 4],
	                matrix[11] - matrix[ 8],
			- matrix[15] + matrix[12]);
	right_plane.normalize();
	
	Plane top_plane=Plane(matrix[ 3] - matrix[ 1],
	                matrix[ 7] - matrix[ 5],
	                matrix[11] - matrix[ 9],
			-matrix[15] + matrix[13]);
	top_plane.normalize();
	
	Vector3 res;
	near_plane.get_intersection_with_planes(right_plane,top_plane,&res);

	r_width=res.x;
	r_height=res.y;
}


/**
 * Creates a LH orthogonal matrix
 *
 * @note algorithm based on DX8's help library
 *
 * @param real_t left   <left   component of the matrix>
 * @param real_t right  <right  component of the matrix>
 * @param real_t bottom <bottom component of the matrix>
 * @param real_t top    <top    component of the matrix>
 * @param real_t z_near <z_near component of the matrix>
 * @param real_t z_far  <z_far  component of the matrix>
 * @return a reference to self
 */
const Matrix4& Matrix4::set_orthogonal(real_t left, real_t right, real_t bottom, real_t top,  real_t z_near, real_t z_far)
{

		// zero the matrix
	load_zero();
		// row 1
	elements[0][0] = 2.0/(right-left);
	elements[0][3] = -((right+left)/(right-left));
		// row 2
	elements[1][1] = 2.0/(top-bottom);
	elements[1][3] = -((top+bottom)/(top-bottom));
		// row 3
	elements[2][2] = -2.0/(z_far-z_near);
	elements[2][3] = -((z_far+z_near)/(z_far-z_near));
		// row 4
	elements[3][3] = 1.0;

		// return self
	return *this;
	return *this;
}



const Matrix4& Matrix4::set_frustum(real_t p_left, real_t p_right, real_t p_bottom, real_t p_top, real_t p_near, real_t p_far) {

	real_t left=( p_left );
	real_t right=( p_right );
	real_t bottom=( p_bottom );
	real_t top=( p_top );
	real_t near=( p_near );
	real_t far=( p_far );


	real_t x, y, a, b, c, d;

	x = f_div((2*near) , (right-left));
	y = f_div((2*near) , (top-bottom));
	a = f_div((right+left) , (right-left));
	b = f_div((top+bottom) , (top-bottom));
	c = f_div(-(far+near) , ( far-near));
	d = f_div(-f_mul(2*far,near) , (far-near));  /* error? */

#define M(row,col)  ((real_t*)elements)[col*4+row]
	M(0,0) = x;     M(0,1) = 0;  M(0,2) = a;      M(0,3) = 0;
	M(1,0) = 0;  M(1,1) = y;     M(1,2) = b;      M(1,3) = 0;
	M(2,0) = 0;  M(2,1) = 0;  M(2,2) = c;      M(2,3) = d;
	M(3,0) = 0;  M(3,1) = 0;  M(3,2) = -1;  M(3,3) = 0;
#undef M

//   gl_mat_mul_real_ts( mat, m, MAT_FLAG_PERSPECTIVE );
   return *this;

}



/**
 * brief: adds the passed Matrix4 to self, overloaded with + and +=
 *
 * @param const Matrix4 &p_m1 <source Matrix4 you want to add>
 * @return some function returns self to be used as parameters to other functions or to be constructed as equations
 */

const Matrix4& Matrix4::add(const Matrix4 &p_m1) {
		// adds p_m1 to self
		this->operator +=(p_m1);

		// returns self
		return *this;
}


const Matrix4& Matrix4::add(const Matrix4 &p_m1, const Matrix4 &p_m2)
{
	for( int j = 0; j < 4; j++ ) {
		for( int i = 0; i < 4; i++ ) {
			elements[j][i] = p_m1.elements[j][i] + p_m2.elements[j][i];
		}
	}

	return *this;
}

Matrix4 Matrix4::operator +(const Matrix4 &p_m1)
{
		Matrix4 new_matrix;

		for( int j = 0; j < 4; j++ ) {
				for( int i = 0; i < 4; i++ ) {
						new_matrix[j][i] = elements[j][i] + p_m1.elements[j][i];
				}
		}

		return new_matrix;
}

void Matrix4::operator +=(const Matrix4 &p_m1)
{
		for( int j = 0; j < 4; j++ ) {
				for( int i = 0; i < 4; i++ ) {
						elements[j][i] += p_m1.elements[j][i];
				}
		}

		return;
}

/**
 * brief: subtracts the passed Matrix4 to self, overloaded with - and -=
 *
 * long: @note was the 3dmath lib implementation wrong? check it!
 *
 * @param const Matrix4 &p_m1 <source Matrix4 you want to subtract>
 * @return some function returns self to be used as parameters to other functions or to be constructed as equations
 */

const Matrix4& Matrix4::sub(const Matrix4 &p_m1)
{
		// adds p_m1 to self
		this->operator -=(p_m1);

		// returns self
		return *this;
}

const Matrix4& Matrix4::sub(const Matrix4 &p_m1, const Matrix4 &p_m2)
{
		for( int j = 0; j < 4; j++ ) {
				for( int i = 0; i < 4; i++ ) {
						elements[j][i] = p_m1.elements[j][i] - p_m2.elements[j][i];
				}
		}

		return *this;
}

Matrix4 Matrix4::operator -(const Matrix4 &p_m1)
{
		Matrix4 new_matrix;

		for( int j = 0; j < 4; j++ ) {
				for( int i = 0; i < 4; i++ ) {
						new_matrix[j][i] = elements[j][i] - p_m1.elements[j][i];
				}
		}

		return new_matrix;
}

void Matrix4::operator -=(const Matrix4 &p_m1)
{
		for( int j = 0; j < 4; j++ ) {
				for( int i = 0; i < 4; i++ ) {
						elements[j][i] -= p_m1.elements[j][i];
				}
		}

		return;
}


/**
 * brief: multiplies the passed Matrix4 to self, overloaded with * and *=
 *
 * long: zzzz
 *
 * @param const Matrix4 &p_m1 <source Matrix4 you want to subtract>
 * @return some function returns self to be used as parameters to other functions or to be constructed as equations
 */
const Matrix4& Matrix4::mul(const Matrix4 &p_m1)
{
		// adds p_m1 to self
		this->operator *=(p_m1);

		// returns self
		return *this;
}

const Matrix4& Matrix4::mul(const Matrix4 &p_m1, const Matrix4 &p_m2)
{
	real_t ab;

	for( int j = 0; j < 4; j++ ) {
		for( int i = 0; i < 4; i++ ) {
			ab = 0;
			for( int k = 0; k < 4; k++ )
				ab += p_m1.elements[k][i] * p_m2.elements[j][k];
			elements[j][i] = ab;
		}
	}

	return *this;
}

Matrix4 Matrix4::set_as_inverse4x3() {


	SWAP( elements[0][1], elements[1][0] );
	SWAP( elements[0][2], elements[2][0] );
	SWAP( elements[1][2], elements[2][1] );

	float x= -elements[3][0];
	float y= -elements[3][1];
	float z= -elements[3][2];

	elements[3][0]= (elements[0][0]*x ) + ( elements[1][0]*y ) + ( elements[2][0]*z );
	elements[3][1]= (elements[0][1]*x ) + ( elements[1][1]*y ) + ( elements[2][1]*z );
	elements[3][2]= (elements[0][2]*x ) + ( elements[1][2]*y ) + ( elements[2][2]*z );

	return *this;
}
Matrix4 Matrix4::inverse4x3() const {

	Matrix4 inv = *this;
	inv.set_as_inverse4x3();
	return inv;

}

Matrix4 Matrix4::inverse () const {

	Matrix4 ret(*this);
	ret.set_as_inverse();
	return ret;
};

/**
 * brief: computes the inverse of the matrix
 *
 * long: GGemsII, K.Wu, Fast Matrix Inversion
 *
 * @return self
 */
bool Matrix4::set_as_inverse(){
	int i,j,k;
	int pvt_i[4], pvt_j[4];            /* Locations of pivot elements */
	real_t pvt_val;                     /* Value of current pivot element */
	real_t hold;                        /* Temporary storage */
	real_t determinat;                  /* Determinant */

	determinat = 1.0;
	for (k=0; k<4; k++)  {
		/** Locate k'th pivot element **/
		pvt_val=elements[k][k];            /** Initialize for search **/
		pvt_i[k]=k;
		pvt_j[k]=k;
		for (i=k; i<4; i++) {
			for (j=k; j<4; j++) {
				if (Math::absd(elements[i][j]) > Math::absd(pvt_val)) {
					pvt_i[k]=i;
					pvt_j[k]=j;
					pvt_val=elements[i][j];
				}
			}
		}

		/** Product of pivots, gives determinant when finished **/
		determinat*=pvt_val;
		if (Math::absd(determinat)<1e-7) {
			return(false);  /** Matrix is singular (zero determinant). **/
		}

		/** "Interchange" rows (with sign change stuff) **/
		i=pvt_i[k];
		if (i!=k) {               /** If rows are different **/
			for (j=0; j<4; j++) {
				hold=-elements[k][j];
				elements[k][j]=elements[i][j];
				elements[i][j]=hold;
			}
		}

		/** "Interchange" columns **/
		j=pvt_j[k];
		if (j!=k) {              /** If columns are different **/
			for (i=0; i<4; i++) {
				hold=-elements[i][k];
				elements[i][k]=elements[i][j];
				elements[i][j]=hold;
			}
		}

		/** Divide column by minus pivot value **/
		for (i=0; i<4; i++) {
			if (i!=k) elements[i][k]/=( -pvt_val) ;
		}

		/** Reduce the matrix **/
		for (i=0; i<4; i++) {
			hold = elements[i][k];
			for (j=0; j<4; j++) {
				if (i!=k && j!=k) elements[i][j]+=hold*elements[k][j];
			}
		}

		/** Divide row by pivot **/
		for (j=0; j<4; j++) {
			if (j!=k) elements[k][j]/=pvt_val;
		}

		/** Replace pivot by reciprocal (at last we can touch it). **/
		elements[k][k] = 1.0/pvt_val;
	}

	/* That was most of the work, one final pass of row/column interchange */
	/* to finish */
	for (k=4-2; k>=0; k--) { /* Don't need to work with 1 by 1 corner*/
		i=pvt_j[k];            /* Rows to swap correspond to pivot COLUMN */
		if (i!=k) {            /* If rows are different */
			for(j=0; j<4; j++) {
				hold = elements[k][j];
				elements[k][j]=-elements[i][j];
				elements[i][j]=hold;
			}
		}

		j=pvt_i[k];           /* Columns to swap correspond to pivot ROW */
		if (j!=k)             /* If columns are different */
			for (i=0; i<4; i++) {
			hold=elements[i][k];
			elements[i][k]=-elements[i][j];
			elements[i][j]=hold;
			}
	}
	return(true);
	}

const Matrix4& Matrix4::load_identity()
{
	*this=IDENTITY;

	return *this;
}

/**
 * brief: sets all the class elements to 0
 */
const Matrix4& Matrix4::load_zero()
{
		elements[0][0] = 0;  elements[1][0] = 0;  elements[2][0] = 0;  elements[3][0] = 0;
		elements[0][1] = 0;  elements[1][1] = 0;  elements[2][1] = 0;  elements[3][1] = 0;
		elements[0][2] = 0;  elements[1][2] = 0;  elements[2][2] = 0;  elements[3][2] = 0;
		elements[0][3] = 0;  elements[1][3] = 0;  elements[2][3] = 0;  elements[3][3] = 0;

//		memset(elements, 0x00, (4*4)*sizeof(real_t));

		return *this;
}

/**
 * brief: Builds a matrix using the specified offsets.
 *
 * remarks: calls load_identity() before acting
 *
 * @param real_t tx,ty,tz <offsets used for translation>
 * @return return self so it can be used as function parameters
 */
const Matrix4& Matrix4::load_translate(real_t p_tx, real_t p_ty, real_t p_tz)
{
		load_identity();

		elements[3][0] = (p_tx);
		elements[3][1] = (p_ty);
		elements[3][2] = (p_tz);

		return *this;
}


const Matrix4& Matrix4::load_translate(const Vector3 &p_vector) {

		load_identity();

		elements[3][0] = p_vector.x;
		elements[3][1] = p_vector.y;
		elements[3][2] = p_vector.z;

		return *this;
}



/**
 * brief: translates the existing matrix by the given offset coordinates
 *
 * remarks:
 *
 * @param real_t p_tx,p_ty,p_tz <offsets used for translation>
 * @param const Vector3 &p_v1 <Vector3 containing the offset coordinates for translation>
 * @return return self so it can be used as function parameters
 */
const Matrix4& Matrix4::translate(real_t p_tx, real_t p_ty, real_t p_tz)
{

	real_t tx=(p_tx);
	real_t ty=(p_ty);
	real_t tz=(p_tz);

	for( int i = 0; i < 3; i++ )
		elements[3][i] += f_mul(elements[0][i] , tx) + f_mul(elements[1][i] , ty) + f_mul(elements[2][i] , tz);

	return *this;
}

const Matrix4& Matrix4::translate(const Vector3 &p_v1) {
	for( int i = 0; i < 3; i++ )
		elements[3][i] += f_mul(elements[0][i] , p_v1.x) + f_mul(elements[1][i] , p_v1.y) + f_mul(elements[2][i] , p_v1.z);

	return *this;
}

/**
 * brief: Builds a matrix that scales along the x-, y-, and z-axes.
 *
 * remarks: calls load_identity() before acting
 *
 * @param real_t p_sx,p_sy,p_sz <offsets used for scaling>
 * @return return self so it can be used as function parameters
 */
const Matrix4& Matrix4::load_scale(real_t p_sx, real_t p_sy, real_t p_sz)
{
	load_zero();

	elements[0][0] = (p_sx);
	elements[1][1] = (p_sy);
	elements[2][2] = (p_sz);
	elements[3][3] = 1;

	return *this;
}


/**
 * brief: Builds a matrix that scales along the x-, y-, and z-axes.
 *
 * remarks: calls load_identity() before acting
 *
 * @param real_t p_sx,p_sy,p_sz <offsets used for scaling>
 * @return return self so it can be used as function parameters
 */
const Matrix4& Matrix4::load_scale(const Vector3 &p_scale)
{
	load_zero();

	elements[0][0] = p_scale.x;
	elements[1][1] = p_scale.y;
	elements[2][2] = p_scale.z;
	elements[3][3] = 1;

	return *this;
}

/**
 * brief: scales the existing matrix along the x-, y-, and z-axes.
 *
 * remarks:
 *
 * @param real_t ps_x,p_sy,p_sz <offsets used for scaling>
 * @param const Vector3 &p_v1 <Vector3 containing the sacaling coordinates>
 * @return return self so it can be used as function parameters
 */
const Matrix4& Matrix4::scale(real_t p_sx, real_t p_sy, real_t p_sz)
{
	for( int i = 0; i < 4; i++ ) {
		elements[0][i] = f_mul(elements[0][i],(p_sx));
		elements[1][i] = f_mul(elements[1][i],(p_sy));
		elements[2][i] = f_mul(elements[2][i],(p_sz));
	}

	return *this;
}

const Matrix4& Matrix4::scale(const Vector3 &p_v1)
{
	for( int i = 0; i < 4; i++ ) {
		elements[0][i] = f_mul(elements[0][i],p_v1.x);
		elements[1][i] = f_mul(elements[1][i],p_v1.y);
		elements[2][i] = f_mul(elements[2][i],p_v1.z);
	}

	return *this;
}


/**
 * brief: matrix rotation functions
 *
 * long: functions that rotates the matrix in all the euler angles
 *		 load_ functions also calls load_zero() before acting
 *
 * @param real_t p_phi <rotation offset>
 * @param real_t p_yaw,p_pitch,p_roll <rotation offsets>
 * @return return self so it can be used as function parameters
 */


const Matrix4 &Matrix4::rebuild_from_rotated_vector_and_roll(const Vector3& p_initial,const Vector3& p_final,real_t p_roll) {

        Matrix4 ret;

        Vector3 src = p_initial;
        Vector3 dst = p_final;

	src.normalize();
	dst.normalize();

	// roll is easy
	Matrix4 roll;
	roll.load_rotate(dst,p_roll);

        //Now onto the other..

        Vector3 pivot;

	pivot.set_as_cross_of(src,dst);

	real_t angle = Math::acos ( src.dot(dst) );

	if ( pivot.length() > 1) { // They may be colinear!! Check just in case

		// Seems not!
		pivot.normalize();
		ret.load_rotate(pivot,angle);

	} else {

		//Being them colinear, it seems that i need to just check for the sign
		real_t sign=(angle>0)?1.0:-1.0;
		for (int i=0;i<4;i++) {
			for (int j=0;j<4;j++) {
				if (i==j) {
					ret[i][j]= (i<(2.0)) ? sign : 1.0;
				} else {

					ret[i][j]=0;
				}
        		}
		}
	}



	//ok, finally.
	*this = roll * ret;

	return *this;
}



const Matrix4& Matrix4::rotate_x(real_t p_phi)
{
	real_t a1[4], a2[4];

	real_t sin_phi = Math::sin(p_phi);
	real_t cos_phi = Math::cos(p_phi);

	copymem(a1, elements[1], 4*sizeof(elements[1][0]));
	copymem(a2, elements[2], 4*sizeof(elements[2][0]));

	elements[1][0] = (cos_phi * a1[0]) + (sin_phi * a2[0]);
	elements[1][1] = (cos_phi * a1[1]) + (sin_phi * a2[1]);
	elements[1][2] = (cos_phi * a1[2]) + (sin_phi * a2[2]);
	elements[1][3] = (cos_phi * a1[3]) + (sin_phi * a2[3]);

	elements[2][0] = (-sin_phi * a1[0]) + (cos_phi * a2[0]);
	elements[2][1] = (-sin_phi * a1[1]) + (cos_phi * a2[1]);
	elements[2][2] = (-sin_phi * a1[2]) + (cos_phi * a2[2]);
	elements[2][3] = (-sin_phi * a1[3]) + (cos_phi * a2[3]);

	return *this;}




const Matrix4& Matrix4::rotate_y(real_t p_phi)
{
	real_t a0[4], a2[4];

	real_t sin_phi = Math::sin(p_phi);
	real_t cos_phi = Math::cos(p_phi);

	copymem(a0, elements[0], 4*sizeof(elements[0][0]));
	copymem(a2, elements[2], 4*sizeof(elements[2][0]));

	elements[0][0] = (cos_phi * a0[0]) - (sin_phi * a2[0]);
	elements[0][1] = (cos_phi * a0[1]) - (sin_phi * a2[1]);
	elements[0][2] = (cos_phi * a0[2]) - (sin_phi * a2[2]);
	elements[0][3] = (cos_phi * a0[3]) - (sin_phi * a2[3]);

	elements[2][0] = (sin_phi * a0[0]) + (cos_phi * a2[0]);
	elements[2][1] = (sin_phi * a0[1]) + (cos_phi * a2[1]);
	elements[2][2] = (sin_phi * a0[2]) + (cos_phi * a2[2]);
	elements[2][3] = (sin_phi * a0[3]) + (cos_phi * a2[3]);


	return *this;
}

const Matrix4& Matrix4::rotate_z(real_t p_phi)
{
	real_t a0[4], a1[4];

	real_t sin_phi = Math::sin(p_phi);
	real_t cos_phi = Math::cos(p_phi);

	copymem(a0, elements[0], 4*sizeof(elements[0][0]));
	copymem(a1, elements[1], 4*sizeof(elements[1][0]));

	elements[0][0] = (cos_phi * a0[0]) + (sin_phi * a1[0]);
	elements[0][1] = (cos_phi * a0[1]) + (sin_phi * a1[1]);
	elements[0][2] = (cos_phi * a0[2]) + (sin_phi * a1[2]);
	elements[0][3] = (cos_phi * a0[3]) + (sin_phi * a1[3]);

	elements[1][0] = (-sin_phi * a0[0]) + (cos_phi * a1[0]);
	elements[1][1] = (-sin_phi * a0[1]) + (cos_phi * a1[1]);
	elements[1][2] = (-sin_phi * a0[2]) + (cos_phi * a1[2]);
	elements[1][3] = (-sin_phi * a0[3]) + (cos_phi * a1[3]);

	return *this;
}


const Matrix4& Matrix4::load_rotate_euler(const Vector3& p_axis) {

	load_identity();
	rotate_euler(p_axis);

	return *this;

}


const Matrix4& Matrix4::rotate_euler(const Vector3& p_axis) {

	rotate_x( p_axis.x );
	rotate_y( p_axis.y );
	rotate_z( p_axis.z );

	return *this;
}

bool Matrix4::get_euler_rotation(real_t &pitch, real_t &yaw, real_t &roll) {

        pitch = Math::asin(elements[0][2]);
        if ( pitch < (Math_PI/2) )
        {
            if ( pitch > (-Math_PI/2) )
            {
                yaw = Math::atan2(-elements[1][2],elements[2][2]);
                roll = Math::atan2(-elements[0][1],elements[0][0]);
                return true;
            }
            else
            {
                // WARNING.  Not a unique solution.
                real_t rmy = Math::atan2(elements[1][0],elements[1][1]);
                roll = (0.0);  // any angle works
                yaw = roll - rmy;
                return false;
            }
        }
        else
        {
            // WARNING.  Not a unique solution.
            real_t rpy = Math::atan2(elements[1][0],elements[1][1]);
            roll = (0.0);  // any angle works
            yaw = rpy - roll;
            return false;
        }
}

void Matrix4::set_euler_rotation(real_t pitch, real_t yaw, real_t roll) {


        real_t cosv, sinv;

        cosv = Math::cos(yaw);
        sinv = Math::sin(yaw);

        Matrix4 xmat(1,0,0,0,
		     0,cosv,-sinv,0,
		     0,sinv,cosv,0,
		    0,0,0,1);

        cosv = Math::cos(pitch);
        sinv = Math::sin(pitch);
        Matrix4 ymat(cosv,0,sinv,0,
		     0,1,0,0,
		     -sinv,0,cosv,0,
		    0,0,0,1);


        cosv = Math::cos(roll);
        sinv = Math::sin(roll);
        Matrix4 zmat(cosv,-sinv,0,0,
		     sinv,cosv,0,0,
		     0,0,1,0,
		    0,0,0,1);


        *this = (zmat*ymat)*xmat;
}

const Matrix4& Matrix4::load_rotate(const Quat &q)
{

	load_identity();

	real_t sqw = f_mul(q.w,q.w);
	real_t sqx = f_mul(q.x,q.x);
	real_t sqy = f_mul(q.y,q.y);
	real_t sqz = f_mul(q.z,q.z);

	elements[0][0] =  sqx - sqy - sqz + sqw;
	elements[1][1] = -sqx + sqy - sqz + sqw;
	elements[2][2] = -sqx - sqy + sqz + sqw;

	real_t tmp1 = f_mul(q.x,q.y);
	real_t tmp2 = f_mul(q.z,q.w);
	elements[0][1] = 2 * (tmp1 + tmp2);
	elements[1][0] = 2 * (tmp1 - tmp2);

	tmp1 = f_mul(q.x,q.z);
	tmp2 = f_mul(q.y,q.w);
	elements[0][2] = 2 * (tmp1 - tmp2);
	elements[2][0] = 2 * (tmp1 + tmp2);

	tmp1 = f_mul(q.y,q.z);
	tmp2 = f_mul(q.x,q.w);
	elements[1][2] = 2 * (tmp1 + tmp2);
	elements[2][1] = 2 * (tmp1 - tmp2);

	return *this;
}

const Matrix4& Matrix4::rotate(const Quat &q)
{
	Matrix4 M;
	M.load_rotate(q);
	operator *=(M);

	return *this;
}

const Matrix4& Matrix4::load_rotate(const Vector3 &p_axis, real_t p_phi) {
/*
	Quat Q;
	Q.set(p_axis,p_phi);
	load_rotate(Q);
	return *this;
*/

	Vector3 axis_sq(f_mul(p_axis.x,p_axis.x),f_mul(p_axis.y,p_axis.y),f_mul(p_axis.z,p_axis.z));

	real_t cosine= Math::cos(p_phi);
	real_t sine= Math::sin(p_phi);

	elements[0][0] = axis_sq.x + f_mul(cosine  , ( 1.0 - axis_sq.x ));
	elements[0][1] = f_mul(f_mul(p_axis.x , p_axis.y) , ( 1.0 - cosine )) + f_mul(p_axis.z , sine);
	elements[0][2] = f_mul(f_mul(p_axis.z , p_axis.x) , ( 1.0 - cosine )) - f_mul(p_axis.y , sine);
	elements[0][3] = 0;
	elements[1][0] = f_mul(f_mul(p_axis.x , p_axis.y) , ( 1.0 - cosine )) - f_mul(p_axis.z , sine);
	elements[1][1] = axis_sq.y + f_mul(cosine  , ( 1.0 - axis_sq.y ));
	elements[1][2] = f_mul(f_mul(p_axis.y , p_axis.z) , ( 1.0 - cosine )) + f_mul(p_axis.x , sine);
	elements[1][3] = 0;
	elements[2][0] = f_mul(f_mul(p_axis.z , p_axis.x) , ( 1.0 - cosine )) + f_mul(p_axis.y , sine);
	elements[2][1] = f_mul(f_mul(p_axis.y , p_axis.z) , ( 1.0 - cosine )) - f_mul(p_axis.x , sine);
	elements[2][2] = axis_sq.z + f_mul(cosine  , ( 1.0 - axis_sq.z ));
	elements[2][3] = 0;
	elements[3][0] = 0;
	elements[3][1] = 0;
	elements[3][2] = 0;
	elements[3][3] = 1;
	return *this;
}

const Matrix4& Matrix4::rotate(const Vector3 &p_axis, real_t p_phi)
{

		Matrix4 aux;
		aux.load_rotate(p_axis,p_phi);
		operator*=(aux);

		return *this;
}


void Matrix4::orthonormalize() {

#define ROWS 3
#define COLS 3
	int p,q,k,i,j;
	real_t innersum;
	real_t length;
    //Matrix scr = S.mul(this);
	Matrix4 result;


	for(p=0; p<COLS; p++) {// columns - Loops over all vectors
    		for(i=0; i<ROWS; i++) // ROWS
			result.elements[i][p] = elements[i][p];

      		for(k=0; k<p; k++) { // Substracts the previous vector

			// First the calculation of the product <phi_p|phi_k>=length
			length = 0;
			for(i=0; i<ROWS; i++) {// Loops over all vectors

				innersum = 0;
				for(j=0; j<ROWS; j++)
					innersum += f_mul(result.elements[j][p],elements[i][j]);

				length += f_mul(result.elements[i][k],innersum);
			}

			// Then the substraction of  phi_k*length
			for(q=0; q<ROWS; q++)
				result.elements[q][p] -= f_mul(result.elements[q][k],length);
		}

      // Calculates the integral for normalization
		length = 0;
		for(i=0; i<ROWS; i++)
			for(j=0; j<ROWS; j++)
				length += f_mul(f_mul(result.elements[i][p],result.elements[j][p]),elements[i][j]);

		length = Math::sqrt(length);

      // Normalizes the vector
		if (Math::absd(length)>CMP_EPSILON) {
			for(q=0; q<ROWS; q++)
				result.elements[q][p]=f_div(result.elements[q][p], length);
		} else {
			ERR_PRINT("Null length in vector");
		}
    }

    result.set_translation( get_translation() );
    *this=result;
}

/**
 * brief: matrix with vector transformations
 *
 * @param Vector3 or 4D <transformation>
 * @return return self so it can be used as function parameters
 */





void Matrix4::transpose() {
	real_t elements_copy[4][4];

	//elements_copy = elements;
	copymem(elements_copy, elements, 4*4*sizeof(real_t));

//	elements[0][0] = elements_copy[0][0];
	elements[0][1] = elements_copy[1][0];
	elements[0][2] = elements_copy[2][0];
	elements[0][3] = elements_copy[3][0];

	elements[1][0] = elements_copy[0][1];
//	elements[1][1] = elements_copy[1][1];
	elements[1][2] = elements_copy[2][1];
	elements[1][3] = elements_copy[3][1];

	elements[2][0] = elements_copy[0][2];
	elements[2][1] = elements_copy[1][2];
//	elements[2][2] = elements_copy[2][2];
	elements[2][3] = elements_copy[3][2];


	elements[3][0] = elements_copy[0][3];
	elements[3][1] = elements_copy[1][3];
	elements[3][2] = elements_copy[2][3];
//	elements[3][3] = elements_copy[3][3];


// Note, the transposed matrix of a 4x4 rotation matrix (without
// translation) is guaranteed to be its inverse.

}



void Matrix4::set_cylindrical_billboard(const Matrix4 &p_camera_matrix,
					 const Matrix4 &p_transformation_matrix,
					 Matrix4 *p_inverse_camera_matrix) {

}


//extern lua_State* testing_lua_state;

void Matrix4::set_spherical_billboard(const Matrix4 &p_camera_matrix,
				       const Matrix4 &p_transformation_matrix,
				       Matrix4 *p_inverse_camera_matrix,
				       Vector3 *p_scale_factor) {

	if (p_inverse_camera_matrix) {

		set(*p_inverse_camera_matrix);

	} else {

		set(p_camera_matrix);
		set_as_inverse();
	};

	for(int i=0;i<4;i++) {

		elements[i][3]=p_transformation_matrix.elements[i][3];
	}


	if (p_scale_factor) {

		Matrix4 dist = p_camera_matrix * p_transformation_matrix;
		scale(*p_scale_factor * dist.get_translation().length());
	};

	return;
}


Matrix4::Matrix4(const Quat& p_quat) {

	load_identity();

	real_t sqw = f_mul(p_quat.w,p_quat.w);
	real_t sqx = f_mul(p_quat.x,p_quat.x);
	real_t sqy = f_mul(p_quat.y,p_quat.y);
	real_t sqz = f_mul(p_quat.z,p_quat.z);

	elements[0][0] =  sqx - sqy - sqz + sqw;
	elements[1][1] = -sqx + sqy - sqz + sqw;
	elements[2][2] = -sqx - sqy + sqz + sqw;

	real_t tmp1 = f_mul(p_quat.x,p_quat.y);
	real_t tmp2 = f_mul(p_quat.z,p_quat.w);

	elements[0][1] = f_mul((2.0) , (tmp1 + tmp2));
	elements[1][0] = f_mul((2.0) , (tmp1 - tmp2));

	tmp1 = f_mul(p_quat.x,p_quat.z);
	tmp2 = f_mul(p_quat.y,p_quat.w);

	elements[0][2] = f_mul((2.0) , (tmp1 - tmp2));
	elements[2][0] = f_mul((2.0) , (tmp1 + tmp2));

	tmp1 = f_mul(p_quat.y,p_quat.z);
	tmp2 = f_mul(p_quat.x,p_quat.w);

	elements[1][2] = f_mul((2.0) , (tmp1 + tmp2));
	elements[2][1] = f_mul((2.0) , (tmp1 - tmp2));

}

const Matrix4 &Matrix4::translate_neg(const Vector3& T) { return translate(-T); };

void Matrix4::transform(Plane &p_plane) const {



	Vector3 point=p_plane.normal*p_plane.d;
	Vector3 point_dir=point+p_plane.normal;
	transform(point);
	transform(point_dir);

	Vector3 normal=point_dir-point;
	normal.normalize();
	real_t d=normal.dot(point);

	p_plane.normal=normal;
	p_plane.d=d;

}

void Matrix4::inverse_transform(Plane &p_plane) const {



	Vector3 point=p_plane.normal*p_plane.d;
	Vector3 point_dir=point+p_plane.normal;
	inverse_transform(point);
	inverse_transform(point_dir);

	Vector3 normal=point_dir-point;
	normal.normalize();
	real_t d=normal.dot(point);

	p_plane.normal=normal;
	p_plane.d=d;

}


Vector3 Matrix4::get_scale() const {

	return Vector3( get_x().length(), get_y().length(), get_z().length() );
};

Vector3 Matrix4::get_x() const
{
    return Vector3(elements[0][0],elements[0][1],elements[0][2]);
}

Vector3 Matrix4::get_y() const
{
    return Vector3(elements[1][0],elements[1][1],elements[1][2]);
}

Vector3 Matrix4::get_z() const
{
    return Vector3(elements[2][0],elements[2][1],elements[2][2]);
}


