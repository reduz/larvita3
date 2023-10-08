//
// C++ Interface: grid_container
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef PIGUIGRID_CONTAINER_H
#define PIGUIGRID_CONTAINER_H

#include "base/container.h"

namespace GUI {

/**
	@author Juan Linietsky <reduzio@gmail.com>
*/

class GridContainer : public Container {

public:
	enum {

		VCACHE_RESIZE_EVERY=4
	};

private:
	struct GridElement : public Element {
						
		bool h_expand;
		bool v_expand;
			
		GridElement() { h_expand=false; v_expand=false; }
	};
	
	struct CacheData {
		int min_size;
		bool expand;
		int size;
		CacheData() { min_size=-1;expand=false; } //-1 means that it shouldnt not be shown
	};


	CacheData *vcache;
	CacheData *hcache;
	
	int vcache_size;
	int columns;
	int rows;



	int separation;

	void compute_cachedatas();
	void compute_sizes(CacheData *p_cache, int p_elements, int p_total);
	virtual Element *create_new_element();
	virtual Size get_minimum_size_internal();
	virtual void resize_internal(const Size& p_new_size);


public:

	void add_frame(Frame *p_frame, bool p_h_expand, bool p_v_expand);
	void set_separation(int p_separation);

	template<class T>
	T *add(T *p_frame, bool p_h_expand, bool p_v_expand) {

		add_frame(p_frame,p_h_expand,p_v_expand);
		return p_frame;

	}
	
	GridContainer(int p_columns=2);
	~GridContainer();

};

}

#endif
