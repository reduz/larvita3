//
// C++ Implementation: range
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "range.h"

#ifndef NO_MATH_LIB
#include <math.h>
#endif

namespace GUI {
void RangeBase::config(double p_min, double p_max, double p_val,double p_step) {
	
	set_min(p_min);
	set_max(p_max);
	set_step(p_step);
	set(p_val);
	
}
void RangeBase::step_increment() {
	
	set( get()+get_step() );	
}
void RangeBase::step_decrement(){
	
	set( get()-get_step() );	
	
}

String RangeBase::get_as_text() {
	
	
	double r=get_step();
	
	int r_int=(int)r;
	// never use more than 4 decimals, otherwise doubles become unprecise
	int max=4;
	int decs=0;
	while ( (r-(double)r_int)!=0 && max) {
		
		r-=(double)r_int;
		r*=10.0;
		max--;
		decs++;
		r_int=(int)r;
	}

	return String::num( get(), decs );
}

double RangeBase::get_unit_value() {
	
	double range=get_max()-get_min();
	double val=(get()-get_min());

	if (range==0)
		return 0;
	
	double unit_val=val/range;

	return unit_val;
		
}

void RangeBase::set_unit_value(double p_v) {
	
	if (p_v==1.0) {
		
		set( get_max() );
		return;
	}
	
	double range=get_max()-get_min();

	p_v*=range;
	

	p_v+=get_min();

#ifndef NO_MATH_LIB
	if (get_step()!=0) {

		p_v=floor( p_v / get_step() + 0.5 ) * get_step();

	}
#endif
	
	set( p_v );
	
}


void Range::set_min(double p_min) {

	min=p_min;
	range_changed_signal.call();
}
double Range::get_min() {

	return min;
	
}

void Range::set_max(double p_max) {


	max=p_max;
	range_changed_signal.call();
}
double Range::get_max() {


	return max;
}

void Range::set(double p_val) {

	if  (p_val>(max-page))
		p_val=(max-page);
	if  (p_val<min)
		p_val=min;

	if (value==p_val)
		return;
	
#ifndef NO_MATH_LIB
	if (step!=0) {

		p_val=floor( p_val / step + 0.5) * step;

	}
#endif
	
	value=p_val;

	value_changed_signal.call(value);
	
}
double Range::get() {

	return value;
}

void Range::set_step(double p_step) {

	step=p_step;
	range_changed_signal.call();
}
double Range::get_step() {

	return step;
}

void Range::set_page(double p_page) {

	page=p_page;
	
	if  (value>(max-page))
		set(max-page);
	
	range_changed_signal.call();
}
double Range::get_page() {

	return page;
}

Range::Range() {

	min=0;
	value=0;
	max=100;
	step=1;
	page=0;
	

}


}
