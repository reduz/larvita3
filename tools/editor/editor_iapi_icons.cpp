//
// C++ Implementation: editor_iapi_icons
//
// Description: 
//
//
// Author: ,,, <red@lunatea>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "editor_iapi_icons.h"
#include "editor_skin.h"



GUI::BitmapID EditorIAPIIcons::get(String p_type) {

	if (custom.has(p_type))
		return custom.get(p_type);

	if (internal.has(p_type))
		return skin->get_bitmap( internal.get(p_type) );
		
		
	return -1;
}
void EditorIAPIIcons::add(String p_type,GUI::BitmapID p_bitmap) {

	custom[p_type]=p_bitmap;
}

EditorIAPIIcons::EditorIAPIIcons(GUI::Skin * p_skin) {

	skin=p_skin;
	
	internal["DummyNode"]=BITMAP_EDITOR_ICON_DUMMY;
	internal["MeshNode"]=BITMAP_EDITOR_ICON_MESH;
	internal["ParticleSystemNode"]=BITMAP_EDITOR_ICON_PARTICLES;
	internal["SplineNode"]=BITMAP_EDITOR_ICON_SPLINE;
	internal["LightNode"]=BITMAP_EDITOR_ICON_LIGHT;
	internal["CameraNode"]=BITMAP_EDITOR_ICON_CAMERA;
	internal["CollisionNode"]=BITMAP_EDITOR_ICON_COLLISION;
	internal["RigidBodyNode"]=BITMAP_EDITOR_ICON_RIGID_BODY;
	internal["InfluenceZoneNode"]=BITMAP_EDITOR_ICON_INFLUENCE_ZONE;
	internal["SkeletonNode"]=BITMAP_EDITOR_ICON_SKELETON;
	internal["AnimationNode"]=BITMAP_EDITOR_ICON_ANIMATION_NODE;
	internal["PortalNode"]=BITMAP_EDITOR_ICON_PORTAL;
	internal["VisibilityAreaNode"]=BITMAP_EDITOR_ICON_VISIBILITY_AREA;
	internal["SceneNode"]=BITMAP_EDITOR_ICON_SCENE;
	internal["SceneInstanceNode"]=BITMAP_EDITOR_ICON_SCENE_INSTANCE;
	internal["Mesh"]=BITMAP_EDITOR_ICON_MESHR;
	internal["Surface"]=BITMAP_EDITOR_ICON_SURFACE;
	internal["Material"]=BITMAP_EDITOR_ICON_MATERIAL;
	internal["Texture"]=BITMAP_EDITOR_ICON_TEXTURE;
	internal["Light"]=BITMAP_EDITOR_ICON_LIGHTR;
	internal["Image"]=BITMAP_EDITOR_ICON_IMAGE;
	internal["Volume"]=BITMAP_EDITOR_ICON_VOLUME;
	internal["Skeleton"]=BITMAP_EDITOR_ICON_SKELETONR;
	internal["Animation"]=BITMAP_EDITOR_ICON_ANIMATION;
	internal["AnimationSet"]=BITMAP_EDITOR_ICON_ANIMATION_SET;
	
}


EditorIAPIIcons::~EditorIAPIIcons()
{
}


