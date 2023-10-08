//
// C++ Interface: editor_skin
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef EDITOR_SKIN_H
#define EDITOR_SKIN_H

#include "base/skin.h"
#include "base/painter.h"
			
enum EditorStyleBoxList{
	EDITOR_SB_FIRST=GUI::SB_MAX,
 	SB_EDITOR_VIEWPORT_FOCUS,
 	EDITOR_SB_MAX
};
	
enum EditorColorList {
	EDITOR_COLOR_FIRST=GUI::COLOR_MAX,
	EDITOR_COLOR_MAX
};

enum EditorConstantList {

	EDITOR_C_FIRST=GUI::C_MAX,
	C_EDITOR_VARIANT_LIST_WIDTH,
	C_EDITOR_VARIANT_LIST_HEIGHT,
	EDITOR_C_MAX
};
	
enum EditorFontList {
	EDITOR_FONT_FIRST=GUI::FONT_MAX,
	EDITOR_FONT_MAX
};

enum EditorBitmapList {
	EDITOR_FIRST=GUI::BITMAP_MAX,
#include "editor/icons/xpm_skin.h" 
	EDITOR_BITMAP_MAX,
};


class EditorSkin : public GUI::Skin {
	
	GUI::Painter *painter;

public:	
	EditorSkin(GUI::Painter *p_painter,int p_stylebox_max=EDITOR_SB_MAX,int p_constant_max=EDITOR_C_MAX,int p_bitmap_max=EDITOR_BITMAP_MAX,int p_font_max=EDITOR_FONT_MAX,int p_color_max=EDITOR_COLOR_MAX);
	~EditorSkin();
};

#endif
