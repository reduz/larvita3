//
// C++ Implementation: memory_static_malloc
//
// Description: 
//
//
// Author: Juan Linietsky <red@lunatea>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "memory_pool_static_malloc.h"
#include "error_macros.h"
#include <stdlib.h>
#include <stdio.h>
/**
 * NOTE NOTE NOTE NOTE
 * in debug mode, this appends the memory size before the allocated, returned pointer
 * so BE CAREFUL!
 */

 
 
void* MemoryPoolStaticMalloc::alloc(unsigned int p_bytes,const char *p_description) {
 
 	ERR_FAIL_COND_V(p_bytes==0,0);
	
#ifdef DEBUG_MEMORY_ALLOC
	void *mem=malloc(p_bytes+sizeof(RingPtr)); /// add for size and ringlist

	if (!mem) {
		printf("**ERROR: out of memory while allocating %i bytes by %s?\n", p_bytes, p_description);
		printf("**ERROR: memory usage is %i\n", get_total_mem_used());
	};
	
	ERR_FAIL_COND_V(!mem,0); //out of memory, or unreasonable request
		
	/* setup the ringlist element */
	
	RingPtr *ringptr = (RingPtr*)mem;
	
	/* setup the ringlist element data (description and size ) */
	
	ringptr->size = p_bytes;
	ringptr->descr=p_description;
	
	if (ringlist) { /* existing ringlist */
		
		/* assign next */
		ringptr->next = ringlist->next;
		ringlist->next = ringptr;
		/* assign prev */
		ringptr->prev = ringlist;
		ringptr->next->prev = ringptr;
	} else { /* non existing ringlist */
		
		ringptr->next=ringptr;
		ringptr->prev=ringptr;
		ringlist=ringptr;
		
	}
	
	total_mem+=p_bytes;
	
	/* update statistics */
	if (total_mem > max_mem )
		max_mem = total_mem;
	
	total_pointers++;
	
	if (total_pointers > max_pointers)
		max_pointers=total_pointers;
	
	return ringptr + 1; /* return memory after ringptr */
		
#else		
	void *mem=malloc(p_bytes);

	ERR_FAIL_COND_V(!mem,0); //out of memory, or unreasonable request
	return mem;			
#endif	
}

void MemoryPoolStaticMalloc::set_debug_level(int p_lev) {

	debug_level = p_lev;
};

void* MemoryPoolStaticMalloc::realloc(void *p_memory,unsigned int p_bytes) {
	
	if (p_memory==NULL) {
		
		return alloc( p_bytes );
	}
	
	if (p_bytes<=0) {
		
		this->free(p_memory);
		ERR_FAIL_COND_V( p_bytes < 0 , NULL );
		return NULL;
	}
	
#ifdef DEBUG_MEMORY_ALLOC	
	
	
	RingPtr *ringptr = (RingPtr*)p_memory;
	ringptr--; /* go back an element to find the tingptr */
	
	bool single_element = (ringptr->next == ringptr) && (ringptr->prev == ringptr);
	bool is_list = ( ringlist == ringptr );
	
	
	RingPtr *new_ringptr=(RingPtr*)::realloc(ringptr, p_bytes+sizeof(RingPtr));
	
	ERR_FAIL_COND_V( new_ringptr == 0, NULL ); /// reallocation failed 
		
	/* actualize mem used */
	total_mem -= new_ringptr->size;
	new_ringptr->size = p_bytes;
	total_mem +=  new_ringptr->size;
	
	if (total_mem > max_mem ) //update statistics
		max_mem = total_mem;
	
	if (new_ringptr == ringptr )
		return ringptr + 1; // block didn't move, don't do anything
	
	if (single_element) {
		
		new_ringptr->next=new_ringptr;
		new_ringptr->prev=new_ringptr;
	} else {
		
		new_ringptr->next->prev=new_ringptr;
		new_ringptr->prev->next=new_ringptr;		
	}
	
	if (is_list)
		ringlist=new_ringptr;
	
	
	return new_ringptr + 1;
		
#else
	return ::realloc( p_memory, p_bytes );
#endif
}

void MemoryPoolStaticMalloc::free(void *p_ptr) {
	
	
#ifdef DEBUG_MEMORY_ALLOC			
		
	if (p_ptr==0) {
		printf("**ERROR: Static Alloc: Attempted free of NULL pointer.\n");
		return;
	};
	
	RingPtr *ringptr = (RingPtr*)p_ptr;
	
	ringptr--; /* go back an element to find the ringptr */
	
	
	if (debug_level >= LEVEL_INSANE) { /* check if the pointer to free is valid */
	
		RingPtr *p = ringlist;
		
		bool found=false;
		
		if (ringlist) {
			do {
				if (p==ringptr) {
					found=true;
					break;
				}
						
				p=p->next;
			} while (p!=ringlist);
		}
		
		if (!found) {
			printf("**ERROR: Static Alloc: Attempted free of unknown pointer at %p\n",(ringptr+1));
			return;
		}
		
	}		
	
	/* proceed to erase */
	
	bool single_element = (ringptr->next == ringptr) && (ringptr->prev == ringptr);
	bool is_list = ( ringlist == ringptr );
	
	if (single_element) {
		/* just get rid of it */
		ringlist=0;
			
	} else {
		/* auto-remove from ringlist */
		if (is_list)
			ringlist=ringptr->next;
			
		ringptr->prev->next = ringptr->next;
		ringptr->next->prev = ringptr->prev;
	}
	
	total_mem -= ringptr->size;
	total_pointers--;
	
	::free(ringptr); //just free that pointer
		
#else
	ERR_FAIL_COND(p_ptr==0);
		
	::free(p_ptr);
#endif
}

int MemoryPoolStaticMalloc::get_total_mem_used() {
	
#ifdef DEBUG_MEMORY_ALLOC	
		
	return total_mem;
#else	
	return 0;
#endif

}
			
/* Most likely available only if memory debugger was compiled in */
int MemoryPoolStaticMalloc::get_alloc_count() {
	
	return 0;
}
void * MemoryPoolStaticMalloc::get_alloc_ptr(int p_alloc_idx) {
	
	return 0;
}
const char* MemoryPoolStaticMalloc::get_alloc_description(int p_alloc_idx) {
	
	
	return "";
}
int MemoryPoolStaticMalloc::get_alloc_size(int p_alloc_idx) {
	
	return 0;
}


void MemoryPoolStaticMalloc::debug_print_all_memory() {

#ifdef DEBUG_MEMORY_ALLOC	
	
	ERR_FAIL_COND( !ringlist ); /** WTF BUG !? */
	RingPtr *p = ringlist;
	
	int total = 0;
	
	do {
		fprintf(stderr,"MEM: %p - %i bytes - %s\n", p+1, p->size, (p->descr?p->descr:"") );
		total += p->size;
		p=p->next;
	} while (p!=ringlist);
	
	printf("MEM: TOTAL %i\n", total);
#endif 

}

MemoryPoolStaticMalloc::MemoryPoolStaticMalloc() {
	
#ifdef DEBUG_MEMORY_ALLOC	
	total_mem=0;
	total_pointers=0;
	ringlist=0;
	max_mem=0;
	max_pointers=0;
	debug_level = LEVEL_DETAIL;

#endif
		
}


MemoryPoolStaticMalloc::~MemoryPoolStaticMalloc() {
	
#ifdef DEBUG_MEMORY_ALLOC

	if (debug_level >= LEVEL_SUMMARY) {
		if (total_mem > 0 ) {
			printf("**ERROR: Static Alloc: ** MEMORY LEAKS DETECTED **\n");
			printf("**ERROR: Static Alloc: ** MEMORY LEAKS DETECTED **\n");
			printf("**ERROR: Static Alloc: ** MEMORY LEAKS DETECTED **\n");
			printf("**ERROR: Static Alloc: %i bytes of memory in use at exit.\n",total_mem);
			
			if (debug_level >= LEVEL_DETAIL) {
				printf("**ERROR: Static Alloc: Following is the list of leaked allocations: \n");
				
				ERR_FAIL_COND( !ringlist ); /** WTF BUG !? */
				RingPtr *p = ringlist;
				
				do {
					printf("\t%p - %i bytes - %s\n", (RingPtr*)(p+1), p->size, (p->descr?p->descr:"") );
					p=p->next;
				} while (p!=ringlist);
				
				printf("**ERROR: Static Alloc: End of Report.\n");
			};
		} else {
			
			printf("**DEBUG: Static Alloc: No Memory Leaks.\n");
		}
		
		printf("**DEBUG: Static Alloc: Maximum HEAP used: %i bytes\n",max_mem);
		printf("**DEBUG: Static Alloc: Maximum HEAP pointers used: %i\n",max_pointers);
	};
#endif
}


