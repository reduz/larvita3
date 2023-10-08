
//
// C++ Implementation: custom_label
//
// Description:
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "custom_label.h"
#include "base/window.h"
#include "base/skin.h"
namespace GUI {

void CustomLabel::set_font(FontID p_font) {

	font_id=p_font;
}
FontID CustomLabel::get_font() const {

	return font_id;
}
void CustomLabel::set_skin_font(int p_font) {

	skin_font=p_font;
}
int CustomLabel::get_skin_font() const {

	return skin_font;
}

FontID CustomLabel::font(int p_which) {

	if (p_which==FONT_LABEL) {
		 if (font_id!=INVALID_FONT_ID) {

			return font_id;
		 } else if (skin_font!=-1) {
            return get_window()->get_skin()->get_font( skin_font );
		 }

	}

	return Frame::font(p_which);
}


CustomLabel::CustomLabel(String p_text,Align p_align) : Label(p_text,p_align) {

	skin_font=-1;
	font_id=INVALID_FONT_ID;

}

CustomLabel::CustomLabel(int p_skin_font,String p_text,Align p_align) : Label(p_text,p_align) {

	skin_font=p_skin_font;
	font_id=INVALID_FONT_ID;
}
CustomLabel::~CustomLabel()
{
}

}
