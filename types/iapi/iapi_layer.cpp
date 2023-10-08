//
// C++ Implementation: iapi_layer
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "iapi_layer.h"


void IAPI_Layer::set(String p_path, const Variant& p_value) {

	set_signal.call(p_path,p_value);
	property_changed_signal.call(p_path);	
}

Variant IAPI_Layer::get(String p_path) const {
	
	Variant res;
	
	Signal< Method2< const String&, Variant& > > * s;
	s= const_cast<Signal< Method2< const String&, Variant& > >*>(&get_signal);

	s->call(p_path,res);
	
	// This is a bug trap, since properties 
	//if (res.get_type() == Variant::NIL) printf("path is %ls\n", p_path.c_str());
	if (__debug_empty)
		ERR_FAIL_COND_V(res.get_type()==Variant::NIL,res);
	
	return res;
} 

void IAPI_Layer::get_property_list( List<PropertyInfo> *p_list ) const {
	
	Signal< Method1< List<PropertyInfo>* > > *s;
	
	s = const_cast< Signal< Method1< List<PropertyInfo>* > > *>(&get_property_list_signal); ///< Signal to obtain the list of properties.
		
	s->call( p_list );
}

IAPI::PropertyHint IAPI_Layer::get_property_hint(String p_path) const {
	
	PropertyHint hint;
	
	Signal< Method2< const String&, PropertyHint& > > *s;
	s = const_cast< Signal< Method2< const String&, PropertyHint& > > * >( &get_property_hint_signal );
	s->call( p_path, hint );
	
	return hint;
	
}

Variant IAPI_Layer::call(String p_method, const List<Variant>& p_params) {
	
	Variant res;
	call_signal.call( p_method, p_params,res );
	return res;
}

void IAPI_Layer::get_method_list( List<MethodInfo> *p_list ) const {

	Signal< Method1< List<MethodInfo> * > > *s = const_cast< Signal< Method1< List<MethodInfo> * > > *> ( &get_method_list_signal );
	s->call(p_list);
}

void IAPI_Layer::init() {

	init_signal.call();	
}

void IAPI_Layer::predelete() {
	
	predelete_signal.call();	
}

IAPI_Layer::IAPI_Layer() {
	
	__debug_empty = false;
	
}


IAPI_Layer::~IAPI_Layer()
{
}


