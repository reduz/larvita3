//
// C++ Implementation: tab_bar
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "tab_bar.h"
#include "base/painter.h"
#include "base/skin.h"
#include <stdio.h>

namespace GUI {

int TabBar::get_tab_count() {
	
	Tab *l=list;
	
	int idx=0;
	while(l) {
		
		l=l->next;
		idx++;
	}
	
	return idx;
}

void TabBar::select_tab(int p_select) {
	
	if (selected==p_select)
		return;
	
	pre_tab_changed_signal.call(selected);
	selected=p_select;
	tab_changed_signal.call(selected);
	
	update();
	
}

bool TabBar::key(unsigned long p_unicode, unsigned long p_scan_code,bool p_press,bool p_repeat,int p_modifier_mask) {
	
	if (!p_press)
		return false;
	switch (p_scan_code) {
	
		case KEY_LEFT: {
			
			int newtab=get_selected_tab();
			while(newtab>0) { 
			
				newtab--;
				if (is_tab_visible( newtab )) {
					select_tab( newtab );
					break;
				}
			}
			
		} break;
		case KEY_RIGHT: {
			
			int newtab=get_selected_tab();
			while(newtab<(get_tab_count()-1)) { 
				
				newtab++;
				if (is_tab_visible( newtab )) {
					select_tab( newtab );
					break;
				}
			}
				    
				    
		} break;
		default: {
			
			return false;
		} break;
	}
				    
	return true;
	
}

void TabBar::mouse_button(const Point& p_pos, int p_button,bool p_press,int p_modifier_mask) {
	
	if (!p_press)
		return;
	Painter *p=get_painter();
	
	Tab *l=list;
	int cselected=-1;
	int idx=0;
	
	while(l) {
		
		int width=p->get_stylebox_min_size( stylebox( (idx==selected)?COLOR_TABBAR_FONT_RAISED:COLOR_TABBAR_FONT )).width;
		
		width+=p->get_font_string_width( font( FONT_TABBAR ), l->text );
		
		if (p_pos.x >= l->offset && p_pos.x< (l->offset+width ) && l->visible ) {
			/* Found tab */
			
			cselected=idx;
			break;
			
		}
		
		width+=constant( C_TABBAR_TAB_SEPARATOR );
		
		l=l->next;
		idx++;
	}
	
	
	if (cselected<0)
		return;
	
	select_tab( cselected );
	
}


Size TabBar::get_minimum_size_internal() {
	
	Size min;
	Painter *p=get_painter();
	
	min.width+=p->get_stylebox_min_size( stylebox(SB_TABBAR_LEFT) ).width;
	min.width+=p->get_stylebox_min_size( stylebox(SB_TABBAR_RIGHT) ).width;
	
	
	if (list) {
	
		
		min.height=MAX(p->get_stylebox_min_size( stylebox(SB_TABBAR_RAISED) ).height,p->get_stylebox_min_size( stylebox(SB_TABBAR_TAB) ).height);
		
		min.height+=p->get_font_height( font( FONT_TABBAR ) );
		
		Tab *l=list;
	
		bool first=true;
		int current=0;
		
		while (l) {
			
			if (l->visible) {
				min.width+=p->get_font_string_width( font( FONT_TABBAR) ,l->text);
				
				if (current==selected)
					min.width+=p->get_stylebox_min_size( stylebox(SB_TABBAR_RAISED) ).width;
				else
					min.width+=p->get_stylebox_min_size( stylebox(SB_TABBAR_TAB) ).width;
					
				if (!first) {
					min.width+=constant( C_TABBAR_TAB_SEPARATOR );
				}
					first=false;
			}
				
			
			l=l->next;
		}
	}
	
	
	return min;
	
	
}
void TabBar::draw(const Point& p_pos,const Size& p_size,const Rect& p_exposed) {
	
	Painter *p=get_painter();
	
	int center_w=get_minimum_size().width;
	center_w-=p->get_stylebox_min_size( stylebox(SB_TABBAR_LEFT) ).width; 
	center_w-=p->get_stylebox_min_size( stylebox(SB_TABBAR_RIGHT) ).width; 
	
	int center_ofs=constant(C_TABBAR_CENTERED)?( (p_size.width-center_w)/2 ):p->get_stylebox_min_size( stylebox(SB_TABBAR_LEFT) ).width;
	
	int ofs=0;
	
	p->draw_stylebox( stylebox(SB_TABBAR_LEFT), Point(0,0), Size(center_ofs,p_size.height) );
	
	ofs+=center_ofs;
		
	int font_h=p->get_font_height( font( FONT_TABBAR ) );
	
	
	int idx=0;
	
	Tab *l=list;
	
	bool first=true;
	while(l) {
		
		if (!l->visible) {
			
			l=l->next;
			idx++;
			continue;
			
		}
		
		if (!first) {
		
			ofs+=constant( C_TABBAR_TAB_SEPARATOR );
		} else
			first=false;
		
		const StyleBox &sb= stylebox( (idx==selected) ?SB_TABBAR_RAISED:SB_TABBAR_TAB );
		Color col=color((idx==selected) ?COLOR_TABBAR_FONT_RAISED:COLOR_TABBAR_FONT);
		
		int string_w=p->get_font_string_width( font( FONT_TABBAR) ,l->text);
		
		Size sbmin=p->get_stylebox_min_size( sb );
		
		sbmin.width+=string_w;
		sbmin.height+=font_h;
		
		Point sbpos=Point( ofs, p_size.height-sbmin.height );
		
		p->draw_stylebox( sb , sbpos, sbmin );
		if (has_focus() && (idx==selected))
			p->draw_stylebox( stylebox(SB_TABBAR_FOCUS) , sbpos, sbmin );
		
		Point txtpos=sbpos+Point( p->get_stylebox_margin( sb, MARGIN_LEFT), p->get_stylebox_margin( sb, MARGIN_TOP)+p->get_font_ascent( font(FONT_TABBAR) ) );
		
		p->draw_text( font(FONT_TABBAR), txtpos, l->text,col);
		
		l->offset=sbpos.x;
		
		ofs+=sbmin.width;
		
		
		l=l->next;
		idx++;
	}
	
	p->draw_stylebox( stylebox(SB_TABBAR_RIGHT), Point(ofs,0), Size(p_size.width-ofs,p_size.height) );
}

void TabBar::add_tab(String p_text,bool p_front) {
	
	Tab *t = GUI_NEW( Tab );
	t->text=p_text;
	
	if (!list|| p_front) {
		t->next=list;
		list=t;
	} else {
		Tab *l=list;	
		while (l) {
	
			if (!l->next) {
	
				l->next=t;
				break;
	
			}
	
			l=l->next;
	
		}
	}	
	
	check_minimum_size();
	update();
}


void TabBar::set_tab_visible(int p_which, bool p_visible) {
	
	Tab *l=list;
	
	int idx=0;
	while(l) {
		
		if (p_which==idx) {
			
			l->visible=p_visible;
			update();
			return;
		}
		
		l=l->next;
		idx++;
	}
	
	PRINT_ERROR("INVALID TAB");
}
bool TabBar::is_tab_visible(int p_which) {
	
	Tab *l=list;
	
	int idx=0;
	while(l) {
		
		if (p_which==idx) {
			
			return l->visible;
		}
		
		l=l->next;
		idx++;
	}
	
	PRINT_ERROR("INVALID TAB");	
	return false;
}

int TabBar::get_selected_tab() {
	
	return selected;	
}
void TabBar::set_tab(int p_selected) {
	
	selected=p_selected;
	update();
}

String TabBar::get_type() {
	
	return "TabBar";
}

TabBar::TabBar()
{
	
	set_focus_mode( FOCUS_ALL );
	list=0;
	selected=0;
}


TabBar::~TabBar()
{
}


}
