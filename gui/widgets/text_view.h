
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


#ifndef PIGUITEXT_H
#define PIGUITEXT_H

#include "widgets/range_owner.h"

namespace GUI {

/**
	@author Juan Linietsky <reduzio@gmail.com>
*/
class TextView : public RangeOwner {
public:

	enum Align {

		ALIGN_LEFT,
		ALIGN_CENTER,
		ALIGN_RIGHT,
		ALIGN_FILL
	};
private:
	Align align;
	String text;

	struct WordCache {

		enum {
			CHAR_NEWLINE=-1
		};
		int char_pos; // if -1, then newline
		int word_len;
		int pixel_width;
		WordCache *next;
		WordCache() { char_pos=0; word_len=0; pixel_width=0; next=0; }
	};


	bool word_cache_dirty;
	void regenerate_word_cache();

	WordCache *word_cache;

	virtual void resize(const Size& p_new_size);
	virtual Size get_minimum_size_internal();
protected:

	virtual String get_type();
public:

	//void mouse_enter() { update(); } for testing

	void draw(const Point& p_pos,const Size& p_size,const Rect& p_exposed);

	void set_align(Align p_align);
	Align get_align();

	void set_text(const String& p_string);
	String get_text();

	TextView(String p_text="",Align p_align=ALIGN_LEFT);

    ~TextView();

};

}

#endif

