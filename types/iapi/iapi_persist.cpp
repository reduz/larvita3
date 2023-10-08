//
// C++ Implementation: iapi_owner
//
// Description:
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "iapi_persist.h"
#include "stdio.h"

#include "global_vars.h"
#include "fileio/sql_client_sqlite.h"
#include "fileio/sql_client_postgres.h"
#include "fileio/iapi_db_file.h"

#include "fileio/iapi_db_sql.h"

IAPI_Persist *IAPI_Persist::singleton=NULL;

IAPI_Persist *IAPI_Persist::get_singleton() {

	return IAPI_Persist::singleton;
}

void IAPI_Persist::_lock() {

	_THREAD_SAFE_LOCK_;
}
void IAPI_Persist::_unlock() {

	_THREAD_SAFE_UNLOCK_;

}

IObjShared *IAPI_Persist::allocate_ishared( IObj *p_iobj ) {

	ERR_FAIL_COND_V( !p_iobj->is_type("IAPI"), NULL );
	static int last_allocated=0;

	for (int i=0;i<MAX_IAPIREFS;i++) {

		int idx=(i+last_allocated)&(MAX_IAPIREFS-1);
		if (iapi_refs[idx].obj==NULL) {

			iapi_refs[idx].obj=p_iobj;
			iapi_refs[idx].check++;
			iapi_refs[idx].refcount=0;
			iapi_refs[idx].needs_lock=false;
			last_allocated=idx;
			return &iapi_refs[idx];
		}
	}

	ERR_PRINT("No more iapi slots left.");
	return NULL;
}
void IAPI_Persist::deallocate_ishared( IObjShared *p_iobj ) {

	ERR_FAIL_COND( p_iobj<&iapi_refs[0] || p_iobj>&iapi_refs[MAX_IAPIREFS-1]);
	ERR_FAIL_COND( p_iobj->refcount != 0 );

	if (p_iobj->obj->get_ID()!=OBJ_INVALID_ID) {
		// came from database, let's see if it's cached
		String cache_str=p_iobj->obj->persist_as()+":"+String::num(p_iobj->obj->get_ID());

		if (singleton_cache.has(cache_str)) {

			singleton_cache.erase(cache_str);
		}
	}

	p_iobj->obj=NULL;
	++p_iobj->check;
}

void IAPI_Persist::register_type(String p_type,IAPI_InstanceFunc p_instance_hint,bool p_singleton,const IAPI::MethodInfo& p_instance_hints,String p_type_supply) {

	_THREAD_SAFE_METHOD_
	ERR_FAIL_COND(type_info_table.has(p_type));

	type_info_table[p_type]=TypeInfo();
	TypeInfo &tinfo=type_info_table[p_type];

	tinfo.instance_func=p_instance_hint;
	tinfo.singleton=p_singleton;
	tinfo.instance_hints=p_instance_hints;
	tinfo.supplies=p_type_supply;
}

IAPIRef IAPI_Persist::instance_type(String p_type,const IAPI::CreationParams& p_params) {

	_THREAD_SAFE_METHOD_
	ERR_FAIL_COND_V(!type_info_table.has(p_type),IAPIRef());
	TypeInfo &tinfo=type_info_table[p_type];

	return tinfo.instance_func(p_type,p_params);
}

IAPI::MethodInfo IAPI_Persist::get_creation_param_hints(String p_type) {

	_THREAD_SAFE_METHOD_
	ERR_FAIL_COND_V(!type_info_table.has(p_type), IAPI::MethodInfo());
	TypeInfo &tinfo=type_info_table[p_type];

	return tinfo.instance_hints;
};

IAPIRef IAPI_Persist::load(ObjID p_id,IRef<IAPI> p_instance) {

	ERR_FAIL_COND_V(!db,IAPIRef());
	_THREAD_SAFE_METHOD_

	IAPIRef res;

	String type;
	Error err = db->get_type(p_id,type);
	ERR_FAIL_COND_V(err!=OK,res);
	ERR_FAIL_COND_V(!type_info_table.has(type),res);

	bool is_singleton=false;

	if (!p_instance.is_null()) {
		ERR_FAIL_COND_V( !p_instance->is_type(type), IAPIRef() );
		res=p_instance;
	} else {

		is_singleton=type_info_table[type].singleton;

		/* Check if this is already loaded first */
		if (is_singleton) {

			String cache_str=type+":"+String::num(p_id);

			if (singleton_cache.has(cache_str)) {

				return IAPIRef( singleton_cache[cache_str] );
			}
		}

		/* Create the IAPIRef using the creation params from the database */


		IAPI::CreationParams method_params;
		err = db->get_creation_params(p_id,method_params);
		ERR_FAIL_COND_V(err!=OK,res);

		res = IAPIRef( type_info_table[type].instance_func(type,method_params) );
		ERR_FAIL_COND_V( res.is_null(), IAPIRef() ); // bug check
	}
	/* Load the serialized data from the database */

	err = db->load(res,p_id);
	ERR_FAIL_COND_V(err!=OK,IAPIRef());

	/* cache this if it is a singleton */
	if (is_singleton) {

		String cache_str=type+":"+String::num(p_id);

		ERR_FAIL_COND_V(singleton_cache.has(cache_str),IAPIRef()); // bug check

		singleton_cache[cache_str]=res.get_ishared();
	}

	return res;
}
IAPIRef IAPI_Persist::load(String p_path,IRef<IAPI> p_instance) {

	ERR_FAIL_COND_V(!db,IAPIRef());

	_THREAD_SAFE_METHOD_
	ObjID id = db->get_ID(p_path);
	return load(id,p_instance);
}

Error IAPI_Persist::save(IAPIRef p_instance,String p_path) {

	ERR_FAIL_COND_V(!db,ERR_UNCONFIGURED);

//	ObjID id=p_instance->get_ID();

	Error err=db->save(p_instance,p_path);
	return err;
}

void IAPI_Persist::set_db(IAPI_DB* p_db) {

	db = p_db;
};

IAPI_DB* IAPI_Persist::get_db() {

	return db;
};

void IAPI_Persist::load_db() {
	
	GlobalVars* global_variables = GlobalVars::get_singleton();
	
	String type = global_variables->get("iapi_db_type").get_string();
	printf("db type is %ls\n", type.c_str());

	SQL_Client *sql_client=NULL;
	IAPI_DB *iapi_db=NULL;

	if (type=="sqlite") {
		printf("using sqlite\n");
		String sqlite_path = global_variables->translate_path("res://"+global_variables->get("sqlite_db_file").get_string());
		sql_client=memnew( SQL_Client_SQLite( sqlite_path ) );
		iapi_db = memnew( IAPI_DB_SQL( sql_client ) );

		set_db(iapi_db);
	};

	#ifdef POSTGRES_ENABLED	
	if (type == "postgres") {
		
		printf("using postgres\n");
		String db_info = global_variables->get("postgres_db_info");
		sql_client = memnew( SQL_Client_Postgres( db_info ) );
		iapi_db = memnew( IAPI_DB_SQL( sql_client ) );
		
		set_db(iapi_db);
	};
	#endif

	if (type == "local") {

		printf("using local\n");
		String db_path = global_variables->get("local_db_path");
		iapi_db = memnew( IAPI_DB_File( "res://"+db_path ) );
	
		set_db(iapi_db);
	};
	/* init sound */
};

void IAPI_Persist::get_registered_types(List<String> *p_list) {

	p_list->clear();
	const String *k=NULL;

	while( (k=type_info_table.next(k)) ) {

		p_list->push_back(*k);
	}

	p_list->sort();
}

void IAPI_Persist::get_suppliers( String p_type, List<String>* p_list) {

	p_list->clear();
	const String *k=NULL;

	while( (k=type_info_table.next(k)) ) {

		if (*k==p_type || type_info_table[*k].supplies==p_type) {

			p_list->push_back(*k);
		}
	}


}



IAPI_Persist::IAPI_Persist(IAPI_DB *p_database) {


	ERR_FAIL_COND(IAPI_Persist::singleton && IAPI_Persist::singleton!=this);

	db=p_database;

	for (int i=0;i<MAX_IAPIREFS;i++) {

		iapi_refs[i].check=0;
		iapi_refs[i].refcount=0;
		iapi_refs[i].obj=NULL;
	}
	singleton=this;

}


IAPI_Persist::~IAPI_Persist() {

	bool critical_safety_check=true;

	for (int i=0;i<MAX_IAPIREFS;i++) {

		if (iapi_refs[i].obj!=NULL) {

			String error_str="IAPIRef still alive: ";
			error_str += iapi_refs[i].obj->get_type() + ":" + String::num( iapi_refs[i].obj->get_ID() );
			error_str+="\n";
			ERR_PRINT( error_str.ascii().get_data() );
			critical_safety_check=false;
		}
	}

	if (!critical_safety_check) {
		ERR_PRINT("*CRITICAL* IAPI_Persist uninitialized before all IAPIRefs were destroyed! *CRITICAL*");
	}

}


