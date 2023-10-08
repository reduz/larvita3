//
// C++ Interface: box_container
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef PIGUIBOX_CONTAINER_H
#define PIGUIBOX_CONTAINER_H

#include "base/container.h"

namespace GUI {

/**
	@author Juan Linietsky <reduzio@gmail.com>
*/
class BoxContainer : public Container {
	
	
	struct BoxElement : public Element {
						
		struct StretchCache {
			bool will_stretch;
			int min_size;
			int perp_min_size; //perpendicular size to min size
		} stretch_cache;
				
		int stretch;
		BoxElement() { stretch=0; }
	};
	
	virtual Element *create_new_element();	
	virtual Size get_minimum_size_internal();
	virtual void resize_internal(const Size& p_new_size);
	
	bool vertical;
	int separation;	
public:

	/* Very helpful templatized handler for reducing code size */
	template<class T>
	inline T *add(T *p_frame,int p_stretch=0) {
		add_frame(p_frame,p_stretch);
		return p_frame;
	}

	void set_separation( int p_separation );
	void add_frame(Frame *p_frame,int p_stretch=0);
	
	BoxContainer(bool p_vertical=true);
	~BoxContainer();

};

// Specialized box containers
class VBoxContainer : public BoxContainer {


public:	
	
	VBoxContainer() : BoxContainer(true) {}
};

class HBoxContainer : public BoxContainer {


public:	
	
	HBoxContainer() : BoxContainer(false) {}

};

} //end of namespace

#endif
