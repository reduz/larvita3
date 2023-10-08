//
// C++ Interface: spatial_indexer
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef SPATIAL_INDEXER_H
#define SPATIAL_INDEXER_H
                                             
/**
	@author Juan Linietsky <reduzio@gmail.com>
*/
#include "signals/signals.h"
#include "aabb.h"
#include "render/light.h"
#include "convex_group.h"
#include "render/renderer.h"

struct SpatialRenderData {
	
	ConvexGroup frustum_planes;
	Matrix4 inv_camera_matrix;
};

class SpatialObject {
public:
	Matrix4 matrix;
	bool cast_shadows;
	virtual void set_visible_callback( const Method1< const SpatialRenderData& > & p_callback )=0;
	virtual void set_draw_callback( const Method1< const SpatialRenderData& > & p_callback )=0;
	virtual void set_light_callback( const Method2<const IRef<Light>&, const AABB&> & p_callback )=0;
	virtual void set_AABB(const AABB& p_AABB)=0;
	virtual void set_owner(const IAPIRef& p_owner)=0;
	virtual ~SpatialObject() {}		
};


class SpatialIndexer : public SignalTarget {	
public:

	Signal<> render_scene_signal; ///< emited when rendering, allows to insert commands into the render scene list
	
	virtual SpatialObject *create_spatial_object()=0;	///< Allocate a spatial object, don't delete! ith remove_spatial_object=
	virtual void remove_spatial_object(SpatialObject *p_object)=0;	
	virtual void render()=0; ///< render the spatial indexer to a render list
	virtual void add_light(const IRef<Light>& p_light)=0; ///< Plot a light, only works inside a render() call
	
	virtual void add_global_light(const IRef<Light>& p_light)=0; ///< Plot a light, only works inside a render() call
	virtual void remove_global_light(const IRef<Light>& p_light)=0; ///< Plot a light, only works inside a render() call
		
	virtual void set_fog(Renderer::FogMode p_mode, float p_density, float p_start, float p_end, float p_index, Color p_color, Renderer::FogCoordSrc p_src)=0;
	
	virtual void get_objects_in_segment(const Vector3& p_from,const Vector3& p_to,List< IAPIRef > *objects )=0;
	
	SpatialIndexer();
	virtual ~SpatialIndexer();
};

#endif
