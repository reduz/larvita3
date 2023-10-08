//
// C++ Implementation: animation_set
//
// Description: 
//
//
// Author: ,,, <red@lunatea>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "animation_set.h"


	
void AnimationSet::set(String p_path, const Variant& p_value) {

	if (p_path.find("set")==0) {
	
		add_animation( p_path.get_slice("/",1), p_value.get_IOBJ() );
	} 
	
	if (p_path.find("blend")==0) {
	
		String str=p_path.get_slice("/",1);
		String a=str.get_slice(":",0);
		String b=str.get_slice(":",1);
		ERR_FAIL_COND(p_value.get_type()!=Variant::REAL && p_value.get_type()!=Variant::INT );
		
		add_blend_time(a,b,p_value.get_real());
	} 
	
	
}
Variant AnimationSet::get(String p_path) const {

	if (p_path.find("set")==0) {
	
		String anim=p_path.get_slice("/",1);
		ERR_FAIL_COND_V( !animation_set.has(anim), Variant() );
		return IObjRef(animation_set[anim]);
	} 
	
	if (p_path.find("blend")==0) {
	
		String str=p_path.get_slice("/",1);
		ERR_FAIL_COND_V( !blend_time.has(str), Variant() );
		return blend_time[str];		
	} 
	
	return Variant();
}

void AnimationSet::get_property_list( List<PropertyInfo> *p_list ) const {


	List<String> anim_list;
	get_animation_list(&anim_list);
	
	List<String>::Iterator *I=anim_list.begin();
	
	while(I) {
	
		p_list->push_back( PropertyInfo( Variant::IOBJ, "set/"+I->get() ) );
		I=I->next();
	}
	
	const String*k=NULL;
	
	
	while( (k=blend_time.next(k) ) ) {
	
		p_list->push_back( PropertyInfo( Variant::REAL, "blend/"+*k ) );
		
	}
	
}

IAPI::PropertyHint AnimationSet::get_property_hint(String p_path) const {
	
	if (p_path.find("set/")==0)
		return PropertyHint( PropertyHint::HINT_IOBJ_TYPE, "Animation" );
	
	return PropertyHint();
}
Variant AnimationSet::call(String p_method, const List<Variant>& p_params) {
	
	if (p_method=="add_animation") {
		
		if (p_params.size()<2)
			return Variant();
		
		String name=p_params[0];
		IRef<Animation> anim=p_params[1].get_IOBJ();
		ERR_FAIL_COND_V(anim.is_null(),Variant());
		
		add_animation(name,anim);		
	}
	
	if (p_method=="add_blend_time") {
		
		if (p_params.size()<3)
			return Variant();
		
		String anim_from=p_params[0];
		String anim_to=p_params[1];
		float time=p_params[2];
		
		add_blend_time(anim_from,anim_to,time);
	}
	
	return Variant();
}
void AnimationSet::get_method_list( List<MethodInfo> *p_list ) const {
	
	{
		MethodInfo mi;
		mi.parameters.push_back( IAPI::MethodInfo::ParamInfo( Variant::STRING, "name" ) );
		mi.parameters.push_back( IAPI::MethodInfo::ParamInfo( Variant::IOBJ, "object", PropertyHint( PropertyHint::HINT_IOBJ_TYPE, "Animation") ) );
		mi.name="add_animation";
		
		p_list->push_back(mi);
	}
	
	{
		MethodInfo mi;
		mi.name="add_blend_time";
		mi.parameters.push_back( IAPI::MethodInfo::ParamInfo( Variant::STRING, "anim_from" ) );
		mi.parameters.push_back( IAPI::MethodInfo::ParamInfo( Variant::STRING, "anim_to" ) );
		mi.parameters.push_back( IAPI::MethodInfo::ParamInfo( Variant::REAL, "blend_time" , PropertyHint( PropertyHint::HINT_RANGE, "0,16,0.01", 0.0) ));
		
		p_list->push_back(mi);
	}
	
	
}


void AnimationSet::add_animation(String p_name,IRef<Animation> p_animation) {

	ERR_FAIL_COND(p_name=="");

	ERR_FAIL_COND( p_name!="" && p_name.find(".")!=-1 || p_name.find("/")!=-1 || p_name.find(":")!=-1);
	 // forbidden
	ERR_FAIL_COND(p_animation.is_null());
	
	animation_set[p_name]=p_animation;
	property_changed_signal.call("");

}
IRef<Animation> AnimationSet::get_animation(String p_name) const {

	ERR_FAIL_COND_V( !animation_set.has(p_name), IRef<Animation>() );
	
	return animation_set[p_name];

}
void AnimationSet::remove_animation(String p_name) {

	ERR_FAIL_COND( !animation_set.has(p_name) );
	
	animation_set.erase(p_name);

}

void AnimationSet::get_animation_list( List<String> *p_animation_list) const {

	const String*k=NULL;
	
	p_animation_list->clear();
	
	while( (k=animation_set.next(k) ) ) {
	
		p_animation_list->push_back(*k);
	}
	
	p_animation_list->sort();

}

void AnimationSet::add_blend_time(String p_anim_from,String p_anim_to,float p_time) {

	ERR_FAIL_COND(p_time<0);
	ERR_FAIL_COND( (p_anim_from != "*" && !animation_set.has(p_anim_from)) || (p_anim_to != "*" && !animation_set.has(p_anim_to)) );

	blend_time[p_anim_from+":"+p_anim_to]=p_time;

}
float AnimationSet::get_blend_time(String p_anim_from,String p_anim_to) const {

	String key=p_anim_from+":"+p_anim_to;
	if (blend_time.has(key)) return blend_time[key];

	key = "*:" + p_anim_to;
	if (blend_time.has(key)) return blend_time[key];
	
	key = p_anim_from+":*";
	if (blend_time.has(key)) return blend_time[key];

	key = "*:*";
	if (blend_time.has(key)) return blend_time[key];
	
	return 0;

}
void AnimationSet::clear_blend_time(String p_anim_from,String p_anim_to) {

	String key=p_anim_from+":"+p_anim_to;
	ERR_FAIL_COND( !blend_time.has(key));
	
	blend_time.erase(key);
	
	property_changed_signal.call("");	
	
}

void AnimationSet::clear() {

	animation_set.clear();
	blend_time.clear();

}

AnimationSet::AnimationSet()
{
}


AnimationSet::~AnimationSet()
{
}


