//
// C++ Implementation: math_funcs
//
// Description:
//
//
// Author: Juan Linietsky <red@lunatea>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "math_funcs.h"


#include <math.h>

Uint32 Math::default_seed=1234567;

Uint32 Math::rand_from_seed(Uint32 *seed) {

	Uint32 k;
	Uint32 s = (*seed);
	if (s == 0)
		s = 0x12345987;
	k = s / 127773;
	s = 16807 * (s - k * 127773) - 2836 * k;
	if (s < 0)
		s += 2147483647;
	(*seed) = s;
	return (s & Math::RANDOM_MAX);
}

Uint32 Math::rand() {

	return rand_from_seed(&default_seed);
}

double Math::randf() {

	return (double)rand() / (double)RANDOM_MAX;
}

double Math::sin(double p_x) {

	return ::sin(p_x);
}
double Math::cos(double p_x) {

	return ::cos(p_x);

}

double Math::round(double p_val) {

	if (p_val>0) {
		return ::floor(p_val+0.5);
	} else {
		p_val=-p_val;
		return -::floor(p_val+0.5);
	}
}
double Math::asin(double p_x) {

	return ::asin(p_x);

}
double Math::acos(double p_x) {

	return ::acos(p_x);
}
double Math::atan2(double p_y, double p_x) {

	return ::atan2(p_y,p_x);
}

double Math::tan(double p_x) {

	return ::tan(p_x);
};


double Math::sqrt(double p_x) {

	return ::sqrt(p_x);
}

double Math::fmod(double p_x,double p_y) {

	return ::fmod(p_x,p_y);
}

double Math::floor(double p_x) {

	return ::floor(p_x);
}

double Math::random(double from, double to) {

	unsigned int seed = Math::rand();
    double ret = (float)seed/(float)Math::RANDOM_MAX;
	return (ret)*(to-from) + from;
}

double Math::pow(double x, double y) {

	return ::pow(x,y);
}
