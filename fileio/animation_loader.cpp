//
// C++ Implementation: animation_loader
//
// Description: 
//
//
// Author: ,,, <red@lunatea>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "animation_loader.h"


AnimationFormatLoader::AnimationFormatLoader()
{
}


AnimationFormatLoader::~AnimationFormatLoader()
{
}


AnimationFormatLoader *AnimationLoader::loader[MAX_LOADERS];
	
int AnimationLoader::loader_count=0;

IRef<Animation> AnimationLoader::load_animation(String p_path, FileAccess *p_custom) {

	FileAccess *f = p_custom?p_custom:FileAccess::create();
	IRef<Animation> result;
	for(int i=0;i<loader_count;i++) {

		result = loader[i]->load_animation(p_path,f);
		if (!result.is_null()) {
			if (!p_custom)
				memdelete(f);
			return result;
		}
	}
	
	if (!p_custom)
		memdelete(f);
	
	return result;
}
void AnimationLoader::add_animation_format_loader(AnimationFormatLoader *p_loader) {

	ERR_FAIL_COND(loader_count==MAX_LOADERS);
	loader[loader_count++]=p_loader;
}
