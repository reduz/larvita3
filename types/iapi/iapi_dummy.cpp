#include "iapi_dummy.h"

void IAPIDummy::set(String p_path, const Variant& p_value) {

	if (!properties.has(p_path)) {
		
		property_list.push_back(PropertyInfo(p_value.get_type(), p_path));
	};
	
	properties.set(p_path, p_value);
};

Variant IAPIDummy::get(String p_path) const {

	if (properties.has(p_path)) {
		
		return properties.get(p_path);
	};
	
	return Variant();
};

void IAPIDummy::get_property_list( List<PropertyInfo> *p_list ) const {

	const List<PropertyInfo>::Iterator* I = property_list.begin();
	while (I) {
		
		p_list->push_back(I->get());
		
		I = I->next();
	};
};

void IAPIDummy::set_creation_params(CreationParams& p_params) {

	const String* key = NULL;
	while ( (key = p_params.next(key)) ) {
	
		Variant val = p_params.get(*key);
		property_list.push_back(PropertyInfo(val.get_type(), *key, PropertyInfo::USAGE_CREATION_PARAM));
		properties.set(*key, val);
	};
};

IAPIDummy::IAPIDummy(String p_type) {

	type = p_type;	
};

