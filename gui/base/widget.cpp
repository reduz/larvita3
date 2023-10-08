
#include "widget.h"
#include "base/window.h"

#include <stdio.h>
namespace GUI {

/** ARIEL, DEJA DE VOLVER A PONER __PRINT_ERROR ACA!  :( ponelo en el .cpp de algun otro lado! */
struct WidgetPrivate {
	
	Size minimum_size;
	
};

Frame* Widget::find_closest_focusable_to(Point p_point,Direction p_dir,int &mindist) {
	
	return 0;
}



void Widget::set_minimum_size(const Size & p_size ) {
	
	_wp->minimum_size=p_size;
}
Size Widget::get_minimum_size() {
	
	if (!get_window())
		return _wp->minimum_size;
		
	Size internal=get_minimum_size_internal();
	if (internal.width < _wp->minimum_size.width )
		internal.width=_wp->minimum_size.width;
				
	if (internal.height < _wp->minimum_size.height )
		internal.height=_wp->minimum_size.height;
		
	return internal;
}

Size Widget::get_minimum_size_internal() {
	
	return Size();
}


Timer *Widget::get_timer() {
	
	if (!get_window())
		return 0;
	return get_window()->get_timer();
	
}


Painter *Widget::get_painter() {
	if (!get_window())
		return 0;
	return get_window()->get_painter();
}

void Widget::resize(const Size& p_new_size) {
	
	
}

void Widget::draw(const Point& p_pos,const Size& p_size,const Rect& p_exposed) {
	
	
}

void Widget::mouse_button(const Point& p_pos, int p_button,bool p_press,int p_modifier_mask) {
	
	
}
void Widget::mouse_motion(const Point& p_pos, const Point& p_rel, int p_button_mask) {
	
	
}

void Widget::mouse_doubleclick(const Point& p_pos,int p_modifier_mask) {
	
	
}

bool Widget::key(unsigned long p_unicode, unsigned long p_scan_code,bool p_press,bool p_repeat,int p_modifier_mask) {
	
	return false;
}

void Widget::draw_tree(const Point& p_global,const Size& p_size,const Rect& p_exposed) {

	
	draw(p_global, p_size, p_exposed);
}

Widget::Widget() {
	
	_wp = GUI_NEW( WidgetPrivate );
}
Widget::~Widget() {
	
	GUI_DELETE( _wp );
}



} //end of namespace
