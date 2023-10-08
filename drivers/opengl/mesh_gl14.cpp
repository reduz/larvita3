#ifdef OPENGL_ENABLED

//
// C++ Implementation: mesh_gl
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "mesh_gl14.h"

void Mesh_GL14::set(String p_path,const Variant& p_value) {
	
	if (p_path=="surface_count")
		surfaces.resize(p_value.get_int());
	if (p_path.find("surfaces/")==0) {
			
		int idx=p_path.get_slice("/",1).to_int();
		ERR_FAIL_INDEX(idx,surfaces.size());
		surfaces[idx]=p_value.get_IOBJ();
	}
	if (p_path == "skip_presave") {
		skip_presave = p_value;
	};
}
Variant Mesh_GL14::get(String p_path) const {
	
	if (p_path=="surface_count")
		return surfaces.size();
	if (p_path.find("surfaces/")==0) {
			
		int idx=p_path.get_slice("/",1).to_int();
		ERR_FAIL_INDEX_V(idx,surfaces.size(),Variant());
		return IObjRef(surfaces[idx]);
	}
	if (p_path == "skip_presave") {
		return skip_presave;
	};

	return Variant();
}

Variant Mesh_GL14::call(String p_method, const List<Variant>& p_params) {

	if  (p_method=="needs_presave")
		return !skip_presave;
		
	return Variant();
}


void Mesh_GL14::get_property_list( List<PropertyInfo> *p_list ) const {
	
	p_list->push_back( PropertyInfo( Variant::INT, "surface_count" ) );
	
	for(int i=0;i<surfaces.size();i++) {
		
		p_list->push_back( PropertyInfo( Variant::IOBJ, "surfaces/"+String::num(i) ) );
	}

	p_list->push_back( PropertyInfo( Variant::BOOL, "skip_presave" ) );
}

IAPI::PropertyHint Mesh_GL14::get_property_hint(String p_path) const {
	
	if (p_path=="surface_count")
		return IAPI::PropertyHint(PropertyHint::HINT_RANGE,"0,1023,1");
	else if (p_path.find("surfaces")==0)
		return IAPI::PropertyHint(PropertyHint::HINT_IOBJ_TYPE,"Surface");
	
	return IAPI::PropertyHint();
}

void Mesh_GL14::set_surface_count(int p_count) {
	
	surfaces.resize(p_count);
}
int Mesh_GL14::get_surface_count() const {
	
	return surfaces.size();
}
void Mesh_GL14::add_surface(IRef<Surface> p_surface) {
	
	surfaces.push_back(p_surface);
}

void Mesh_GL14::set_surface(int p_index,IRef<Surface> p_surface) {
	
	ERR_FAIL_INDEX(p_index,surfaces.size());
	
	surfaces[p_index]=p_surface;
	
}

IRef<Surface> Mesh_GL14::get_surface(int p_index) const {
	
	ERR_FAIL_INDEX_V(p_index,surfaces.size(),IRef<Surface>());
	
	return surfaces[p_index];
}


Mesh_GL14::Mesh_GL14()
{

	skip_presave = false;
}


Mesh_GL14::~Mesh_GL14()
{
}


#endif // OPENGL_ENABLED
