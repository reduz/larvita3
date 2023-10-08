#include "memory.h"
#include "memory_pool_static.h"

int Memory::memory_pools_in_use=0;
MemoryPoolDynamic *Memory::memory_pools[MAX_MEMORY_POOLS];

void * operator new(size_t p_size,const char *p_description) {

	return Memory::alloc_static( p_size, p_description );
}

/*
void * operator new(size_t p_size,void *p_placement) {

	return p_placement;
}
*/


void * Memory::alloc_static(unsigned int p_bytes,const char *p_alloc_from) {

	ERR_FAIL_COND_V( !MemoryPoolStatic::get_singleton(), NULL );
	return MemoryPoolStatic::get_singleton()->alloc(p_bytes,p_alloc_from);
}

void * Memory::calloc_static(unsigned int p_elems,unsigned int p_bytes,const char *p_alloc_from) {

	void * data = alloc_static(p_bytes*p_elems,p_alloc_from);
	ERR_FAIL_COND_V(!data,data);
	
	Uint8*bytes=(Uint8*)data;
	for (int i=0;i<p_bytes*p_elems;i++) {
		
		bytes[i]=0;
	}
	
	return data;
}
void * Memory::realloc_static(void *p_memory,unsigned int p_bytes) {

	ERR_FAIL_COND_V( !MemoryPoolStatic::get_singleton(), NULL );
	return MemoryPoolStatic::get_singleton()->realloc(p_memory,p_bytes);
}

void Memory::free_static(void *p_ptr) {

	ERR_FAIL_COND( !MemoryPoolStatic::get_singleton());

	MemoryPoolStatic::get_singleton()->free(p_ptr);
}

unsigned int Memory::get_static_mem_usage() {

	ERR_FAIL_COND_V( !MemoryPoolStatic::get_singleton(), 0);
	return MemoryPoolStatic::get_singleton()->get_total_mem_used();
}

unsigned int Memory::get_dynamic_mem_usage() {


	unsigned int total=0;
	for (int i=0;i<memory_pools_in_use;i++) {

		total+= memory_pools[i]->get_total_mem_used();
	}

	return total;
}



/* standard alloc */
MID Memory::alloc_dynamic(int p_bytes,const char* p_descr,int p_pool_index) {

	ERR_FAIL_INDEX_V( p_pool_index, memory_pools_in_use, MID() );

	MID mem= memory_pools[ p_pool_index ]->alloc_mem( p_bytes,p_descr );
	ERR_FAIL_COND_V( mem.is_null(), MID() );
	mem._pool_index=p_pool_index;

	return mem;

}

Error Memory::resize_dynamic(MID p_MID,int p_bytes) {

	ERR_FAIL_COND_V( p_MID.is_null(), ERR_PARAMETER_RANGE_ERROR );
	ERR_FAIL_INDEX_V( p_MID._pool_index, memory_pools_in_use, ERR_PARAMETER_RANGE_ERROR );

	return memory_pools[ p_MID._pool_index ]->realloc_mem(p_MID,p_bytes);

}


void Memory::free_dynamic(MID p_MID) {

	ERR_FAIL_INDEX( p_MID._pool_index, memory_pools_in_use );
	ERR_FAIL_COND( p_MID.is_null() );

	memory_pools[ p_MID._pool_index ]->free_mem( p_MID );
}


bool Memory::is_locked(MID p_MID) {

	ERR_FAIL_INDEX_V( p_MID._pool_index, memory_pools_in_use, false );
	ERR_FAIL_COND_V( p_MID.is_null(), false );

	return memory_pools[ p_MID._pool_index ]->is_locked( p_MID );
}

Error Memory::lock(MID p_MID) {

	ERR_FAIL_INDEX_V( p_MID._pool_index, memory_pools_in_use, ERR_INVALID_PARAMETER );
	ERR_FAIL_COND_V( p_MID.is_null(), ERR_INVALID_PARAMETER );

	return memory_pools[ p_MID._pool_index ]->lock_mem( p_MID );
}

void *Memory::get(MID p_MID) {

	ERR_FAIL_INDEX_V( p_MID._pool_index, memory_pools_in_use, NULL );
	ERR_FAIL_COND_V( p_MID.is_null(), NULL );

	return memory_pools[ p_MID._pool_index ]->get_mem( p_MID );

}

void Memory::unlock(MID p_MID) {

	ERR_FAIL_INDEX( p_MID._pool_index, memory_pools_in_use );
	ERR_FAIL_COND( p_MID.is_null() );

	return memory_pools[ p_MID._pool_index ]->unlock_mem( p_MID );

}


void Memory::add_dynamic_pool( MemoryPoolDynamic *p_pool ) {

	ERR_FAIL_COND( memory_pools_in_use == MAX_MEMORY_POOLS );

	memory_pools[ memory_pools_in_use ] = p_pool;
	memory_pools_in_use++;

}

int Memory::get_size(MID p_MID) {

	ERR_FAIL_INDEX_V( p_MID._pool_index, memory_pools_in_use, -1 );
	ERR_FAIL_COND_V( p_MID.is_null(), -1 );

	return memory_pools[ p_MID._pool_index ]->get_mem_size(p_MID);

}

bool Memory::is_valid(MID p_MID) {

	if (p_MID.is_null())
		return false;

	ERR_FAIL_INDEX_V( p_MID._pool_index, memory_pools_in_use, false );

	return memory_pools[ p_MID._pool_index ]->is_mem_valid(p_MID);


}

void * operator new(size_t p_size,MID *p_mid,int p_pool, int p_extra_pad) {

	void *failptr=0; //get rid of a warning
	ERR_FAIL_COND_V(!p_mid->is_null(),failptr);

	*p_mid = Memory::alloc_dynamic( p_size + p_extra_pad );

	ERR_FAIL_COND_V( p_mid->is_null() , failptr);
	ERR_FAIL_COND_V( Memory::lock( *p_mid ), failptr );

	unsigned char *mem = (unsigned char*)Memory::get( *p_mid );

	return &mem[p_extra_pad];
}


void * operator new(size_t p_size,void *p_pointer,size_t check, const char *p_description) {

	void *failptr=0;
	ERR_FAIL_COND_V( check < p_size , failptr); /** bug, or strange compiler, most likely */

	return p_pointer;
}


