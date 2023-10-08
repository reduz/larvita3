#ifdef OPENGL_ENABLED

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
#include "renderer_gl14.h"
#include "render/default_indexer.h"
#include "iapi_persist.h"
#include "rasterizer_gl14.h"

Renderer_GL14 * Renderer_GL14::singleton=NULL;;



Renderer_GL14 * Renderer_GL14::get_singleton() {

	return Renderer_GL14::singleton;
}


String Renderer_GL14::get_name() const {

	return "OpenGL";
}

void Renderer_GL14::render_lock() {

	
}
void Renderer_GL14::render_unlock() {

	
}


IRef<Texture> Renderer_GL14::create_texture() {

	return IRef<Texture>( memnew( Texture_GL ) );
}
IRef<Material> Renderer_GL14::create_material() {

	return IRef<Material>( memnew( Material_GL ) );
}
IRef<Shader> Renderer_GL14::create_shader() {

	return IRef<Shader>( memnew( Shader_GL2 ) );
}

IRef<Light> Renderer_GL14::create_light() {

	return IRef<Light>( memnew( Light_GL ) );
}
IRef<Skeleton> Renderer_GL14::create_skeleton() {

	return IRef<Skeleton>( memnew( Skeleton_GL ) );
}
IRef<Surface> Renderer_GL14::create_surface() {

	return IRef<Surface>( memnew( Surface_GL14 ) );

}
IRef<Mesh> Renderer_GL14::create_mesh() {

	return IRef<Mesh>( memnew( Mesh_GL14 ) );

}


void Renderer_GL14::render_set_fog(FogMode p_mode, float p_density, float p_start, float p_end, float p_index, Color p_color, FogCoordSrc p_src) {

	render_list.cmd_set_fog(p_mode,p_density,p_start,p_end,p_index,p_color,p_src);
};


void Renderer_GL14::render_clear(Uint32 p_flags,const Color& p_color,float p_depth) {

	

	ERR_FAIL_COND(rasterizer_active);
	
	render_list.cmd_clear(p_flags,p_depth,p_color);
}


void Renderer_GL14::render_scene_begin(IRef<Light>*p_visible_lights, int p_light_count) {

	ERR_FAIL_COND(rasterizer_active);	
	render_list.cmd_scene_begin(p_visible_lights, p_light_count);
}

void Renderer_GL14::render_set_matrix(MatrixMode p_matrix_mode,const Matrix4& p_matrix) {

	

	ERR_FAIL_COND(rasterizer_active);	
	render_list.cmd_set_matrix(p_matrix_mode,p_matrix);

}

Matrix4 Renderer_GL14::render_get_matrix(MatrixMode p_matrix_mode) const {

	
	
	ERR_FAIL_COND_V(rasterizer_active,Matrix4());	
	return render_list.get_matrix_cache(p_matrix_mode);

}

void Renderer_GL14::render_mesh(IRef<Mesh> p_mesh,IRef<Skeleton> p_skeleton,IRef<Material> p_extra_material)  {
	

	ERR_FAIL_COND(rasterizer_active);	
	
	IRef<Mesh_GL14> mesh;
	mesh.cast_static(p_mesh);
	
	for (int i=0;i<mesh->surfaces.size();i++) {
	
		IRef<Surface_GL14> surf=mesh->surfaces[i];
		render_list.cmd_add_surface(surf,p_skeleton,p_extra_material);
	}

}


Renderer::LightProfile Renderer_GL14::render_set_lights(IRef<Light>*p_lights,int p_light_count) {

	ERR_FAIL_COND_V(rasterizer_active, LightProfile());
	LightProfile p;
	p.lights = render_list.cmd_set_lights((IRef<Light_GL>*) p_lights,p_light_count);
	p.light_count = p_light_count;
	return p;
}

void Renderer_GL14::render_set_lights(LightProfile p_lights) {
	
	render_list.cmd_set_light_profile(p_lights);
};

void Renderer_GL14::render_primitive_change_texture(IRef<Texture> p_texture) {

	
			
	ERR_FAIL_COND(rasterizer_active);	
	ERR_FAIL_COND(p_texture.is_null());
	render_list.cmd_set_texture(p_texture);


}

void Renderer_GL14::render_primitive_set_material(IRef<Material> p_material) {

	

	ERR_FAIL_COND(rasterizer_active);	
	render_list.cmd_set_material(p_material);
			

}

void Renderer_GL14::render_primitive(int p_points,const Vector3 *p_vertices, const Vector3 *p_normals,const Vector3 *p_binormals_and_tangents,const Color *p_colors,const float *p_uv) {


	
	ERR_FAIL_COND(rasterizer_active);	
	render_list.cmd_add_primitive(p_points,p_vertices,p_normals,p_binormals_and_tangents,p_colors,p_uv);

}

void Renderer_GL14::render_to_texture(IRef<Texture> p_texture, Renderer::BufferSource p_src) {
	
	
	render_list.cmd_copy_to_texture(p_texture, p_src);
	
}

void Renderer_GL14::render_scene_end() {

	
	ERR_FAIL_COND(rasterizer_active);	
	render_list.cmd_scene_end();

}

void Renderer_GL14::frame_begin() {

	
	render_list.clear();
	glColor3f(1,1,1);
	glDisable(GL_FOG);
}

void Renderer_GL14::frame_end() {

	

	if (render_list.get_element_count()==0)
		return;
	rasterizer_active=true;
	rasterizer->raster_list(&render_list);
	rasterizer_active=false;

}

bool Renderer_GL14::can_render() const {

	

	return true;

}

bool Renderer_GL14::is_idle() const {

	return false;
}

void Renderer_GL14::get_viewport_size(int &r_width,int &r_height) {

	float vp[4];
	glGetFloatv( GL_VIEWPORT, vp );
	
	r_width=vp[2];
	r_height=vp[3];	
}

SpatialIndexer *Renderer_GL14::get_spatial_indexer() {

	return indexer;
}


Renderer* Renderer_GL14::create_func_GL14() {

	return memnew( Renderer_GL14 );
}

void Renderer_GL14::set_as_default() {

	create_func=create_func_GL14;
}

Renderer_GL14::Renderer_GL14() {

	ERR_FAIL_COND( init_gl_extensions() != OK );

	ERR_FAIL_COND(Renderer_GL14::singleton && Renderer_GL14::singleton!=this);
	Renderer_GL14::singleton=this;


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
	//IAPI_Persist::get_singleton()->register_type("Shader_GL2", &Material_GL::IAPI_DEFAULT_INSTANCE_FUNC,true);
	IAPI_Persist::get_singleton()->register_type("Surface", &Surface_GL14::IAPI_DEFAULT_INSTANCE_FUNC,true);
	IAPI_Persist::get_singleton()->register_type("Skeleton", &Skeleton_GL::IAPI_DEFAULT_INSTANCE_FUNC,true);
	IAPI_Persist::get_singleton()->register_type("Mesh", &Mesh_GL14::IAPI_DEFAULT_INSTANCE_FUNC,true);
	IAPI_Persist::get_singleton()->register_type("Light", &Light_GL::IAPI_DEFAULT_INSTANCE_FUNC,true);
	
	rasterizer=memnew( Rasterizer_GL14 );
	rasterizer_active=false;
}


Renderer_GL14::~Renderer_GL14() {

	render_list.clear();
	
	memdelete(indexer);
	memdelete(rasterizer);

}


#endif // OPENGL_ENABLED
