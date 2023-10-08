//
// C++ Interface: iapi_owner
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef IAPI_PERSIST_H
#define IAPI_PERSIST_H

/**
	@author Juan Linietsky <reduzio@gmail.com>
*/

#include "iapi.h"
#include "iobj.h"
#include "iapi_db.h"
#include "os/thread_safe.h"

class IAPI_Persist : public IObjOwner {
	
	enum {
	
		MAX_IAPIREFS=32768 //< MUST BE ALWAYS A POWER OF 2
	};
	
	_THREAD_SAFE_CLASS_;
	
	
	IObjShared iapi_refs[MAX_IAPIREFS];
		
	struct TypeInfo {
	
		String supplies;
		bool singleton;
		IAPI::MethodInfo instance_hints;
		IAPI_InstanceFunc instance_func;
	};
	
	Table<String,TypeInfo> type_info_table;
	Table<String,IObjShared*> singleton_cache;
	
	IAPI_DB *db;
	
	static IAPI_Persist *singleton;
		
	virtual void _lock(); ///< global lock
	virtual void _unlock(); ///< global lock
	
	virtual IObjShared *allocate_ishared( IObj *p_iobj );
	virtual void deallocate_ishared( IObjShared *p_iobj );
	
public:

	static IAPI_Persist *get_singleton();

	void load_db();
	
	void get_registered_types(List<String> *p_list);
	void register_type(String p_type,IAPI_InstanceFunc p_instance_func,bool p_singleton,const IAPI::MethodInfo& p_instance_hints=IAPI::MethodInfo(),String p_type_supply="");
	
	IAPIRef instance_type(String p_type,const IAPI::CreationParams& p_params=IAPI::CreationParams());
	
	IAPI::MethodInfo get_creation_param_hints(String p_type);
	
	IAPIRef load(String p_path,IRef<IAPI> p_instance=IRef<IAPI>()); // helper that looks up the ID in the database
	IAPIRef load(ObjID p_id,IRef<IAPI> p_instance=IRef<IAPI>());
	Error save(IAPIRef p_instance,String p_path="");

	void get_suppliers( String p_type, List<String>* r_list);
	
	void set_db(IAPI_DB* p_database);
	IAPI_DB* get_db();
	
	IAPI_Persist(IAPI_DB *p_database=NULL);
	~IAPI_Persist();

};

#define IAPI_REGISTER_TYPE(m_type) IAPI_Persist::get_singleton()->register_type(#m_type,&m_type::IAPI_DEFAULT_INSTANCE_FUNC,false)

#define IAPI_REGISTER_TYPE_SUPPLY(m_type,m_supply) IAPI_Persist::get_singleton()->register_type(#m_type,&m_type::IAPI_DEFAULT_INSTANCE_FUNC,false,IAPI::MethodInfo(),#m_supply)

#define IAPI_REGISTER_TYPE_STATIC(m_type) IAPI_Persist::get_singleton()->register_type(#m_type,&m_type::IAPI_DEFAULT_INSTANCE_FUNC,true)

#define IAPI_REGISTER_TYPE_STATIC_SUPPLY(m_type,m_supply) IAPI_Persist::get_singleton()->register_type(#m_type,&m_type::IAPI_DEFAULT_INSTANCE_FUNC,true,IAPI::MethodInfo(),#m_supply)

#endif
