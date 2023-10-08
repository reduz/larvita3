#ifdef OPENGL2_ENABLED
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
#ifndef RASTERIZER_FORWARD_GL2_H
#define RASTERIZER_FORWARD_GL2_H

#include "drivers/opengl/renderer_gl2.h"


/**
	@author Juan Linietsky <reduzio@gmail.com>
*/
class RasterizerForward_GL2 : public Rasterizer_GL2 {
	
	enum {
		
		MAX_GL_LIGHTS=8		
	};
	
	void setup_material_base(const Material_GL *p_material);
	void setup_material_simple(const Material_GL *p_material);
	void draw_primitive(RenderList_GL2::PrimitiveElement *p);
	void draw_surface(RenderList_GL2::SurfaceElement *s);
	void draw_surface_data(Surface_GL2 *s,Material_GL *m,int p_index_idx);
	void init_light(int p_idx);
	void update_lights(Light_GL **p_lights,int p_light_count);
	void setup_visual_element(RenderList_GL2::Visual *p_visual);
	
	
	/* render state */
	
	Matrix4 projection_matrix;
	Matrix4 world_matrix;
	Matrix4 inv_camera_matrix;

	IRef<Light_GL> gl_lights[MAX_GL_LIGHTS];
	
public:
		
	virtual void raster_list(RenderList_GL2 *p_render_list);
	RasterizerForward_GL2();	
	~RasterizerForward_GL2();

};

#endif
#endif // OPENGL2_ENABLED
