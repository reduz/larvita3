//
// C++ Implementation: message_box
//
// Description:
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "message_box.h"
#include "containers/center_container.h"
#include "bundles/window_box.h"
#include "widgets/button.h"
#include <stdio.h>
namespace GUI {

Button* MessageBox::get_ok_button() {

    return ok_button;
}

void MessageBox::show(String p_text) {


	text->set_text( p_text );
	Window::show();
}

MessageBox::MessageBox(Window *p_parent,SizeMode p_size_mode) : Window(p_parent,Window::MODE_POPUP, p_size_mode) {

	window_box = GUI_NEW( WindowBox("!!") );
	set_root_frame( window_box );
	custom_vb= window_box->add(GUI_NEW( VBoxContainer),1);
	text = window_box->add(GUI_NEW(Label));
	ok_button = GUI_NEW( Button("OK"));
	window_box->add( GUI_NEW( CenterContainer) )->set( ok_button )->pressed_signal.connect( static_cast<Window*>(this), &Window::hide );

}


MessageBox::~MessageBox()
{
}


}
