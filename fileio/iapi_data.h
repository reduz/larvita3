#ifndef IAPI_DATA_H
#define IAPI_DATA_H

#include "types/iapi/iapi.h"

class IAPIData : public IAPI {
	
	IAPI_TYPE( IAPIData, IAPI );
	IAPI_INSTANCE_FUNC( IAPIData );
	
	Table<String, Variant> data;
	
public:
	
	virtual void set(String p_path, const Variant& p_value); /** set a property, by path */
	virtual Variant get(String p_path) const; /** get a property, by path */
	virtual void get_property_list( List<PropertyInfo> *p_list ) const; ///< Get a list with all the properies inside this object

	IAPIData();
	~IAPIData();
};


#endif
