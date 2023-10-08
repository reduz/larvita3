#ifdef COLLADA_ENABLED

#ifndef COLLADA_H
#define COLLADA_H

#include "scene/nodes/scene_node.h"
#include "scene/nodes/skeleton_node.h"
#include "animation/animation.h"
#include "fileio/animation_loader.h"
#include "global_vars.h"

class Collada {

public:

	static IRef<SceneNode> import_scene(String p_fname, IRef<SceneNode> p_root = IRef<SceneNode>());
	static IRef<Animation> import_animation(String p_fname);
};



class AnimationFormatLoaderCollada : public AnimationFormatLoader {
public:

	virtual IRef<Animation> load_animation(String p_path,FileAccess *p_file) {

		p_path = GlobalVars::get_singleton()->translate_path(p_path);
		return Collada::import_animation(p_path);
	}

};


#endif

#endif
