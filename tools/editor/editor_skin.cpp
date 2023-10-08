//
// C++ Implementation: editor_skin
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "editor_skin.h"
#include "editor/icons/xpm_inc.h"


#define SET_STYLEBOX( m_which , m_style )\
set_stylebox_name(m_which,#m_which);\
set_stylebox(m_which,m_style);

#define SET_CONSTANT( m_which , m_constant )\
set_constant_name(m_which,#m_which);\
set_constant(m_which,m_constant);

#define SET_FONT( m_which , m_font )\
set_font_name(m_which,#m_which);\
set_font(m_which,m_font);

#define SET_BITMAP( m_which , m_bitmap )\
set_bitmap_name(m_which,#m_which);\
set_bitmap(m_which,m_bitmap);

#define SET_COLOR( m_which , m_color )\
set_color_name(m_which,#m_which);\
set_color(m_which,m_color);


EditorSkin::EditorSkin(GUI::Painter *p_painter,int p_stylebox_max,int p_constant_max,int p_bitmap_max,int p_font_max,int p_color_max) : GUI::Skin( p_stylebox_max, p_constant_max, p_bitmap_max, p_font_max, p_color_max) {
	
	painter=p_painter;
#include "editor/icons/xpm_skin.inc"		
			 
	SET_CONSTANT(C_EDITOR_VARIANT_LIST_WIDTH,170);
	SET_CONSTANT(C_EDITOR_VARIANT_LIST_HEIGHT,200);
	
	SET_STYLEBOX(SB_EDITOR_VIEWPORT_FOCUS, GUI::StyleBox(2,GUI::Color(255,0,0),GUI::Color(255,0,0) ) );
}


EditorSkin::~EditorSkin() {
	
}
