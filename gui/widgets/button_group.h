//
// C++ Interface: button_group
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef PIGUIBUTTON_GROUP_H
#define PIGUIBUTTON_GROUP_H


#include "widgets/base_button.h"

namespace GUI {

/**
	@author Juan Linietsky <reduzio@gmail.com>
*/

class ButtonGroup : public SignalTarget {
public:
	struct ButtonList : public SignalTarget {
		
		ButtonList *next;
		
		BaseButton *button;
		
		void activated() { activated_signal.call(this); }
		
		Signal< Method1<ButtonList*> > activated_signal;
		
		ButtonList() { next=0; button=0; }
	};

private:	
	ButtonList *button_list;
	
	void activated(ButtonList *p_which);
	
	bool selecting; //avoid recursive select

public:
	
	Signal< Method1<int> > current_button_changed_signal;

	void clear();	

	void set_current(int p_idx);
	int get_current();
	
	BaseButton* add_button(BaseButton *p_button); //< append at the end

	template<class T>
	inline T* add(T* p_button) {
		
		add_button(p_button);
		return p_button;
	}

	void remove(BaseButton *p_button);
	
	ButtonGroup();
	~ButtonGroup();
};

}

#endif
