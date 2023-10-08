//
// C++ Interface: memory_static
//
// Description: 
//
//
// Author: Juan Linietsky <red@lunatea>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef MEMORY_POOL_STATIC_H
#define MEMORY_POOL_STATIC_H

/**
	@author Juan Linietsky <red@lunatea>
*/
class MemoryPoolStatic {

public:

	enum DebugLevel {

		LEVEL_NONE,
		LEVEL_SUMMARY,
		LEVEL_DETAIL,
		LEVEL_INSANE,
	};
	virtual void set_debug_level(int p_lev)=0;

private:
	
	static MemoryPoolStatic *singleton;

public:
	
	static MemoryPoolStatic *get_singleton();
	
	virtual void* alloc(unsigned int p_bytes,const char *p_description)=0; ///< Pointer in p_description shold be to a const char const like "hello"
	virtual void* realloc(void * p_memory,unsigned int p_bytes)=0; ///< Pointer in p_description shold be to a const char const like "hello"
	virtual void free(void *p_ptr)=0; ///< Pointer in p_description shold be to a const char const	
	
	virtual int get_total_mem_used()=0;
				
	/* Most likely available only if memory debugger was compiled in */
	virtual int get_alloc_count()=0;
	virtual void * get_alloc_ptr(int p_alloc_idx)=0;
	virtual const char* get_alloc_description(int p_alloc_idx)=0;
	virtual int get_alloc_size(int p_alloc_idx)=0;

	virtual void debug_print_all_memory()=0;

	MemoryPoolStatic();
	virtual ~MemoryPoolStatic();

};

#endif
