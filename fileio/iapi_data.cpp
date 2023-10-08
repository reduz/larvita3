#include "iapi_data.h"

void IAPIData::set(String p_path, const Variant& p_value) {

	data[p_path] = p_value;
};

Variant IAPIData::get(String p_path) const {
	
	ERR_FAIL_COND_V( !data.has(p_path), Variant() );
	
	return data[p_path];
};

void IAPIData::get_property_list( List<PropertyInfo> *p_list ) const {
	
	const String* key = data.next(NULL);
	while (key) {
		
		const Variant &val = data.get(*key);
		p_list->push_back(PropertyInfo(val.get_type(), *key));
		
		key = data.next(key);
	};
};

IAPIData::IAPIData() {
	
};

IAPIData::~IAPIData() {
	
};

