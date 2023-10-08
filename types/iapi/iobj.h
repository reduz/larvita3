//
// C++ Interface: iobj
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef IOBJ_H
#define IOBJ_H

#include "rstring.h"
#include "error_list.h"
#include "signals/signals.h"
#include <stdio.h>
/**
	@author Juan Linietsky <reduzio@gmail.com>
*/
class Serializer;
class Deserializer;

#define IOBJ_TYPE( m_type , m_parent_type )\
	public:\
	static String get_type_static() { return String(#m_type); }	\
	virtual String get_type() const { return String(#m_type); }	\
	virtual bool is_type(String p_type) const {  return (p_type==__STR(m_type)?true:m_parent_type::is_type(p_type)); }	\
	private:

class IObj;
struct IObjShared {
	
	IObj *obj;
	int refcount;	
	int check;
	bool needs_lock;
};

SCRIPT_BIND_BEGIN;

#define OBJ_INVALID_ID -1
typedef int ObjID;

class IObj : public SignalTarget {
friend class IRefBase;
	IObjShared *_ishared;
public:
	
	static String get_type_static() { return String("IObj"); }
	virtual String get_type() const; ///< get object type
	virtual bool is_type(String p_type) const; ///< get object type
	virtual String persist_as() const; ///< inheritance level to persist as
	
	virtual Error serialize( Serializer *p_serializer,unsigned int p_usage) const=0;
	virtual Error deserialize( Deserializer *p_deserializer )=0;
		
	virtual void init(); ///< called when the object becomes an iref
	virtual void predelete();  ///< Called before being destroyed
	
	virtual ObjID get_ID() const=0;
	
	IObj();
	virtual ~IObj();

};

SCRIPT_BIND_END;


class IObjOwner {
	
	static IObjOwner *singleton;
	
protected:	
	virtual void _lock()=0; ///< global lock
	virtual void _unlock()=0; ///< global lock
	
friend class IRefBase;	
	virtual IObjShared *allocate_ishared( IObj *p_iobj )=0;
	virtual void deallocate_ishared( IObjShared *p_iobj )=0;
public:
	
	static IObjOwner *get_singleton();
	
	static void lock();
	static void unlock();
		
	IObjOwner();
	virtual ~IObjOwner();
};

#endif
