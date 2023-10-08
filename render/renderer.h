//
// C++ Interface: renderer
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef RENDERER_H
#define RENDERER_H

#include "render/texture.h"
#include "render/material.h"
#include "render/shader.h"
#include "render/skeleton.h"
#include "render/surface.h"
#include "render/mesh.h"
#include "render/light.h"



/**
	@author Juan Linietsky <reduzio@gmail.com>
*/

class SpatialIndexer;

class Renderer : public SignalTarget {

	static Renderer * singleton;
	IRef<Material> default_material;
	IRef<Material> gui_material;
	IRef<Material> indicator_material;
	IRef<Texture> error_texture;
	IRef<Mesh> test_cube;
	
protected:
	static Renderer*(*create_func)();	

public:

	static Renderer * get_singleton();

	/* Data */

	virtual IRef<Texture> create_texture()=0;
	virtual IRef<Material> create_material()=0;
	virtual IRef<Shader> create_shader()=0; ///< empty on unsupported platforms
	virtual IRef<Light> create_light()=0;
	virtual IRef<Skeleton> create_skeleton()=0;
	virtual IRef<Surface> create_surface()=0;
	virtual IRef<Mesh> create_mesh()=0;										      	
	/* Matrix */
	
	enum MatrixMode {
	
		MATRIX_PROJECTION,
		MATRIX_CAMERA,
		MATRIX_WORLD
	};
	
	/* depth test */
	enum DepthTest {
	
		DEPTH_TEST_NONE,
		DEPTH_TEST_NEVER,
		DEPTH_TEST_LESS,
		DEPTH_TEST_EQUAL,
		DEPTH_TEST_LEQUAL,
		DEPTH_TEST_GREATER,
		DEPTH_TEST_NOTEQUAL,
		DEPTH_TEST_GEQUAL,
		DEPTH_TEST_ALWAYS,
	};
	
	/* clear */
	
	enum ClearFlags {
	
		CLEAR_COLOR=1,
		CLEAR_DEPTH=2,
		CLEAR_STENCIL=4,
		CLEAR_COLOR_DEPTH=CLEAR_COLOR|CLEAR_DEPTH
	};

	/*******************/
	/******* FOG *******/
	/*******************/
	enum FogMode {
		FOG_DISABLED,
		FOG_LINEAR,
		FOG_EXP,
		FOG_EXP2,
		FOG_DEFAULT = FOG_LINEAR,
	};

	enum FogCoordSrc {

		SRC_FOG_COORD,
		SRC_FRAGMENT_DEPTH,
		SRC_FOG_DEFAULT = SRC_FOG_COORD,
	};
	
	enum BufferSource {
		SOURCE_COLOR,
		SOURCE_STENCIL,
		SOURCE_DEPTH,
	};
	
	struct LightProfile {
		void* lights;
		int light_count;
		LightProfile() {
			lights = NULL;
			light_count = 0;
		};
	};

	virtual void render_set_fog(FogMode p_mode, float p_density, float p_start, float p_end, float p_index, Color p_color, FogCoordSrc p_src)=0;

	
	/* rendering */
	
	virtual void render_clear(Uint32 p_flags,const Color& p_color=Color(),float p_depth=1.0)=0;
	
	virtual void render_scene_begin(IRef<Light>*p_visible_lights=NULL, int p_light_count=-1)=0; ///< if render list is activated, then sorting and other stuff is performed, otherwise rendering is sent as-is (mmediate)	
	virtual void render_set_matrix(MatrixMode p_matrix_mode,const Matrix4& p_matrix)=0;
	virtual Matrix4 render_get_matrix(MatrixMode p_matrix_mode) const=0;
	
	virtual void render_mesh(IRef<Mesh> p_mesh,IRef<Skeleton> p_skeleton=IRef<Skeleton>(), IRef<Material> p_extra_material=IRef<Material>())=0;

	virtual LightProfile render_set_lights(IRef<Light>*p_lights=NULL,int p_light_count=0)=0; // returns a pointer to the set of lights for later use
	virtual void render_set_lights(LightProfile p_lights)=0;
	virtual void render_primitive_set_material(IRef<Material> p_material)=0;
	virtual void render_primitive_change_texture(IRef<Texture> p_texture)=0;
	virtual void render_primitive(int p_points,const Vector3 *p_vertices, const Vector3 *p_normals=NULL,const Vector3 *p_binormals_and_tangents=NULL,const Color *p_colors=NULL,const float *p_uv=NULL)=0;
	
	virtual void render_scene_end()=0;
        
	virtual void render_to_texture(IRef<Texture> p_texture, BufferSource p_source = SOURCE_COLOR)=0;
	
	virtual void frame_begin()=0;
	virtual void frame_end()=0;
	virtual bool can_render() const=0;
	virtual bool is_idle() const=0;
	
	virtual void render_lock()=0;
	virtual void render_unlock()=0;
	
	virtual void get_viewport_size(int &r_width,int &r_height)=0;
	
	virtual String get_name() const=0;
        
        
        
	static Renderer* create();	
        
        
        
        /* Helpers */
        
	virtual IRef<Material> get_default_material();
	virtual IRef<Material> get_gui_material();
	virtual IRef<Material> get_indicator_material();
	virtual IRef<Texture> get_error_texture();
	virtual IRef<Mesh> get_test_cube();
	virtual IRef<Mesh> make_sphere(int p_lats,int p_lons,float p_scale=1.0);
        
        /* Spatial Indexer */
        
	virtual SpatialIndexer *get_spatial_indexer()=0;
		
	Renderer();
	virtual ~Renderer();
};


class RenderLock {

public:
	RenderLock() { Renderer::get_singleton()->render_lock(); }
	~RenderLock() { Renderer::get_singleton()->render_unlock(); }

};

#endif
