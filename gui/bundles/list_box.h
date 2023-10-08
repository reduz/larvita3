#ifndef LIST_BOX_H
#define LIST_BOX_H

#include "containers/box_container.h"

namespace GUI {
	
class List;
class VScrollBar;
	
class ListBox : HBoxContainer {

private:
	
	List* list;
	VScrollBar* scroll_bar;

public:

	List* get_list();

	ListBox();
	~ListBox();
};
	
};

#endif
