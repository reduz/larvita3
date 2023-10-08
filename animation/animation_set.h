//
// C++ Interface: animation_set
//
// Description: 
//
//
// Author: ,,, <red@lunatea>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef ANIMATION_SET_H
#define ANIMATION_SET_H

#include "animation/animation.h"
/**
	@author ,,, <red@lunatea>
*/
class AnimationSet : public IAPI {

	IAPI_TYPE( AnimationSet, IAPI );
	IAPI_INSTANCE_FUNC( AnimationSet );
	
	Table<String,IRef<Animation> > animation_set;
	
	Table<String,float> blend_time; // blend info in animname:animname format

public:
	/* Properties */
	
	virtual void set(String p_path, const Variant& p_value); 
	virtual Variant get(String p_path) const;
	virtual void get_property_list( List<PropertyInfo> *p_list ) const;

	virtual PropertyHint get_property_hint(String p_path) const;
	virtual Variant call(String p_method, const List<Variant>& p_params=List<Variant>());	
	virtual void get_method_list( List<MethodInfo> *p_list ) const; ///< Get a list with all the properies inside this object
	
	void add_animation(String p_name,IRef<Animation> p_animation);
	IRef<Animation> get_animation(String p_name) const;
	void remove_animation(String p_name);
	
	void get_animation_list( List<String> *p_animation_list) const;
	
	void add_blend_time(String p_anim_from,String p_anim_to,float p_time);
	float get_blend_time(String p_anim_from,String p_anim_to) const;
	void clear_blend_time(String p_anim_from,String p_anim_to);
	
	void clear();
	

	AnimationSet();
	~AnimationSet();

};

#endif
