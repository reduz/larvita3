#ifdef OPENGL2_ENABLED

//
// C++ Implementation: render_gl
//
// Description:
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "renderer_gl2.h"
#include "render/default_indexer.h"
#include "iapi_persist.h"
#include "rasterizer_forward_gl2.h"

Renderer_GL2 * Renderer_GL2::singleton=NULL;;



Renderer_GL2 * Renderer_GL2::get_singleton() {

	return Renderer_GL2::singleton;
}


String Renderer_GL2::get_name() const {

	return "OpenGL";
}

void Renderer_GL2::render_lock() {

	_THREAD_SAFE_LOCK_;
}
void Renderer_GL2::render_unlock() {

	_THREAD_SAFE_UNLOCK_;
}


IRef<Texture> Renderer_GL2::create_texture() {

	return IRef<Texture>( memnew( Texture_GL ) );
}
IRef<Material> Renderer_GL2::create_material() {

	return IRef<Material>( memnew( Material_GL ) );
}
IRef<Shader> Renderer_GL2::create_shader() {

	return IRef<Shader>( memnew( Shader_GL2 ) );
}

IRef<Light> Renderer_GL2::create_light() {

	return IRef<Light>( memnew( Light_GL ) );
}
IRef<Skeleton> Renderer_GL2::create_skeleton() {

	return IRef<Skeleton>( memnew( Skeleton_GL ) );
}
IRef<Surface> Renderer_GL2::create_surface() {

	return IRef<Surface>( memnew( Surface_GL2 ) );

}
IRef<Mesh> Renderer_GL2::create_mesh() {

	return IRef<Mesh>( memnew( Mesh_GL2 ) );

}


void Renderer_GL2::render_set_fog(FogMode p_mode, float p_density, float p_start, float p_end, float p_index, Color p_color, FogCoordSrc p_src) {

	if (p_mode == FOG_DISABLED) {
		
		glDisable(GL_FOG);
		return;
	};

	glEnable(GL_FOG);
	
	int val;
	switch (p_mode) {

		default:
		case FOG_LINEAR: {
			val = GL_LINEAR;
		}; break;
		case FOG_EXP: {
			val = GL_EXP;
		}; break;
		case FOG_EXP2: {
			val = GL_EXP2;
		}; break;
	};
	float f = (float)val;
	glFogfv(GL_FOG_MODE, &f);
	
	glFogfv(GL_FOG_DENSITY, &p_density);
	
	glFogfv(GL_FOG_START, &p_start);
	
	glFogfv(GL_FOG_END, &p_end);
	
	glFogfv(GL_FOG_INDEX, &p_index);
	
	float c[4] = { (float)p_color.r / 255.0, (float)p_color.g / 255.0, (float)p_color.b / 255.0, (float)p_color.a / 255.0 }; 
	glFogfv(GL_FOG_COLOR, c);
	
	int src;
	if (p_src == SRC_FOG_COORD) src = GL_FRAGMENT_DEPTH;
	else src = GL_FRAGMENT_DEPTH;
	
	f = (int)src;
	glFogfv(GL_FOG_COORD_SRC, &f);
};


void Renderer_GL2::render_clear(Uint32 p_flags,const Color& p_color,float p_depth) {

	_THREAD_SAFE_METHOD_;

	ERR_FAIL_COND(rasterizer_active);
	
	render_list.cmd_clear(p_flags,p_depth,p_color);
}


void Renderer_GL2::render_scene_begin(IRef<Light>*, int) {

	_THREAD_SAFE_METHOD_;

	ERR_FAIL_COND(rasterizer_active);	
	render_list.cmd_scene_begin(NULL, -1);

}

void Renderer_GL2::render_set_matrix(MatrixMode p_matrix_mode,const Matrix4& p_matrix) {

	_THREAD_SAFE_METHOD_;

	ERR_FAIL_COND(rasterizer_active);	
	render_list.cmd_set_matrix(p_matrix_mode,p_matrix);

}

Matrix4 Renderer_GL2::render_get_matrix(MatrixMode p_matrix_mode) const {

	_THREAD_SAFE_METHOD_;
	
	ERR_FAIL_COND_V(rasterizer_active,Matrix4());	
	return render_list.get_matrix_cache(p_matrix_mode);

}

void Renderer_GL2::render_mesh(IRef<Mesh> p_mesh,IRef<Skeleton> p_skeleton,IRef<Material> p_extra_material)  {
	_THREAD_SAFE_METHOD_;

	ERR_FAIL_COND(rasterizer_active);	
	
	for (int i=0;i<p_mesh->get_surface_count();i++) {
	
		IRef<Surface_GL2> surf=p_mesh->get_surface(i);
		render_list.cmd_add_surface(surf,p_skeleton, p_extra_material);
	}

}


Renderer::LightProfile Renderer_GL2::render_set_lights(IRef<Light>*p_lights,int p_light_count) {

	_THREAD_SAFE_METHOD_;

	ERR_FAIL_COND_V(rasterizer_active, LightProfile());
	LightProfile p;
	p.lights = render_list.cmd_set_lights((IRef<Light_GL>*) p_lights,p_light_count);
	p.light_count = p_light_count;
	return p;
}

void Renderer_GL2::render_set_lights(LightProfile p_lights) {

	_THREAD_SAFE_METHOD_
	render_list.cmd_set_light_profile(p_lights);
};


void Renderer_GL2::render_primitive_change_texture(IRef<Texture> p_texture) {

	_THREAD_SAFE_METHOD_
			
	ERR_FAIL_COND(rasterizer_active);	
	ERR_FAIL_COND(p_texture.is_null());
	render_list.cmd_set_texture(p_texture);


}

void Renderer_GL2::render_primitive_set_material(IRef<Material> p_material) {

	_THREAD_SAFE_METHOD_

	ERR_FAIL_COND(rasterizer_active);	
	render_list.cmd_set_material(p_material);
			

}

void Renderer_GL2::render_primitive(int p_points,const Vector3 *p_vertices, const Vector3 *p_normals,const Vector3 *p_binormals_and_tangents,const Color *p_colors,const float *p_uv) {


	_THREAD_SAFE_METHOD_
	ERR_FAIL_COND(rasterizer_active);	
	render_list.cmd_add_primitive(p_points,p_vertices,p_normals,p_binormals_and_tangents,p_colors,p_uv);

}

void Renderer_GL2::render_to_texture(IRef<Texture> p_texture, Renderer::BufferSource p_src) {
	
	
}

void Renderer_GL2::render_scene_end() {

	_THREAD_SAFE_METHOD_
	ERR_FAIL_COND(rasterizer_active);	
	render_list.cmd_scene_end();

}

void Renderer_GL2::frame_begin() {

	
	render_list.clear();
	glColor3f(1,1,1);
}

void Renderer_GL2::frame_end() {

	_THREAD_SAFE_METHOD_

	if (render_list.get_element_count()==0)
		return;
	rasterizer_active=true;
	rasterizer->raster_list(&render_list);
	rasterizer_active=false;

}

bool Renderer_GL2::can_render() const {

	_THREAD_SAFE_METHOD_;

	return true;

}

bool Renderer_GL2::is_idle() const {

	return false;
}



SpatialIndexer *Renderer_GL2::get_spatial_indexer() {

	return indexer;
}


Renderer* Renderer_GL2::create_func_GL2() {

	return memnew( Renderer_GL2 );
}

void Renderer_GL2::set_as_default() {

	create_func=create_func_GL2;
}

Renderer_GL2::Renderer_GL2() {

	ERR_FAIL_COND( init_gl_extensions() != OK );

	ERR_FAIL_COND(Renderer_GL2::singleton && Renderer_GL2::singleton!=this);
	Renderer_GL2::singleton=this;


	glEnable( GL_DEPTH_TEST );
	glDepthFunc( GL_LEQUAL );
	glFrontFace(GL_CW);
	glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//	glEnable( GL_BLEND );
	indexer = memnew( DefaultIndexer );

	IAPI::MethodInfo texinfo;
	texinfo.parameters.push_back(
	   IAPI::MethodInfo::ParamInfo( Variant::STRING, "file", IAPI::PropertyHint(IAPI::PropertyHint::HINT_FILE,".png;.tga") ) );
	texinfo.parameters.push_back(
	   IAPI::MethodInfo::ParamInfo( Variant::INT, "flags", IAPI::PropertyHint(IAPI::PropertyHint::HINT_FLAGS,
	      "Scissor, Mipmaps, Repeat, Compress, Filters, Intensity, Normalmap", Texture::FLAGS_DEFAULT) ) );

	IAPI_Persist::get_singleton()->register_type("Texture", &Texture_GL::IAPI_DEFAULT_INSTANCE_FUNC,true, texinfo);
	IAPI_Persist::get_singleton()->register_type("Material", &Material_GL::IAPI_DEFAULT_INSTANCE_FUNC,true);
	IAPI_Persist::get_singleton()->register_type("Surface", &Surface_GL2::IAPI_DEFAULT_INSTANCE_FUNC,true);
	IAPI_Persist::get_singleton()->register_type("Skeleton", &Skeleton_GL::IAPI_DEFAULT_INSTANCE_FUNC,true);
	IAPI_Persist::get_singleton()->register_type("Mesh", &Mesh_GL2::IAPI_DEFAULT_INSTANCE_FUNC,true);
	IAPI_Persist::get_singleton()->register_type("Light", &Light_GL::IAPI_DEFAULT_INSTANCE_FUNC,true);
	
	rasterizer=memnew( RasterizerForward_GL2 );
	rasterizer_active=false;
}


Renderer_GL2::~Renderer_GL2() {

	render_list.clear();
	
	memdelete(indexer);
	memdelete(rasterizer);

}


#endif // OPENGL2_ENABLED
