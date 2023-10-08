//
// C++ Interface: pool_allocator
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef POOL_ALLOCATOR_H
#define POOL_ALLOCATOR_H

#include "typedefs.h"

/**
	@author Juan Linietsky <reduzio@gmail.com>
 * Generic Pool Allocator.
 * This is a generic memory pool allocator, with locking, compacting and alignment. (@TODO alignment)
 * It used as a standard way to manage alloction in a specific region of memory, such as texture memory,
 * audio sample memory, or just any kind of memory overall.
 * (@TODO) abstraction should be greater, because in many platforms, you need to manage a nonreachable memory.
*/

enum {
	
	POOL_ALLOCATOR_INVALID_ID=-1 ///< default invalid value. use INVALID_ID( id ) to test
};

class PoolAllocator {
public:	
	typedef int ID;
private:	
	enum {
		DEFAULT_MAX_ALLOCS=4096,
		CHECK_BITS=8,
  		CHECK_LEN=(1<<CHECK_BITS),
		CHECK_MASK=CHECK_LEN-1
	
	};
	
	
	struct Entry {
		
		unsigned int pos;
		unsigned int len;
		unsigned int lock;
		unsigned int check;		
		
		inline void clear() { pos=0; len=0; lock=0; check=0; }
		Entry() { clear(); }
	};
	
	
	typedef int EntryArrayPos;
	typedef int EntryIndicesPos;
	
	Entry *entry_array;
	int *entry_indices;
	int entry_max;
	int entry_count;
	
	Uint8 *pool;
	void *mem_ptr;
	int pool_size;
	
	int free_mem;

	unsigned int check_count;
	int align;
	
	bool needs_locking;
	
	inline int entry_end(const Entry& p_entry) const { 
		return p_entry.pos+aligned(p_entry.len); 
	}
	inline int aligned(int p_size) const {
			
		int rem=p_size%align;
		if (rem)
			p_size+=align-rem;
			
		return p_size;
	}
	
	void compact();
	bool get_free_entry(EntryArrayPos* p_pos);
	bool find_hole(EntryArrayPos *p_pos, int p_for_size);
	bool find_entry_index(EntryIndicesPos *p_map_pos,Entry *p_entry);
	Entry* get_entry(ID p_mem);
	const Entry* get_entry(ID p_mem) const;
	
	void create_pool(void * p_mem,int p_size,int p_max_entries);
protected:	
	virtual void mt_lock() const; ///< Reimplement for custom mt locking
	virtual void mt_unlock() const; ///< Reimplement for custom mt locking
	
public:
	
	
	ID alloc(int p_size); ///< Alloc memory, get an ID on success, POOL_ALOCATOR_INVALID_ID on failure
	void free(ID p_mem); ///< Free allocated memory
	Error resize(ID p_mem,int p_new_size); ///< resize a memory chunk
	int get_size(ID p_mem) const;
	
	int get_free_mem(); ///< get free memory
	
	Error lock(ID p_mem); //@todo move this out
	void *get(ID p_mem);
	const void *get(ID p_mem) const;
	void unlock(ID p_mem);
		
	PoolAllocator(int p_size,bool p_needs_locking=false,int p_max_entries=DEFAULT_MAX_ALLOCS);
	PoolAllocator(void * p_mem,int p_size,bool p_needs_locking=false,int p_max_entries=DEFAULT_MAX_ALLOCS);
	PoolAllocator(int p_align,int p_size,bool p_needs_locking=false,int p_max_entries=DEFAULT_MAX_ALLOCS);
		
	virtual ~PoolAllocator();
	
};

#endif
