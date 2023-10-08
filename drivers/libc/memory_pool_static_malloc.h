//
// C++ Interface: memory_static_malloc
//
// Description: 
//
//
// Author: Juan Linietsky <red@lunatea>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef MEMORY_POOL_STATIC_MALLOC_H
#define MEMORY_POOL_STATIC_MALLOC_H

#include "os/memory_pool_static.h"
/**
	@author Juan Linietsky <red@lunatea>
*/
class MemoryPoolStaticMalloc : public MemoryPoolStatic {

	struct RingPtr {
		
		int size;
		const char *descr; /* description of memory */
		RingPtr *next;
		RingPtr *prev;
	};
	
	RingPtr *ringlist;
	int total_mem;
	int total_pointers;
	
	int max_mem;
	int max_pointers;
	
	int debug_level;
		
public:

	virtual void set_debug_level(int p_lev);

	virtual void* alloc(unsigned int p_bytes,const char *p_description=""); ///< Pointer in p_description shold be to a const char const like "hello"
	virtual void free(void *p_ptr); ///< Pointer in p_description shold be to a const char const	
	virtual void* realloc(void *p_memory,unsigned int p_bytes); ///< Pointer in 	
	virtual int get_total_mem_used();
				
	/* Most likely available only if memory debugger was compiled in */
	virtual int get_alloc_count();
	virtual void * get_alloc_ptr(int p_alloc_idx);
	virtual const char* get_alloc_description(int p_alloc_idx);
	virtual int get_alloc_size(int p_alloc_idx);
	
	virtual void debug_print_all_memory();	
	
	MemoryPoolStaticMalloc();
	~MemoryPoolStaticMalloc();

};

#endif
