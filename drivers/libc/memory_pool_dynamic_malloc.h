#ifndef MEMORY_POOL_MALLOC_H
#define MEMORY_POOL_MALLOC_H

#include "os/memory_pool_dynamic.h"

class MemoryPoolDynamicMalloc : public MemoryPoolDynamic {


	enum {

		MAX_ALLOCS=32768 //must be power of 2
	};


	struct AllocItem {

		void *ptr;
		Uint32 len;
		Uint16 check;
		Uint16 lock;
		const char *descr;

		AllocItem() { ptr=NULL; len=0; check=0; lock=0; descr=0; }
	};

	AllocItem alloc_items[MAX_ALLOCS];

	Uint32 check_incr;
	int last_alloc_idx;

public:

	MemoryPoolDynamic_Handle alloc_mem(Uint32 p_amount,const char *p_descr=0);
	bool is_mem_valid(MemoryPoolDynamic_Handle p_handle); /* check if memory is valid */
	void free_mem(MemoryPoolDynamic_Handle p_handle);
	Error realloc_mem(MemoryPoolDynamic_Handle p_handle,Uint32 p_new_amount); /* true on error */
	int get_mem_size(MemoryPoolDynamic_Handle p_handle);

	Error lock_mem(MemoryPoolDynamic_Handle p_handle); /* try to lock the mem */
	void *get_mem(MemoryPoolDynamic_Handle p_handle);
	void unlock_mem(MemoryPoolDynamic_Handle p_handle);
	bool is_locked(MemoryPoolDynamic_Handle p_handle);

	Uint32 get_total_mem_used();
	Uint32 get_total_mem_allocs_used();

	MemoryPoolDynamicMalloc();
	~MemoryPoolDynamicMalloc();
};



#endif
