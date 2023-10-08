//
// C++ Interface: label
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef PIGUILABEL_H
#define PIGUILABEL_H

#include "base/widget.h"

namespace GUI {

/**
	@author Juan Linietsky <reduzio@gmail.com>
*/
class Label : public Widget {
public:	
	enum Align {
		
		ALIGN_LEFT,
		ALIGN_CENTER,
		ALIGN_RIGHT
	};
private:
	
	struct LineCache {
		
		int line_end_ofs;
		int line_size;
		String text;
		LineCache() { line_size=0; line_end_ofs=0; }
	};
	
	LineCache *line_cache;
	int line_cache_count;
	int line_cache_max_w;
	
	Point shadow_offset;
	int shadow_size;
	
	Align align;
	
	String text;
	
	void regenerate_line_cache();
	Size get_minimum_size_internal();
	void set_in_window();
protected:

	
	virtual String get_type();	
public:
	
	//void mouse_enter() { update(); } for testing
	
	void draw(const Point& p_pos,const Size& p_size,const Rect& p_exposed);
	
	void set_align(Align p_align);
	Align get_align();
	
	void set_text(const String& p_string);
	String get_text();
	
	void set_shadow_size(int p_size);
	void set_shadow_offset(const Point& p_offset);
	
	Label(String p_text="",Align p_align=ALIGN_LEFT);

    ~Label();

};

}

#endif
