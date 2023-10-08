#include "memory_pool_dynamic_malloc.h"

#include <stdlib.h>
#include <stdio.h>

#include "error_macros.h"

MemoryPoolDynamic_Handle MemoryPoolDynamicMalloc::alloc_mem(Uint32 p_amount,const char *p_descr) {

	int free_idx=-1;
	for (int i=0;i<MAX_ALLOCS;i++) {

		int idx=(i+last_alloc_idx)&(MAX_ALLOCS-1);
		if (alloc_items[idx].ptr==NULL) {

			free_idx=idx;
			break;
		}
	}

	ERR_FAIL_COND_V( free_idx==-1 , MemoryPoolDynamic_Handle() );

	last_alloc_idx=free_idx; //optimization!
	alloc_items[free_idx].ptr=malloc(p_amount);
	if (alloc_items[free_idx].ptr==NULL)
		return MemoryPoolDynamic_Handle(); //out of memory

	alloc_items[free_idx].len=p_amount;
	alloc_items[free_idx].check=check_incr++;
	alloc_items[free_idx].lock=0;
	alloc_items[free_idx].descr=p_descr;

	return MemoryPoolDynamic_Handle(&alloc_items[free_idx],alloc_items[free_idx].check);

}
bool MemoryPoolDynamicMalloc::is_mem_valid(MemoryPoolDynamic_Handle p_handle) {

	ERR_FAIL_COND_V( p_handle.is_null() , false );

	AllocItem *alloc_item=(AllocItem*)get_private( p_handle );



	/* If not in range of the pool, it's not valid pointer inside */
	ERR_FAIL_COND_V( alloc_item<&alloc_items[0] || alloc_item>&alloc_items[MAX_ALLOCS-1] , false);

	/* Mem has been freed */
	ERR_FAIL_COND_V( alloc_item->ptr==NULL , false );

	/* Check if the item "check" integer is the same */
	ERR_FAIL_COND_V(alloc_item->check!=get_check( p_handle ), false);


	return true;

}
void MemoryPoolDynamicMalloc::free_mem(MemoryPoolDynamic_Handle p_handle) {

	ERR_FAIL_COND( !is_mem_valid(p_handle) );

	AllocItem *alloc_item=(AllocItem*)get_private( p_handle );

	/* Cant free locked memory, unlock first! */
	ERR_FAIL_COND( alloc_item->lock>0 );

	free(alloc_item->ptr);
	alloc_item->ptr=NULL;
	alloc_item->check=0;
	alloc_item->len=0;
	alloc_item->lock=0;


}
Error MemoryPoolDynamicMalloc::realloc_mem(MemoryPoolDynamic_Handle p_handle,Uint32 p_new_amount) {


	ERR_FAIL_COND_V( !is_mem_valid(p_handle), ERR_INVALID_PARAMETER );

	AllocItem *alloc_item=(AllocItem*)get_private( p_handle );

	/* Cant be locked when realloc-ing */
	ERR_FAIL_COND_V( alloc_item->lock>0, ERR_LOCKED );

	if (alloc_item->len==p_new_amount)
		return OK;



	void *new_mem=realloc(alloc_item->ptr,p_new_amount);

	ERR_FAIL_COND_V( !new_mem, ERR_OUT_OF_MEMORY );
	
	alloc_item->ptr=new_mem;
	alloc_item->len=p_new_amount;

	return OK;
		
}

int MemoryPoolDynamicMalloc::get_mem_size(MemoryPoolDynamic_Handle p_handle) {

	ERR_FAIL_COND_V( !is_mem_valid(p_handle), -1 );

	AllocItem *alloc_item=(AllocItem*)get_private( p_handle );
	return alloc_item->len;
}

bool MemoryPoolDynamicMalloc::is_locked(MemoryPoolDynamic_Handle p_handle) {
	
	ERR_FAIL_COND_V( !is_mem_valid(p_handle), false );

	AllocItem *alloc_item=(AllocItem*)get_private( p_handle );
	return alloc_item->lock!=0;

	return false;	
}

Error MemoryPoolDynamicMalloc::lock_mem(MemoryPoolDynamic_Handle p_handle) {

	ERR_FAIL_COND_V( !is_mem_valid(p_handle), ERR_INVALID_PARAMETER );

	AllocItem *alloc_item=(AllocItem*)get_private( p_handle );
	alloc_item->lock++;

	return OK;
}

void *MemoryPoolDynamicMalloc::get_mem(MemoryPoolDynamic_Handle p_handle) {

	ERR_FAIL_COND_V( !is_mem_valid(p_handle), NULL );

	AllocItem *alloc_item=(AllocItem*)get_private( p_handle );
	ERR_FAIL_COND_V( alloc_item->lock==0 , NULL);

	return alloc_item->ptr;
}

void MemoryPoolDynamicMalloc::unlock_mem(MemoryPoolDynamic_Handle p_handle) {

	ERR_FAIL_COND( !is_mem_valid(p_handle) );

	AllocItem *alloc_item=(AllocItem*)get_private( p_handle );
	ERR_FAIL_COND( alloc_item->lock==0 );

	alloc_item->lock--;

	return;

}

Uint32 MemoryPoolDynamicMalloc::get_total_mem_used() {
	Uint32 total=0;
	for (int i=0;i<MAX_ALLOCS;i++) {

		if (alloc_items[i].ptr)
			total+=alloc_items[i].len;
	}

	return total;

}

Uint32 MemoryPoolDynamicMalloc::get_total_mem_allocs_used() {

	Uint32 total=0;
	for (int i=0;i<MAX_ALLOCS;i++) {

		if (alloc_items[i].ptr)
			total++;
	}

	return total;


}


MemoryPoolDynamicMalloc::MemoryPoolDynamicMalloc() {

	check_incr=1;
	last_alloc_idx=0;
}

MemoryPoolDynamicMalloc::~MemoryPoolDynamicMalloc() {

	int mem_used = get_total_mem_used();
	if (mem_used==0) {
		printf("Dynamic Alloc: ## 0 bytes of Memory used at exit. No memory leaks detected##\n");
		return;
	}

	printf("Dynamic Alloc: **MEMORY LEAK DETECTED**\n");
	printf("Dynamic Alloc: %i bytes of memory in use at program exit.", get_total_mem_used());
	printf("\tDetail follows:");

	Uint32 count=1;

	for (int i=0;i<MAX_ALLOCS;i++) {

		if (alloc_items[i].ptr) {
			printf("\t%p - %i bytes: %s\n",alloc_items[i].ptr,alloc_items[i].len,(alloc_items[i].descr?alloc_items[i].descr:NULL));
			count++;
		}
	}

}


