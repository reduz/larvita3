//
// C++ Implementation: editor_creation_params
//
// Description: 
//
//
// Author: ,,, <red@lunatea>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "editor_creation_params.h"
#include "containers/center_container.h"
#include "widgets/button.h"

void EditorCreationParams::ParamCreate::set(String p_path, const Variant& p_value) {

	if (props.has(p_path))
		props[p_path].v=p_value;
}
Variant EditorCreationParams::ParamCreate::get(String p_path) const {

	if (props.has(p_path))
		return props[p_path].v;
		
	return Variant();

}
void EditorCreationParams::ParamCreate::get_property_list( List<PropertyInfo> *p_list ) const {

	const String *k = NULL;
	
	while ( (k=props.next(k)) ) {
	
		p_list->push_back( PropertyInfo( props[*k].v.get_type(), *k ) );
	}

}
IAPI::PropertyHint EditorCreationParams::ParamCreate::get_property_hint(String p_path) const {

	if (props.has(p_path))
		return props[p_path].hint;
		
	return PropertyHint();

}

void EditorCreationParams::set(const IAPI::MethodInfo& p_hints) {

	iapi_edit->set_iapi(IAPIRef());

	params->props.clear();
	
	const List<IAPI::MethodInfo::ParamInfo>::Iterator *I=p_hints.parameters.begin();
	
	while(I) {
	
		ParamCreate::Prop p;
		p.v=(I->get().hint.default_value.get_type()==I->get().type)?I->get().hint.default_value:Variant(I->get().type);
		p.hint=I->get().hint;
		params->props[I->get().name]=p;
		I=I->next();
	}

	iapi_edit->set_iapi(params);
	
}

void EditorCreationParams::submit_params() {

	
	const String *k = NULL;
	
	IAPI::CreationParams cp;
	
	while ( (k=params->props.next(k)) ) {
	
		cp[*k]=params->props.get(*k).v;
	}
	
	submit_params_signal.call(cp);
}

EditorCreationParams::EditorCreationParams() {

	params.create();
	iapi_edit = add( memnew( EditorIAPI(true) ), 1 );
	add( memnew( GUI::CenterContainer ) )->set( memnew( GUI::Button("Ok") ) )->pressed_signal.connect( this, &EditorCreationParams::submit_params );

	
}


EditorCreationParams::~EditorCreationParams()
{
}


