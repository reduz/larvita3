//
// C++ Interface: dvector
//
// Description:
//
//
// Author: Juan Linietsky <reduz@codenix.com>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef DVECTOR_H
#define DVECTOR_H

/**
 * @class DVector
 * @author Juan Linietsky
 * Vector container. It works almost like Vector class, except it stores data in
 * dynamic memory instead of static memory. This makes it ideal for storing very large
 * amounts of data easily. due to it's nature, operator[] only works for retrieving, not reading.
 * To operate quickly with the vector, use lock/unlock and operator(), example:
 *
 * myvector.lock();
 *
 * for (int i=0;i<5;i++)
 *    myvector(i)=somevar;
 *
 * myvector.unlock();
*/
#include "os/memory.h"
#include "error_macros.h"

template<class T>
class DVector {

	
	struct Header {
		
		int size;
		int refcount;
	};
	
	MID mem;
	void copy_on_write();
	void reference(const DVector& p_from);
	void dereference();
	

public:

	class WriteLocker {

		DVector<T>* vec;
	public:
		WriteLocker(DVector<T>& p_vec) {
			vec = &p_vec;
			vec->write_lock();
		};
		~WriteLocker() {
			vec->write_unlock();
		};
	};

	class ReadLocker {

		DVector<T>* vec;
	public:
		ReadLocker(DVector<T>& p_vec) {
			vec = &p_vec;
			vec->read_lock();
		};
		~ReadLocker() {
			vec->read_unlock();
		};
	};


	void clear() { resize(0); }
	int size() const;
	Error resize(int p_size);
	bool push_back(T p_elem);
	const T operator[](int p_index) const;

	void remove(int p_index);

	int find(const T& p_val) const;

	void set(int p_index,T p_elem);
	T get(int p_index) const;

	inline bool is_locked() const { return Memory::is_locked(mem); }
	void read_lock() const;
	const T * read() const {
	
		if (mem.is_null())
			return NULL;
		unsigned char *mem_ptr=(unsigned char *)Memory::get( mem );
		mem_ptr+=sizeof(Header);
		return (const T*)mem_ptr;		
	}
	void read_unlock() const;
	
	void write_lock();
	T * write() {
	
		if (mem.is_null())
			return NULL;
		
		unsigned char *mem_ptr=(unsigned char *)Memory::get( mem );
		mem_ptr+=sizeof(Header);
		return (T*)mem_ptr;		
	}
	void write_unlock();
	
	/*
	inline T& operator()(int p_index) { ///< fast access to elements, only by locking
		ERR_FAIL_COND_V( !Memory::is_locked(mem ),*((T*)0) );
		copy_on_write();
		
		unsigned char *mem_ptr=(unsigned char *)Memory::get( mem );
		Header * header=(Header*)mem_ptr;
		ERR_FAIL_INDEX_V(p_index,header->size,*((T*)0));
		mem_ptr+=sizeof(Header);
		T* array_ptr = (T*)mem_ptr;
		return array_ptr[p_index];
	}

	inline const T& operator()(int p_index) const { ///< fast access to elements, only by locking, readonly!

		ERR_FAIL_COND_V( !Memory::is_locked(mem ),*((T*)0) );
		
		unsigned char *mem_ptr=(unsigned char *)Memory::get( mem );
		Header * header=(Header*)mem_ptr;
		ERR_FAIL_INDEX_V(p_index,header->size,*((T*)0));
		mem_ptr+=sizeof(Header);
		T* array_ptr = (T*)mem_ptr;
		return array_ptr[p_index];
	}
	*/
	void operator=(const DVector& p_from);
	DVector(const DVector& p_from);

	DVector();
	~DVector();

};

template<class T>
void DVector<T>::copy_on_write() {


	if (mem.is_null())
		return;
	
	Error err = Memory::lock( mem );
	ERR_FAIL_COND( err );
	Header *header = (Header*)Memory::get( mem );
	if (header->refcount>1) {
		//printf("refcount is %i, must cow\n",header->refcount);
			
		MID mem_new=Memory::alloc_dynamic( sizeof(Header)+header->size*sizeof(T), "DVector::copy_on_write()");
		Memory::lock( mem_new );
		Header *header_new = (Header*)Memory::get( mem_new );
		header_new->refcount=1;
		header_new->size=header->size;
		
		T * elems_new = (T*)(header_new+1);
		T * elems = (T*)(header+1);
		
		for (int i=0;i<header->size;i++) {
		
			memnew_placement(&elems_new[i], T(elems[i]) );
		}
		
		header->refcount--; //dereference
		Memory::unlock(mem);
		Memory::unlock(mem_new);
		mem=mem_new;
		
	} else {
	
		Memory::unlock( mem );
	}
		
}


template<class T>
void DVector<T>::dereference() {

	if (mem.is_null())
		return;
	
	Error err = Memory::lock( mem );
	ERR_FAIL_COND( err );
	Header *header = (Header*)Memory::get( mem );
	header->refcount--;
	bool deallocate=header->refcount<=0;
	Memory::unlock( mem );
	
	if (deallocate) {
		resize(0);
	}	
	
	mem = MID();
	
}

template<class T>
void DVector<T>::reference(const DVector& p_from) {

	if (this==&p_from)
		return;
	
	dereference();
	
	mem=p_from.mem;
	
	if (mem.is_null())
		return;
	
	Error err = Memory::lock( mem );
	ERR_FAIL_COND( err );
	Header *header = (Header*)Memory::get( mem );
	header->refcount++;
	Memory::unlock( mem );

}

template<class T>
int DVector<T>::size() const {

	if (mem.is_null())
		return 0;
	
	int sz=0;
	Memory::lock( mem );
	Header *header = (Header*)Memory::get( mem );
	sz=header->size;
	Memory::unlock(mem);			
	
	return sz;
}

template<class T>
int DVector<T>::find(const T& p_val) const {

	int ret = -1;
	int element_count=size();
	if (element_count == 0) 
		return ret;
	read_lock();
	const T * data= read();
	for (int i=0; i<element_count; i++) {
		if (data[i] == p_val) {
			ret = i;
			break;
		};
	};
	read_unlock();

	return ret;
};

template<class T>
Error DVector<T>::resize(int p_size) {

	ERR_FAIL_COND_V(p_size<0,ERR_INVALID_PARAMETER);
				
	int localsize=size();
	if (p_size==localsize)
		return OK;
	
	copy_on_write();
	
	if (p_size>localsize) {
		

		if (localsize==0) {
			/* Create */
			mem = Memory::alloc_dynamic(sizeof(Header)+p_size*sizeof(T),"DVector::resize()",0);			
			ERR_FAIL_COND_V( mem.is_null() ,ERR_OUT_OF_MEMORY);
			Error err = Memory::lock( mem );
			ERR_FAIL_COND_V(err, err);
			Header *header = (Header*)Memory::get( mem );
			header->refcount=1;
			header->size=0;
			Memory::unlock(mem);			
		} else {
			ERR_FAIL_COND_V( Memory::resize_dynamic(mem,sizeof(Header)+p_size*sizeof(T)) ,ERR_OUT_OF_MEMORY);
		}

		Error err = Memory::lock( mem );
		ERR_FAIL_COND_V( err, err );

		unsigned char *mem_ptr=(unsigned char *)Memory::get( mem );
		Header * header=(Header*)mem_ptr;
		mem_ptr+=sizeof(Header);
		
		for (int i=localsize;i<p_size;i++) {
			
			memnew_placement(&mem_ptr[sizeof(T)*i],T);
		}
		
		header->size=p_size;
		Memory::unlock( mem );

	} else if (p_size<localsize) {

		Error err = Memory::lock( mem );
		ERR_FAIL_COND_V( err, err );

		unsigned char *mem_ptr=(unsigned char *)Memory::get( mem );
		Header * header=(Header*)mem_ptr;
		mem_ptr+=sizeof(Header);
		
		for (int i=p_size;i<localsize;i++) {

			T *elem=(T*)&mem_ptr[sizeof(T)*i];
			elem->~T();
		}
			
		header->size=p_size;
		
		Memory::unlock( mem );

		if (p_size>0) {

			Error err = Memory::resize_dynamic(mem,sizeof(Header)+p_size*sizeof(T));
			ERR_FAIL_COND_V( err, err );
		} else {
			Memory::free_dynamic( mem );
			mem=MID();
		}
	}

	return OK;
}

template<class T>
void DVector<T>::set(int p_index,T p_elem) {

	ERR_FAIL_INDEX(p_index,size());
	ERR_FAIL_COND( Memory::is_locked(mem));
	
	copy_on_write();
	
	Error err = Memory::lock(mem);
	ERR_FAIL_COND( err );
	
	unsigned char *mem_ptr=(unsigned char *)Memory::get( mem );
//	Header * header=(Header*)mem_ptr;
	mem_ptr+=sizeof(Header);
	T* array_ptr = (T*)mem_ptr;
	array_ptr[p_index]=p_elem;
	
	Memory::unlock(mem);
	

}

template<class T>
T DVector<T>::get(int p_index) const {

	T ret;
	
	ERR_FAIL_INDEX_V(p_index,size(),ret);
	ERR_FAIL_COND_V( Memory::is_locked(mem),ret);
	
	Error err = Memory::lock(mem);
	ERR_FAIL_COND_V( err, ret );
	
	unsigned char *mem_ptr=(unsigned char *)Memory::get( mem );
//	Header * header=(Header*)mem_ptr;
	mem_ptr+=sizeof(Header);
	T* array_ptr = (T*)mem_ptr;
	ret = array_ptr[p_index];
	
	Memory::unlock(mem);
	
	return ret;

}

template<class T>
bool DVector<T>::push_back(T p_elem) {

	resize(size()+1);
	set(size()-1,p_elem);
	
	return false;
}

template<class T>
const T DVector<T>::operator[](int p_index) const {

	return get(p_index);
}

template<class T>
void DVector<T>::remove(int p_index) {

	ERR_FAIL_INDEX(p_index, size());
	int localsize=size();
	for (int i=p_index; i<localsize-1; i++) {

		set(i, get(i+1));
	};

	resize(localsize-1);
};

template<class T>
void DVector<T>::read_lock() const {

	if (mem.is_null())
		return;
	DVector<T> *_unconst_this=(DVector<T>*)this;
	
	Error err = Memory::lock( _unconst_this->mem );
	ERR_FAIL_COND( err );

}

template<class T>
void DVector<T>::read_unlock() const {

	if (mem.is_null())
		return;
	
	DVector<T> *_unconst_this=(DVector<T>*)this;
	
	Memory::unlock( _unconst_this->mem );

}

template<class T>
void DVector<T>::write_lock() {

	if (mem.is_null())
		return;

	copy_on_write();
	
	Error err = Memory::lock( mem );
	ERR_FAIL_COND( err );

}

template<class T>
void DVector<T>::write_unlock() {

	if (mem.is_null())
		return;
		
	Memory::unlock( mem );

}



template<class T>
void DVector<T>::operator=(const DVector& p_from) {

	reference(p_from);
}

template<class T>
DVector<T>::DVector(const DVector& p_from) {

	reference(p_from);
}

template<class T>
DVector<T>::DVector() {


}


template<class T>
DVector<T>::~DVector() {

	dereference();
}


#endif
