	
#include "text_edit.h"
#include "base/painter.h"
#include "base/skin.h"


namespace GUI {

int TextEdit::get_char_count() {
	
	int totalsize=0;
	
	for (int i=0;i<text.size();i++) {
		
		if (i>0)
			totalsize++; // incliude \n
		totalsize+=text[i].length();
	}
	
	return totalsize; // omit last \n
}

Size TextEdit::get_minimum_size_internal() {

	return get_painter()->get_stylebox_min_size( stylebox( SB_TEXT_EDIT ) );
}
int TextEdit::get_visible_rows() {

	if (!get_painter())
		return 0;

	int total=size.height;
	total-=get_painter()->get_stylebox_min_size( stylebox( SB_TEXT_EDIT ) ).height;
	total/=get_row_height();
	return total;
}
void TextEdit::adjust_viewport_to_cursor() {

	
	if (cursor.row_ofs>cursor.y)
		cursor.row_ofs=cursor.y;

	if (!get_painter())
		return;
	
	int total_width=size.width-get_painter()->get_stylebox_min_size( stylebox( SB_TEXT_EDIT ) ).width;
	
	//printf("rowofs %i, visrows %i, cursor.y %i\n",cursor.row_ofs,get_visible_rows(),cursor.y);
	
	if ((cursor.row_ofs+get_visible_rows())<=cursor.y)
		cursor.row_ofs=cursor.y-(get_visible_rows()-1);

	if (get_pixel_pos_for( cursor.x, get_line( cursor.y ) )>(cursor.x_ofs+total_width))
		cursor.x_ofs=get_pixel_pos_for( cursor.x, get_line( cursor.y ) )-total_width;
	
	if (get_pixel_pos_for( cursor.x, get_line( cursor.y ) )< cursor.x_ofs)
		cursor.x_ofs=get_pixel_pos_for( cursor.x, get_line( cursor.y ) );

	update();
	
	get_range()->set_max(text.size());
	
	get_range()->set_page(get_visible_rows());
	
	get_range()->set((int)cursor.row_ofs);
	
	
	
}


void TextEdit::set_col(int p_col) {

	if (p_col<0)
		p_col=0;

	cursor.x=p_col;
	if (cursor.x > get_line( cursor.y ).length())
		cursor.x=get_line( cursor.y ).length();
	
	cursor.last_fit_x=get_pixel_pos_for(cursor.x,get_line(cursor.y));
	
	adjust_viewport_to_cursor();
}


void TextEdit::set_row(int p_row) {

	if (setting_row)
		return;
	
	setting_row=true;
	if (p_row<0)
		p_row=0;


	if (p_row>=(int)text.size())
		p_row=(int)text.size()-1;

	cursor.y=p_row;
	cursor.x=get_char_pos_for( cursor.last_fit_x, get_line( cursor.y) );
	
	
	adjust_viewport_to_cursor();

	setting_row=false;
	
}


int TextEdit::get_col() {

	return cursor.x;
}


int TextEdit::get_row() {

	return cursor.y;
}

void TextEdit::resize(const Size& p_new_size) {

	size=p_new_size;

//	if (cursor.x-cursor.x_ofs > size.width)
//		cursor.x = cursor.x_ofs + size.width;

	//if ((cursor.y-cursor.row_ofs) > size.height)
	//	cursor.y = cursor.row_ofs + size.height;

	reconfig_sizes();
	adjust_viewport_to_cursor();
		
}

void TextEdit::reconfig_sizes() {

	/*
	if ((int)text.size()>=rowheight) {

	vscroll.show();
	range.set_max(text.size());
	range.set_page_size(size.height);
	range.set_val(cursor.row_ofs);
} else {

	vscroll.hide();
} */

}


void TextEdit::value_changed(double p_to_val) {

	
	cursor.row_ofs=(int)p_to_val;
	if (cursor.y<cursor.row_ofs)
		set_row(cursor.row_ofs);
	if (cursor.y>=(cursor.row_ofs+get_visible_rows()))
		set_row(cursor.row_ofs+get_visible_rows()-1);
	
	adjust_viewport_to_cursor();
	update();
}


void TextEdit::mouse_button(const Point& p_pos, int p_button,bool p_press,int p_modifier_mask) {


	if (!p_press)
		return;

	if (p_button==BUTTON_WHEEL_UP) {

		get_range()->step_decrement();
		
	}
	if (p_button==BUTTON_WHEEL_DOWN) {

		get_range()->step_increment();
		
	}

	if (p_button==BUTTON_LEFT ) {
		int row=p_pos.y;
		row-=get_painter()->get_stylebox_margin( stylebox( SB_TEXT_EDIT ), MARGIN_TOP );
		row/=get_row_height();
		if (row<0 || row>=get_visible_rows())
			return;
		row+=cursor.row_ofs;
	
		int col=p_pos.x-get_painter()->get_stylebox_margin( stylebox( SB_TEXT_EDIT ), MARGIN_LEFT );;
		col+=cursor.x_ofs;
		col=get_char_pos_for( col, get_line(row) );
	
		set_row( row );
		set_col( col );
		update();
	}	

	//	??	new_cursor_pos=((p_event->mouse_button.x-2+(draw_area->get_font_width(0)/2))/draw_area->get_font_width(0));
//				if (new_cursor_pos>(get_window_length()+1)) new_cursor_pos=get_window_length()+1;
//				if (new_cursor_pos>text.length()) new_cursor_pos=text.length();
//				set_cursor_pos(window_pos+new_cursor_pos);


	update();
}




bool TextEdit::key(unsigned long p_unicode, unsigned long p_scan_code,bool p_press,bool p_repeat,int p_modifier_mask) {
		

	if (!p_press)
		return false;
			


	switch (p_scan_code) {

		case KEY_RETURN: {

			newline_at_cursor();
		} break;
		case KEY_BACKSPACE: {
			if (readonly)
				break;

			backspace_at_cursor();
		} break;
		case KEY_LEFT: {

			set_col(get_col()-1);
		} break;
		case KEY_RIGHT: {

			set_col(get_col()+1);
		} break;
		case KEY_UP: {

			
			set_row(get_row()-1);
		} break;
		case KEY_DOWN: {

			set_row(get_row()+1);
		} break;

		case KEY_DELETE: {

			if (readonly)
				break;
			delete_at_cursor();
		} break;
		case KEY_HOME: {

			set_col(0);

		} break;
		case KEY_END: {

			set_col(text[cursor.y].length());
		} break;
		case KEY_PAGEUP: {

			set_row(get_row()-get_visible_rows());
		} break;
		case KEY_PAGEDOWN: {

			set_row(get_row()+get_visible_rows());
		} break;
		default: {

			if (p_unicode>=32) {
				
				if (readonly)
					break;

				const char chr[2] = {p_unicode, 0};
				append_at_cursor(chr);
			} else {

				return false;
			}
		} break;

	}

	return true;
}

int TextEdit::get_row_height() {

	if (!get_painter())
		return 0;

	return get_painter()->get_font_height( font(FONT_LINE_EDIT) ) + constant(C_TEXT_EDIT_LINE_SPACING);
}

int TextEdit::get_char_pos_for(int p_px,String p_str) {

	if (!get_painter())
		return 0;
	
	int px=0;
	int c=0;

	while (px<p_px && c<p_str.length()) {
		
		px+=get_painter()->get_font_char_width( font(FONT_LINE_EDIT), p_str[c] );
		c++;
	}

	return c;
}
int TextEdit::get_pixel_pos_for(int p_char,String p_str) {

	if (!get_painter())
		return 0;
	
	int px=0;

	for (int i=0;i<p_char;i++) {

		if (i>=p_str.length())
			break;

		px+=get_painter()->get_font_char_width( font(FONT_LINE_EDIT), p_str[i] );
	}

	return px;

}


void TextEdit::draw(const Point& p_pos,const Size& p_size,const Rect& p_exposed) {


	get_painter()->draw_stylebox( stylebox(SB_TEXT_EDIT), Point(0,0),size);

	int xmargin_beg=get_painter()->get_stylebox_margin( stylebox(SB_TEXT_EDIT), MARGIN_LEFT );
	int xmargin_end=size.width-get_painter()->get_stylebox_margin( stylebox(SB_TEXT_EDIT), MARGIN_RIGHT );
	//let's do it easy for now:

	int ascent=get_painter()->get_font_ascent( font(FONT_TEXT_EDIT ) );
	for (int i=0;i<get_visible_rows();i++) {


		int line=i+cursor.row_ofs;
		
		if (line<0 || line>=(int)text.size())
			continue;

		String &str=text[line];

		int char_x=xmargin_beg-cursor.x_ofs;
		int ofs_y=i*get_row_height()+constant(C_TEXT_EDIT_LINE_SPACING)/2;

		for (int j=0;j<str.length();j++) {

			

			int char_w=get_painter()->get_font_char_width( font(FONT_TEXT_EDIT), str[j] );
			
			if ( (char_x)<xmargin_beg) {
				char_x+=char_w;
				continue;
			}
			
			if ( (char_x+char_w)>=xmargin_end)
				break;
			
			const char chr[2] = { str[j], 0 };
			get_painter()->draw_text( font(FONT_TEXT_EDIT), Point( char_x, ofs_y+ascent),chr,color(COLOR_TEXT_EDIT_FONT) );


			if (cursor.x==j && cursor.y==line) {

				get_painter()->draw_fill_rect( Point( char_x, ofs_y ), Size(1,get_row_height()),color(COLOR_TEXT_EDIT_CURSOR));
				
			}
			char_x+=char_w;
			
		}
		
		if (cursor.x==str.length() && cursor.y==line) {

			get_painter()->draw_fill_rect( Point( char_x, ofs_y ), Size(1,get_row_height()),color(COLOR_TEXT_EDIT_CURSOR));
				
		}
		
	}
}

void TextEdit::new_text_at_cursor(String p_char) {

	if (max_chars > 0 ) {
	
		int rest = get_char_count() + p_char.length() - max_chars;
		
		if (rest>0) {
			
			p_char=p_char.substr(0,p_char.length() - rest);
		}
	}
	
	//while (cursor.x>(int)text[cursor.y].length()) 
	//	text[cursor.y]+=' ';
		
	if ( wrap && get_painter()) {
	 
	 	if (get_painter()->get_font_string_width( font(FONT_TEXT_EDIT),text[cursor.y] ) > get_size_cache().width-get_painter()->get_stylebox_min_size( stylebox( SB_TEXT_EDIT ) ).width ) {
	
			int lastidx = text[cursor.y].find_last(" ");
			
			if (lastidx<0) { // can't be cut
				
				lastidx=text[cursor.y].length()/2;
			}
			int extraofs=text[cursor.y].length() - lastidx;
			cursor.x=lastidx;
			newline_at_cursor();
			text[cursor.y]=text[cursor.y].substr(1,text[cursor.y].length());				
			cursor.x+=extraofs-1;
		}
	}
	
	text[cursor.y].insert(cursor.x,p_char);
	set_col(get_col()+p_char.length());

	cursor.last_fit_x=get_pixel_pos_for(cursor.x,get_line(cursor.y));
	
	text_changed_signal.call();
}

void TextEdit::newline_at_cursor() {

	if (max_chars > 0 && get_char_count() >= max_chars)
		return;
		
	String newline;

	if (cursor.x<(int)text[cursor.y].length()) {

		newline=text[cursor.y].substr(cursor.x,text[cursor.y].length()-cursor.x);
		text[cursor.y].erase(cursor.x,text[cursor.y].length()-cursor.x);
	}

	text.insert(text.begin()+get_row()+1,newline);
	set_row(get_row()+1);
	set_col(0);

	cursor.last_fit_x=get_pixel_pos_for(cursor.x,get_line(cursor.y));
	
	text_changed_signal.call();
}

void TextEdit::backspace_at_cursor() {



	if (cursor.x==0) {

		if (cursor.y==0) return; // let's not do STUPID stuff. period.

		set_row(get_row()-1);
		set_col(text[cursor.y].length());
		text[cursor.y].insert(cursor.x,text[cursor.y+1]);
		text.erase(text.begin()+cursor.y+1);

	} else {


		if (cursor.x<=(int)text[cursor.y].length()) {

			text[cursor.y].erase(cursor.x-1,1);
		}

		set_col(get_col()-1);
	}

	cursor.last_fit_x=get_pixel_pos_for(cursor.x,get_line(cursor.y));
	
	text_changed_signal.call();

	
}

void TextEdit::delete_at_cursor() {

	if (cursor.x>=(int)text[cursor.y].length()) {

		if ((cursor.y+1)==(int)text.size()) return; //nuthin to delete

		while (cursor.x>(int)text[cursor.y].length()) text[cursor.y]+=' ';
		set_row(get_row()+1);
		set_col(0);
		backspace_at_cursor();

	} else {

		set_col(get_col()+1);
		backspace_at_cursor();


	}

	cursor.last_fit_x=get_pixel_pos_for(cursor.x,get_line(cursor.y));
	
}



void TextEdit::append_at_cursor(String p_text) {

	
	if (max_chars > 0 ) {
	
		int rest = get_char_count() + p_text.length() - max_chars;
		
		if (rest>0) {
			
			p_text=p_text.substr(0,p_text.length() - rest);
		}
	}
	
	/* this will suck, better tomorrow */
	int i;

	int slices=p_text.get_slice_count("\n");
	
	p_text.replace("\r","");
	for (int i=0;i<slices;i++) {

		if (i>0)
			newline_at_cursor();
		
		new_text_at_cursor(p_text.get_slice("\n",i));
	}

	cursor.last_fit_x=get_pixel_pos_for(cursor.x,get_line(cursor.y));
//	set_cursor_pos(get_cursor_pos()+p_text.length());

}



void TextEdit::delete_char(){};
void TextEdit::delete_line(){};
void TextEdit::set_text(String p_text){

	clear();	
	append_at_cursor( p_text );
	
	cursor.x=0;
	cursor.y=0;
	cursor.x_ofs=0;
	cursor.row_ofs=0;
	cursor.last_fit_x=0;
	
	get_range()->set(0);
};

String TextEdit::get_text() {
	String longthing;
	for (int i=0;i<text.size();i++) {

		longthing+=text[i]+"\n";
	}

	return longthing;
	
};
String TextEdit::get_line(int line) {

	if (line<0 || line>=text.size())
		return "";

	return 	text[line];

};
void TextEdit::clear() {

	text.clear();
	cursor.x=0;
	cursor.y=0;
	cursor.x_ofs=0;
	cursor.row_ofs=0;
	cursor.last_fit_x=0;
	text.push_back("");

};

void TextEdit::set_readonly(bool p_readonly) {
	
	
	readonly=p_readonly;
}

void TextEdit::set_wrap(bool p_wrap) {

	wrap=p_wrap;
}

void TextEdit::set_max_chars(int p_max_chars) {

	max_chars=p_max_chars;
}
TextEdit::TextEdit()  {

	readonly=false;
	setting_row=false;
	max_chars=0;
	clear();
	wrap=false;
	set_focus_mode(FOCUS_ALL);
}

TextEdit::~TextEdit(){
}

}
