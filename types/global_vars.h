#ifndef GLOBAL_VARS_H
#define GLOBAL_VARS_H

#include "types/iapi/iapi.h"

/**
 * Global variables. These are just global variables, with default values, overridable
 * by system environment or a configuration file.
 */

class GlobalVars : public IAPI {

	IAPI_TYPE( GlobalVars, IAPI );
	
	void load_option(String p_section,String p_variable,String p_value);
	
private:

	static GlobalVars* instance;

	Table<String, Variant> vars;

public:

	virtual void set(String p_path, const Variant& p_value); /** set a property, by path */
	virtual Variant get(String p_path) const; /** get a property, by path */
	
	virtual void get_property_list( List<PropertyInfo> *p_list ) const;
	virtual PropertyHint get_property_hint(String p_path) const { return PropertyHint(); }; ///< get a property hint
	
	static GlobalVars* get_singleton();
		
	String translate_path(String p_path);
	
	Error load_from_file(String p_file);
			
	GlobalVars();
};

#endif


