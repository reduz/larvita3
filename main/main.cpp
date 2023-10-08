//
// C++ Implementation: main
//
// Description: 
//
//
// Author: ,,, <red@lunatea>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "main.h"
#include "error_macros.h"
#include "iapi_persist.h"

Main* Main::singleton=NULL;
Main* (*Main::create_func)(const VideoMode&)=NULL;

Main* Main::create(const VideoMode& p_preferred) {

	ERR_FAIL_COND_V(!create_func,NULL);
	return create_func(p_preferred);
}

/* register instance functions */

#include "fileio/iapi_data.h"

#include "collision/plane_volume.h"
#include "collision/sphere_volume.h"
#include "collision/convex_polygon_volume.h"
#include "collision/triangle_mesh_volume.h"

#include "scene/nodes/camera_node.h"
#include "scene/nodes/camera_follow_node.h"
#include "scene/nodes/camera_fps_node.h"
#include "scene/nodes/collision_node.h"
#include "scene/nodes/rigid_body_node.h"		 
#include "scene/nodes/dummy_node.h"
#include "scene/nodes/mesh_node.h"		 
#include "scene/nodes/light_node.h"		 
#include "scene/nodes/primitive_list_node.h"		 
#include "scene/nodes/skeleton_node.h"
#include "scene/nodes/scene_node.h"
#include "scene/nodes/animation_node.h"
#include "scene/nodes/particle_system_node.h"
#include "scene/nodes/spline_node.h"

#include "audio/audio_stream_ogg_vorbis.h"
#include "audio/audio_stream_xm.h"
#include "audio/audio_sample.h"

#include "animation/animation.h"
#include "animation/animation_set.h"
			 
Main::Main() {

	IAPI_REGISTER_TYPE(IAPIData);
	
	IAPI_REGISTER_TYPE_STATIC(PlaneVolume);
	IAPI_REGISTER_TYPE_STATIC(SphereVolume);
	IAPI_REGISTER_TYPE_STATIC(ConvexPolygonVolume);
	IAPI_REGISTER_TYPE_STATIC(TriangleMeshVolume);
			 
	IAPI_REGISTER_TYPE( CameraNode );
	IAPI_REGISTER_TYPE( CameraFollowNode );
	IAPI_REGISTER_TYPE( CameraFPSNode );
	IAPI_REGISTER_TYPE( CollisionNode );	
	IAPI_REGISTER_TYPE( RigidBodyNode );	
	IAPI_REGISTER_TYPE( DummyNode );	
	IAPI_REGISTER_TYPE( MeshNode );
	IAPI_REGISTER_TYPE( LightNode );
	IAPI_REGISTER_TYPE( PrimitiveListNode );
	IAPI_REGISTER_TYPE( ParticleSystemNode );
	IAPI_REGISTER_TYPE( SkeletonNode );							
	IAPI_REGISTER_TYPE( SceneNode );							
	IAPI_REGISTER_TYPE( AnimationNode );							
	IAPI_REGISTER_TYPE( SplineNode );
	
	IAPI_REGISTER_TYPE_STATIC( AudioSample );
	IAPI_REGISTER_TYPE_SUPPLY( AudioStreamOGGVorbis,AudioStream );	
	IAPI_REGISTER_TYPE_SUPPLY( AudioStreamXM,AudioStream );		
	
	IAPI_REGISTER_TYPE_STATIC( Animation );		
	IAPI_REGISTER_TYPE_STATIC( AnimationSet );		
	
	
	singleton=this;
}


Main::~Main()
{
}


