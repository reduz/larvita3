#ifndef MEMORY_H
#define MEMORY_H

#include "os/memory_pool_dynamic.h"

/**
 * Static interface for memory access.
 * it is static for ease of use. The actual implementations are
 * derivates from MemoryManager class.
 */


#ifdef DEBUG_MEMORY_ALLOC

#define memalloc(m_size) Memory::alloc_static(m_size, __FILE__":"__STR(__LINE__)", type: "__STR(m_class))
#define memcalloc(m_elem,m_size) Memory::calloc_static(m_elem,m_size, __FILE__":"__STR(__LINE__)", type: "__STR(m_class))
#define memfree(m_size) Memory::free_static(m_size)
#define memrealloc(m_ptr,m_newsize) Memory::realloc_static(m_ptr,m_newsize)

#else

#define memalloc(m_size) Memory::alloc_static(m_size)
#define memcalloc(m_elem,m_size) Memory::calloc_static(m_elem,m_size)
#define memfree(m_size) Memory::free_static(m_size)
#define memrealloc(m_ptr,m_newsize) Memory::realloc_static(m_ptr,m_newsize)
	
#endif

typedef MemoryPoolDynamic_Handle MID;

class Memory {

public:
	enum {
		MAX_MEMORY_POOLS=16,
		DEFAULT_MEMORY_POOL=0,
	};

private:
	static int memory_pools_in_use;
	static MemoryPoolDynamic *memory_pools[MAX_MEMORY_POOLS];

public:
	/* static alloc */
	static void * alloc_static(unsigned int p_bytes,const char *p_alloc_from=""); ///< Alloc a memory chunk, just as libc malloc. A description of the memory can be passed. This kind of memory is recommended for small blocks.
	static void * calloc_static(unsigned int p_elems,unsigned int p_bytes,const char *p_alloc_from=""); ///< Alloc a zeroed memory chunk, just as libc malloc. A description of the memory can be passed. This kind of memory is recommended 	
	static void * realloc_static(void *p_memory,unsigned int p_bytes); ///< Realloc a previously allocated chunk of mem
	static void free_static(void *p_ptr); ///< free a staticly allocated chunk of mem
	static unsigned int get_static_mem_usage(); ///< get usage of static mem, may only work in debug mode

	static MID alloc_dynamic(int p_bytes,const char* p_descr=0,int p_pool_index=DEFAULT_MEMORY_POOL); ///< Allocate a memory block, from the dynamic memory pool. return a Memory ID. This kind of memory is recommended for large objects, since it wont suffer from fragmentation and can be compacted.
	static Error resize_dynamic(MID p_MID,int p_bytes); ///< resize a memory block.

	static void free_dynamic(MID p_MID); ///< Free a memory block

	static Error lock(MID p_MID); ///< Lock region, so get() can happen, return true if can't lock
	static bool is_locked(MID p_MID);
	static void *get(MID p_MID); ///< Get locked region, NULL if not locked, this is the only way to access this memory */
	static void unlock(MID p_MID); ///< Unlock region

	static int get_size(MID p_MID); ///< get size of block
	static bool is_valid(MID p_MID); ///< is MID valid
	static void add_dynamic_pool( MemoryPoolDynamic *p_pool ); ///< add a memory pool
	static unsigned int get_dynamic_mem_usage(); ///< get memory usage by the dynamic memory pool
};


void * operator new(size_t p_size,const char *p_description); ///< operator new that takes a description and uses MemoryStaticPool

void * operator new(size_t p_size,void *p_pointer,size_t check, const char *p_description); ///< operator new that takes a description and uses a pointer to the preallocated memory

#ifdef DEBUG_MEMORY_ALLOC

#define memnew(m_class) (new(__FILE__":"__STR(__LINE__)", type: "__STR(m_class)) m_class)
#define memnew_placement(m_placement,m_class) (new(m_placement,sizeof(m_class),__FILE__":"__STR(__LINE__)", type: "__STR(m_class)) m_class)

#else

#define memnew(m_class) (new("") m_class)
#define memnew_placement(m_placement,m_class) (new(m_placement,sizeof(m_class),"") m_class)

#endif

/**
 * predelete handler handles notifications to objects that they will be deleted.
 * this is useful because of the C++ limitation on destructor call order.
 * It also alows for out-of-order deleting if needed (objects act as if they were
 * deleted, but actually die later).
 * To make a specific type of class handle a predelete, create a predelete_handler
 * function that takes a specific pointer type instead of void*. This also works for
 * inheritance. Function must return false if it does not wish to be deleted.
 */

template<class T>
void memdelete(T *p_class) {
		
	p_class->~T();
	Memory::free_static(p_class);
}

void *operator new[] (size_t,const char *p_description,void **p_real_address);   // for arays

#ifdef DEBUG_MEMORY_ALLOC

#define memnew_arr( m_class, m_count ) memnew_arr_template<m_class>(m_count,__FILE__":"__STR(__LINE__)", type: "_STR(m_class))

#else

#define memnew_arr( m_class, m_count ) memnew_arr_template<m_class>(m_count)

#endif

template<typename T>
T* memnew_arr_template(size_t p_elements,const char *p_descr="") {

	if (p_elements==0)
		return 0;
	/** overloading operator new[] cannot be done , because it may not return the real allocated address (it may pad the 'element count' before the actual array). Because of that, it must be done by hand. This is the
	same strategy used by std::vector, and the DVector class, so it should be safe.*/

	size_t len = sizeof(T) * p_elements;
	unsigned int *mem = (unsigned int*)Memory::alloc_static( len + sizeof(int), p_descr );
	T *failptr=0; //get rid of a warning
	ERR_FAIL_COND_V( !mem, failptr );
	*mem=p_elements;
	mem++;
	T* elems = (T*)mem;

	/* call operator new */
	for (size_t i=0;i<p_elements;i++) {
		new(&elems[i],sizeof(T),p_descr) T;
	}

	return (T*)mem;
}

/**
 * Wonders of having own array functions, you can actually check the length of
 * an allocated-with memnew_arr() array
 */

template<typename T>
size_t memarr_len(const T *p_class) {

	const unsigned int *len = reinterpret_cast<const unsigned int*>(p_class);
	len--;

	return *len;
}

template<typename T>
void memdelete_arr(T *p_class) {


	unsigned int * elems=(unsigned int*)p_class;
	elems--;

	for (unsigned int i=0;i<*elems;i++) {

		p_class[i].~T();
	};
	Memory::free_static(elems);
}


/**
 * Operador new to help allocate into memory regions
 * @param p_size
 * @param p_mid
 * @param p_pool
 * @param p_extra_pad
 * @return
 */

void * operator new(size_t p_size,MID *p_mid,int p_pool=0, int p_extra_pad=0);

#endif


