 //
// C++ Interface: math_funcs
//
// Description:
//
//
// Author: Juan Linietsky <red@lunatea>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "typedefs.h"

/**
 * Generic Math Functions the float math classes use
 */

#ifndef MATH_FUNCS_H
#define MATH_FUNCS_H
class Math {

	Math();
	static Uint32 default_seed;
public:

	enum {
		RANDOM_MAX=2147483647L
	};

	static double sin(double p_x);
	static double cos(double p_x);
	static double asin(double p_x);
	static double acos(double p_x);
	static double atan2(double p_y, double p_x);
	static double tan(double p_x);
	static double sqrt(double p_x);
	static double fmod(double p_x,double p_y);
	static Uint32 rand_from_seed(Uint32 *seed);
	static double floor(double p_x);
	
	static Uint32 rand();
	static double randf();
	
	static double round(double p_val);

	static double random(double from, double to);
	
	static _FORCE_INLINE_ float absf(float g) {
		
		union {
			float f;
			Uint32 i;
		} u;

		u.f=g;
		u.i&=2147483647u;
		return u.f;
	}

	static _FORCE_INLINE_ double absd(double g) {

		union {
			double d;
			Uint64 i;
		} u;
		u.d=g;
		u.i&=(Uint64)9223372036854775807ll;
		return u.d;
	}

	static double pow(double x, double y);
	
};


#define Math_PI 3.14159265358979323846

#endif

