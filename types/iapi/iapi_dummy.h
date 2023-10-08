#ifndef IAPI_DUMMY_H
#define IAPI_DUMMY_H

#include "iapi.h"

class IAPIDummy : public IAPI {

	String type;
	
	virtual String get_type() const { return type; }; ///< get object type
	virtual bool is_type(String p_type) { return true; }; ///< get object type
	virtual String persist_as() const { return type; }; ///< inheritance level to persist as

	List<PropertyInfo> property_list;
	Table<String, Variant> properties;
	
public:

	virtual void set(String p_path, const Variant& p_value); /** set a property, by path */
	virtual Variant get(String p_path) const; /** get a property, by path */

	virtual void get_property_list( List<PropertyInfo> *p_list ) const; ///< Get a list with all the properies inside this object
	
	void set_creation_params(CreationParams& p_params);

	IAPIDummy(String p_type);
};

#endif
