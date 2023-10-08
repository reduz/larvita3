//
// C++ Implementation: iapi_db_sql
//
// Description: 
//
//
// Author: ,,, <red@lunatea>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "iapi_db_sql.h"
#include "fileio/serializer_array.h"
#include "fileio/deserializer_buffer.h"


String IAPI_DB_SQL::validate_path_syntax(String p_path) {

	// no trailing/pre edges
	p_path.strip_edges();
	
	// must start with / , must not end with /
	if (p_path.length()) {
		
		if (p_path[0]!='/')
			p_path="/"+p_path;
		
		if (p_path[p_path.length()-1]=='/')
			p_path.erase(p_path.length()-1, 1);
	} else 
		p_path="";
	
	// must not contain double-path slash
		
	while (p_path.find("//")!=-1) {
		
		p_path.replace("//","/");
	}
		
	return p_path;
}

String IAPI_DB_SQL::validate_insert_path(ObjID p_ID,String p_path) {
		
	p_path=validate_path_syntax(p_path);	
	
	if (p_path=="") {
		
		p_path="object_"+String::num(p_ID);
	}

	ObjID id = get_ID(p_path);
	if (id == OBJ_INVALID_ID || id == p_ID) {
		return p_path;
	};
	
	int retry=0;
	while(true) {
		
		/* could definitely optimize */
		
		String insert_path=p_path+((retry>0)?("-"+String::num(retry+1)):"");
		
		id = get_ID(insert_path);
		if (id == OBJ_INVALID_ID) return insert_path;
		
		ERR_FAIL_COND_V(retry>10000,"");
		++retry;
	}

	return "";
}

Error IAPI_DB_SQL::save(IAPIRef p_iapi, String p_path) {
		
	bool exists=p_iapi->get_ID()!=OBJ_INVALID_ID;
	
	
	if (exists) { // double check if the object exists (could be removed )
	
		sql_client->query("SELECT \"id\" FROM \"objects\" WHERE \"id\"='"+String::num(p_iapi->get_ID())+"';");
		ERR_FAIL_COND_V( sql_client->cursor_is_at_end(), ERR_BUG ); // ID doesnt exist, but object has an ID, this is a bug!
		sql_client->cursor_close();
		
		if (p_path == "") {
			
			p_path = get_path(p_iapi->get_ID());
		};
	}
	
	
	// check if path exists

	p_path = validate_insert_path(p_iapi->get_ID(),p_path);

	
	ERR_FAIL_COND_V(p_path=="",ERR_BUG);
	sql_client->transaction_begin();

	List<ObjID> dependencies;
	p_iapi->get_dependency_list(&dependencies);
	
	// proceed to build query
	String type=p_iapi->persist_as();
	
	ERR_FAIL_COND_V( !p_iapi->is_type( type ), ERR_INVALID_PARAMETER ); //< persist_as must be in the level of inheritances
	SerializerArray serializer;
	p_iapi->serialize_creation_params(&serializer);	
	serializer.get_array().read_lock();
	String creation_params_blob=sql_client->make_blob(serializer.get_array().read(),serializer.get_array().size());
	serializer.get_array().read_unlock();
	
	serializer.clear();
	
	p_iapi->serialize(&serializer,IAPI::PropertyInfo::USAGE_DATABASE);
	
	serializer.get_array().read_lock();
	String data_blob=sql_client->make_blob(serializer.get_array().read(),serializer.get_array().size());
	serializer.get_array().read_unlock();

	String query;

	if (exists) {
				
		query="UPDATE \"objects\" SET \"path\"='"+p_path+"',\"type\"='"+type+"',\"creation_params\"="+creation_params_blob+",\"data\"="+data_blob+" WHERE \"id\"='"+String::num(p_iapi->get_ID())+"';";
	} else {
		query="INSERT INTO \"objects\" (\"path\",\"type\",\"creation_params\",\"data\") VALUES('"+p_path+"','"+type+"',"+creation_params_blob+","+data_blob+");";				
	}
	
	Error err = sql_client->query(query);	

	if (err!=OK) {
		sql_client->transaction_cancel();
		ERR_FAIL_COND_V(err!=OK,err);
	}

	if (!exists) {
		
		iapi_set_objid( p_iapi, sql_client->get_last_insert_ID("objects","ID") );
	}
	
	// add_dependencies
	if (exists) {
		query = "delete from dependencies where \"id\"= '"+String::num(p_iapi->get_ID())+"'";

		Error err = sql_client->query(query);	
		if (err!=OK) {
			sql_client->transaction_cancel();
			ERR_FAIL_COND_V(err!=OK,err);
		}
	};
	List<ObjID>::Iterator* I = dependencies.begin();
	while (I) {
		
		query = "insert into \"dependencies\" (\"id\", \"depends_id\") values('"+String::num(p_iapi->get_ID())+"', '"+String::num(I->get())+"')";

		Error err = sql_client->query(query);	
		if (err!=OK) {
			sql_client->transaction_cancel();
			ERR_FAIL_COND_V(err!=OK,err);
		}
		
		I = I->next();
	};
	
	sql_client->transaction_end();
	
	return err;	

}

Error IAPI_DB_SQL::load(IAPIRef p_iapi,ObjID p_ID) {
	
	/* fetch data FIRST */
	Error err = sql_client->query("SELECT \"data\" FROM \"objects\" WHERE \"id\"='"+String::num(p_ID)+"';");
	ERR_FAIL_COND_V(err!=OK,err);
	if (sql_client->cursor_is_at_end())
		return ERR_DOES_NOT_EXIST;
	
	Variant data;
	sql_client->cursor_get_column(0,data);
	sql_client->cursor_close();
	
	/* fetch and preload dependencies */
	
	DVector<Uint8> array=data.get_byte_array();
	array.read_lock();
	DeserializerBuffer deserializer(array.read(),array.size());
	p_iapi->deserialize(&deserializer);
	array.read_unlock();

	iapi_set_objid( p_iapi, p_ID );

	return OK;
}


Error IAPI_DB_SQL::get_type(ObjID p_ID, String &p_type)  {

	Error err = sql_client->query("SELECT \"type\" FROM \"objects\" WHERE \"id\"='"+String::num(p_ID)+"';");
	ERR_FAIL_COND_V(err!=OK,err);
	
	if (sql_client->cursor_is_at_end())
		return ERR_DOES_NOT_EXIST;
		
	Variant v;
	sql_client->cursor_get_column(0,v);
	p_type=v.get_string();

	sql_client->cursor_close();
	
	return OK;

}
String IAPI_DB_SQL::get_path(ObjID p_ID)  {

	Error err = sql_client->query("SELECT \"path\" FROM \"objects\" WHERE \"id\"='"+String::num(p_ID)+"';");
	ERR_FAIL_COND_V(err!=OK,"");
	
	if (sql_client->cursor_is_at_end())
		return "";
		
	Variant v;
	sql_client->cursor_get_column(0,v);
	String path=v.get_string();

	sql_client->cursor_close();
	
	return path;

}

ObjID IAPI_DB_SQL::get_ID(String p_path)  {

	p_path=validate_path_syntax(p_path);
	Error err = sql_client->query("SELECT \"id\" FROM \"objects\" WHERE \"path\"='"+p_path+"';");
	ERR_FAIL_COND_V(err!=OK,OBJ_INVALID_ID);
	
	if (sql_client->cursor_is_at_end())
		return OBJ_INVALID_ID;
		
	Variant v;
	sql_client->cursor_get_column(0,v);
	ObjID id=v.get_int();

	sql_client->cursor_close();
	
	return id;

}

Error IAPI_DB_SQL::set_path(ObjID p_ID,String p_path) {

	p_path=validate_path_syntax(p_path);
	
	/* check existing path */
	
	Error err = sql_client->query("SELECT \"id\" FROM \"objects\" WHERE \"path\"='"+p_path+"';");
	ERR_FAIL_COND_V(err!=OK,err);
	
	if (!sql_client->cursor_is_at_end()) { // in use?
		
		Variant v;
		sql_client->cursor_get_column(0,v);
		ObjID id=v.get_int();
		sql_client->cursor_close();
		
		if (id==p_ID)
			return OK; //renaming to same
		
	} else {
	
		return ERR_DOES_NOT_EXIST;
	
	}
		
	err = sql_client->query("UPDATE \"objects\" SET \"path\"='"+p_path+"' WHERE \"id\"='"+String::num(p_ID)+"';");
	
	ERR_FAIL_COND_V( err != OK, err );
	return err;
}


Error IAPI_DB_SQL::get_creation_params(ObjID p_ID,IAPI::CreationParams &p_params) {
	
	p_params.clear();

	Error err = sql_client->query("SELECT \"creation_params\" FROM \"objects\" WHERE \"id\"='"+String::num(p_ID)+"';");
	ERR_FAIL_COND_V(err!=OK,err);
	if (sql_client->cursor_is_at_end())
		return OK;
	
	Variant data;
	sql_client->cursor_get_column(0,data);
	sql_client->cursor_close();
	
	DVector<Uint8> array = data.get_byte_array();
	array.read_lock();
	DeserializerBuffer deserializer(array.read(),array.size());
	
	String prop=deserializer.get_string();
	while(prop!="") {
		
		p_params[prop]=Variant();
		deserializer.get_variant(p_params[prop]);
		prop=deserializer.get_string();
	}
	
	array.read_unlock();
	
	return OK;

}

Error IAPI_DB_SQL::get_listing(List<ListingElement>* p_listing, ListingMode p_mode,String p_string,bool p_exact_match) {

	String query;
	
	p_listing->clear();

	switch(p_mode) {
	
		case LISTING_PATH: {
		
			if (p_string=="") {
			
				query="SELECT \"id\",\"path\",\"type\" FROM \"objects\" ORDER "
					"BY \"path\";";
			
			} else if (p_exact_match) {			
				
				query="SELECT \"id\",\"path\",\"type\" FROM \"objects\" WHERE "
				   "\"path\"='"+p_string+"' ORDER BY \"path\";";
			} else {
			
				query="SELECT \"id\",\"path\",\"type\" FROM \"objects\" WHERE "
					"\"path\" LIKE '"+p_string+"%' ORDER BY \"path\";";	
			}
		} break;
		case LISTING_TYPE: {
		
			if (p_string=="") {
			
				query="SELECT \"id\",\"path\",\"type\" FROM \"objects\" ORDER "
					"BY \"type\";";
			
			} else if (p_exact_match) {			
				
				query="SELECT \"id\",\"path\",\"type\" FROM \"objects\" WHERE "
					"\"type\"='"+p_string+"' ORDER BY \"path\";";
			} else {
			
				query="SELECT \"id\",\"path\",\"type\" FROM \"objects\" WHERE "
					"\"type\" LIKE '"+p_string+"%' ORDER BY \"path\";";
			}
		} break;
	
	}
		
	sql_client->query(query);

	if (sql_client->cursor_is_at_end())
		return OK;
		
	do {
	
		ListingElement e;
		Variant v;
		sql_client->cursor_get_column(0,v);
		e.id=v.get_int();
		sql_client->cursor_get_column(1,v);
		e.path=v.get_string();
		sql_client->cursor_get_column(2,v);
		e.type=v.get_string();
				
		p_listing->push_back(e);
	
	} while (sql_client->cursor_next());
	
	sql_client->cursor_close();

	return OK;

}


IAPI_DB_SQL::IAPI_DB_SQL(SQL_Client *p_sql_client) { 

	sql_client=p_sql_client;
}


IAPI_DB_SQL::~IAPI_DB_SQL()
{

}

