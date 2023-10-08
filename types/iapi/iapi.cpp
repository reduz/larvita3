//
// C++ Implementation: iapi_base
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "iapi.h"
#include "fileio/serializer.h"
#include "fileio/deserializer.h"


Variant IAPI::call(String p_method, const List<Variant>& p_params) {
	
	return Variant();
}

Error IAPI::serialize( Serializer *p_serializer,unsigned int p_usage ) const {
	
	
	List<PropertyInfo> plist;
	get_property_list(&plist);
	List<PropertyInfo>::Iterator *I=plist.begin();
		
	p_serializer->add_string(persist_as()); // save type, for checking
	
	for(;I;I=I->next()) {
		
		String path = I->get().path;
		ERR_CONTINUE( path=="" );
		if (I->get().usage_flags&p_usage) {

			Variant v = get(path);
			ERR_CONTINUE( v.get_type() == Variant::NIL );
			p_serializer->add_string( path );
			p_serializer->add_variant( v ); 
		}
	}
	
	p_serializer->add_string(""); // empty string at the end

	return OK;
}

Error IAPI::deserialize( Deserializer *p_deserializer ) {
	
	String t = p_deserializer->get_string();
//printf("************** type is %ls\n", t.c_str());
	//ERR_FAIL_COND_V( !is_type( t ), FAILED ); // or should be get_type() ?
	
	String path = p_deserializer->get_string();
	
	while (path!="") {

		Variant v;
		p_deserializer->get_variant(v);
		set(path,v);
		path = p_deserializer->get_string();		
		
	}
	
	return OK;
}

Error IAPI::serialize_creation_params( Serializer *p_serializer ) const {
	
	List<IAPI::PropertyInfo> pinfo_list;
	get_property_list( &pinfo_list );
		
	// add creation params and dependeces	
		
	for(List<IAPI::PropertyInfo>::Iterator *I=pinfo_list.begin();I;I=I->next()) {
		
		if (I->get().usage_flags&IAPI::PropertyInfo::USAGE_CREATION_PARAM) {

			p_serializer->add_string(I->get().path);
			p_serializer->add_variant(get( I->get().path ));
			
		} 
	}
	
	p_serializer->add_string(""); // end
	
	return OK;
}

Error IAPI::deserialize_creation_params( Deserializer *p_deserializer, CreationParams& p_params ) {
		
	String prop=p_deserializer->get_string();
	while(prop!="") {

		p_params[prop]=Variant();
		p_deserializer->get_variant(p_params[prop]);
		prop=p_deserializer->get_string();
	}
	
	return OK;
}

static void add_dep(Variant& v, List<ObjID>* p_dep) {

	IRef<IAPI> ref = v.get_IOBJ();
	if (ref) {
		if (ref->get_ID() != OBJ_INVALID_ID) {
	
			p_dep->push_back(ref->get_ID());
		} else {
			ref->get_dependency_list(p_dep);
		};
	};
};


void IAPI::get_dependency_list( List<ObjID>* p_list ) {
	
	List<IAPI::PropertyInfo> list;
	
	get_property_list(&list);
	
	List<IAPI::PropertyInfo>::Iterator *I = list.begin();
	while (I) {
		
		Variant v = get(I->get().path);
		
		if (v.get_type() == Variant::IOBJ) {
			add_dep(v, p_list);
		};
		
		if (v.get_type() == Variant::VARIANT_ARRAY) {
			
			for (int i=0; i<v.size(); i++) {
				
				Variant va = v.array_get(i);
				if (va.get_type() == Variant::IOBJ) {

					add_dep(va, p_list);
				};
			};
		};
		
		I = I->next();
	};
	
};


ObjID IAPI::get_ID() const {
	
	return _objid;	
}

void IAPI::set_ID(ObjID p_ID) {
	
	_objid=p_ID;
}


IAPI::IAPI() {
	
	_objid=OBJ_INVALID_ID;
}

IAPI::~IAPI() {

}


