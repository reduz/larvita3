//
// C++ Implementation: default_indexer
//
// Description: 
//
//
// Author: ,,, <red@lunatea>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "default_indexer.h"
#include "renderer.h"
#include "global_vars.h"

void DefaultObject::set_visible_callback( const Method1< const SpatialRenderData& > & p_callback ) {
	
	visible_callback.clear();
	visible_callback.connect(p_callback);
}
void DefaultObject::set_draw_callback( const Method1< const SpatialRenderData& > & p_callback ) {
	
	draw_callback.clear();
	draw_callback.connect(p_callback);
}

void DefaultObject::set_light_callback( const Method2<const IRef<Light>&, const AABB&> & p_callback ) {
	
	light_callback.clear();
	light_callback.connect(p_callback);
};


void DefaultObject::set_AABB(const AABB& p_AABB) {

	set_AABB_callback.call(p_AABB);
}

void DefaultObject::set_owner(const IAPIRef& p_owner) {

	owner=p_owner;
}

DefaultObject::DefaultObject() {

	light_pass=0;
	light_count=0;
	
}

DefaultObject *DefaultIndexer::create_spatial_object() {

	DefaultObject *dobj=octree.create();
	dobj->set_AABB_callback.connect( Method1<const AABB&>( Method2<const AABB&,DefaultObject*>( this, &DefaultIndexer::object_moved), dobj ) );
	
	return dobj;
}


void DefaultIndexer::remove_spatial_object(SpatialObject *p_object) {

	octree.erase((DefaultObject*)p_object);
}
void DefaultIndexer::object_moved( const AABB& p_new_pos, DefaultObject* p_obj ) {

	octree.move( p_obj, p_new_pos );

}
void DefaultIndexer::render() {

	shadows_enabled = !GlobalVars::get_singleton()->get("use_fixed_pipeline") && !GlobalVars::get_singleton()->get("disable_shadows");

	Renderer *r = Renderer::get_singleton();
	ERR_FAIL_COND(!r);

	IRef<Light> tmp_lights[DefaultObject::MAX_LIGHTS];
	int tmp_light_count=0;
	int global_light_count=0;

	visible_lights_count = 0;
	
	/* add global lights */
	for (int i=0;i<MAX_GLOBAL_LIGHTS;i++) {
		if (tmp_light_count>=DefaultObject::MAX_LIGHTS)
			break;
		if (global_lights[i].is_null()) {

			continue;
		}

		tmp_lights[tmp_light_count]=global_lights[i];
		global_light_count++;
		tmp_light_count++;
		
		visible_lights[visible_lights_count++] = global_lights[i];
	}
	
	
	pass++;
	
	SpatialRenderData render_data;
	render_data.inv_camera_matrix=r->render_get_matrix( Renderer::MATRIX_CAMERA ).inverse();
	
	render_data.frustum_planes.create_from_projection( 
			r->render_get_matrix( Renderer::MATRIX_PROJECTION ) * render_data.inv_camera_matrix );
		 
	
	int culled_count = octree.cull_convex( render_data.frustum_planes, render_queue_buffer,MAX_OBJECT_COUNT );
	
	if (culled_count==0)
		return; //save time
		
	/* pre pass, useful for lights, portals, render targets, etc */
	
	for (int i=0;i<culled_count;i++) {
	
		render_queue_buffer[i]->visible_callback.call(render_data);
	}
	
	/* fog */
	
	r->render_set_fog( fog.mode, fog.density, fog.start, fog.end, fog.index, fog.color, fog.src );
	
	
	/* render list pass */
	
	r->render_scene_begin(visible_lights, visible_lights_count);
	
	render_scene_signal.call();
	
	for (int i=0;i<culled_count;i++) {
	
		int light_count=global_light_count;
		
		DefaultObject *o=render_queue_buffer[i];
		
		if ( o->light_pass==pass) {
		
			for (int i=0;i<o->light_count;i++) {
			
				if (light_count>=DefaultObject::MAX_LIGHTS) 
					break;
					
				tmp_lights[light_count]=o->lights[i];
				light_count++;
			}
		}
		
		r->render_set_lights(tmp_lights,light_count);
		
		render_queue_buffer[i]->draw_callback.call(render_data);
	}
	
	r->render_scene_end();		
	r->render_set_fog( Renderer::FOG_DISABLED, fog.density, fog.start, fog.end, fog.index, fog.color, fog.src );
} 

static inline void hash_matrix(const Matrix4& p_mat, Uint64& seed) {
	for (int i=0; i<16; i++) {
		hash_djb2(((real_t*)p_mat.elements)[i], seed);
	};
};

void DefaultIndexer::add_light(const IRef<Light>& p_light) {

	AABB light_aabb = p_light->get_AABB();
	p_light->get_matrix().transform_aabb(light_aabb);
	
	int lighted_count =  octree.cull_AABB( light_aabb, light_queue_buffer, MAX_OBJECT_COUNT );
	Uint64 light_hash;

	bool shadow = shadows_enabled && !p_light->get("shadow_texture").get_IOBJ().is_null();	
	if (shadow) {
		light_hash = hash_djb2(0);
		hash_matrix(p_light->get_matrix(), light_hash);
	};
	
	for (int i=0;i<lighted_count;i++) {
	
		DefaultObject *o=light_queue_buffer[i];
		
		if (o->light_pass!=pass) {
			
			o->light_pass=pass;
			o->light_count=0;
		}

		if (shadow && o->cast_shadows) {
			hash_djb2(o, light_hash);
			hash_matrix(o->matrix, light_hash);
		};
		o->light_callback.call(p_light, light_aabb);
		if (o->light_count>=DefaultObject::MAX_LIGHTS)
			continue;
			
		o->lights[o->light_count]=p_light;
		++o->light_count;
		
	}

	if (shadow) {
		p_light->set_influence_hash(light_hash);
	};

	if (visible_lights_count < MAX_VISIBLE_LIGHTS) {
		visible_lights[visible_lights_count++] = p_light;
	};
}

void DefaultIndexer::add_global_light(const IRef<Light>& p_light) {

	for (int i=0;i<MAX_GLOBAL_LIGHTS;i++) {
			
		if (global_lights[i]==p_light) {

			return; // already in 
		}
	}
	
	for (int i=0;i<MAX_GLOBAL_LIGHTS;i++) {
	
		if (!global_lights[i].is_null()) {
			
			continue; // find empty slot
		}
			

		global_lights[i]=p_light; 
		return;
	}
	
	
}
void DefaultIndexer::remove_global_light(const IRef<Light>& p_light) {

	
	for (int i=0;i<MAX_GLOBAL_LIGHTS;i++) {
	
		if (global_lights[i]!=p_light)
			continue; // find empty slot
			
		global_lights[i].clear();		
	}

}

void DefaultIndexer::set_fog(Renderer::FogMode p_mode, float p_density, float p_start, float p_end, float p_index, Color p_color, Renderer::FogCoordSrc p_src) {
	
	fog.mode=p_mode;
	fog.density=p_density;
	fog.start=p_start;
	fog.end=p_end;
	fog.index=p_index;
	fog.color=p_color;
	fog.src=p_src;
}

void DefaultIndexer::get_objects_in_segment(const Vector3& p_from,const Vector3& p_to,List< IAPIRef > *objects ) {

	ERR_FAIL_COND( !objects );
	int count = octree.cull_segment(p_from,p_to,render_queue_buffer,MAX_OBJECT_COUNT);
	for (int i=0;i<count;i++) {
	
		objects->push_back( render_queue_buffer[i]->owner );
	}	
}

DefaultIndexer::DefaultIndexer()
{

	fog.mode= Renderer::FOG_DISABLED;
}


DefaultIndexer::~DefaultIndexer()
{
}


