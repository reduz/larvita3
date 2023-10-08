//
// C++ Interface: range
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef PIGUIRANGE_H
#define PIGUIRANGE_H


#include "signals/signals.h"
#include "string/rstring.h"

namespace GUI {

/**
	@author Juan Linietsky <reduzio@gmail.com>
*/

/**
 * Range class. This holds ranges of all kinds.
 * Internally, it works by using double numbers. This
 * will allow it to just represent any kind of useful numbers.
 */

class RangeBase {
public:

	Signal<> range_changed_signal; //if step/min/max changes
	Signal< Method1<double> > value_changed_signal; //if value changes
	
	virtual void set_min(double p_min)=0;
	virtual double get_min()=0;

	virtual void set_max(double p_max)=0;
	virtual double get_max()=0;

	virtual void set(double p_val)=0;
	virtual double get()=0;
	
	virtual void set_step(double p_step)=0;
	virtual double get_step()=0;
	
	virtual void set_page(double p_step)=0;
	virtual double get_page()=0;
	
	virtual double get_unit_value(); ///< return value in range 0 - 1 , can be overriden for "curving" 
	virtual void set_unit_value(double p_v); ///< return set in range 0 - 1 , can be overriden for "curving" 
	
	virtual String get_as_text();
	void step_increment();
	void step_decrement();
	
	void config(double p_min, double p_max, double p_val,double p_step);
	
	virtual ~RangeBase() {}
};
 
class Range : public RangeBase {

	double min;
	double max;
	double value;
	double step;
	double page;

public:

	
	void set_min(double p_min);
	double get_min();

	void set_max(double p_max);
	double get_max();

	void set(double p_val);
	double get();
	
	void set_step(double p_step);
	double get_step();
	
	void set_page(double p_page);
	double get_page();
	
	Range();


};

}

#endif
