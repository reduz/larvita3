#include "list_box.h"

#include "widgets/list.h"
#include "widgets/scroll_bar.h"

namespace GUI {

List* ListBox::get_list() {
	
	return list;
};
	
ListBox::ListBox() {

	list = add( GUI_NEW( List ), 1 );
	scroll_bar = add( GUI_NEW( VScrollBar ) );

	list->set_range(scroll_bar->get_range());
};

ListBox::~ListBox() {


};

};


