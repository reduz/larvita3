

#ifndef PIGUI_WIDGET_H
#define PIGUI_WIDGET_H

#include "base/frame.h"

namespace GUI {


struct WidgetPrivate;
class Painter;
class Timer;

class Widget : public Frame {

	WidgetPrivate *_wp;
	
	void draw_tree(const Point& p_global,const Size& p_size,const Rect& p_exposed);
	
	Frame* find_closest_focusable_to(Point p_point,Direction p_dir,int &mindist);
	
protected:	
	
	virtual Size get_minimum_size_internal(); ///< Use this one when writing widgets
	Painter *get_painter(); ///< Will return 0 unless inside a window
	Timer *get_timer(); ///< Will return 0 unless inside a window
	
	
public:

	virtual void set_minimum_size(const Size & p_size );
	virtual Size get_minimum_size();
	
	/* Events & Ddefault Handlers */
	virtual void resize(const Size& p_new_size);
	virtual void draw(const Point& p_pos,const Size& p_size,const Rect& p_exposed);
	virtual void mouse_button(const Point& p_pos, int p_button,bool p_press,int p_modifier_mask); ///< either pressed or released, 
	virtual void mouse_doubleclick(const Point& p_pos,int p_modifier_mask);
	
	virtual void mouse_motion(const Point& p_pos, const Point& p_rel, int p_button_mask);
	virtual bool key(unsigned long p_unicode, unsigned long p_scan_code,bool p_press,bool p_repeat,int p_modifier_mask);
	

	
	Widget();
	~Widget();

};


} //end of namespace


#endif

