#ifdef OPENGL_ENABLED

//
// C++ Interface: render_gl
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef RENDERER_GL14_H
#define RENDERER_GL14_H

#include "render/renderer.h"
#include "drivers/opengl/material_gl.h"
#include "drivers/opengl/shader_gl2.h"
#include "drivers/opengl/texture_gl.h"
#include "drivers/opengl/surface_gl14.h"
#include "drivers/opengl/mesh_gl14.h"
#include "drivers/opengl/light_gl.h"
#include "drivers/opengl/skeleton_gl.h"
#include "drivers/opengl/render_list_gl14.h"

#include "os/thread_safe.h"
#include "os/thread.h"
#include "os/semaphore.h"
#include "gl_extensions.h"

/**
 *	@author Juan Linietsky <reduzio@gmail.com>
 * OpenGL14 Renderer
 * Due to big limitations in opengl multi-threaded portability, this renderer is not multithreaded.
 * See: http://www.equalizergraphics.com/documentation/parallelOpenGL14FAQ.html
*/

class Rasterizer_GL14;

class Renderer_GL14 : public Renderer {
			
	RenderList_GL14 render_list;
	Rasterizer_GL14 *rasterizer;
	bool rasterizer_active;

	
	static Renderer* create_func_GL14();
	
	SpatialIndexer *indexer;
	
	static Renderer_GL14* singleton;
	
public:
	
	static Renderer_GL14 * get_singleton();
	
	/* Data */

	virtual IRef<Texture> create_texture();
	virtual IRef<Material> create_material();
	virtual IRef<Shader> create_shader();
	virtual IRef<Light> create_light();
	virtual IRef<Skeleton> create_skeleton();
	virtual IRef<Surface> create_surface();
	virtual IRef<Mesh> create_mesh();
									
	/* rendering */
	
	virtual void render_clear(Uint32 p_flags,const Color& p_color=Color(),float p_depth=1.0);

	virtual void render_set_fog(FogMode p_mode, float p_density, float p_start, float p_end, float p_index, Color p_color, FogCoordSrc p_src);
	
	virtual void render_scene_begin(IRef<Light>*p_visible_lights, int p_light_count);	
	virtual void render_set_matrix(MatrixMode p_matrix_mode,const Matrix4& p_matrix);
	virtual Matrix4 render_get_matrix(MatrixMode p_matrix_mode) const;
	
	virtual void render_mesh(IRef<Mesh> p_mesh,IRef<Skeleton> p_skeleton=IRef<Skeleton>(),IRef<Material> p_extra_material=IRef<Material>());

	virtual LightProfile render_set_lights(IRef<Light>*p_lights=NULL,int p_light_count=0);
	void render_set_lights(LightProfile p_lights);
	virtual void render_primitive_change_texture(IRef<Texture> p_texture);
	virtual void render_primitive_set_material(IRef<Material> p_material);
	virtual void render_primitive(int p_points,const Vector3 *p_vertices, const Vector3 *p_normals=NULL,const Vector3 *p_binormals_and_tangents=NULL,const Color *p_colors=NULL,const float *p_uv=NULL);
	
	virtual void render_to_texture(IRef<Texture> p_texture, Renderer::BufferSource p_src);
		
	virtual void render_scene_end();
	
	virtual void render_lock(); // lock render thread
	virtual void render_unlock(); // unlock render thread
	
	virtual void frame_begin();
	virtual void frame_end();
	
	virtual bool can_render() const;
	virtual bool is_idle() const;

	virtual String get_name() const;
	
	virtual void get_viewport_size(int &r_width,int &r_height);	
	virtual SpatialIndexer *get_spatial_indexer();

	static void set_as_default();		
	
	Renderer_GL14();
	~Renderer_GL14();
};

#endif
#endif // OPENGL_ENABLED
