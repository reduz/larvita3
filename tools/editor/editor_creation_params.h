//
// C++ Interface: editor_creation_params
//
// Description: 
//
//
// Author: ,,, <red@lunatea>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef EDITOR_CREATION_PARAMS_H
#define EDITOR_CREATION_PARAMS_H


#include "editor/editor_iapi.h"

/**
	@author ,,, <red@lunatea>
*/
class EditorCreationParams : public GUI::VBoxContainer {


	struct ParamCreate : public IAPI {
	
		IAPI_TYPE( ParamCreate, IAPI );
		
		
	public:	
		
		struct Prop {
		
			Variant v;
			PropertyHint hint;		
		};
		
		Table<String,Prop> props;
		
		virtual void set(String p_path, const Variant& p_value); /** set a property, by path */
		virtual Variant get(String p_path) const; /** get a property, by path */
		virtual void get_property_list( List<PropertyInfo> *p_list ) const; ///< Get a list with all the properies inside this object
		virtual PropertyHint get_property_hint(String p_path) const; ///< get a property hint
		
			
	
	};

	IRef<ParamCreate> params;
	EditorIAPI *iapi_edit;
	
	void submit_params();
public:

	Signal< Method1<const IAPI::CreationParams&> > submit_params_signal;

	void set(const IAPI::MethodInfo& p_hints);


	EditorCreationParams();
	~EditorCreationParams();

};

#endif
