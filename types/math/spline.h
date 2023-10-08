#ifndef SPLINE_H
#define SPLINE_H

#include "vector3.h"
#include "dvector.h"

class Spline {
	
	DVector<Vector3> points;
	DVector<float> offset_cache;
	bool dirty;
	float length;

	void rebuild_offset_cache();

public:
	
	int get_point_count() const;
	void set_point_count(int p_count);
	
	Vector3 get_point(int p_idx) const;
	void set_point(int p_idx, Vector3 p_point);

	int add_point(Vector3 point);

	float get_length();
	
	Vector3 interpolate(float p_t);
	
	Spline();
	~Spline();	
};


#endif
