//
// C++ Implementation: button_group
//
// Description:
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "button_group.h"

namespace GUI {

void ButtonGroup::activated(ButtonList *p_which) {


	if (selecting)
		return; //avoid recursivity

	int which=-1;

	ButtonList *b=button_list;

	int idx=0;
	while (b) {

		if (p_which==b) {
			which=idx;
			break;
		}

		b=b->next;
		idx++;
	}

	set_current(which);
}


int ButtonGroup::get_current() {

	int curr=-1;

	ButtonList *b=button_list;

	int idx=0;
	while (b) {

		if (b->button->is_pressed()) {
			curr=idx;
			break;
		}

		b=b->next;
		idx++;
	}

	return curr;

}

void ButtonGroup::set_current(int p_idx){

	if (selecting)
		return; //lock against recursive call

	selecting=true;

	ButtonList *b=button_list;

	int idx=0;
	while (b) {

		b->button->set_pressed( p_idx==idx );
		b=b->next;
		idx++;
	}


	current_button_changed_signal.call(p_idx);
	selecting=false;


}


BaseButton* ButtonGroup::add_button(BaseButton *p_button){

	ButtonList *bl = GUI_NEW( ButtonList );

	/* Append at the end */
	ButtonList **bl_ptr=&button_list;

	while (*bl_ptr)
		bl_ptr=&(*bl_ptr)->next;

	*bl_ptr=bl;

	bl->button=p_button;
	bl->button->pressed_signal.connect( bl, &ButtonList::activated );
	bl->activated_signal.connect( this, &ButtonGroup::activated );
	p_button->set_toggle_mode(true);

	if (bl==button_list)
		p_button->set_pressed( true );

    	p_button->set_toggle_mode( true );

	return p_button;
}

void ButtonGroup::remove(BaseButton *p_button){

	ButtonList **bl_ptr=&button_list;

	while (*bl_ptr) {

		if ((*bl_ptr)->button==p_button) {

			ButtonList *b = (*bl_ptr);
			bl_ptr=&(*bl_ptr)->next;
			GUI_DELETE( b );
			continue;
		}
		bl_ptr=&(*bl_ptr)->next;
	}

}

void ButtonGroup::clear() {

	while (button_list) {

		ButtonList*aux=button_list;
		button_list=button_list->next;
		GUI_DELETE( aux );
	}
};

ButtonGroup::ButtonGroup() {

	button_list=0;
	selecting=false;
}


ButtonGroup::~ButtonGroup() {

	clear();
}


}
