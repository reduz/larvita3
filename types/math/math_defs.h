

#ifndef MATH_DEFS_H
#define MATH_DEFS_H

#define EPSILON 0.000001
#define CMP_EPSILON 0.00001
#define CMP_EPSILON2 (CMP_EPSILON*CMP_EPSILON)
#define CMP_NORMALIZE_TOLERANCE 0.000001
#define CMP_POINT_IN_PLANE_EPSILON 0.00001

/**
  * "Real" is a type that will be translated to either floats or fixed depending
  * on the compilation setting
  */

enum ClockDirection {

        CLOCKWISE,
	COUNTERCLOCKWISE
};


#ifdef FLOAT_T_IS_DOUBLE

typedef double real_t;

#else

typedef float real_t;

#endif

#endif
