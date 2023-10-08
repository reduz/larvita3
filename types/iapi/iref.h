//
// C++ Interface: iref
//
// Description:
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef IREF_H
#define IREF_H

#include "iobj.h"

class IRefBase {
protected:
	inline IObjShared * get_iobjshared(const IObj *p_iobj ) const { return p_iobj?p_iobj->_ishared:NULL; }
	inline void set_iobjshared(IObj *p_iobj,IObjShared *p_ishared) const {
		 if (p_iobj->_ishared)
			 return; //shouldnt be able to do that
		 p_iobj->_ishared=p_ishared;
	}
	inline IObjShared * get_ishared(const IRefBase *p_ref) const { return p_ref->get_ishared(); };
	virtual IObjShared * get_ishared() const=0;
	IObjShared *allocate_ishared( IObj *p_iobj );
	void deallocate_ishared( IObjShared *p_ishared );
	
	virtual ~IRefBase() {};
};


#define IOBJ_LOCK\
	{ if (shared && shared->needs_lock) IObjOwner::lock(); }

#define IOBJ_UNLOCK\
	{ if (shared && shared->needs_lock) IObjOwner::unlock(); }

template <class T>
class IRef : public IRefBase {

	struct Shared {
		/* Structure must be EXACTLY like IObjShared */
		T *obj;
		int refcount;
		int check;
		bool needs_lock;
	};


	Shared *shared;

	/** Default constructor
	*
	* Afterwards is_null() will return true and use of -> will cause a segmentation fault.
	*/

	_FORCE_INLINE_ void unref() {

		if (shared) {

			shared->refcount--;
			if (shared->refcount==0) {
				shared->obj->predelete(); // warn about being deleted
				if (shared->refcount==0) {
					// object may have attemoted to avoid being deleted by copying itself somewhere else,
					// but if not.. actually deleye it.
					IObj *obj = shared->obj;
					deallocate_ishared((IObjShared*)shared);
					memdelete(obj); //erase object
				}
			}

			shared=NULL;
		}
	}

	_FORCE_INLINE_ void ref(const Shared *p_shared) {

		IOBJ_LOCK

		if (p_shared==shared) {
			IOBJ_UNLOCK
			return; //nothing to do
		}

		unref();
		if (!p_shared) {
			IOBJ_UNLOCK
			return;
		};
		shared=const_cast<Shared*>(p_shared);
		shared->refcount++;
		IOBJ_UNLOCK

	}

friend class IAPI_Persist;

	IRef(IObjShared *p_shared) {

		shared = (Shared*)p_shared;
		shared->refcount++;
	}

	virtual IObjShared * get_ishared() const {

		return (IObjShared*)shared;
	}

public:

	/**
	 * operator ->
	 * works, as long as is_null() returns true, otherwise it will crash.
	 */

	_FORCE_INLINE_ const T* operator->() const {
		ERR_FAIL_COND_V(!shared,NULL);
		return shared->obj;
	}

	_FORCE_INLINE_ T* operator->() {
		ERR_FAIL_COND_V(!shared,NULL);
		return shared->obj;
	}

	_FORCE_INLINE_ T* ptr() {
		ERR_FAIL_COND_V(!shared,NULL);
		return shared->obj;
	}

	_FORCE_INLINE_ const T* ptr() const {
		ERR_FAIL_COND_V(!shared,NULL);
		return shared->obj;
	}

	T * take() { // take the instance (deallocate the iref) refcount must be 1

		IOBJ_LOCK

		ERR_FAIL_COND_V( !shared, NULL);
		ERR_FAIL_COND_V( shared->refcount!=1, NULL );
		shared->refcount=0; // give it up
		T * obj = shared->obj;
		deallocate_ishared((IObjShared*)shared);
		shared=NULL;
		IOBJ_UNLOCK

		return obj;
	}

	_FORCE_INLINE_ T& operator*() {

		//ERR_FAIL_COND_V(!shared,NULL);
		return *shared->obj;
	}

	_FORCE_INLINE_ const T& operator*() const {
		//ERR_FAIL_COND_V(!shared,NULL);
		return *shared->obj;
	}

	template<class T_Other>
	bool operator==(const IRef<T_Other>& p_rval) const {
		if (p_rval.is_null() && is_null())
			return true;
		if (p_rval.is_null() || is_null())
			return false;

		return get_iobjshared(ptr())==get_iobjshared(p_rval.ptr());
	}

	template<class T_Other>
	bool operator!=(const IRef<T_Other>& p_rval) const {
		if (p_rval.is_null() && is_null())
			return false;
		if (p_rval.is_null() || is_null())
			return true;
		return get_iobjshared(ptr())!=get_iobjshared(p_rval.ptr());
	}

	void swap(IRef<T>& p_other) {

		IOBJ_LOCK
		Shared *const aux = shared;
		shared = p_other.shared;
		p_other.shared=aux;
		IOBJ_UNLOCK
	}

	inline bool is_null() const {
		return shared==NULL;
	}

	inline operator const void*() const {

		return is_null()?NULL:this;
	}
	int get_refcount() const {

		if (shared)
			return shared->refcount;
		return 0;
	}
	void clear() {

		IOBJ_LOCK
		unref();
		IOBJ_UNLOCK
	}
	void create() {

		*this = IRef<T>( memnew( T ) );
	}
	/* assign from higher cast */

	_FORCE_INLINE_ void operator=(const IRef<T>& p_rval) {

		IOBJ_LOCK
		ref( p_rval.shared );
		IOBJ_UNLOCK
	}

	template<class T_Other>
	void operator=(const IRef<T_Other>& p_rval) {

		IOBJ_LOCK

		if (!IRefBase::get_ishared(&p_rval)) {
			unref();
			IOBJ_UNLOCK
			return;
		}

#ifdef NO_SAFE_CAST

		if (!IRefBase::get_ishared(&p_rval)->obj->is_type( T::get_type_static() )) {

			IOBJ_UNLOCK
			ERR_PRINT("Invalid Type Assignment (cast failed)");
			return;
		}

#else
		T *tptr = SAFE_CAST<T*>( IRefBase::get_ishared(&p_rval)->obj );

		if (!tptr) {

			IOBJ_UNLOCK
			ERR_PRINT("Invalid Type Assignment (safe cast failed)");
			return;
		}

#endif
		ref( (Shared*)( IRefBase::get_ishared(&p_rval) ) );

		IOBJ_UNLOCK

	}

	template<class T_Other>
	_FORCE_INLINE_ void cast_static( IRef<T_Other>& p_other) {
		/* USE ONLY WHEN YOU _REALLY_ KNOW WHAT YOU ARE DOING */

		IOBJ_LOCK
		unref();
		if (p_other.is_null()) {
			IOBJ_UNLOCK
			ERR_FAIL_COND( p_other.is_null() );

		}

		shared=(Shared*)get_iobjshared( p_other.ptr() );
		shared->refcount++;
		IOBJ_UNLOCK
	}

	template<class T_Other>
	IRef<T_Other> cast_dynamic() {

		IRef<T_Other> other;

		IOBJ_LOCK
		// act as safeguard if RTTI is enabled
		T_Other *t_oth=SAFE_CAST<T_Other*>( shared->obj );
		if (!t_oth) {

			IOBJ_UNLOCK
			return other;
		}
		other.shared=static_cast< typename IRef<T_Other>::Shared* >(shared);
		other.shared->refcount++;
		IOBJ_UNLOCK

		return other;
	}


	/* constructors */

	IRef(const IRef<T>& p_rval) {

		shared=NULL;
		IOBJ_LOCK
		ref( p_rval.shared );
		IOBJ_UNLOCK
	}

	/* assign from higher cast */
	template<class T_Other>
	IRef(const IRef<T_Other>& p_rval) {

		shared=NULL;
		IOBJ_LOCK
		if (!IRefBase::get_ishared(&p_rval)) {
			IOBJ_UNLOCK
			return;
		}

#ifdef NO_SAFE_CAST

		if (!IRefBase::get_ishared(&p_rval)->obj->is_type( T::get_type_static() )) {

			IOBJ_UNLOCK
			ERR_PRINT("Invalid Type Assignment (cast failed)");
			return;
		}

#else

		T *tptr = SAFE_CAST<T*>( IRefBase::get_ishared(&p_rval)->obj );

		if (!tptr) {

			IOBJ_UNLOCK
			ERR_PRINT("Invalid Type Assignment (safe cast failed)");
			return;
		}

#endif
		ref( (Shared*)( IRefBase::get_ishared(&p_rval) ) );

		IOBJ_UNLOCK

	}
	IRef(T *p_instance) {

		if (p_instance==NULL) {
			shared=NULL;
			return;
		}

		shared=NULL;
		IOBJ_LOCK
		/* multiple inheritance check begin*/
		IObj *base=p_instance;
		if ((void*)base!=(void*)p_instance) {
			IOBJ_UNLOCK
			ERR_PRINT("Multiple Inheritance Detected");
		}
		/* multiple inheritance check end*/

		if (this->get_iobjshared(base)) {
			/* already shared */
			shared=(Shared*)get_iobjshared(p_instance);
			shared->refcount++;
			IOBJ_UNLOCK
		} else {
			shared = (Shared*)allocate_ishared(p_instance);

			if (!shared) {
				IOBJ_UNLOCK
				ERR_PRINT("Can't register object!");
				return;
			}
			shared->refcount=1;
			set_iobjshared(base,(IObjShared*)shared);
			IOBJ_UNLOCK
			p_instance->init();
		}

	}

	String get_type_static() const {

		return T::get_type_static();
	}


	_FORCE_INLINE_ IRef() {
		shared= NULL;
	}

	virtual _FORCE_INLINE_ ~IRef() {

		IOBJ_LOCK
		unref();
		IOBJ_UNLOCK
	}

};

typedef IRef<IObj> IObjRef;


template <class T>
class WeakRef : public IRefBase {

	struct Shared {

		T *obj;
		int refcount;
		int check;
		bool needs_lock;
	};

	Shared *shared;
	int check;
	virtual IObjShared * get_ishared() const { return (shared && shared->check==check)?(IObjShared*)shared:NULL; }

public:

	const T* operator->() const {
		return (shared && check==shared->check)?shared->obj:NULL;
	}

	T* operator->() {
		return (shared && check==shared->check)?shared->obj:NULL;
	}

	T* ptr() {
		return (shared && check==shared->check)?shared->obj:NULL;
	}

	const T* ptr() const {
		return (shared && check==shared->check)?shared->obj:NULL;
	}

	T& operator*() {
		//ERR_FAIL_COND_V(!shared,NULL);
		return (shared && check==shared->check)?*shared->obj:*(T*)NULL;
	}

	operator IRef<IObj>() const {

		return IObjRef( this );
	}
	const T& operator*() const {
		//ERR_FAIL_COND_V(!shared,NULL);
		return (shared && check==shared->check)?*shared->obj:*(T*)NULL;
	}

	bool is_null() const {

		return (!shared || check!=shared->check);
	}

	operator const void *() const {

		return is_null()?NULL:this;

	}

	WeakRef( IObjShared *p_ishared) {

		shared=NULL;
		check=0;
		if (!p_ishared)
			return;
		shared=(Shared*)p_ishared;
	};


	IRef<T> get_iref() const {

		if (shared && check==shared->check)
			return IRef<T>( shared->obj );
		return IRef<T>();
	}

	operator IRef<T>() const {

		return get_iref();
	}



	WeakRef( IObj *p_ishared) {

		shared=NULL;
		check=0;
		if (!p_ishared)
			return;
		shared=(Shared*)p_ishared;
	};

	void operator=( const IRef<T> &p_iref ) {

		shared=NULL;
		check=0;

		IOBJ_LOCK

		if (p_iref.ptr()
				  && this->get_iobjshared(p_iref.ptr())) {
			/* already shared */
			shared=(Shared*)get_iobjshared(p_iref.ptr());
			check=shared->check;
		}
		IOBJ_UNLOCK

	}

	WeakRef( const IRef<T>& p_iref ) {

		shared=NULL;
		check=0;

		IOBJ_LOCK

		if (p_iref.ptr() && this->get_iobjshared(p_iref.ptr())) {
			/* already shared */
			shared=(Shared*)get_iobjshared(p_iref.ptr());
			check=shared->check;
		}
		IOBJ_UNLOCK

	}

	WeakRef( T *p_instance ) {

		shared=NULL;
		check=0;

		IOBJ_LOCK

		if (this->get_iobjshared(p_instance)) {
			/* already shared */
			shared=(Shared*)get_iobjshared(p_instance);
			check=shared->check;
		}
		IOBJ_UNLOCK

	}

	WeakRef() {

		shared=NULL;
		check=0;
	}
	
	virtual ~WeakRef() {};
};


#endif
