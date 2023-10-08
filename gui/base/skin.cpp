//
// C++ Implementation: skin
//
// Description:
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "skin.h"
#include "base/style_box.h"
#include "base/geometry.h"


namespace GUI {

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


int Skin::find_stylebox(String p_by_name) {


	for (int i=0;i<stylebox_count;i++) {

		if (stylebox_name[i].nocasecmp_to(p_by_name)==0)
			return i;

	}

	return -1;

}
int Skin::find_constant(String p_by_name) {

	for (int i=0;i<constant_count;i++) {

		if (constant_name[i].nocasecmp_to(p_by_name)==0)
			return i;

	}

	return -1;

}
int Skin::find_bitmap(String p_by_name) {

	for (int i=0;i<bitmap_count;i++) {

		if (bitmap_name[i].nocasecmp_to(p_by_name)==0)
			return i;

	}

	return -1;

}
int Skin::find_font(String p_by_name) {

	for (int i=0;i<font_count;i++) {

		if (font_name[i].nocasecmp_to(p_by_name)==0)
			return i;

	}

	return -1;

}
int Skin::find_color(String p_by_name) {

	for (int i=0;i<color_count;i++) {

		if (color_name[i].nocasecmp_to(p_by_name)==0)
			return i;

	}

	return -1;

}

void Skin::set_stylebox_name(int p_which,String p_name) {

	if (p_which<0 || p_which>=stylebox_count)
		return;

	stylebox_name[p_which]=p_name;
}
void Skin::set_constant_name(int p_which,String p_name){

	if (p_which<0 || p_which>=constant_count)
		return;

	constant_name[p_which]=p_name;

}
void Skin::set_bitmap_name(int p_which,String p_name){

	if (p_which<0 || p_which>=bitmap_count)
		return;

	bitmap_name[p_which]=p_name;


}
void Skin::set_font_name(int p_which,String p_name){

	if (p_which<0 || p_which>=font_count)
		return;

	font_name[p_which]=p_name;

}
void Skin::set_color_name(int p_which,String p_name){

	if (p_which<0 || p_which>=color_count)
		return;

	color_name[p_which]=p_name;


}


const StyleBox& Skin::get_stylebox(unsigned int p_which) {

	static StyleBox error;

	if (p_which<0 || p_which>=stylebox_count)
		return error;


	return stylebox_array[p_which];
}
int Skin::get_constant(unsigned int p_which){


	if (p_which<0 || p_which>=constant_count)
		return 0;

	return constant_array[p_which];

}
BitmapID Skin::get_bitmap(unsigned int p_which){

	if (p_which<0 || p_which>=bitmap_count)
		return INVALID_BITMAP_ID;

	return bitmap_array[p_which];

}
FontID Skin::get_font(unsigned int p_which){
	
	if (p_which<0 || p_which>=font_count)
		return 0;

	return font_array[p_which];

}
const Color& Skin::get_color(unsigned int p_which){

	static Color error;

	if (p_which<0 || p_which>=color_count)
		return error;

	return color_array[p_which];

}

void Skin::set_stylebox(int p_which,const StyleBox& p_style) {


	if (p_which<0 || p_which>=stylebox_count)
		return;

	stylebox_array[p_which]=p_style;

}
void Skin::set_constant(int p_which,int p_constant){

	if (p_which<0 || p_which>=constant_count)
		return;

	constant_array[p_which]=p_constant;

}
void Skin::set_bitmap(int p_which,BitmapID p_bitmap){

	if (p_which<0 || p_which>=bitmap_count)
		return;

	bitmap_array[p_which]=p_bitmap;

}
void Skin::set_font(int p_which,FontID p_font){

	if (p_which<0 || p_which>=font_count)
		return;

	font_array[p_which]=p_font;

}
void Skin::set_color(int p_which,const Color& p_color){

	if (p_which<0 || p_which>=color_count)
		return;

	color_array[p_which]=p_color;

}


Skin::Skin(int p_stylebox_max,int p_constant_max,int p_bitmap_max,int p_font_max,int p_color_max) {


	if (p_stylebox_max<SB_MAX)
		p_stylebox_max=SB_MAX;

	if (p_constant_max<C_MAX)
		p_constant_max=C_MAX;

	if (p_bitmap_max<BITMAP_MAX)
		p_bitmap_max=BITMAP_MAX;

	if (p_font_max<FONT_MAX)
		p_font_max=FONT_MAX;

	if (p_color_max<COLOR_MAX)
		p_color_max=COLOR_MAX;


	stylebox_array = GUI_NEW_ARRAY(StyleBox,p_stylebox_max);
	stylebox_name = GUI_NEW_ARRAY(String,p_stylebox_max);
	stylebox_count=p_stylebox_max;


	constant_array = GUI_NEW_ARRAY(int,p_constant_max);
	constant_name = GUI_NEW_ARRAY(String,p_constant_max);
	for (int i=0;i<p_constant_max;i++) {
		constant_array[i]=0;
	}
	constant_count=p_constant_max;

	bitmap_array = GUI_NEW_ARRAY(BitmapID,p_bitmap_max);
	bitmap_name = GUI_NEW_ARRAY(String,p_bitmap_max);
	for (int i=0;i<p_bitmap_max;i++) {
		bitmap_array[i]=-1;
	}
	bitmap_count = p_bitmap_max;

	font_array = GUI_NEW_ARRAY(FontID,p_font_max);
	font_name = GUI_NEW_ARRAY(String,p_font_max);
	for (int i=0;i<p_font_max;i++) {
		font_array[i]=-1;
	}
	font_count = p_font_max;

	color_array = GUI_NEW_ARRAY(Color,p_color_max);
	color_name = GUI_NEW_ARRAY(String,p_color_max);
	
	color_count = p_color_max;

	set_default();

}
Skin::~Skin(){

	GUI_DELETE_ARRAY( stylebox_array );
	GUI_DELETE_ARRAY( constant_array );
	GUI_DELETE_ARRAY( bitmap_array );
	GUI_DELETE_ARRAY( font_array );
	GUI_DELETE_ARRAY( color_array );
	GUI_DELETE_ARRAY( stylebox_name );
	GUI_DELETE_ARRAY( constant_name );
	GUI_DELETE_ARRAY( bitmap_name );
	GUI_DELETE_ARRAY( font_name );
	GUI_DELETE_ARRAY( color_name );

}


/*************** DEFAULT SKIN ********************/


void Skin::set_default() {


	SET_STYLEBOX( SB_EMPTY, StyleBox() )
	SET_FONT( FONT_TOOLTIP, 0 );
	SET_COLOR( COLOR_TOOLTIP_FONT, Color(0) );
	
	/* Base Container */

	SET_CONSTANT( C_TOOLTIP_DISPLACEMENT, 10 );
	SET_STYLEBOX( SB_ROOT_CONTAINER, StyleBox( 0, Color(200,200,200),Color(250,250,250),Color(90,90,90) ) );
	SET_STYLEBOX( SB_CONTAINER, StyleBox() );

	SET_STYLEBOX( SB_TOOLTIP_BG, StyleBox( 1, Color(230,230,200),Color(80,80,0),Color(80,80,0) ) );
	/* Box Container */

	SET_CONSTANT(C_BOX_CONTAINER_DEFAULT_SEPARATION,2);

	/* Margin Group Container */

	SET_CONSTANT(C_MARGIN_GROUP_SPACER_SIZE,10);

	/* PopUp Background */


	SET_STYLEBOX( SB_POPUP_BG, StyleBox( 2, Color(200,200,200),Color(250,250,250),Color(90,90,90) ) );
	SET_STYLEBOX( SB_POPUP_MENU_BG, StyleBox( 2, Color(200,200,200),Color(250,250,250),Color(90,90,90) ) );
	
	for(int i=0;i<4;i++) {
		stylebox_array[SB_POPUP_BG].margins[i]=10;

		stylebox_array[SB_POPUP_MENU_BG].margins[i]=10;
	}
	
	/* WindowTop */


	SET_STYLEBOX( SB_WINDOWTOP,StyleBox( 0, Color( 20,50,255 ), Color( 0 ), Color( 0 ) ));
	stylebox_array[SB_WINDOWTOP].margins[MARGIN_LEFT]=4;

	SET_COLOR( COLOR_WINDOWTOP_FONT, Color(255) );
	SET_FONT( FONT_WINDOWTOP, 0 );

	SET_CONSTANT(C_WINDOWBOX_SPACER,10);
	/* Separator */

	SET_STYLEBOX( SB_SEPARATOR, StyleBox( 2, Color( 160,160,160 ), Color( 130,130,130 ), Color( 200,200,200 ) ) );
	SET_CONSTANT( C_SEPARATOR_EXTRA_MARGIN,3);

	/* Label */

	SET_FONT(FONT_LABEL,0);
	SET_COLOR(COLOR_LABEL_FONT,Color(0));
	SET_COLOR(COLOR_LABEL_FONT_SHADOW,Color(170));
	SET_CONSTANT(C_LABEL_MARGIN,3);


	/* Button */


	SET_STYLEBOX( SB_BUTTON_NORMAL, StyleBox( 2, Color( 150,150,150 ), Color( 250,250,250 ), Color( 50,50,50 ) ) );
	SET_STYLEBOX( SB_BUTTON_PRESSED, StyleBox( 2, Color( 110,110,110 ), Color( 50,50,50 ), Color( 200,200,200 ) ) );
	SET_STYLEBOX( SB_BUTTON_HOVER, StyleBox( 2, Color( 190,190,190 ), Color( 250,250,250 ), Color( 50,50,50 ) ) );
	SET_STYLEBOX( SB_BUTTON_FOCUS, StyleBox( 1, Color( 255,50,50 ), Color( 255,50,50 ) ) );
	SET_STYLEBOX( SB_BUTTON_DISABLED, StyleBox( 2, Color( 110,110,110 ), Color( 50,50,50 ), Color( 200,200,200 ) ) );

	SET_STYLEBOX( SB_BUTTON_CHECKED, StyleBox() ); //of no use here by default
	SET_STYLEBOX( SB_BUTTON_UNCHECKED, StyleBox() ); //of no use here by default

	SET_CONSTANT( C_BUTTON_SEPARATION, 2 );
	SET_CONSTANT( C_BUTTON_EXTRA_MARGIN, 2 );
	SET_CONSTANT( C_BUTTON_DISPLACEMENT, 1 );
	SET_CONSTANT( C_BUTTON_LABEL_ALIGN_CENTER, 1 );
	SET_CONSTANT( C_BUTTON_HAS_CHECKBOX, 0 );
	SET_CONSTANT( C_BUTTON_CHECKBOX_SIZE, 0 );

	SET_BITMAP( BITMAP_BUTTON_CHECKED , -1 );
	SET_BITMAP( BITMAP_BUTTON_UNCHECKED , -1 );

	SET_FONT( FONT_BUTTON, 0 );

	SET_COLOR(COLOR_BUTTON_FONT,Color(0));
	SET_COLOR(COLOR_BUTTON_FONT_HOVER,Color(0));
	SET_COLOR(COLOR_BUTTON_SHORTCUT_FONT,Color(110));
	
	SET_BITMAP( BITMAP_BUTTON_DEFAULT_ICON, -1 );
	/* CheckButton */


	SET_STYLEBOX( SB_CHECKBUTTON_NORMAL, StyleBox() ); //of no use here by default
	SET_STYLEBOX( SB_CHECKBUTTON_PRESSED, StyleBox() ); //of no use here by default
	SET_STYLEBOX( SB_CHECKBUTTON_HOVER, StyleBox( 0, Color( 190,190,190 ), Color( 250,250,250 ), Color( 50,50,50 ) ) );
	SET_STYLEBOX( SB_CHECKBUTTON_FOCUS, StyleBox( 1,  Color( 255,50,50 ), Color( 255,50,50 ) ) );

	SET_STYLEBOX( SB_CHECKBUTTON_CHECKED, StyleBox( 2, Color( 0,0,0 ), Color( 240,240,240 ), Color( 240,240,240 ) ) );
	SET_STYLEBOX( SB_CHECKBUTTON_UNCHECKED, StyleBox( 1, Color( 240,240,240 ), Color( 0,0,0 ), Color( 0,0,0 ) ) );

	SET_CONSTANT( C_CHECKBUTTON_SEPARATION, 8 );
	SET_CONSTANT( C_CHECKBUTTON_EXTRA_MARGIN, 4 );
	SET_CONSTANT( C_CHECKBUTTON_DISPLACEMENT, 0 );
	SET_CONSTANT( C_CHECKBUTTON_CHECKBOX_SIZE, 8 );

	SET_BITMAP( BITMAP_CHECKBUTTON_CHECKED , -1 );
	SET_BITMAP( BITMAP_CHECKBUTTON_UNCHECKED , -1 );

	SET_FONT( FONT_CHECKBUTTON, 0 );

	SET_COLOR(COLOR_CHECKBUTTON_FONT,Color(0));
	SET_COLOR(COLOR_CHECKBUTTON_FONT_HOVER,Color(0));
	SET_COLOR(COLOR_CHECKBUTTON_SHORTCUT_FONT,Color(110));

	/* MenuButton */

	SET_STYLEBOX( SB_MENUBUTTON_NORMAL, StyleBox() ); //of no use here by default
	SET_STYLEBOX( SB_MENUBUTTON_PRESSED, StyleBox( 2, Color( 110,110,110 ), Color( 50,50,50 ), Color( 200,200,200 ) ) );

	SET_STYLEBOX( SB_MENUBUTTON_HOVER, StyleBox( 0, Color( 190,190,190 ), Color( 250,250,250 ), Color( 50,50,50 ) ) );
	SET_STYLEBOX( SB_MENUBUTTON_FOCUS, StyleBox( 1,  Color( 255,50,50 ), Color( 255,50,50 ) ) );

	SET_STYLEBOX( SB_MENUBUTTON_CHECKED, StyleBox( 2, Color( 0,0,0 ), Color( 240,240,240 ), Color( 240,240,240 ) ) );
	SET_STYLEBOX( SB_MENUBUTTON_UNCHECKED, StyleBox( 1, Color( 240,240,240 ), Color( 0,0,0 ), Color( 0,0,0 ) ) );

	SET_CONSTANT( C_MENUBUTTON_SEPARATION, 8 );
	SET_CONSTANT( C_MENUBUTTON_EXTRA_MARGIN, 4 );
	SET_CONSTANT( C_MENUBUTTON_DISPLACEMENT, 0 );
	SET_CONSTANT( C_MENUBUTTON_CHECKBOX_SIZE, 8 );

	SET_BITMAP( BITMAP_MENUBUTTON_CHECKED , -1 );
	SET_BITMAP( BITMAP_MENUBUTTON_UNCHECKED , -1 );

	SET_FONT( FONT_MENUBUTTON, 0 );

	SET_COLOR(COLOR_MENUBUTTON_FONT,Color(0));
	SET_COLOR(COLOR_MENUBUTTON_SHORTCUT_FONT,Color(110));
	SET_COLOR(COLOR_MENUBUTTON_FONT_HOVER,Color(0));

	/* WindowButton */

	SET_FONT(FONT_WINDOWBUTTON,0);
	SET_CONSTANT(C_WINDOWBUTTON_DISPLACEMENT,1);
	SET_BITMAP(BITMAP_WINDOWBUTTON_CLOSE,-1);
	SET_BITMAP(BITMAP_WINDOWBUTTON_MAXIMIZE,-1);
	SET_BITMAP(BITMAP_WINDOWBUTTON_MINIMIZE,-1);

	SET_STYLEBOX( SB_WINDOWBUTTON_NORMAL, StyleBox( 2, Color( 150,150,150 ), Color( 250,250,250 ), Color( 50,50,50 ) ) );
	SET_STYLEBOX( SB_WINDOWBUTTON_PRESSED, StyleBox( 2, Color( 110,110,110 ), Color( 50,50,50 ), Color( 200,200,200 ) ) );
	SET_STYLEBOX( SB_WINDOWBUTTON_HOVER, StyleBox( 2, Color( 190,190,190 ), Color( 250,250,250 ), Color( 50,50,50 ) ) );
	SET_STYLEBOX( SB_WINDOWBUTTON_FOCUS, StyleBox( 1,  Color( 255,50,50 ), Color( 255,50,50 ) ) );


	SET_COLOR(COLOR_WINDOWBUTTON_FONT,0);
	/* Option button */

	SET_STYLEBOX( SB_OPTIONBUTTON_NORMAL, StyleBox( 2, Color( 150,150,150 ), Color( 250,250,250 ), Color( 50,50,50 ) ) );
	SET_STYLEBOX( SB_OPTIONBUTTON_PRESSED, StyleBox( 2, Color( 110,110,110 ), Color( 50,50,50 ), Color( 200,200,200 ) ) );
	SET_STYLEBOX( SB_OPTIONBUTTON_HOVER, StyleBox( 2, Color( 190,190,190 ), Color( 250,250,250 ), Color( 50,50,50 ) ) );
	SET_STYLEBOX( SB_OPTIONBUTTON_FOCUS, StyleBox( 1,  Color( 255,50,50 ), Color( 255,50,50 ) ) );

	SET_CONSTANT( C_OPTIONBUTTON_SEPARATION, 2 );
	SET_CONSTANT( C_OPTIONBUTTON_EXTRA_MARGIN, 2 );
	SET_CONSTANT( C_OPTIONBUTTON_DISPLACEMENT, 1 );

	SET_FONT( FONT_OPTIONBUTTON, 0 );

	SET_COLOR(COLOR_OPTIONBUTTON_FONT,Color(0));
	SET_COLOR(COLOR_OPTIONBUTTON_SHORTCUT_FONT,Color(110));

	/* Option Box */

	SET_STYLEBOX( SB_OPTIONBOX_NORMAL, StyleBox( 2, Color( 150,150,150 ), Color( 250,250,250 ), Color( 50,50,50 ) ) );
	SET_STYLEBOX( SB_OPTIONBOX_PRESSED, StyleBox( 2, Color( 110,110,110 ), Color( 50,50,50 ), Color( 200,200,200 ) ) );
	SET_STYLEBOX( SB_OPTIONBOX_HOVER, StyleBox( 2, Color( 190,190,190 ), Color( 250,250,250 ), Color( 50,50,50 ) ) );
	SET_STYLEBOX( SB_OPTIONBOX_FOCUS, StyleBox( 1,  Color( 255,50,50 ), Color( 255,50,50 ) ) );

	SET_CONSTANT( C_OPTIONBOX_SEPARATION, 2 );
	SET_CONSTANT( C_OPTIONBOX_EXTRA_MARGIN, 2 );
	SET_CONSTANT( C_OPTIONBOX_DISPLACEMENT, 1 );

	SET_FONT( FONT_OPTIONBOX, 0 );

	SET_COLOR(COLOR_OPTIONBOX_FONT,Color(0));
	SET_COLOR(COLOR_OPTIONBOX_SHORTCUT_FONT,Color(110));
	SET_COLOR(COLOR_OPTIONBOX_FONT_HOVER,Color(0));
	
	/* Menu Box */

	//SET_STYLEBOX( SB_MENUBOX_NORMAL, StyleBox( 1, Color( 190,190,190  ), Color( 250,250,250 ), Color( 50,50,50 ) ) );
	SET_STYLEBOX( SB_MENUBOX_NORMAL, StyleBox() ); //of no use here by default
	SET_STYLEBOX( SB_MENUBOX_PRESSED, StyleBox( 2, Color( 110,110,110 ), Color( 50,50,50 ), Color( 200,200,200 ) ) );

	SET_STYLEBOX( SB_MENUBOX_HOVER, StyleBox( 0, Color( 190,190,190 ), Color( 250,250,250 ), Color( 50,50,50 ) ) );
	SET_STYLEBOX( SB_MENUBOX_FOCUS, StyleBox( 1,  Color( 255,50,50 ), Color( 255,50,50 ) ) );
/*	
	SET_STYLEBOX( SB_MENUBOX_NORMAL, StyleBox() );
	SET_STYLEBOX( SB_MENUBOX_PRESSED, StyleBox( 1, Color( 110,110,110 ), Color( 50,50,50 ), Color( 200,200,200 ) ) );
	SET_STYLEBOX( SB_MENUBOX_HOVER, StyleBox( 1, Color( 220,220,220 ), Color( 250,250,250 ), Color( 50,50,50 ) ) );
	SET_STYLEBOX( SB_MENUBOX_FOCUS, StyleBox( 1,  Color( 255,50,50 ), Color( 255,50,50 ) ) );
*/
	SET_CONSTANT( C_MENUBOX_SEPARATION, 8 );
	SET_CONSTANT( C_MENUBOX_EXTRA_MARGIN, 4 );
	SET_CONSTANT( C_MENUBOX_DISPLACEMENT, 0 );

	SET_FONT( FONT_MENUBOX, 0 );

	SET_COLOR(COLOR_MENUBOX_FONT,Color(0));

	/* Slider */

	SET_STYLEBOX( SB_SLIDER_NORMAL_H, StyleBox( 2, Color(120,120,120), Color(60,60,60) ,Color(180,180,180) ) );
	SET_STYLEBOX( SB_SLIDER_NORMAL_V, StyleBox( 2, Color(120,120,120), Color(60,60,60) ,Color(180,180,180) ) );
	SET_STYLEBOX( SB_SLIDER_FOCUS, StyleBox( 1,  Color( 255,80,80 ), Color( 255,80,80 ) ) );
	SET_STYLEBOX( SB_SLIDER_GRABBER_H, StyleBox( 2, Color( 150,150,150 ), Color( 250,250,250 ), Color( 50,50,50) ) );
	SET_STYLEBOX( SB_SLIDER_GRABBER_V, StyleBox( 2, Color( 150,150,150 ), Color( 250,250,250 ), Color( 50,50,50 ) ) );
	SET_CONSTANT( C_SLIDER_GRABBER_SIZE, 14 );
	SET_BITMAP( BITMAP_SLIDER_GRABBER_H, -1 );
	SET_BITMAP( BITMAP_SLIDER_GRABBER_V, -1 );

	/* ScrollBar */

	SET_STYLEBOX( SB_SCROLLBAR_NORMAL_H, StyleBox( 2, Color(120,120,120), Color(60,60,60) ,Color(180,180,180) ) );
	SET_STYLEBOX( SB_SCROLLBAR_NORMAL_V, StyleBox( 2, Color(120,120,120), Color(60,60,60) ,Color(180,180,180) ) );
	SET_STYLEBOX( SB_SCROLLBAR_FOCUS, StyleBox( 1,  Color( 255,80,80 ), Color( 255,80,80 ) ) );
	SET_STYLEBOX( SB_SCROLLBAR_GRABBER_H, StyleBox( 2, Color( 150,150,150 ), Color( 250,250,250 ), Color( 50,50,50 ) ) );
	SET_STYLEBOX( SB_SCROLLBAR_GRABBER_V, StyleBox( 2, Color( 150,150,150 ), Color( 250,250,250 ), Color( 50,50,50 ) ) );
	SET_CONSTANT( C_SCROLLBAR_GRABBER_SIZE, 10 );
	SET_BITMAP( BITMAP_SCROLLBAR_GRIP_V, INVALID_BITMAP_ID );
	SET_BITMAP( BITMAP_SCROLLBAR_GRIP_H, INVALID_BITMAP_ID );

	/* List */

	SET_STYLEBOX( SB_LIST_SELECTED, StyleBox( 0,  Color( 70,100,255 ), Color( 70,100,255 ), Color( 70,100,255 ) ) );
	SET_STYLEBOX( SB_LIST_NORMAL, StyleBox( 2,  Color(240,240,240 ), Color( 60,60,60 ), Color( 150,150,150 ) ) );
	SET_STYLEBOX( SB_LIST_FOCUS, StyleBox( 1,  Color( 255,80,80 ), Color( 255,80,80 ) ) );
	SET_STYLEBOX( SB_LIST_CURSOR, StyleBox( 1,  Color(0,0,0 ), Color(0,0,0 )  ) );

	SET_STYLEBOX(SB_LIST_EDITOR_BG,StyleBox( 0, Color( 150,150,150 ), Color( 250,250,250 ), Color( 50,50,50 ) ));

	SET_FONT( FONT_LIST, 0 );

	SET_COLOR(COLOR_LIST_FONT,Color(0));
	SET_COLOR(COLOR_LIST_FONT_SELECTED,Color(255));

	SET_CONSTANT( C_LIST_SPACING, 2 );
	SET_CONSTANT( C_LIST_HMARGIN, 3 );

	/* Tree */

	SET_STYLEBOX( SB_TREE_SELECTED, StyleBox( 0,  Color( 70,100,255 ), Color( 70,100,255 ), Color( 70,100,255 ) ) );
	SET_STYLEBOX( SB_TREE_NORMAL, StyleBox( 2,  Color(240,240,240 ), Color( 60,60,60 ), Color( 150,150,150 ) ) );
	SET_STYLEBOX( SB_TREE_FOCUS, StyleBox( 1,  Color(0,0,0 ), Color( 255,80,80 ), Color( 255,80,80 ) ) );
	stylebox_array[SB_TREE_FOCUS].draw_center=false;
	SET_STYLEBOX( SB_TREE_CURSOR, StyleBox( 1,  Color(0,0,0 ), Color(0,0,0 ), Color(0,0,0 ) ) );

	SET_STYLEBOX(SB_TREE_EDITOR_BG,StyleBox( 0, Color( 150,150,150 ), Color( 250,250,250 ), Color( 50,50,50 ) ));

	SET_FONT( FONT_TREE, 0 );

	SET_COLOR(COLOR_TREE_FONT,Color(0));
	SET_COLOR(COLOR_TREE_FONT_SELECTED,Color(255));
	SET_COLOR(COLOR_TREE_GUIDES,Color(0));
	SET_COLOR(COLOR_TREE_GRID,Color(200));
	SET_CONSTANT( C_TREE_HSPACING, 3 );
	SET_CONSTANT( C_TREE_VSPACING, 2 );
	SET_CONSTANT(C_TREE_GUIDE_WIDTH, 10 );
	SET_CONSTANT(C_TREE_CHECK_SIZE, 9 );
	SET_CONSTANT(C_TREE_GUIDE_BOX_SIZE,8);

	/* Line Edit */

	SET_COLOR( COLOR_LINE_EDIT_CURSOR, Color( 0,0,0 ) );
	SET_COLOR( COLOR_LINE_EDIT_SELECTION, Color( 70,100,255 ) );
	SET_STYLEBOX( SB_LINE_EDIT_NORMAL, StyleBox( 2,  Color(240,240,240 ), Color( 60,60,60 ), Color( 150,150,150 ) ) );
	SET_STYLEBOX( SB_LINE_EDIT_FOCUS, StyleBox( 1,  Color( 255,80,80 ), Color( 255,80,80 ) ) );

	SET_CONSTANT( C_LINE_EDIT_MARGIN, 1 );
	SET_CONSTANT( C_LINE_EDIT_MIN_WIDTH, 70 );

	SET_FONT( FONT_LINE_EDIT, 0 );

	SET_COLOR(COLOR_LINE_EDIT_FONT,Color(0));
	SET_COLOR(COLOR_LINE_EDIT_FONT_SELECTED,Color(255));

	/* Text Edit */

	SET_COLOR( COLOR_TEXT_EDIT_CURSOR, Color( 0,0,0 ) );
	SET_COLOR( COLOR_TEXT_EDIT_SELECTION, Color( 70,100,255 ) );
	SET_STYLEBOX( SB_TEXT_EDIT, StyleBox( 2,  Color(240,240,240 ), Color( 60,60,60 ), Color( 150,150,150 ) ) );
	SET_STYLEBOX( SB_TEXT_EDIT_FOCUS, StyleBox( 1,  Color( 255,80,80 ), Color( 255,80,80 ) ) );

	SET_CONSTANT(C_TEXT_EDIT_LINE_SPACING,1);
	SET_FONT( FONT_TEXT_EDIT, 0 );

	SET_COLOR(COLOR_TEXT_EDIT_FONT,Color(0));
	SET_COLOR(COLOR_TEXT_EDIT_FONT_SELECTED,Color(255));

	/* Arrow Button */

	SET_STYLEBOX( SB_ARROWBUTTON_NORMAL, StyleBox( 2, Color( 150,150,150 ), Color( 250,250,250 ), Color( 50,50,50 ) ) );
	SET_STYLEBOX( SB_ARROWBUTTON_PRESSED, StyleBox( 2, Color( 110,110,110 ), Color( 50,50,50 ), Color( 200,200,200 ) ) );
	SET_STYLEBOX( SB_ARROWBUTTON_HOVER, StyleBox( 2, Color( 190,190,190 ), Color( 250,250,250 ), Color( 50,50,50 ) ) );
	SET_STYLEBOX( SB_ARROWBUTTON_FOCUS, StyleBox( 1,  Color( 255,50,50 ), Color( 255,50,50 ) ) );
	SET_CONSTANT( C_ARROWBUTTON_DISPLACEMENT, 0 );

	SET_CONSTANT( C_ARROWBUTTON_ARROW_SIZE, 12);

	SET_COLOR( COLOR_ARROWBUTTON_ARROW_COLOR, Color(0,0,0) );

	SET_BITMAP( BITMAP_ARROW_UP, INVALID_BITMAP_ID );
	SET_BITMAP( BITMAP_ARROW_DOWN, INVALID_BITMAP_ID );
	SET_BITMAP( BITMAP_ARROW_LEFT, INVALID_BITMAP_ID );
	SET_BITMAP( BITMAP_ARROW_RIGHT, INVALID_BITMAP_ID );

	/* UpDown */

	SET_CONSTANT( C_UPDOWN_ARROW_SIZE, 10 );
	SET_CONSTANT( C_UPDOWN_MARGIN, 2 );
	SET_BITMAP( BITMAP_UPDOWN, 0 );
	SET_COLOR( COLOR_UPDOWN, Color(0,0,0) );

	/* Progress Bar */

	SET_STYLEBOX( SB_PROGRESSBAR_FRAME, StyleBox( 2,  Color(200,200,200 ), Color( 60,60,60 ), Color( 150,150,150 ) ) );
	SET_STYLEBOX( SB_PROGRESSBAR_PROGRESS, StyleBox( 0,  Color( 70,100,255 ), Color( 70,100,255 ), Color( 70,100,255 ) ) );
	SET_FONT( FONT_PROGRESSBAR, 0 );
	SET_CONSTANT( C_PROGRESSBAR_MARGIN, 2);
	SET_COLOR( COLOR_PROGRESSBAR_FONT, Color(0));
	SET_COLOR( COLOR_PROGRESSBAR_FONT_PROGRESS, Color(0));


	/* Tab Bar */

	SET_STYLEBOX( SB_TABBAR_LEFT, StyleBox() );
	stylebox_array[SB_TABBAR_LEFT].margins[MARGIN_LEFT]=5; //little margin
	SET_STYLEBOX( SB_TABBAR_RIGHT, StyleBox() );
	SET_STYLEBOX( SB_TABBAR_TAB,StyleBox( 1, Color( 150,150,150 ), Color( 250,250,250 ), Color( 50,50,50 ) ) );
	for(int i=0;i<4;i++)
		stylebox_array[SB_TABBAR_TAB].margins[i]=3;

	SET_STYLEBOX( SB_TABBAR_RAISED,StyleBox( 2, Color( 190,190,190 ), Color( 250,250,250 ), Color( 50,50,50 ) )  );

	SET_STYLEBOX( SB_TABBAR_FOCUS, StyleBox( 1, Color( 255,50,50 ), Color( 255,50,50 ) ) );

	for(int i=0;i<4;i++)
		stylebox_array[SB_TABBAR_RAISED].margins[i]=4;

	SET_FONT(FONT_TABBAR,0);
	SET_CONSTANT(C_TABBAR_CENTERED,0);
	SET_CONSTANT(C_TABBAR_TAB_SEPARATOR,0);

	SET_COLOR(COLOR_TABBAR_FONT_RAISED,Color(0));
	SET_COLOR(COLOR_TABBAR_FONT,Color(0));

	/* Tab Box */

	SET_STYLEBOX(SB_TABBOX, StyleBox( 2, Color(200,200,200),Color(250,250,250),Color(90,90,90) ) );

	for(int i=0;i<4;i++)
		stylebox_array[SB_TABBOX].margins[i]=4;

	SET_CONSTANT( C_TABBOX_SEPARATION, 0 );

	/* Combo Box */

	SET_CONSTANT(C_COMBOBOX_POPUP_MAX_HEIGHT,200);
	SET_STYLEBOX(SB_COMBO_POPUP,StyleBox( 2, Color( 150,150,150 ), Color( 250,250,250 ), Color( 50,50,50 ) ));


	/* Text View */

	SET_COLOR(COLOR_TEXT_VIEW_FONT,Color(0));
	SET_FONT(FONT_TEXT_VIEW,0);

	/* Bitmap button */
	
	SET_BITMAP( BITMAP_BUTTON_NORMAL, -1 );
	SET_BITMAP( BITMAP_BUTTON_PRESSED, -1 );
	SET_BITMAP( BITMAP_BUTTON_HOVER, -1 );
	SET_BITMAP( BITMAP_BUTTON_ICON, -1 );
	
	/* Knob */
	
	SET_CONSTANT(C_KNOB_SIZE, 48 );
	SET_CONSTANT(C_KNOB_HANDLE_SIZE,8 );
	SET_CONSTANT(C_KNOB_ANGLE_BEGIN, 30 );
	SET_CONSTANT(C_KNOB_HANDLE_AT_RADIUS, 16 );
	SET_COLOR(COLOR_KNOB_BG, Color( 110,110,110 ) );
	SET_COLOR(COLOR_KNOB_HANDLE, Color(0) );
	SET_BITMAP( BITMAP_KNOB_ANIMATION, -1 );
	
	/* set spinbox */
	
	SET_CONSTANT(C_SPINBOX_LINE_EDIT_MIN_WIDTH,35);
	
	SET_BITMAP( BITMAP_ICON_DEFAULT, INVALID_BITMAP_ID );
	
	set_default_extra();
		
}


}
