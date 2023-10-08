//
// C++ Implementation: label
//
// Description:
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "text_view.h"
#include "base/painter.h"
#include "base/skin.h"


namespace GUI {


void TextView::regenerate_word_cache() {

	while (word_cache) {

		WordCache *current=word_cache;
		word_cache=current->next;
		GUI_DELETE( current );
	}


	Size size = get_size_cache();


	int current_word_size=0;
	int word_pos=0;
	int line_width=0;
	int last_width=0;
	int line_count=1;

	WordCache *last;

	for (int i=0;i<text.size()+1;i++) {

		String::CharType current=i<text.length()?text[i]:' '; //always a space at the end, so the algo works

		bool insert_newline=false;

		if (current<33) {

			if (current_word_size>0) {

				WordCache *wc = GUI_NEW( WordCache );
				if (word_cache) {
					last->next=wc;
				} else {
					word_cache=wc;
				}
				last=wc;

				wc->pixel_width=current_word_size;
				wc->char_pos=word_pos;
				wc->word_len=i-word_pos;
				current_word_size=0;

			}

			if (current=='\n') {

				insert_newline=true;
			}
		} else {

			if (current_word_size==0) {
				if (line_width>0) // add a space before the new word if a word existed before
					line_width+=get_painter()->get_font_char_width(font(FONT_TEXT_VIEW),' ');
				word_pos=i;
			}

			int char_width=get_painter()->get_font_char_width(font(FONT_TEXT_VIEW),current);
			current_word_size+=char_width;
			line_width+=char_width;

		}

		if ((line_width>=size.width && last_width<size.width) || insert_newline) {

			WordCache *wc = GUI_NEW( WordCache );
			if (word_cache) {
				last->next=wc;
			} else {
				word_cache=wc;
			}
			last=wc;

			wc->pixel_width=0;
			wc->char_pos=WordCache::CHAR_NEWLINE;
			line_width=current_word_size;
			line_count++;

		}

		last_width=line_width;

	}


	get_range()->set_max(line_count);
	get_range()->set_page( size.height/get_painter()->get_font_height(font(FONT_TEXT_VIEW)) );
	word_cache_dirty=false;
}

void TextView::resize(const Size& p_new_size) {

	word_cache_dirty=true;
}

Size TextView::get_minimum_size_internal() {

	return Size(1,1);
}


void TextView::draw(const Point& p_pos,const Size& p_size,const Rect& p_exposed) {

	if (word_cache_dirty)
        regenerate_word_cache();

	Size string_size;

	Painter *p=get_painter();
	int font_h = p->get_font_height( font( FONT_TEXT_VIEW ) );
	int line_from=(int)get_range()->get(); // + p_exposed.pos.y / font_h;
	int line_to=(int)get_range()->get() + p_size.y/font_h; //p_exposed.pos.y+p_exposed.size.height / font_h;
	int space_w=p->get_font_char_width( font( FONT_TEXT_VIEW), ' ' );


	WordCache *wc = word_cache;
	if (!wc)
		return;

	int line=0;
	while(wc) {

		/* handle lines not meant to be drawn quickly */
		if  (line>line_to)
			break;
		if  (line<line_from) {

			while (wc && wc->char_pos!=WordCache::CHAR_NEWLINE)
				wc=wc->next;
			if (wc)
				wc=wc->next;
			line++;
			continue;
		}

		/* handle lines normally */

		if (wc->char_pos==WordCache::CHAR_NEWLINE) {
			//empty line
			wc=wc->next;
			line++;
			continue;
		}

		WordCache *from=wc;
		WordCache *to=wc;

		int taken=0;
		int spaces=0;
		while(to && to->char_pos!=WordCache::CHAR_NEWLINE) {

			taken+=to->pixel_width;
			if (to!=from) {
				spaces++;
			}
			to=to->next;
		}

		int x_ofs=0;

		switch (align) {

			case ALIGN_FILL:
			case ALIGN_LEFT: {

				x_ofs=0;
			} break;
			case ALIGN_CENTER: {

				x_ofs=(p_size.width-(taken+spaces*p->get_font_char_width( font( FONT_TEXT_VIEW ),' ' )))/2;;

			} break;
			case ALIGN_RIGHT: {

				x_ofs=p_size.width-(taken+spaces*p->get_font_char_width( font( FONT_TEXT_VIEW ),' ' ));

			} break;
		}

		int y_ofs=(line-(int)get_range()->get())*font_h + get_painter()->get_font_ascent( font( FONT_TEXT_VIEW ) );

		while(from!=to) {

			// draw a word
			int pos = from->char_pos;
			if (from->char_pos==WordCache::CHAR_NEWLINE) {

				PRINT_ERROR("BUG");
				return;
			}
			if (from!=wc) {
				/* spacing */
				x_ofs+=space_w;
				if (align==ALIGN_FILL && spaces) {

					x_ofs+=(p_size.width-(taken+space_w*spaces))/spaces;
				}


			}
			for (int i=0;i<from->word_len;i++) {

				p->draw_char( font( FONT_TEXT_VIEW), Point( x_ofs, y_ofs ), text[i+pos], color( COLOR_TEXT_VIEW_FONT ) );
				x_ofs+=p->get_font_char_width( font( FONT_TEXT_VIEW), text[i+pos] );
			}
			from=from->next;
		}

		wc=to?to->next:0;
		line++;

	}

}

void TextView::set_align(Align p_align) {

	align=p_align;
	update();
}

TextView::Align TextView::get_align(){

	return align;
}



void TextView::set_text(const String& p_string) {

	if (text==p_string)
		return;

	text=p_string;
	word_cache_dirty=true;
	update();

}
String TextView::get_text() {

	return text;
}

String TextView::get_type() {

	return "TextView";
}

TextView::TextView(String p_text,Align p_align)
{
	align=p_align;
	text=p_text;
	word_cache=NULL;
	word_cache_dirty=true;
}


TextView::~TextView() {

	while (word_cache) {

		WordCache *current=word_cache;
		word_cache=current->next;
		GUI_DELETE( current );
	}

}


}
