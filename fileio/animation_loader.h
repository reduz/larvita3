//
// C++ Interface: animation_loader
//
// Description: 
//
//
// Author: ,,, <red@lunatea>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef ANIMATION_LOADER_H
#define ANIMATION_LOADER_H

/**
	@author ,,, <red@lunatea>
*/

#include "animation/animation.h"
#include "os/file_access.h"

/**
	@author ,,, <red@lunatea>
*/


class AnimationFormatLoader {
public:

	virtual IRef<Animation> load_animation(String p_path,FileAccess *p_file)=0;

	AnimationFormatLoader();
	virtual ~AnimationFormatLoader();
};


class AnimationLoader {

	enum {
		MAX_LOADERS=8
	};

	static AnimationFormatLoader *loader[MAX_LOADERS];
	static int loader_count;

protected:


public:
	
	static IRef<Animation> load_animation(String p_path, FileAccess *p_custom=NULL);	
	static void add_animation_format_loader(AnimationFormatLoader *p_loader);

};


#endif
