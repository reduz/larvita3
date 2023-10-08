//
// C++ Implementation: grid_container
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "grid_container.h"


namespace GUI {

Container::Element *GridContainer::create_new_element() {


	return GUI_NEW( GridElement );
}

void GridContainer::compute_cachedatas() {


	int visible_elements=0;
	
	GridElement *ge=(GridElement*)get_element_list_end();

	while (ge) {

		if (ge->frame->is_visible()) {
			
			visible_elements++;
		}

		ge=(GridElement*)ge->prev;
	}

	
	rows=((visible_elements-1)/columns)+1;

	if (vcache && ( vcache_size!=rows || rows==0)) {
	
		GUI_DELETE_ARRAY( vcache );
		vcache=0;
	}
	
	if (rows && !vcache) {
		vcache=GUI_NEW_ARRAY( CacheData, rows );
	}
	
	vcache_size=rows;
		
	int row=0,col=0;

	int sep=separation;

	for (int r=0;r<rows;r++) {

		vcache[r].min_size=0;
		vcache[r].expand=false;
		vcache[r].size=0;
	}

	for (int c=0;c<columns;c++) {

		hcache[c].min_size=0;
		hcache[c].expand=false;
		hcache[c].size=0;
	}
	
	ge=(GridElement*)get_element_list_end();
	
	while (ge) {


		Size emin;
		if (ge->frame->is_visible()) {
			
			emin=ge->frame->get_minimum_size();
	
			if (row<(rows-1))
				emin.height+=sep;
			
			if (col<(columns-1))
				emin.width+=sep;
			
			if (emin.width>hcache[col].min_size)
				hcache[col].min_size=emin.width;
	
			if (emin.height>vcache[row].min_size)
				vcache[row].min_size=emin.height;
	
			if (ge->h_expand)
				hcache[col].expand=true;
						
			if (ge->v_expand)
				vcache[row].expand=true;
			
			col++;
			if (col>=columns) {
				col=0;
				row++;
			}
		}

		ge=(GridElement*)ge->prev;

	}

}
Size GridContainer::get_minimum_size_internal() {

	
	if (!get_element_list())
		return Size();

	int sep=separation;
	Size min;
		
	compute_cachedatas();
	
	for (int r=0;r<rows;r++) {

		min.height+=vcache[r].min_size;
	}
	

	for (int c=0;c<columns;c++) {

		min.width+=hcache[c].min_size;
	}
	
	return min;

	
}

void GridContainer::compute_sizes(CacheData *p_cache, int p_elements, int p_total) {

	int expand_available=p_total;
	int expand_count=0;
	
	for (int i=0;i<p_elements;i++) {
	
		expand_available-=p_cache[i].min_size;
		if (p_cache[i].expand) {			
			expand_count++;
		}
	}
	
	for (int i=0;i<p_elements;i++) {

		int final=p_cache[i].min_size;

		if (p_cache[i].expand && expand_available>0) {
			
			final+=expand_available/expand_count;
		}
		p_cache[i].size=final;
	}	
#if 0	
	float expand_count=0;
	float expand_sum=0;
	float shrink_sum=0;
	int display_count=0;

	
	for (int i=0;i<p_elements;i++) {

		if (p_cache[i].expand) {
			expand_sum+=p_cache[i].min_size;
			expand_count++;
		} else {

			shrink_sum+=p_cache[i].min_size;
		}
		display_count++;
	}
	
	
	if (display_count==0)
		return; //nothing to do

	float expand_space=p_total-shrink_sum;
	

	for (int i=0;i<p_elements;i++) {

		if (p_cache[i].min_size<1) {
			p_cache[i].size=0;
			continue;
		}

		float final;

		final=p_cache[i].min_size;

		if (p_cache[i].expand) {

			final*=expand_space/expand_sum;
		}
		p_cache[i].size=final;
	}

#endif
}

void GridContainer::resize_internal(const Size& p_new_size) {


	compute_cachedatas();
	compute_sizes(vcache,rows,p_new_size.height);
	compute_sizes(hcache,columns,p_new_size.width);

	int sep=separation;
	Point ofs;
	int row=0,col=0;

	GridElement *ge=(GridElement*)get_element_list_end();

	while (ge) {

		if (ge->frame->is_visible()) {
	
			Size dst_size(hcache[col].size,vcache[row].size);
	
			if (row<(rows-1))
				dst_size.height-=sep;
			
			if (col<(columns-1))
				dst_size.width-=sep;
	
			
			ge->set_rect( Rect( ofs, dst_size ) );
			//ge->rect.pos=ofs;
			//ge->rect.size=dst_size;
			ge->frame->resize_tree( dst_size );
	
			ofs.x+=hcache[col].size;
			
			col++;
			if (col>=columns) {
				col=0;
				ofs.x=0;
				ofs.y+=vcache[row].size;
				row++;
				
			}
		}
		ge=(GridElement*)ge->prev;
	}

}

void GridContainer::set_separation(int p_separation) {
	
	separation=p_separation;
	check_minimum_size();
}

void GridContainer::add_frame(Frame *p_frame, bool p_h_expand, bool p_v_expand) {


	GridElement *ge=(GridElement*)add_frame_internal( p_frame );
	if (!ge)
		return;
	ge->h_expand=p_h_expand;
	ge->v_expand=p_v_expand;

	
	/* vcache */
	

	
}

GridContainer::GridContainer(int p_columns) {

	if (p_columns<1)
		p_columns=1;

        
	columns = p_columns;

	hcache = GUI_NEW_ARRAY( CacheData, columns);
	vcache=0;
	rows=0;
	vcache_size=0;

	separation=2;

}


GridContainer::~GridContainer()
{
	if (vcache) {
		GUI_DELETE_ARRAY( vcache );
	}
	if (hcache){
		GUI_DELETE_ARRAY( hcache );;
	}
}


}
