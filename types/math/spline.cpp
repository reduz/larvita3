#include "spline.h"

int Spline::get_point_count() const {

	return points.size();
};

int Spline::add_point(Vector3 point) {
	
	points.push_back(point);
	return points.size() - 1;
};


void Spline::set_point_count(int p_count) {
	
	points.resize(p_count);
	dirty = true;
};

Vector3 Spline::get_point(int p_idx) const {

	ERR_FAIL_INDEX_V( p_idx, points.size(), Vector3() );

	return points[p_idx];
};

void Spline::set_point(int p_idx, Vector3 p_point) {

	ERR_FAIL_INDEX( p_idx, points.size() );
	
	points.set(p_idx, p_point);
	dirty = true;
};

void Spline::rebuild_offset_cache() {
	
	offset_cache.resize(points.size());

	offset_cache.write_lock();
	points.read_lock();
	
	float total = 0;
	for (int i=1; i<points.size(); i++) {
		
		float d = points.read()[i-1].distance_to(points.read()[i]);
		offset_cache.write()[i] = d; // set the segment length
		total += d;
	};
	
	length = total;

	offset_cache.write()[0] = 0;
	for (int i=1; i<points.size(); i++) {

		float last = offset_cache.read()[i-1];
		float len = offset_cache.read()[i];
		offset_cache.write()[i] = last + (len / total);
	};

	offset_cache.write_unlock();
	points.read_unlock();
	
	dirty = false;
};

float Spline::get_length() {
	
	if (dirty) {
		
		rebuild_offset_cache();
	};
	
	return length;
};

Vector3 Spline::interpolate(float p_t) {

	if (dirty) {

		rebuild_offset_cache();
	};
	
	ERR_FAIL_COND_V( p_t < 0 || p_t > 1, Vector3() );

	if (p_t == 0)
		return points.get(0);
	
	if (p_t == 1)
		return points.get(points.size() -1);
	
	offset_cache.read_lock();

	// binary search
	//*
	int sz = points.size();
	if (sz==0)
		return Vector3();

	int low = 0;
	int high = sz -1;
	int middle;
	
	while( low <= high ) {
		
		middle = (low + high) / 2;

		float f = offset_cache.read()[middle];
		if (f == p_t)

			break;

		else if( p_t < f )

			high = middle - 1; //search low end of array
		else

			low = middle + 1; //search high end of array
	}

	if (offset_cache.read()[middle]>p_t)
		middle--;
	//	*/

/*	
	int middle = 0;
	for (int i=0; i<points.size(); i++) {
		
		if (offset_cache.read()[i] > p_t) {
			middle = i-1;
			break;
		};
	};
	*/

	float t1 = offset_cache.read()[middle];
	float t2 = offset_cache.read()[middle+1];

	offset_cache.read_unlock();

	float time = (p_t - t1) / (t2 - t1);
	
	Vector3 p1 = points.get(middle);
	Vector3 p2 = points.get(middle + 1);
	
	return Vector3::lerp(p1, p2, time);
};

Spline::Spline() {

	dirty = false;
	length = 0;
};

Spline::~Spline() {

};

