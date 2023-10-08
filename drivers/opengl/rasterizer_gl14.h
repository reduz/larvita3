#ifdef OPENGL_ENABLED
//
// C++ Interface: rasterizer_forward_gl
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef RASTERIZER_FORWARD_GL14_H
#define RASTERIZER_FORWARD_GL14_H

#include "drivers/opengl/renderer_gl14.h"

#include "shaders/pre_light.glsl.h"
#include "shaders/light.glsl.h"
#include "shaders/material.glsl.h"
#include "shaders/post_process.glsl.h"
#include "shaders/blur_copy.glsl.h"
#include "shaders/blur.glsl.h"
#include "shaders/glow.glsl.h"


/**
	@author Juan Linietsky <reduzio@gmail.com>
*/
class Rasterizer_GL14 {
	
	enum {
		
		MAX_GL_LIGHTS=8,		
		MAX_SKINTRANSFORM_VERTICES=4096,
  		MAX_JOINT_DRAWS=64,
			
	};

	GLuint depth_buffer;
	struct FBO {
		GLuint fbo;
		GLuint data;
		int width,height;
		bool active;
		FBO() {
			width = height = fbo = data = 0;
			active = false;
		};
	} data_fbo, light_fbo, compose_fbo;

	struct BlurFBO {
	
		GLuint fbo[2];
		GLuint color[2];
		int width,height;
	} blur_fbo;
	
	int stencil_pass;
	bool use_stencil;
	bool use_glow;

	void check_resize_fbo();
	void init_fbo(FBO* p_fbo, int width=-1, int height=-1);
	void delete_fbo(FBO* p_fbo = NULL);
	
	RenderList_GL14::SurfaceElement* joint_draws[MAX_JOINT_DRAWS];
	
	_FORCE_INLINE_ void setup_material_base(const Material_GL *p_material);
	_FORCE_INLINE_ void setup_material_simple(const Material_GL *p_material);
	_FORCE_INLINE_ void draw_primitive(RenderList_GL14::PrimitiveElement *p);
	_FORCE_INLINE_ void draw_surface(int p_joints=1);
	_FORCE_INLINE_ void draw_surface_data(Surface_GL14 *s,Material_GL *m,int p_index_idx,int p_joints);
	_FORCE_INLINE_ void init_light(int p_idx, Light_GL* p_ptr = NULL);
	_FORCE_INLINE_ void update_lights(Light_GL **p_lights,int p_light_count);
	_FORCE_INLINE_ void setup_visual_element(RenderList_GL14::Visual *p_visual,bool p_setup_matrix=true);

	_FORCE_INLINE_ void do_light_pass(IRef<Light>* p_lights, int p_count);
	_FORCE_INLINE_ void do_glow_pass(Surface_GL14 *s, bool use_skinned_array, int p_count);
	_FORCE_INLINE_ void render_light_aabb(Light_GL* ld);
	
	_FORCE_INLINE_ int do_normal_pass(RenderList_GL14 *p_render_list, int first_idx);
	_FORCE_INLINE_ void draw_surface_normal_pass(int p_joints);
	
	Vector3 *skinned_array;
	
	/* render state */
	
	Matrix4 projection_matrix;
	Matrix4 world_matrix;
	Matrix4 inv_camera_matrix;

	IRef<Light_GL> gl_lights[MAX_GL_LIGHTS];

	PreLightShaderGL pre_light_shader;
	LightShaderGL light_shader;
	MaterialShaderGL material_shader;
	PostProcessShaderGL post_process_shader;
	BlurCopyShaderGL blur_copy_shader;
	BlurShaderGL blur_shader;
	GlowShaderGL glow_shader;

	bool use_shaders;
	bool use_pre_light_pass;
	bool use_texture_light_pass;
	bool inside_scene;

	void blur_compose();
	void copy_quad_using_viewport(float x, float y, float width, float height, float w_width, float w_height);
	
public:
		
	virtual void raster_list(RenderList_GL14 *p_render_list);
	Rasterizer_GL14();	
	~Rasterizer_GL14();

};

#endif
#endif // OPENGL_ENABLED
