//
// C++ Implementation: pool_allocator
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "pool_allocator.h"
#include "error_macros.h"
#include "os/memory.h"
#include "os/copymem.h"


#define COMPACT_CHUNK( m_entry , m_to_pos ) 			\
do {								\
	void *_dst=&((unsigned char*)pool)[m_to_pos];	\
	void *_src=&((unsigned char*)pool)[(m_entry).pos];	\
	movemem(_dst,_src,aligned((m_entry).len));			\
	(m_entry).pos=m_to_pos;					\
} while (0);

void PoolAllocator::mt_lock() const {
	
}

void PoolAllocator::mt_unlock() const {
	
}


bool PoolAllocator::get_free_entry(EntryArrayPos* p_pos) {

	if (entry_count==entry_max)
		return false;

	for (int i=0;i<entry_max;i++) {

		if (entry_array[i].len==0) {
			*p_pos=i;
			return true;
		}
		
	}

	ERR_PRINT("Out of memory Chunks!");

	return false; //
}

/**
 * Find a hole
 * @param p_pos The hole is behind the block pointed by this variable upon return. if pos==entry_count, then allocate at end
 * @param p_for_size hole size
 * @return false if hole found, true if no hole found
 */
bool PoolAllocator::find_hole(EntryArrayPos *p_pos, int p_for_size) {

	/* position where previous entry ends. Defaults to zero (begin of pool) */
	
	int prev_entry_end_pos=0;

	for (int i=0;i<entry_count;i++) {


		Entry &entry=entry_array[ entry_indices[ i ] ];

		/* determine hole size to previous entry */

		int hole_size=entry.pos-prev_entry_end_pos;

		/* detemine if what we want fits in that hole */
		if (hole_size>=p_for_size) {
			*p_pos=i;
			return true;
		}

		/* prepare for next one */
		prev_entry_end_pos=entry_end(entry);
	}

	/* No holes between entrys, check at the end..*/
	
	if ( (pool_size-prev_entry_end_pos)>=p_for_size )  {
		*p_pos=entry_count;
		return true;
	}
	
	return false;
	
}


void PoolAllocator::compact() {
	
	Uint32 prev_entry_end_pos=0;
	
	for (int i=0;i<entry_count;i++) {


		Entry &entry=entry_array[ entry_indices[ i ] ];

		/* determine hole size to previous entry */

		int hole_size=entry.pos-prev_entry_end_pos;

		/* if we can compact, do it */
		if (hole_size>0 && !entry.lock) {
			
			COMPACT_CHUNK(entry,prev_entry_end_pos);
			
		}
		
		/* prepare for next one */
		prev_entry_end_pos=entry_end(entry);
	}
	
	
}


bool PoolAllocator::find_entry_index(EntryIndicesPos *p_map_pos,Entry *p_entry) {
	
	EntryArrayPos entry_pos=entry_max;
	
	for (int i=0;i<entry_count;i++) {
		
		if (&entry_array[ entry_indices[ i ] ]==p_entry) {
			
			entry_pos=i;
			break;
		}
	}
	
	if (entry_pos==entry_max)
		return false;
	
	*p_map_pos=entry_pos;
	return true;
	
}

PoolAllocator::ID PoolAllocator::alloc(int p_size) {
	
	ERR_FAIL_COND_V(p_size<1,POOL_ALLOCATOR_INVALID_ID);
	ERR_FAIL_COND_V(p_size>free_mem,POOL_ALLOCATOR_INVALID_ID);
		
	mt_lock();
	
	if (entry_count==entry_max) {
		mt_unlock();
		ERR_PRINT("entry_count==entry_max");
		return POOL_ALLOCATOR_INVALID_ID;
	}
	
	
	int size_to_alloc=aligned(p_size);
	
	EntryIndicesPos new_entry_indices_pos;
	
	if (!find_hole(&new_entry_indices_pos, size_to_alloc)) {
		/* No hole could be found, try compacting mem */
		compact();
		/* Then search again */
		
		if (!find_hole(&new_entry_indices_pos, size_to_alloc)) {
			
			mt_unlock();
			ERR_PRINT("memory can't be compacted further");
			return POOL_ALLOCATOR_INVALID_ID;						
		}
	}
	
	EntryArrayPos new_entry_array_pos;
		
	bool found_free_entry=get_free_entry(&new_entry_array_pos);
	
	if (!found_free_entry) {
		mt_unlock();
		ERR_FAIL_COND_V( !found_free_entry , POOL_ALLOCATOR_INVALID_ID );
	}
	
	/* move all entry indices up, make room for this one */
	for (int i=entry_count;i>new_entry_indices_pos;i--  ) {
		
		entry_indices[i]=entry_indices[i-1];
	}
	
	entry_indices[new_entry_indices_pos]=new_entry_array_pos;
	
	entry_count++;
	
	Entry &entry=entry_array[ entry_indices[ new_entry_indices_pos ] ];
	
	entry.len=p_size;
	entry.pos=(new_entry_indices_pos==0)?0:entry_end(entry_array[ entry_indices[ new_entry_indices_pos-1 ] ]); //alloc either at begining or end of previous
	entry.lock=0;
	entry.check=(check_count++)&CHECK_MASK;
	free_mem-=size_to_alloc;
	
	ID retval = (entry_indices[ new_entry_indices_pos ]<<CHECK_BITS)|entry.check;
	mt_unlock();
	return retval;
	
}

PoolAllocator::Entry * PoolAllocator::get_entry(ID p_mem) {
	
	unsigned int check=p_mem&CHECK_MASK;
	int entry=p_mem>>CHECK_BITS;
	ERR_FAIL_INDEX_V(entry,entry_max,NULL);
	ERR_FAIL_COND_V(entry_array[entry].check!=check,NULL);
	ERR_FAIL_COND_V(entry_array[entry].len==0,NULL);
		
	return &entry_array[entry];
}

const PoolAllocator::Entry * PoolAllocator::get_entry(ID p_mem) const {
	
	unsigned int check=p_mem&CHECK_MASK;
	int entry=p_mem>>CHECK_BITS;
	ERR_FAIL_INDEX_V(entry,entry_max,NULL);
	ERR_FAIL_COND_V(entry_array[entry].check!=check,NULL);
	ERR_FAIL_COND_V(entry_array[entry].len==0,NULL);
		
	return &entry_array[entry];
}

void PoolAllocator::free(ID p_mem) {
	
	mt_lock();
	Entry *e=get_entry(p_mem);
	if (!e) {
		mt_unlock();
		ERR_PRINT("!e");
		return;
	}
	if (e->lock) {
		mt_unlock();
		ERR_PRINT("e->lock");
		return;
	}
	
	EntryIndicesPos entry_indices_pos;
	
	bool index_found = find_entry_index(&entry_indices_pos,e);
	if (!index_found) {
	
		mt_unlock();
		ERR_FAIL_COND(!index_found);
	}
	

	
	for (int i=entry_indices_pos;i<(entry_count-1);i++) {
		
		entry_indices[ i ] = entry_indices[ i+1 ];
	}
	
	entry_count--;
	free_mem+=aligned(e->len);
	e->clear();
	mt_unlock();
}

int PoolAllocator::get_size(ID p_mem) const {

	int size;
	mt_lock();

	const Entry *e=get_entry(p_mem);
	if (!e) {
		
		mt_unlock();
		ERR_PRINT("!e");
		return 0;
	}
	
	size=e->len;
	
	mt_unlock();
	
	return size;
} 

Error PoolAllocator::resize(ID p_mem,int p_new_size) {
	
	ERR_FAIL_COND_V( true, ERR_UNAVAILABLE ); //@ todo fix
}


Error PoolAllocator::lock(ID p_mem) {
	
	if (!needs_locking)
		return OK;
	mt_lock();
	Entry *e=get_entry(p_mem);
	if (!e) {
		
		mt_unlock();
		ERR_PRINT("!e");
		return ERR_INVALID_PARAMETER;
	}
	e->lock++;	
	mt_unlock();
	return OK;
}

const void *PoolAllocator::get(ID p_mem) const {

	if (!needs_locking) {
	
		const Entry *e=get_entry(p_mem);
		ERR_FAIL_COND_V(!e,NULL);
		return &pool[e->pos];
	
	}
	
	mt_lock();
	const Entry *e=get_entry(p_mem);
	
	if (!e) {
	
		mt_unlock();
		ERR_FAIL_COND_V(!e,NULL);
	}
	if (e->lock==0) {
	
		mt_unlock();
		ERR_PRINT( "e->lock == 0" );
		return NULL;
	}
	
	if (e->pos<0 || (int)e->pos>=pool_size) {
		
		mt_unlock();
		ERR_PRINT("e->pos<0 || e->pos>=pool_size");
		return NULL;
	}
	const void *ptr=&pool[e->pos];
	
	mt_unlock();
	
	return ptr;

}


void *PoolAllocator::get(ID p_mem) {
	
	if (!needs_locking) {
	
		Entry *e=get_entry(p_mem);
		ERR_FAIL_COND_V(!e,NULL);
		return &pool[e->pos];
	
	}
	
	mt_lock();
	Entry *e=get_entry(p_mem);
	
	if (!e) {
	
		mt_unlock();
		ERR_FAIL_COND_V(!e,NULL);
	}
	if (e->lock==0) {
	
		mt_unlock();
		ERR_PRINT( "e->lock == 0" );
		return NULL;
	}
	
	if (e->pos<0 || (int)e->pos>=pool_size) {
		
		mt_unlock();
		ERR_PRINT("e->pos<0 || e->pos>=pool_size");
		return NULL;
	}
	void *ptr=&pool[e->pos];
	
	mt_unlock();
	
	return ptr;
	
}
void PoolAllocator::unlock(ID p_mem) {
	
	if (!needs_locking)
		return;
	mt_lock();
	Entry *e=get_entry(p_mem);
	if (e->lock == 0 ) {
		mt_unlock();
		ERR_PRINT( "e->lock == 0" );
		return;
	}
	e->lock--;		
	mt_unlock();
}

int PoolAllocator::get_free_mem() {
	
	return free_mem;
}

void PoolAllocator::create_pool(void * p_mem,int p_size,int p_max_entries) {
	
	pool=(Uint8*)p_mem;
	pool_size=p_size;
		
	entry_array = memnew_arr( Entry, p_max_entries );
	entry_indices = memnew_arr( int, p_max_entries );
	entry_max = p_max_entries;
	entry_count=0;
	
	free_mem=p_size;
	
	check_count=0;	
}

PoolAllocator::PoolAllocator(int p_size,bool p_needs_locking,int p_max_entries) {
	
	mem_ptr=Memory::alloc_static( p_size,"PoolAllocator()");
	ERR_FAIL_COND(!mem_ptr);
	align=1;
	create_pool(mem_ptr,p_size,p_max_entries);
	needs_locking=p_needs_locking;
	
}

PoolAllocator::PoolAllocator(void * p_mem,int p_size,bool p_needs_locking,int p_max_entries) {
	
	create_pool( p_mem,p_size,p_max_entries);
	needs_locking=p_needs_locking;
	align=1;
	mem_ptr=NULL;	
}

#include <stdio.h>
PoolAllocator::PoolAllocator(int p_align,int p_size,bool p_needs_locking,int p_max_entries) {
	
	ERR_FAIL_COND(p_align<1);	
	mem_ptr=Memory::alloc_static( p_size+p_align,"PoolAllocator()");
	Uint8 *mem8=(Uint8*)mem_ptr;
	long int ofs = (long int)mem8;
	if (ofs&p_align)
		mem8+=p_align-(ofs&p_align);
	create_pool( mem8 ,p_size,p_max_entries);
	needs_locking=p_needs_locking;
	align=p_align;
	printf("********** pool is %p\n", pool);
}

PoolAllocator::~PoolAllocator() {
	
	if (mem_ptr)
		Memory::free_static( mem_ptr );
	
	memdelete_arr( entry_array );
	memdelete_arr( entry_indices );
	
}

