//
// C++ Implementation: button
//
// Description:
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//


#include "button.h"
#include "base/painter.h"
#include "base/skin.h"

#include <stdio.h>
namespace GUI {


String Button::get_text() {

	return label_text;
}

bool Button::has_icon() {

	return icon>=0;
}
BitmapID Button::get_icon() {

	return (icon>=0)?icon:( (bitmap( BITMAP_BUTTON_DEFAULT_ICON )>=0) ? bitmap( BITMAP_BUTTON_DEFAULT_ICON ): -1 );
}

void Button::set_text(String p_text) {

	label_text=p_text;
	parse_accelerator();
	check_minimum_size();
	update();

}
void Button::set_icon(BitmapID p_icon) {

	icon=p_icon;

	check_minimum_size();
	update();

}

/** Drawing **/


Size Button::get_minimum_size_internal() {


	Painter *p=get_painter();

	Size min;

	if (get_icon() >=0 && p->is_bitmap_valid( get_icon() )) {
		Size icon_size=p->get_bitmap_size( get_icon() );

		min.width+=icon_size.width;
		if (min.height < icon_size.height )
			min.height=icon_size.height;

		if (label_text!="")
			min.width+=constant( C_BUTTON_SEPARATION ); //if there is an icon, add the label-icon separation
	}


	min.width+=p->get_font_string_width( font( FONT_BUTTON ) , label_text );

	if (p->get_font_height( font( FONT_BUTTON ) ) >  min.height )
		min.height=p->get_font_height( font( FONT_BUTTON ) );


	if (constant( C_BUTTON_HAS_CHECKBOX )) {

		if ( constant( C_BUTTON_CHECKBOX_SIZE )>0 ) {
			min.width+=constant( C_BUTTON_CHECKBOX_SIZE );
			if (min.height<constant( C_BUTTON_CHECKBOX_SIZE ))
				min.height=constant( C_BUTTON_CHECKBOX_SIZE );
		} else {

			Size uncheck_size=p->get_bitmap_size( bitmap( BITMAP_BUTTON_UNCHECKED ) );
			min.width+=uncheck_size.width;
			if (min.height<uncheck_size.height)
				min.height=uncheck_size.height;
		}

		min.width+=constant( C_BUTTON_SEPARATION );
	}

	if (shortcut!=0) {
	
		String s_str = Keyboard::get_code_name( shortcut );
		min.width+=constant( C_BUTTON_SEPARATION );
		min.width+=p->get_font_string_width( font(FONT_BUTTON), s_str );
		
	}

	min.width+=p->get_stylebox_margin( stylebox( SB_BUTTON_NORMAL ), MARGIN_LEFT );
	min.width+=p->get_stylebox_margin( stylebox( SB_BUTTON_NORMAL ), MARGIN_RIGHT );

	min.height+=p->get_stylebox_margin( stylebox( SB_BUTTON_NORMAL ), MARGIN_TOP );
	min.height+=p->get_stylebox_margin( stylebox( SB_BUTTON_NORMAL ), MARGIN_BOTTOM );


	min.width+=constant( C_BUTTON_EXTRA_MARGIN )*2+constant( C_BUTTON_DISPLACEMENT ); //both margins and displacement
	min.height+=constant( C_BUTTON_EXTRA_MARGIN )*2+constant( C_BUTTON_DISPLACEMENT ); //both margins and displacement

	

	return min;

}

void Button::draw(const Point& p_pos,const Size& p_size,const Rect& p_exposed) {

	Painter *p=get_painter();

	/* Draw Outline */

	//if disabled...
	bool draw_displaced=false;

	DrawMode draw_mode=get_draw_mode();

	if (draw_mode==DRAW_HOVER) { //optative, otherwise draw normal

		if (stylebox( SB_BUTTON_HOVER ).mode!=StyleBox::MODE_NONE && (!is_toggle_mode() || (is_toggle_mode() && !is_pressed()))) {

			p->draw_stylebox( stylebox( SB_BUTTON_HOVER ) , Point() , p_size, p_exposed );

		} else
			draw_mode=is_pressed()?DRAW_PRESSED:DRAW_NORMAL;
	} ;

	if (draw_mode==DRAW_NORMAL) {

		p->draw_stylebox( stylebox( SB_BUTTON_NORMAL ) , Point() , p_size, p_exposed );

	}

	if (draw_mode==DRAW_PRESSED) {

		p->draw_stylebox( stylebox( SB_BUTTON_PRESSED ) , Point() , p_size, p_exposed );
		draw_displaced=true;

	}


	Rect area_rect=Rect( p_pos, p_size );

	area_rect.pos.x=p->get_stylebox_margin( stylebox( SB_BUTTON_NORMAL ), MARGIN_LEFT );
	area_rect.pos.y=p->get_stylebox_margin( stylebox( SB_BUTTON_NORMAL ), MARGIN_TOP );
	area_rect.size-=area_rect.pos;

	area_rect.size.x-=p->get_stylebox_margin( stylebox( SB_BUTTON_NORMAL ), MARGIN_RIGHT );
	area_rect.size.y-=p->get_stylebox_margin( stylebox( SB_BUTTON_NORMAL ), MARGIN_BOTTOM );

	area_rect.size.x-=constant( C_BUTTON_EXTRA_MARGIN )*2;
	area_rect.pos.x+=constant( C_BUTTON_EXTRA_MARGIN );
	area_rect.size.y-=constant( C_BUTTON_EXTRA_MARGIN )*2;
	area_rect.pos.y+=constant( C_BUTTON_EXTRA_MARGIN );

	area_rect.size.x-=constant( C_BUTTON_DISPLACEMENT );
	area_rect.size.y-=constant( C_BUTTON_DISPLACEMENT );
	if (draw_displaced)
		area_rect.pos+=Point( constant( C_BUTTON_DISPLACEMENT ), constant( C_BUTTON_DISPLACEMENT ) );


	if (constant(C_BUTTON_HAS_CHECKBOX)) {

		Size cbsize;

		if ( constant( C_BUTTON_CHECKBOX_SIZE )>0 ) {

			cbsize.width+=constant( C_BUTTON_CHECKBOX_SIZE );
			cbsize.height+=constant( C_BUTTON_CHECKBOX_SIZE );
		} else {

			cbsize=p->get_bitmap_size( bitmap( BITMAP_BUTTON_UNCHECKED ) );
		}

		Point check_pos=area_rect.pos;
		check_pos.y+=(area_rect.size.height-cbsize.height)/2;

		if ( constant( C_BUTTON_CHECKBOX_SIZE )>0 ) {

			p->draw_stylebox( stylebox( is_pressed() ? SB_BUTTON_CHECKED : SB_BUTTON_UNCHECKED ) , check_pos , cbsize);
		} else {

			p->draw_bitmap( bitmap( is_pressed() ? BITMAP_BUTTON_CHECKED : BITMAP_BUTTON_UNCHECKED ), check_pos );
		}

		area_rect.pos.x+=cbsize.x+constant( C_BUTTON_SEPARATION );
		area_rect.size.x-=cbsize.x+constant( C_BUTTON_SEPARATION );
	}

	if (get_icon() >=0 && p->is_bitmap_valid( get_icon() )) {

		Size icon_size=p->get_bitmap_size( get_icon() );

		Point icon_pos=area_rect.pos;
		icon_pos.y+=(area_rect.size.height-icon_size.height)/2;

		p->draw_bitmap( get_icon(), icon_pos );

		area_rect.pos.x+=icon_size.width;
		area_rect.size.x-=icon_size.width;
		if (label_text!="") {
			
			area_rect.pos.x+=constant( C_BUTTON_SEPARATION ); //separation for the label
			area_rect.size.x-=constant( C_BUTTON_SEPARATION ); //separation for the label
		}

	}

	if (shortcut!=0) {
	

		String s_str = Keyboard::get_code_name( shortcut );
		int w = p->get_font_string_width( font(FONT_BUTTON), s_str );
		
		Point shrc_ofs= area_rect.pos;
		
		shrc_ofs.y+=(area_rect.size.height-p->get_font_height( font( FONT_BUTTON ) ))/2+p->get_font_ascent( font( FONT_BUTTON ) );
		shrc_ofs.x = (area_rect.pos.x+area_rect.size.x)-w;
		
		p->draw_text( font( FONT_BUTTON ) ,  shrc_ofs, s_str, color(COLOR_BUTTON_SHORTCUT_FONT) );
		
		area_rect.size.x-=w;
		
	}

	Point label_ofs=area_rect.pos;
	if (constant(C_BUTTON_LABEL_ALIGN_CENTER)) {

		label_ofs.x+=(area_rect.size.width-p->get_font_string_width( font( FONT_BUTTON ) , label_text ))/2;
	}
	label_ofs.y+=(area_rect.size.height-p->get_font_height( font( FONT_BUTTON ) ))/2+p->get_font_ascent( font( FONT_BUTTON ) );


	
	p->draw_text( font( FONT_BUTTON ) ,  label_ofs, label_text, color(draw_mode==DRAW_HOVER?COLOR_BUTTON_FONT_HOVER:COLOR_BUTTON_FONT) );
	if (accel_char >= 0) {
		int width = 0;
		for (int i=0; i<accel_char; i++) {

			width += p->get_font_char_width(font( FONT_BUTTON ), label_text[i]);
		};
		p->draw_text( font( FONT_BUTTON ) ,  label_ofs + Point(width, 0), "_", color(COLOR_BUTTON_FONT) );
	};


	if (has_focus())
		p->draw_stylebox( stylebox( SB_BUTTON_FOCUS ) , Point() , p_size, p_exposed);

}

String Button::get_type() {

	return "Button";
}

void Button::parse_accelerator() {

	label_text.replace("&&", "\1");
	accel_char = label_text.find("&");
	label_text.replace("&", "");
	label_text.replace("\1", "&");
	if (accel_char >= label_text.size())
		accel_char = -1;
};


void Button::set_shurtcut(unsigned int p_shortcut,bool p_active) {

	shortcut=p_shortcut;
	shortcut_active=p_active;
	check_minimum_size();
	update();
	
}


String::CharType Button::get_accelerator() {

	if (accel_char < 0 || accel_char >= label_text.size()) return 0;

	return label_text[accel_char];
};

Button::Button(BitmapID p_icon) {

	icon=p_icon;
	accel_char = -1;
	shortcut=0;
	shortcut_active=false;
}

void Button::set_flat(bool p_flat) {

	flat=p_flat;
}

Button::Button(String p_text,BitmapID p_icon) {

	accel_char = -1;
	label_text=p_text;
	parse_accelerator();
	icon=p_icon;
	shortcut=0;
	shortcut_active=false;
	flat=false;
}




Button::~Button()
{
}



}
