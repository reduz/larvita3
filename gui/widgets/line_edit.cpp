
//
// C++ Implementation: text_box
//
// Description:
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "line_edit.h"


#include "base/keyboard.h"
#include "base/painter.h"
#include "base/skin.h"


namespace GUI {



void LineEdit::mouse_button(const Point& p_pos, int p_button,bool p_press,int p_modifier_mask) {

	if (p_button!=BUTTON_LEFT)
		return;

	if (p_press) {

		set_cursor_at_pixel_pos(p_pos.x);

		/*if (p_event->mouse_button.doubleclick) {

			selection.enabled=true;
			selection.begin=0;
			selection.end=text.size();
			selection.doubleclick=true;
		}
		*/
		if ((cursor_pos<selection.begin) || (cursor_pos>selection.end) || !selection.enabled)  {

			selection_clear();
			selection.cursor_start=cursor_pos;
			selection.creating=true;
		}

		if (!editable)
			non_editable_clicked_signal.call();
		update();

	} else {

		if ( (!selection.creating) && (!selection.doubleclick)) {
			selection_clear();
		}
		selection.creating=false;
		selection.doubleclick=false;
	}

}

void LineEdit::mouse_motion(const Point& p_pos, const Point& p_rel, int p_button_mask) {

	if (p_button_mask&BUTTON_MASK_LEFT) {

		if (selection.creating) {
			set_cursor_at_pixel_pos(p_pos.x);
			selection_fill_at_cursor();
		}
	}


}




bool LineEdit::key(unsigned long p_unicode, unsigned long p_scan_code,bool p_press,bool p_repeat,int p_modifier_mask) {

	if (!p_press)
		return false;

	unsigned int code=p_scan_code|(p_modifier_mask&~KEY_MASK_SHIFT);

	switch (code) {

		case KEY_RETURN: {

			text_enter_signal.call( text );
			return true;
		} break;

		case KEY_BACKSPACE: {
			if (editable) {
				undo_text = text;
				selection_delete();
				delete_char();
			}
		} break;
		case KEY_LEFT: {
			shift_selection_check_pre(p_modifier_mask&KEY_MASK_SHIFT);
			set_cursor_pos(get_cursor_pos()-1);
			shift_selection_check_post(p_modifier_mask&KEY_MASK_SHIFT);

		} break;
		case KEY_RIGHT: {

			shift_selection_check_pre(p_modifier_mask&KEY_MASK_SHIFT);
			set_cursor_pos(get_cursor_pos()+1);
			shift_selection_check_post(p_modifier_mask&KEY_MASK_SHIFT);
		} break;
		case KEY_DELETE: {

			if (editable) {
				undo_text = text;
				selection_delete();
				if ((unsigned int)cursor_pos<text.length()) {

					set_cursor_pos(get_cursor_pos()+1);
					delete_char();
				}
			}

		} break;
		case KEY_HOME: {

			shift_selection_check_pre(p_modifier_mask&KEY_MASK_SHIFT);
			set_cursor_pos(0);
			shift_selection_check_post(p_modifier_mask&KEY_MASK_SHIFT);
		} break;
		case KEY_END: {

			shift_selection_check_pre(p_modifier_mask&KEY_MASK_SHIFT);
			set_cursor_pos(text.length());
			shift_selection_check_post(p_modifier_mask&KEY_MASK_SHIFT);
		} break;

		case (KEY_x|KEY_MASK_CTRL): { // CUT

			if(editable) {
				cut_text();
			}

		} break;

		case (KEY_c|KEY_MASK_CTRL): { // COPY

			if(p_modifier_mask&KEY_MASK_CTRL) {
				copy_text();
			}

		} break;

		case (KEY_v|KEY_MASK_CTRL): { // PASTE

			if(editable) {

				paste_text();
			}

		} break;

		case (KEY_z|KEY_MASK_CTRL): { // Simple One level undo

			if(editable) {

				int old_cursor_pos = cursor_pos;
				text = undo_text;
				if(old_cursor_pos > text.length()) {
					set_cursor_pos(text.length());
				} else {
					set_cursor_pos(old_cursor_pos);
				}
			}

			text_changed_signal.call(text);

		} break;

		case (KEY_u|KEY_MASK_CTRL): { // Delete from start to cursor

			if(editable) {

				selection_clear();
				undo_text = text;
				text = text.substr(cursor_pos,text.length()-cursor_pos);
				set_cursor_pos(0);
				text_changed_signal.call(text);
			}


		} break;

		case (KEY_y|KEY_MASK_CTRL): { // PASTE (Yank for unix users)

			if(editable) {

				paste_text();
			}

		} break;
		case (KEY_k|KEY_MASK_CTRL): { // Delete from cursor_pos to end

			if(editable) {

				selection_clear();
				undo_text = text;
				text = text.substr(0,cursor_pos);
				text_changed_signal.call(text);
			}

		} break;


		default: {

			if (p_unicode>=32 && p_scan_code!=KEY_DELETE) {

				if (editable) {
					selection_delete();
					String::CharType ucodestr[2]={p_unicode,0};
					append_at_cursor(ucodestr);
					text_changed_signal.call( text );
				}

			} else
				return false;
		} break;
	}

	selection.old_shift=p_modifier_mask&KEY_MASK_SHIFT;
	update();

	return true;


}


void LineEdit::copy_text() {

	if(selection.enabled) {

		///@TODO CLIPBOARD
		//ClipBoard::get_singleton_window()->set_text(text.substr(selection.begin, selection.end - selection.begin));
	}
}

void LineEdit::cut_text() {

	if(selection.enabled) {
		undo_text = text;
		///@TODO CLIPBOARD
		//ClipBoard::get_singleton_window()->set_text(text.substr(selection.begin, selection.end - selection.begin));
		selection_delete();
	}
}

void LineEdit::paste_text() {

	/*String paste_buffer = ClipBoard::get_singleton_window()->get_text();

	if(paste_buffer != "") {

		if(selection.enabled) selection_delete();
		append_at_cursor(paste_buffer);
} */
			///@TODO CLIPBOARD

	text_changed_signal.call(text);
}

void LineEdit::shift_selection_check_pre(bool p_shift) {

	if (!selection.old_shift && p_shift)  {
		selection.cursor_start=cursor_pos;
	}
	if (!p_shift)
		selection_clear();

}

void LineEdit::shift_selection_check_post(bool p_shift) {

	if (p_shift)
		selection_fill_at_cursor();
}

void LineEdit::set_cursor_at_pixel_pos(int p_x) {


	int ofs=window_pos;
	int pixel_ofs=get_painter()->get_stylebox_margin( stylebox( SB_LINE_EDIT_NORMAL ) , MARGIN_LEFT )+constant( C_LINE_EDIT_MARGIN );

	while (ofs<text.length()) {


		int char_w=get_painter()->get_font_char_width( font( FONT_LINE_EDIT ) , text[ofs] );
		pixel_ofs+=char_w;

		if (pixel_ofs > p_x) { //found what we look for


			if ( (pixel_ofs-p_x) < (char_w >> 1 ) ) {

				ofs+=1;
			}

			break;
		}


		ofs++;
	}

	set_cursor_pos( ofs );

	/*
	int new_cursor_pos=p_x;
	int charwidth=draw_area->get_font_char_width(' ',0);
	new_cursor_pos=( ( (new_cursor_pos-2)+ (charwidth/2) ) /charwidth );
	if (new_cursor_pos>(int)text.length()) new_cursor_pos=text.length();
	set_cursor_pos(window_pos+new_cursor_pos); */
}

void LineEdit::resize(const Size& p_new_size) {

	size=p_new_size;
	set_cursor_pos( get_cursor_pos() );
	/*
	if (get_cursor_pos()>window_pos+get_window_length()) {

		set_cursor_pos(window_pos+get_window_length());
	}*/
}

void LineEdit::delete_char() {

	if ((text.length()<=0) || (cursor_pos==0)) return;


	text.erase( cursor_pos-1, 1 );

	set_cursor_pos(get_cursor_pos()-1);

	if (cursor_pos==window_pos) {

	//	set_window_pos(cursor_pos-get_window_length());
	}

	text_changed_signal.call(text);
}

void LineEdit::set_text(String p_text) {

	clear_internal();
	append_at_cursor(p_text);
	update();
	cursor_pos=0;
	window_pos=0;
}

void LineEdit::clear() {

	clear_internal();
}

String LineEdit::get_text() {

	return text;
}

void LineEdit::set_cursor_pos(int p_pos) {

	if (p_pos>(int)text.length())
		p_pos=text.length();

	if(p_pos<0)
		p_pos=0;



	cursor_pos=p_pos;

//	if (cursor_pos>(window_pos+get_window_length())) {
//	set_window_pos(cursor_pos-get_window_lengt//h());
//	}

	if (!get_painter()) {

		window_pos=cursor_pos;
		return;
	}
	if (cursor_pos<window_pos) {
		/* Adjust window if cursor goes too much to the left */
		set_window_pos(cursor_pos);
	} else if (cursor_pos>window_pos) {
		/* Adjust window if cursor goes too much to the right */
		int window_width=size.width-(constant( C_LINE_EDIT_MARGIN )*2+get_painter()->get_stylebox_margin( stylebox( SB_LINE_EDIT_NORMAL ), MARGIN_LEFT ) +	get_painter()->get_stylebox_margin( stylebox( SB_LINE_EDIT_NORMAL ), MARGIN_RIGHT ) );

		if (window_width<0)
			return;
		int width_to_cursor=0;
		int wp=window_pos;

		for (int i=window_pos;i<cursor_pos;i++)
			width_to_cursor+=get_painter()->get_font_char_width( font( FONT_LINE_EDIT ) , text[i] );

		while(width_to_cursor>=window_width && wp<text.length()) {


			width_to_cursor-=get_painter()->get_font_char_width( font( FONT_LINE_EDIT ) , text[ wp ] );
			wp++;
		}

		if (wp!=window_pos)
			set_window_pos( wp );

	}
	update();
}

int LineEdit::get_cursor_pos() {

	return cursor_pos;
}

void LineEdit::set_window_pos(int p_pos) {

	window_pos=p_pos;
	if (window_pos<0) window_pos=0;
}

void LineEdit::append_at_cursor(String p_text) {


	if ( ( max_length <= 0 ) || (text.length()+p_text.length() <= max_length)) {

		undo_text = text;
		String pre = text.substr( 0, cursor_pos );
		String post = text.substr( cursor_pos, text.length()-cursor_pos );
		text=pre+p_text+post;
		set_cursor_pos(cursor_pos+p_text.length());

	}

}

void LineEdit::clear_internal() {

	cursor_pos=0;
	window_pos=0;
	undo_text="";
	text="";
	update();
}

Size LineEdit::get_minimum_size_internal() {


	Size min;

	min.height=get_painter()->get_stylebox_margin( stylebox( SB_LINE_EDIT_NORMAL ), MARGIN_TOP ) +
		get_painter()->get_stylebox_margin( stylebox( SB_LINE_EDIT_NORMAL ), MARGIN_BOTTOM ) +
		constant( C_LINE_EDIT_MARGIN )*2 +
		get_painter()->get_font_height( font( FONT_LINE_EDIT ) );

	min.width=get_painter()->get_stylebox_margin( stylebox( SB_LINE_EDIT_NORMAL ), MARGIN_LEFT ) +
		get_painter()->get_stylebox_margin( stylebox( SB_LINE_EDIT_NORMAL ), MARGIN_RIGHT ) +
		constant( C_LINE_EDIT_MARGIN )*2;

	if (min.width<constant( C_LINE_EDIT_MIN_WIDTH ))
		min.width=constant( C_LINE_EDIT_MIN_WIDTH );

	return min;
}

void LineEdit::draw(const Point& p_pos,const Size& p_size,const Rect& p_exposed) {

	int width,height;

	width=p_size.width;
	height=p_size.height;

	get_painter()->draw_stylebox( stylebox( SB_LINE_EDIT_NORMAL ) , Point() , p_size, p_exposed );
	if (has_focus())
		get_painter()->draw_stylebox( stylebox( SB_LINE_EDIT_FOCUS )  , Point() , p_size, p_exposed);



	int ofs=get_painter()->get_stylebox_margin( stylebox( SB_LINE_EDIT_NORMAL ) , MARGIN_LEFT )+constant( C_LINE_EDIT_MARGIN );
	int ofs_max=width-(ofs+get_painter()->get_stylebox_margin( stylebox( SB_LINE_EDIT_NORMAL ) , MARGIN_RIGHT )+constant( C_LINE_EDIT_MARGIN ));
	int char_ofs=window_pos;


	int y_area=height-(get_painter()->get_stylebox_margin( stylebox( SB_LINE_EDIT_NORMAL ) , MARGIN_TOP ) + get_painter()->get_stylebox_margin( stylebox( SB_LINE_EDIT_NORMAL ) , MARGIN_BOTTOM ) + constant( C_LINE_EDIT_MARGIN )*2 );
	int y_ofs=(y_area-get_painter()->get_font_height( font( FONT_LINE_EDIT ) ))/2 + get_painter()->get_stylebox_margin( stylebox( SB_LINE_EDIT_NORMAL ) , MARGIN_TOP ) + constant( C_LINE_EDIT_MARGIN );

	int font_ascent=get_painter()->get_font_ascent( font( FONT_LINE_EDIT ) );

	while(true) {

		//end of string, break!
		if (char_ofs>=text.size())
			break;

		int char_width=get_painter()->get_font_char_width( font( FONT_LINE_EDIT ) , text[char_ofs] );

		if (pass){
			char_width=get_painter()->get_font_char_width( font( FONT_LINE_EDIT ), '*' );
		}

		// end of widget, break!
		if ( (ofs+char_width) > ofs_max )
			break;


		bool selected=selection.enabled && char_ofs>=selection.begin && char_ofs<selection.end;
		if (selected)
			get_painter()->draw_fill_rect( Point( ofs , y_ofs ), Size( char_width, y_area ), color( COLOR_LINE_EDIT_SELECTION ) );


		const char chr[2] = {text[char_ofs], 0};

		if ( !pass ){
            get_painter()->draw_text( font( FONT_LINE_EDIT ) , Point( ofs , y_ofs+font_ascent ), chr, color( selected?COLOR_LINE_EDIT_FONT_SELECTED:COLOR_LINE_EDIT_FONT ) );
		}
        else{
            get_painter()->draw_text( font( FONT_LINE_EDIT ) , Point( ofs, y_ofs+font_ascent), "*", color( selected?COLOR_LINE_EDIT_FONT_SELECTED:COLOR_LINE_EDIT_FONT ) );
		}

		if (char_ofs==cursor_pos && has_focus())
			get_painter()->draw_fill_rect( Point( ofs , y_ofs ), Size( 1, y_area ), color( COLOR_LINE_EDIT_CURSOR ) );

        ofs+=char_width;
		char_ofs++;
	}

	if (char_ofs==cursor_pos && has_focus()) //may be at the end
		get_painter()->draw_fill_rect( Point( ofs , y_ofs ), Size( 1, y_area ), color( COLOR_LINE_EDIT_CURSOR ) );


}




/* selection */

void LineEdit::selection_clear() {

	selection.begin=0;
	selection.end=0;
	selection.cursor_start=0;
	selection.enabled=false;
	selection.creating=false;
	selection.old_shift=false;
	selection.doubleclick=false;
	update();
}


void LineEdit::selection_delete() {

	if (selection.enabled) {

		undo_text = text;
		text.erase(selection.begin,selection.end-selection.begin);

		if ((unsigned)cursor_pos>=text.size()) {

			cursor_pos=text.size();
		}
		if (window_pos>cursor_pos) {

			window_pos=cursor_pos;
		}
	};

	selection_clear();
}

void LineEdit::set_max_length(int p_max_length) {

	if(max_length >= 0) {
		max_length = p_max_length;
	}
}

void LineEdit::selection_fill_at_cursor() {

	int aux;

	selection.begin=cursor_pos;
	selection.end=selection.cursor_start;

	if (selection.end<selection.begin) {
		aux=selection.end;
		selection.end=selection.begin;
		selection.begin=aux;
	}

	selection.enabled=(selection.begin!=selection.end);
}

void LineEdit::select_all() {

	if (!text.length())
		return;

	selection.begin=0;
	selection.end=text.length();
	selection.enabled=true;
	update();

}
void LineEdit::set_editable(bool p_editable) {

	editable=p_editable;
}


LineEdit::LineEdit(String p_text, bool p_pass) {

	cursor_pos=0;
	window_pos=0;
	max_length = 0;
    pass = p_pass;

	text=p_text;
	selection_clear();
	set_focus_mode( FOCUS_ALL );
	editable=true;
	set_fill_vertical( false );

}

LineEdit::~LineEdit(){


}



}
