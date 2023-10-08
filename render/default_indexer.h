//
// C++ Interface: default_indexer
//
// Description: 
//
//
// Author: ,,, <red@lunatea>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef DEFAULT_INDEXER_H
#define DEFAULT_INDEXER_H


#include "octree.h"
#include "spatial_indexer.h"

/**
	@author ,,, <red@lunatea>
*/

class DefaultObject : public SpatialObject  {
friend class DefaultIndexer;

	enum {
	
		MAX_LIGHTS=8
	};
	
	Uint32 light_pass;
	
	IRef<Light> lights[MAX_LIGHTS];
	int light_count;
	WeakRef<IAPI> owner;

	Signal< Method1< const SpatialRenderData& > > visible_callback;
	Signal< Method1< const SpatialRenderData& > > draw_callback;
	Signal< Method1<const AABB&> > set_AABB_callback;
	Signal< Method2<const IRef<Light>&, const AABB& > > light_callback;

	
public:		
	virtual void set_visible_callback( const Method1< const SpatialRenderData& > & p_callback );
	virtual void set_draw_callback( const Method1< const SpatialRenderData& > & p_callback );
	virtual void set_light_callback( const Method2<const IRef<Light>&, const AABB&> & p_callback );
	virtual void set_owner(const IAPIRef& p_owner);	
	virtual void set_AABB(const AABB& p_AABB);		
	
	DefaultObject();
	
	
};


class DefaultIndexer : public SpatialIndexer {	

	Octree<DefaultObject> octree;
	
	enum {
		MAX_OBJECT_COUNT=8192,
		MAX_GLOBAL_LIGHTS=8,
		MAX_VISIBLE_LIGHTS=64,
	};
	
	struct {
		
		Renderer::FogMode mode;
		float density;
		float start;
		float end;
		float index;
		Color color;
		Renderer::FogCoordSrc src;		
	} fog;
	
	DefaultObject *render_queue_buffer[MAX_OBJECT_COUNT];
	DefaultObject *light_queue_buffer[MAX_OBJECT_COUNT];
	
	void object_moved( const AABB& p_new_pos, DefaultObject* p_obj );
	
	IRef<Light> global_lights[MAX_GLOBAL_LIGHTS];
	IRef<Light> visible_lights[MAX_VISIBLE_LIGHTS];
	int visible_lights_count;

	bool shadows_enabled;
	Uint32 pass;
public:
	
	virtual DefaultObject *create_spatial_object();
	virtual void remove_spatial_object(SpatialObject *p_object);
	virtual void render(); 
	virtual void add_light(const IRef<Light>& p_light); 
	
	virtual void add_global_light(const IRef<Light>& p_light); 
	virtual void remove_global_light(const IRef<Light>& p_light);
	
	virtual void set_fog(Renderer::FogMode p_mode, float p_density, float p_start, float p_end, float p_index, Color p_color, Renderer::FogCoordSrc p_src);
	
	virtual void get_objects_in_segment(const Vector3& p_from,const Vector3& p_to,List< IAPIRef > *objects );	
	
	
	DefaultIndexer();
	virtual ~DefaultIndexer();
};

#endif
