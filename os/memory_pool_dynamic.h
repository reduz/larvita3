#ifndef MEMORY_POOL_DYNAMIC_H
#define MEMORY_POOL_DYNAMIC_H

#include "typedefs.h"

class MemoryPoolDynamic_Handle {
	
friend class MemoryPoolDynamic;
friend class Memory;

	signed short _pool_index;
	Uint32 _check;
	void *_private;
public:

	inline bool operator==(const MemoryPoolDynamic_Handle& p_rval) const {
		return (_pool_index == p_rval._pool_index) && (_check == p_rval._check) && (_private == p_rval._private);
	};

	MemoryPoolDynamic_Handle(void *p_private,Uint32 p_check) { _private=p_private; _check=p_check; _pool_index=-1; }
	MemoryPoolDynamic_Handle() { _private=NULL; _check=0; _pool_index=-1; }
	bool is_null() const { return _private==0; }
};


class MemoryPoolDynamic {


protected:

	inline void* get_private(MemoryPoolDynamic_Handle &p_handle) { return p_handle._private; }
	inline Uint32 get_check(MemoryPoolDynamic_Handle &p_handle) { return p_handle._check; }
public:


	virtual MemoryPoolDynamic_Handle alloc_mem(Uint32 p_amount, const char *p_descr=0)=0; ///< return empty memhandle if failed
	virtual bool is_mem_valid(MemoryPoolDynamic_Handle p_handle)=0; ///< check if memory is valid, true if valid 
	virtual void free_mem(MemoryPoolDynamic_Handle p_handle)=0;
	virtual Error realloc_mem(MemoryPoolDynamic_Handle p_handle,Uint32 p_new_amount)=0; ///< true on error 
	virtual int get_mem_size(MemoryPoolDynamic_Handle p_handle)=0; ///< return size, -1 if invalid 

	virtual Error lock_mem(MemoryPoolDynamic_Handle p_handle)=0; ///< try to lock the mem, true if locked OK
	virtual void *get_mem(MemoryPoolDynamic_Handle p_handle)=0;
	virtual void unlock_mem(MemoryPoolDynamic_Handle p_handle)=0;
	virtual bool is_locked(MemoryPoolDynamic_Handle p_handle)=0;

	virtual Uint32 get_total_mem_used()=0;

	virtual ~MemoryPoolDynamic() {};

};


#endif





