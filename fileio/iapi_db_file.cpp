//
// C++ Implementation: iapi_db_file
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "iapi_db_file.h"
#include "os/dir_access.h"
#include "os/file_access.h"
#include "vector.h"
#include "fileio/deserializer_file.h"
#include "fileio/serializer_file.h"
#include <string.h>

#define HEADER_STRING "CXID"

ObjID IAPI_DB_File::get_new_id() {
	
	return ++max_id;
};

Error IAPI_DB_File::save(IAPIRef p_iapi,String p_path) {

	bool exists=p_iapi->get_ID()!=OBJ_INVALID_ID;
	if (exists) {
		
		ERR_FAIL_COND_V( !id_cache.has(p_iapi->get_ID()), ERR_BUG );
		if (p_path == "") {
			p_path = id_cache[p_iapi->get_ID()];
		};
	};
	
	ERR_FAIL_COND_V(p_path=="",ERR_BUG);

	List<ObjID> dependencies;
	p_iapi->get_dependency_list(&dependencies);

	String type=p_iapi->persist_as();

	FileAccess* f = FileAccess::create();
	ERR_FAIL_COND_V(!f, ERR_BUG);
	
	int id = p_iapi->get_ID();
	if (id == OBJ_INVALID_ID) {
		id = get_new_id();
	};
	
	ERR_FAIL_COND_V( id == OBJ_INVALID_ID, FAILED );
	
	Error err = f->open(db_path+"/"+String::num(id), FileAccess::WRITE);
	if (err != OK) {
		memdelete(f);
		ERR_FAIL_COND_V( err, err );
	};

	const char* header = HEADER_STRING;
	
	f->store_buffer((Uint8*)header, strlen(header));

	// name
	f->store_32(p_path.size() + 1);
	f->store_buffer((Uint8*)p_path.utf8().get_data(), p_path.utf8().length()+1);
	
	// type
	f->store_32(type.size() + 1);
	f->store_buffer((Uint8*)type.utf8().get_data(), type.utf8().length()+1);

	// dependencies	
	f->store_32(dependencies.size());
	List<ObjID>::Iterator* I = dependencies.begin();
	while (I) {
		
		f->store_32(I->get());
		
		I = I->next();
	};

	// creation params
	{
		int creation_pos = f->get_pos();
		f->store_32(0);
		Serializer_File sf(f);
		Error err = p_iapi->serialize_creation_params(&sf);
		if (err != OK) {
			
			memdelete(f);
			ERR_FAIL_COND_V(err != OK, err);
		};
		int pos = f->get_pos();
		int size = (pos - creation_pos) - 4;
		f->seek(creation_pos);
		f->store_32(size);
		f->seek(pos);
	}

	{
		int data_pos = f->get_pos();
		f->store_32(0);
		Serializer_File sf(f);
		Error err = p_iapi->serialize(&sf);
		if (err != OK) {
			
			memdelete(f);
			ERR_FAIL_COND_V(err != OK, err);
		};
		int pos = f->get_pos();
		int size = (pos - data_pos) - 4;
		f->seek(data_pos);
		f->store_32(size);
		f->seek(pos);
	}
	
	name_cache[p_path] = id;
	id_cache[id] = p_path;

	iapi_set_objid( p_iapi, id );

	return OK;
}
Error IAPI_DB_File::load(IAPIRef p_iapi,ObjID p_ID) {
	
	FileAccess *f = FileAccess::create();
		
	Error err = f->open(db_path+"/"+String::num(p_ID),FileAccess::READ);
	if (err != OK) {
		memdelete(f);
		ERR_FAIL_COND_V( err, err );
	};
		
	char hdr[5]={0,0,0,0,0};
	f->get_buffer((Uint8*)&hdr,4);
	if (String(hdr)!=HEADER_STRING) {
							
		f->close();
		memdelete(f);
		ERR_FAIL_COND_V( String(hdr)!=HEADER_STRING, ERR_FILE_UNRECOGNIZED );
	}
		
	// skip the name
	Uint32 len = f->get_32();
	
	if (len==0 || len>0xFFFF) {
			// corrupt file
			
		f->close();
		memdelete(f);
		ERR_FAIL_COND_V( len==0 || len>0xFFFF, ERR_FILE_CORRUPT );
	}
		
	f->seek( f->get_pos() + len );
	
	// skip the type
	len = f->get_32();
	
	if (len==0 || len>0xFFFF) {
			// corrupt file
			
		f->close();
		memdelete(f);
		ERR_FAIL_COND_V( len==0 || len>0xFFFF, ERR_FILE_CORRUPT );
	}
	
	f->seek( f->get_pos() + len );
	
	// skip dependences
	len = f->get_32();
	
	if (len>0xFFFF) {
			// corrupt file
			
		f->close();
		memdelete(f);
		ERR_FAIL_COND_V( len>0xFFFF, ERR_FILE_CORRUPT );
	}
		
	f->seek( f->get_pos() + len*4 ); // each 32 bits
	
	// skip creationparams
	
	len = f->get_32();
	
	if (len==0 || len>0xFFFFFF) {
			// corrupt file
			
		f->close();
		memdelete(f);
		ERR_FAIL_COND_V( len==0 || len>0xFFFFFF, ERR_FILE_CORRUPT );
	}
		
	f->seek( f->get_pos() + len ); // each 32 bits
	
	
	// deserialize 
	
	f->get_32(); // skip blobsize
	
	Deserializer_File fdes(f);
	
	err = p_iapi->deserialize(&fdes);
	
	if (err) {
		
		f->close();
		memdelete(f);
		ERR_FAIL_COND_V( err, ERR_FILE_CORRUPT );
		
	}	
printf("object type is %ls, %i\n", p_iapi->get_type().c_str(), p_ID);
	iapi_set_objid( p_iapi, p_ID );
	f->close();
	memdelete(f);
	return OK;	
}

Error IAPI_DB_File::get_type(ObjID p_ID, String &p_type) {
	
	FileAccess *f = FileAccess::create();
		
	Error err = f->open(db_path+"/"+String::num(p_ID),FileAccess::READ);
	if (err) {
		printf("Can't open p_ID %i\n",p_ID);
	}
	ERR_FAIL_COND_V( err, err );
		
	char hdr[5]={0,0,0,0,0};
	f->get_buffer((Uint8*)&hdr,4);
	if (String(hdr)!="CXID") {
							
		f->close();
		memdelete(f);
		ERR_FAIL_COND_V( String(hdr)!="CXID", ERR_FILE_UNRECOGNIZED );
	}
		
	// skip the name
	Uint32 len = f->get_32();
	
	if (len==0 || len>0xFFFF) {
			// corrupt file
			
		f->close();
		memdelete(f);
		ERR_FAIL_COND_V( len==0 || len>0xFFFF, ERR_FILE_CORRUPT );
	}
		
	f->seek( f->get_pos() + len );
	
	// get type
	len = f->get_32();
	
	if (len==0 || len>0xFFFF) {
			// corrupt file
			
		f->close();
		memdelete(f);
		ERR_FAIL_COND_V( len==0 || len>0xFFFF, ERR_FILE_CORRUPT );
	}
	
	Vector<char> tmpbuf;
	tmpbuf.resize( len );
	f->get_buffer( (Uint8*)&tmpbuf[0], len );
		
	if( tmpbuf[len-1]!=0 ) {
			 // prevent possible buffer overrun when parsing string
		f->close();
		memdelete(f);
		ERR_FAIL_COND_V( tmpbuf[len-1]!=0,ERR_FILE_CORRUPT );
			
	}
	
	String type;
	type.parse_utf8((const char*)&tmpbuf[0]);
		
	f->close();
	memdelete(f);
	
	p_type=type;
	
	return OK;
}

ObjID IAPI_DB_File::get_ID(String p_path) {
	
	if (name_cache.has(p_path))
		return name_cache[p_path];
	
	return OBJ_INVALID_ID;
}

String IAPI_DB_File::get_path(ObjID p_ID) {
	
	if (id_cache.has(p_ID))
		return id_cache[p_ID];
	
	return "";
}
Error IAPI_DB_File::set_path(ObjID p_ID,String p_path) {
	
	ERR_FAIL_V( ERR_UNAVAILABLE );	
}
Error IAPI_DB_File::get_creation_params(ObjID p_ID,IAPI::CreationParams &p_params) {
	
					
	FileAccess *f = FileAccess::create();
		
	Error err = f->open(db_path+"/"+String::num(p_ID),FileAccess::READ);
	ERR_FAIL_COND_V( err, err );
		
	char hdr[5]={0,0,0,0,0};
	f->get_buffer((Uint8*)&hdr,4);
	if (String(hdr)!="CXID") {
							
		f->close();
		memdelete(f);
		ERR_FAIL_COND_V( String(hdr)!="CXID", ERR_FILE_UNRECOGNIZED );
	}
		
	// skip the name
	Uint32 len = f->get_32();
	
//	printf("%i name len %i\n",p_ID,len);
	if (len==0 || len>0xFFFF) {
			// corrupt file
			
		f->close();
		memdelete(f);
		ERR_FAIL_COND_V( len==0 || len>0xFFFF, ERR_FILE_CORRUPT );
	}
		
	f->seek( f->get_pos() + len );
	
	// do it again with the type
	len = f->get_32();
	//printf("%i type len %i\n",p_ID,len);
	
	if (len==0 || len>0xFFFF) {
			// corrupt file
			
		f->close();
		memdelete(f);
		ERR_FAIL_COND_V( len==0 || len>0xFFFF, ERR_FILE_CORRUPT );
	}
		
	f->seek( f->get_pos() + len );
	
	// do it again with the dependencies
	len = f->get_32();
	//printf("%i dependency len %i\n",p_ID,len);
	
	
	if (len>0xFFFF) {
			// corrupt file
			
		f->close();
		memdelete(f);
		ERR_FAIL_COND_V( len>0xFFFF, ERR_FILE_CORRUPT );
	}
		
	f->seek( f->get_pos() + len*4 ); // each 32 bits
	
	// to finally arrive creationparams
	f->get_32(); // ignore size
	Deserializer_File fdes(f);
	
	err = IAPI::deserialize_creation_params( &fdes, p_params );
	
	if (err) {
		
		f->close();
		memdelete(f);
		ERR_FAIL_COND_V( err, ERR_FILE_CORRUPT );
		
	}
	
	f->close();
	memdelete(f);
	return OK;
	
}

class ComparerType {

public:

	inline bool operator()(const IAPI_DB::ListingElement& p_l, const IAPI_DB::ListingElement& p_r) const {
		return p_l.type < p_r.type;
	};
};

class ComparerName {

public:

	inline bool operator()(const IAPI_DB::ListingElement& p_l, const IAPI_DB::ListingElement& p_r) const {
		return p_l.path < p_r.path;
	};
};

	
Error IAPI_DB_File::get_listing(List<ListingElement>* p_listing, ListingMode p_mode,String p_string,bool p_exact_match) {

	const int* I = NULL;
	while ( (I = id_cache.next(I)) ) {
		
		switch (p_mode) {
			
			case LISTING_PATH: {
		
				if (p_string == "" || id_cache[*I] == p_string) {
					ListingElement el;
					el.id = *I;
					el.path = id_cache[*I];
					String type;
					get_type(*I, type);
					el.type = type;
					p_listing->push_back(el);
				};
			} break;
			
			case LISTING_TYPE: {
				
				String type;
				get_type(*I, type);
				if (type == "") break;
				if (p_string == "" || type == p_string) {
					ListingElement el;
					el.id = *I;
					el.path = id_cache[*I];
					el.type = type;
					p_listing->push_back(el);
				};
				
			} break;
			default: break;
		};
	};

	ComparerName c;
	p_listing->sort(c);
	
	return OK;	
}


IAPI_DB_File::IAPI_DB_File(String p_path) {
	
	db_path=p_path;
	
	DirAccess * d = DirAccess::create();
printf("path is %ls\n", p_path.c_str());	
	bool err = d->change_dir( p_path );
	ERR_FAIL_COND( err );
	
	d->list_dir_begin();
	
	Vector<char> tmpbuf;
	bool isdir;
	max_id = -1;
	while( true ) {
		
		String id = d->get_next(&isdir);
		
		if (!id.length())
			break; // end
			
		if (isdir)
			continue;
		if (String::num( id.to_int() )!=id)
			continue; // not a number
				
		FileAccess *f = FileAccess::create();
		
		String testfile=d->get_current_dir()+"/"+id;
		Error err_open = f->open(testfile,FileAccess::READ);
		
		if (err_open) {
							
			f->close();
			memdelete(f);
			ERR_CONTINUE( err_open );
		}
		
		//printf("testfile %s\n",testfile.ascii().get_data());
		
		
		char hdr[5]={0,0,0,0,0};
		f->get_buffer((Uint8*)&hdr,4);
		if (String(hdr)!="CXID") {
							
			f->close();
			memdelete(f);
			ERR_CONTINUE( String(hdr)!="CXID" );
		}
		
		Uint32 len = f->get_32();
		
		if (len==0 || len>0xFFFF) {
			// corrupt file
			
			f->close();
			memdelete(f);
			ERR_CONTINUE( len==0 || len>0xFFFF );
		}
		
		tmpbuf.resize( len );
		f->get_buffer( (Uint8*)&tmpbuf[0], len );
		
		if( tmpbuf[len-1]!=0 ) {
			 // prevent possible buffer overrun when parsing string
			f->close();
			memdelete(f);
			ERR_CONTINUE( tmpbuf[len-1]!=0 );
			
		}
		   
		String name;
		name.parse_utf8( (const char*)&tmpbuf[0 ] );
		int idint = id.to_int();
		name_cache[name]=idint;
		id_cache[idint] = name;

		if (idint > max_id) {
			max_id = idint;
		};
		
		f->close();
		memdelete(f);

	}
	
	d->list_dir_end();
		
	memdelete(d);
}


IAPI_DB_File::~IAPI_DB_File()
{
}

