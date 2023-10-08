//
// C++ Implementation: margin_group
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "margin_group.h"
#include "widgets/label.h"

namespace GUI {


void MarginGroup::set_label_text(String p_label) {
	
	label->set_text(p_label);
}
void MarginGroup::add_frame(Frame *p_frame,int p_stretch) {

	internal_vb->add_frame(p_frame,p_stretch);
}

void MarginGroup::set_in_window() {

	spacer->set_minimum_size( Size( constant( C_MARGIN_GROUP_SPACER_SIZE ), 0 ) );

}

HBoxContainer *MarginGroup::get_label_hb() {
	
	return label_hb;
}

MarginGroup::MarginGroup(String p_label) {

	label_hb = BoxContainer::add( GUI_NEW( HBoxContainer ), 0 );
	label=label_hb->add( GUI_NEW( Label(p_label)),0);

	HBoxContainer *hbc=BoxContainer::add( GUI_NEW( HBoxContainer),1);
	spacer=hbc->add( GUI_NEW( Widget ),0 );
	internal_vb=hbc->add( GUI_NEW( VBoxContainer) , 1);
	
}


MarginGroup::~MarginGroup()
{
}


}
