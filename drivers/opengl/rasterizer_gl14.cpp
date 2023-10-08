#ifdef OPENGL_ENABLED

//
// C++ Implementation: rasterizer_forward_gl
//
// Description:
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "rasterizer_gl14.h"

#include "main/main.h"
#include "GL/glu.h"
#include "types/math/polygon_clipper.h"

#define CHECK_ERROR {\
	int err = glGetError();\
	if (err != GL_NO_ERROR) \
		printf("********************* error %s on line %i\n", gluErrorString(err), __LINE__);\
	}

#define GL_LOAD_MATRIX(m_mtx)\
	if (sizeof(real_t)==4)\
		glLoadMatrixf((float*)(m_mtx).elements);\
	else\
		glLoadMatrixd((double*)(m_mtx).elements);\

#define GL_MULT_MATRIX(m_mtx)\
	if (sizeof(real_t)==4)\
		glMultMatrixf((float*)(m_mtx).elements);\
	else\
		glMultMatrixd((double*)(m_mtx).elements);\



inline void Rasterizer_GL14::setup_material_base(const Material_GL *p_material)  {

	/* Global First */

	if (p_material->flags.double_sided)
		glDisable(GL_CULL_FACE);
	else {
		glEnable(GL_CULL_FACE);
		glCullFace( (p_material->flags.invert_normals)?GL_FRONT:GL_BACK);
	}
	if (p_material->flags.scissor_alpha) {

		if (use_shaders) {
			glDisable(GL_ALPHA_TEST);
			//glEnable(GL_ALPHA_TEST);
			//glAlphaFunc(GL_ALWAYS,0);
			material_shader.set_conditional(MaterialShaderGL::ALPHA_TEST, true);
			material_shader.set_uniform(MaterialShaderGL::ALPHA_THRESHHOLD, p_material->vars.scissor_alpha_treshold);
		} else {
			glEnable(GL_ALPHA_TEST);
			glAlphaFunc(GL_GREATER,p_material->vars.scissor_alpha_treshold);
		};
	} else {

		glDisable(GL_ALPHA_TEST);
		material_shader.set_conditional(MaterialShaderGL::ALPHA_TEST, false);

	}
	
	glEnable(GL_COLOR_MATERIAL); /* unused, unless color array */
	glColorMaterial( GL_FRONT_AND_BACK, GL_DIFFUSE );
	glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );


///ambient @TODO offer global ambient group option
	const Color &ambient_color=p_material->color_ambient;
	float ambient_rgba[4]={
		ambient_color.r/255.0,
		ambient_color.g/255.0,
		ambient_color.b/255.0,
		1.0
	};

	glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,ambient_rgba);
///diffuse
	const Color &diffuse_color=p_material->color_diffuse;
	float diffuse_rgba[4]={
		(float)diffuse_color.r/255.0,
		 (float)diffuse_color.g/255.0,
		  (float)diffuse_color.b/255.0,
		   (float)1.0-p_material->vars.transparency
	};

	glColor4f( diffuse_rgba[0],diffuse_rgba[1],diffuse_rgba[2],diffuse_rgba[3]);

	glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,diffuse_rgba);

//specular

	const Color &specular_color=p_material->color_specular;
	float specular_rgba[4]={
		(float)specular_color.r/255.0,
		 (float)specular_color.g/255.0,
		  (float)specular_color.b/255.0,
		   1.0
	};

	glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,specular_rgba);

	const Color &emission_color=p_material->color_emission;
	float emission_rgba[4]={
		(float)emission_color.r/255.0,
		 (float)emission_color.g/255.0,
		  (float)emission_color.b/255.0,
		   1.0
	};

	glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,emission_rgba);

	glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,p_material->vars.shininess*128.0);

	if (p_material->flags.shaded) {
		material_shader.set_conditional(MaterialShaderGL::NO_LIGHTS, false);
		if (use_pre_light_pass && inside_scene && use_texture_light_pass) {

			//printf("setting texture light pass\n");
			material_shader.set_conditional(MaterialShaderGL::NO_LIGHTS, true);
			material_shader.set_conditional(MaterialShaderGL::USE_TEXTURE_LIGHT_PASS, true);
			glActiveTextureARB(GL_TEXTURE7);
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, light_fbo.data);
			material_shader.set_uniform(MaterialShaderGL::TEXTURE_LIGHT_PASS, 7);
		};
		glEnable(GL_LIGHTING);
	} else {
		material_shader.set_conditional(MaterialShaderGL::NO_LIGHTS, true);
		material_shader.set_conditional(MaterialShaderGL::USE_TEXTURE_LIGHT_PASS, false);
		glDisable(GL_LIGHTING);
	}

	//*
	//printf("flags %i, %i, %i, %i\n", (int)use_pre_light_pass, (int)light_pass_done, p_material->layer, (int)p_material->has_alpha());
	//if (use_pre_light_pass && use_texture_light_pass) {
	if (use_pre_light_pass && inside_scene && use_texture_light_pass) {

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_EQUAL);
	} else {
		
		switch (p_material->flags.test_z) {
	
			case Renderer::DEPTH_TEST_NONE:
				glDisable(GL_DEPTH_TEST);
				break;
			case Renderer::DEPTH_TEST_NEVER:
				glEnable(GL_DEPTH_TEST);
				glDepthFunc(GL_NEVER);
				break;
			case Renderer::DEPTH_TEST_LESS:
				glEnable(GL_DEPTH_TEST);
				glDepthFunc(GL_LESS);
				break;
			case Renderer::DEPTH_TEST_EQUAL:
				glEnable(GL_DEPTH_TEST);
				glDepthFunc(GL_EQUAL);
				break;
			case Renderer::DEPTH_TEST_LEQUAL:
				glEnable(GL_DEPTH_TEST);
				glDepthFunc(GL_LEQUAL);
				break;
			case Renderer::DEPTH_TEST_GREATER:
				glEnable(GL_DEPTH_TEST);
				glDepthFunc(GL_GREATER);
				break;
			case Renderer::DEPTH_TEST_NOTEQUAL:
				glEnable(GL_DEPTH_TEST);
				glDepthFunc(GL_NOTEQUAL);
				break;
			case Renderer::DEPTH_TEST_GEQUAL:
				glEnable(GL_DEPTH_TEST);
				glDepthFunc(GL_GEQUAL);
				break;
			case Renderer::DEPTH_TEST_ALWAYS:
				glEnable(GL_DEPTH_TEST);
				glDepthFunc(GL_ALWAYS);
				break;
		};
	};
	//	*/


	//	if (p_material->flags.draw_z) {
	if(p_material->has_alpha()) {
		glDepthMask(GL_FALSE);
		glEnable(GL_BLEND);
	} else {
		glDepthMask(GL_TRUE);
		glDisable(GL_BLEND);
	};

	if (p_material->flags.wireframe)
		glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);

	if (p_material->textures.diffuse) {
		material_shader.set_conditional(MaterialShaderGL::USE_TEXTURE_BASE, true);
	} else {
		material_shader.set_conditional(MaterialShaderGL::USE_TEXTURE_BASE, false);
	};

	if (p_material->textures.diffuse) {
		material_shader.set_uniform(MaterialShaderGL::TEXTURE_BASE, (int)0);
	};
	
	if (use_glow) {
		if (p_material->has_alpha()) {
			glColorMask(1, 1, 1, 0);
			material_shader.set_conditional(MaterialShaderGL::USE_GLOW, false);
		} else {
			material_shader.set_uniform(MaterialShaderGL::GLOW, p_material->vars.glow);
			material_shader.set_conditional(MaterialShaderGL::USE_GLOW, true);
			glColorMask(1, 1, 1, 1);
		};
	};

	/* texture */
}


inline void Rasterizer_GL14::setup_material_simple(const Material_GL *p_material)  {

	setup_material_base(p_material);

}

static const GLenum prim_type[4]={GL_POINTS,GL_LINES,GL_TRIANGLES,GL_QUADS};

inline void Rasterizer_GL14::draw_primitive(RenderList_GL14::PrimitiveElement *p)  {


	int pcount = (p->flags&RenderList_GL14::PrimitiveElement::POINT_MASK)+1;
	
	glow_shader.set_conditional(GlowShaderGL::USE_TEXTURE_GLOWMAP, false);
	glow_shader.set_uniform(GlowShaderGL::GLOW, p->material->vars.glow);

	if (!(p->flags&RenderList_GL14::PrimitiveElement::HAS_UVS)) {

		glClientActiveTextureARB(GL_TEXTURE0);
		material_shader.set_conditional(MaterialShaderGL::USE_TEXTURE_BASE, false);
		glDisable(GL_TEXTURE_2D);

	} else {
		material_shader.set_conditional(MaterialShaderGL::USE_TEXTURE_BASE, true);
		glow_shader.set_conditional(GlowShaderGL::USE_TEXTURE_BASE, false);
		if (p->texture) {
			glActiveTextureARB(GL_TEXTURE0);
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D,p->texture->get_gl_tex_id());
			material_shader.set_uniform(MaterialShaderGL::TEXTURE_BASE, (int)0);
			
			glow_shader.set_uniform(GlowShaderGL::TEXTURE_BASE, (int)0);
			glow_shader.set_conditional(GlowShaderGL::USE_TEXTURE_BASE, true);
		}
	}
	material_shader.set_conditional(MaterialShaderGL::USE_TEXTURE_NORMALMAP, false);
	material_shader.set_conditional(MaterialShaderGL::USE_TEXTURE_SPECULAR, false);
	material_shader.set_conditional(MaterialShaderGL::USE_TEXTURE_EMISSION, false);
	material_shader.set_conditional(MaterialShaderGL::USE_TEXTURE_REFLECTION, false);
	material_shader.set_conditional(MaterialShaderGL::USE_TEXTURE_REFMASK, false);
	material_shader.set_conditional(MaterialShaderGL::USE_TEXTURE_GLOWMAP, false);

	if (p->has_alpha) {
		glEnable(GL_BLEND);
		glDepthMask(GL_FALSE);
	} else {
		glDisable(GL_BLEND);
		glDepthMask(GL_TRUE);
	}


	if (!(p->flags&RenderList_GL14::PrimitiveElement::HAS_COLORS))
		glColor4f(1,1,1,1);

	if (p->flags&RenderList_GL14::PrimitiveElement::HAS_UVS && p->texture) {
		material_shader.set_uniform(MaterialShaderGL::TEXTURE_BASE, (int)0);
	};

	if (use_shaders && inside_scene) {
		material_shader.bind();
	} else if (glUseProgram) {
		glUseProgram(0);
	};

	int pass_count = 1;
	if (inside_scene && use_glow && p->has_alpha)
		pass_count = p->has_alpha?2:1;

	for (int pass = 0; pass < pass_count; pass++) {

		if (pass == 1) {
			glColorMask(0, 0, 0, 1);
		
			glEnable(GL_BLEND);
			glBlendFunc(GL_ONE, GL_ONE);
			
			if (p->material->flags.double_sided)
				glDisable(GL_CULL_FACE);
			else {
				glEnable(GL_CULL_FACE);
				glCullFace( (p->material->flags.invert_normals)?GL_FRONT:GL_BACK);
			}
		
			glDepthMask(GL_FALSE);
			glEnable(GL_DEPTH_TEST);
			glDepthFunc(GL_LEQUAL);
			glow_shader.bind();
		};

		glBegin( prim_type[p->flags&RenderList_GL14::PrimitiveElement::POINT_MASK] );
		
		for (int i=0;i<pcount; i++ ) {
	
			if (p->flags&RenderList_GL14::PrimitiveElement::HAS_BINORMAL_TANGENT) {
	
			/* uh?
				*/	ERR_PRINT("Unimplemented");
	
			} else if (p->flags&RenderList_GL14::PrimitiveElement::HAS_NORMALS) {
	
				glNormal3f(
					   (float)p->normals[i*3+0]/127.0,
						(float)p->normals[i*3+1]/127.0,
						 (float)p->normals[i*3+2]/127.0
					  );
			}
	
			if (p->flags&RenderList_GL14::PrimitiveElement::HAS_COLORS) {
	
				glColor4f(
					  (float)p->colors[i].r/255.0,
					   (float)p->colors[i].g/255.0,
						(float)p->colors[i].b/255.0,
						 (float)p->colors[i].a/255.0
					 );
			}
	
			if (p->flags&RenderList_GL14::PrimitiveElement::HAS_UVS) {
	
				glTexCoord2f(p->uvs[i*2+0],p->uvs[i*2+1]  );
			}
	
			glVertex3f(p->vertices[i].x, p->vertices[i].y, p->vertices[i].z
				  );
	
	
		}
	
		glEnd();
		if (pass == 1) {
			glColorMask(1, 1, 1, 1);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		};
	};

	if (p->texture) {

		glDisable(GL_TEXTURE_2D);
	}
}

inline void Rasterizer_GL14::draw_surface(int p_joints)  {

	RenderList_GL14::SurfaceElement *s=joint_draws[0];
	Surface_GL14 *surf = s->surface;
	ERR_FAIL_COND(!surf);
	ERR_FAIL_COND(!surf->configured);
	ERR_FAIL_COND( !surf->use_VBO && surf->vertex_mem.is_null());

	const Uint8 *vertex_memptr=NULL;
	if (surf->use_VBO) {

		glBindBufferARB(GL_ARRAY_BUFFER,surf->vertex_id);
	} else {
		Memory::lock( surf->vertex_mem );
		vertex_memptr=(const Uint8*)Memory::get( surf->vertex_mem );
	}

	if (s->has_alpha || ( s->extra_material && s->extra_material->has_alpha() )) {
		glEnable(GL_BLEND);
		glDepthMask(GL_FALSE);
	} else {
		glDisable(GL_BLEND);
		glDepthMask(GL_TRUE);
	}

	int index_array_idx=-1;
	bool use_skinning;
	const Uint8* bones_ptr;
	
	for (int i=0;i<surf->array_count;i++) {

		const Surface_GL14::ArrayData &a=surf->array[i];

		if (!a.configured) {
			ERR_FAIL_COND( a.type==Surface::ARRAY_VERTEX );
			ERR_CONTINUE( !a.configured );
		}

		switch(a.type) {

			case Surface::ARRAY_VERTEX: {
				glEnableClientState(GL_VERTEX_ARRAY);
				if (surf->use_VBO)
					glVertexPointer(3,GL_FLOAT,surf->stride,(GLvoid*)a.ofs);
				else
					glVertexPointer(3,GL_FLOAT,surf->stride,(GLvoid*)&vertex_memptr[a.ofs]);

			} break;
			case Surface::ARRAY_NORMAL: {
				glEnableClientState(GL_NORMAL_ARRAY);
				if (surf->use_VBO)
					glNormalPointer(GL_FLOAT,surf->stride,(GLvoid*)a.ofs);
				else
					glNormalPointer(GL_FLOAT,surf->stride,(GLvoid*)&vertex_memptr[a.ofs]);

			} break;
			case Surface::ARRAY_BINORMAL: {

				glClientActiveTextureARB(GL_TEXTURE7);
				glEnableClientState(GL_TEXTURE_COORD_ARRAY);
				if (surf->use_VBO)
					glTexCoordPointer(3,GL_FLOAT,surf->stride,(GLvoid*)a.ofs);
				else
					glTexCoordPointer(3,GL_FLOAT,surf->stride,&vertex_memptr[a.ofs]);

			} break;

			case Surface::ARRAY_TANGENT: {

				glClientActiveTextureARB(GL_TEXTURE6);
				glEnableClientState(GL_TEXTURE_COORD_ARRAY);
				if (surf->use_VBO)
					glTexCoordPointer(4,GL_FLOAT,surf->stride,(GLvoid*)a.ofs);
				else
					glTexCoordPointer(4,GL_FLOAT,surf->stride,&vertex_memptr[a.ofs]);
			} break;
			case Surface::ARRAY_COLOR: {
				glEnableClientState(GL_COLOR_ARRAY);
				if (surf->use_VBO)
					glColorPointer(4,GL_UNSIGNED_BYTE,surf->stride,(GLvoid*)a.ofs);
				else
					glColorPointer(4,GL_UNSIGNED_BYTE,surf->stride,(GLvoid*)&vertex_memptr[a.ofs]);
			} break;
			case Surface::ARRAY_TEX_UV_0:
			case Surface::ARRAY_TEX_UV_1:
			case Surface::ARRAY_TEX_UV_2:
			case Surface::ARRAY_TEX_UV_3:
			case Surface::ARRAY_TEX_UV_4:
			case Surface::ARRAY_TEX_UV_5:
			case Surface::ARRAY_TEX_UV_6:
				case Surface::ARRAY_TEX_UV_7: {
					glClientActiveTextureARB(GL_TEXTURE0 + (a.type-Surface::ARRAY_TEX_UV_0));
					glEnableClientState(GL_TEXTURE_COORD_ARRAY);
					if (surf->use_VBO)
						glTexCoordPointer(2,GL_FLOAT,surf->stride,(GLvoid*)a.ofs);
					else
						glTexCoordPointer(2,GL_FLOAT,surf->stride,&vertex_memptr[a.ofs]);
				} break;
			case Surface::ARRAY_WEIGHTS: {

				// move the vertexpointer
				use_skinning=true;
				bones_ptr=&vertex_memptr[a.ofs];

				/* ? */
			} break;
			case Surface::ARRAY_CUSTOM_0:
			case Surface::ARRAY_CUSTOM_1:
			case Surface::ARRAY_CUSTOM_2:
				case Surface::ARRAY_CUSTOM_3: {

					/* */
				};

				case Surface::ARRAY_INDEX: {

				//index array must be configured to work
					index_array_idx=i;
					/* */
				} break;

		}


	}

	/* process skinning via software*/

	bool use_skinned_array = false;
	if (s->skeleton && s->skeleton->get_bones_ptr() && use_skinning && bones_ptr && surf->array_len < MAX_SKINTRANSFORM_VERTICES) {

		const Matrix4 * skeleton  = s->skeleton->get_bones_ptr();

		int skeleton_size = s->skeleton->get_bone_count();

		glVertexPointer(3,(sizeof(real_t)==4)?GL_FLOAT:GL_DOUBLE,0,skinned_array);

		int stride=surf->stride;
		Vector3 vtmp;

		for (int j=0;j<surf->array_len;j++) {

			const float *vtx=(float*)(&vertex_memptr[j*surf->stride]);
			skinned_array[j]=Vector3();

			Vector3 v( vtx[0], vtx[1], vtx[2] );
			const Uint8 *bone_ptr=&bones_ptr[j*stride];

#define _TRANSBONE(m_v)\
	ERR_CONTINUE( bone_ptr[m_v<<1] >= skeleton_size );\
	skeleton[ bone_ptr[m_v<<1] ].transform_copy( v, vtmp );\
	skinned_array[j]+=vtmp*(bone_ptr[(m_v<<1)+1]*(1/255.0));\
	if ( !bone_ptr[(m_v<<1)+1] )\
		continue;

			_TRANSBONE(0);
			_TRANSBONE(1);
			_TRANSBONE(2);
			_TRANSBONE(3);

		}

		use_skinned_array = true;
	}
#undef _TRANSBONE
	/* draw by material */
	Material_GL *material = surf->material.is_null() ? IRef<Material_GL>(Renderer::get_singleton()->get_default_material()).ptr() : surf->material.ptr();
	ERR_FAIL_COND(!material);

	while(material) {

		//if (!(use_pre_light_pass && light_pass_shader)) {
		setup_material_base( material );
		//};

		// check if shader must be enabled
		/*
		Shader_GL2 *shader=NULL;
		if (!material->shader.is_null()) {

			shader=material->shader.ptr();
		}

		if (shader && shader->active) {

			glUseProgram(shader->program_id);
		}
		*/


		draw_surface_data( surf, material,index_array_idx,p_joints );
		material=!material->next_pass.is_null()?material->next_pass.ptr():NULL;

		// disable shader
		/*
		if (shader && shader->active) {

			glUseProgram(0);
		}*/

	}

	if (s->extra_material) {

		setup_material_base( s->extra_material );
		draw_surface_data( surf, s->extra_material,index_array_idx,p_joints );
	}
	
	if (use_glow && surf->has_alpha()) {
		do_glow_pass(surf, use_skinned_array, p_joints);
	};

	glDisableClientState(GL_EDGE_FLAG_ARRAY);
	glDisableClientState(GL_INDEX_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_SECONDARY_COLOR_ARRAY);

	if (surf->use_VBO && index_array_idx!=-1 && surf->index_array_len>0)
		glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER,0);

	if (!surf->use_VBO)
		Memory::unlock( surf->vertex_mem );
	else
		glBindBufferARB(GL_ARRAY_BUFFER,0);

	
}

static const GLenum gl_primitive[7]={ GL_POINTS, GL_LINES, GL_LINE_STRIP,GL_LINE_LOOP, GL_TRIANGLES, GL_TRIANGLE_STRIP, GL_TRIANGLE_FAN };

void Rasterizer_GL14::do_glow_pass(Surface_GL14 *s, bool use_skinned_array, int p_joints) {
	
	glColorMask(0, 0, 0, 1);

	glDisable(GL_ALPHA_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);
	
	if (s->material->flags.double_sided)
		glDisable(GL_CULL_FACE);
	else {
		glEnable(GL_CULL_FACE);
		glCullFace( (s->material->flags.invert_normals)?GL_FRONT:GL_BACK);
	}

	glDepthMask(GL_FALSE);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	
	const Uint8 *vertex_memptr=NULL;
	if (s->use_VBO) {
		glBindBufferARB(GL_ARRAY_BUFFER,s->vertex_id);
	} else {
		Memory::lock( s->vertex_mem );
		vertex_memptr=(const Uint8*)Memory::get( s->vertex_mem );
	}

	int index_array_idx = -1;
	
	for (int i=0;i<s->array_count;i++) {

		const Surface_GL14::ArrayData &a=s->array[i];
		if (!a.configured) {
			ERR_FAIL_COND( a.type==Surface::ARRAY_VERTEX );
			ERR_CONTINUE( !a.configured );
		}

		switch(a.type) {

			case Surface::ARRAY_VERTEX: {
					
				glEnableClientState(GL_VERTEX_ARRAY);
				if (use_skinned_array) {
					glVertexPointer(3,(sizeof(real_t)==4)?GL_FLOAT:GL_DOUBLE,0,skinned_array);
				} else if (s->use_VBO)
					glVertexPointer(3,GL_FLOAT,s->stride,(GLvoid*)a.ofs);
				else
					glVertexPointer(3,GL_FLOAT,s->stride,(GLvoid*)&vertex_memptr[a.ofs]);
			} break;

			case Surface::ARRAY_NORMAL: {
				if (!s->material->flags.glow_spherical) {
					break;
				};
				glEnableClientState(GL_NORMAL_ARRAY);
				if (s->use_VBO)
					glNormalPointer(GL_FLOAT,s->stride,(GLvoid*)a.ofs);
				else
					glNormalPointer(GL_FLOAT,s->stride,(GLvoid*)&vertex_memptr[a.ofs]);
			} break;
			
			case Surface::ARRAY_TEX_UV_0: {
				if (s->material->textures.diffuse || (s->material->textures.glowmap && !s->material->flags.glow_spherical)) {
					glClientActiveTextureARB(GL_TEXTURE0);
					glEnableClientState(GL_TEXTURE_COORD_ARRAY);
					if (s->use_VBO)
						glTexCoordPointer(2,GL_FLOAT,s->stride,(GLvoid*)a.ofs);
					else
						glTexCoordPointer(2,GL_FLOAT,s->stride,&vertex_memptr[a.ofs]);
				};
			} break;
			case Surface::ARRAY_INDEX: {

			//index array must be configured to work
				index_array_idx=i;
				/* */
			} break;
		};
	};

	if (s->material->textures.diffuse) {
		
		glow_shader.set_conditional(GlowShaderGL::USE_TEXTURE_BASE, true);
		glActiveTextureARB(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, s->material->textures.diffuse->get_gl_tex_id());
		glow_shader.set_uniform(GlowShaderGL::TEXTURE_BASE, (int)0);
	};
	
	if (s->material->textures.glowmap) {

		glActiveTextureARB(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, s->material->textures.glowmap->get_gl_tex_id());
		glow_shader.set_conditional(GlowShaderGL::USE_TEXTURE_GLOWMAP, true);
		glow_shader.set_uniform(GlowShaderGL::TEXTURE_GLOWMAP, (int)1);
		glow_shader.set_conditional(GlowShaderGL::TEXTURE_GLOW_SPHERE, s->material->flags.glow_spherical);
	} else {
		glow_shader.set_conditional(GlowShaderGL::USE_TEXTURE_GLOWMAP, false);
		glow_shader.set_conditional(GlowShaderGL::TEXTURE_GLOW_SPHERE, false);
	};
	glow_shader.set_uniform(GlowShaderGL::GLOW, s->material->vars.glow);

	glow_shader.bind();

	for (int i=0;i<p_joints;i++) {

		glMatrixMode(GL_MODELVIEW);

		//GL_LOAD_MATRIX(p_visual->world_matrix);
		//GL_MULT_MATRIX(inv_camera_matrix);
		GL_LOAD_MATRIX(inv_camera_matrix);
		if (joint_draws[i]->world_matrix) {
			GL_MULT_MATRIX(*joint_draws[i]->world_matrix);
		};
	
		if (index_array_idx!=-1 && s->index_array_len>0) {
			if (s->use_VBO) {
				glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER,s->index_id);
				glDrawElements(gl_primitive[s->primitive],s->index_array_len, (s->index_array_len>(1<<16))?GL_UNSIGNED_INT:GL_UNSIGNED_SHORT,0);
			} else if (!s->index_mem.is_null()) {
				Memory::lock(s->index_mem);
				glDrawElements(gl_primitive[s->primitive],s->index_array_len, (s->index_array_len>(1<<16))?GL_UNSIGNED_INT:GL_UNSIGNED_SHORT,Memory::get(s->index_mem));
				Memory::unlock(s->index_mem);
			}
		} else {
			glDrawArrays(gl_primitive[s->primitive],0,s->array_len);
		}
	};
	
	glDisableClientState(GL_INDEX_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	
	glColorMask(1, 1, 1, 1);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
};

static int directional_light_conditionals[] = {
				MaterialShaderGL::LIGHT_0_DIRECTIONAL,
				MaterialShaderGL::LIGHT_1_DIRECTIONAL,
				MaterialShaderGL::LIGHT_2_DIRECTIONAL,
				MaterialShaderGL::LIGHT_3_DIRECTIONAL,
				MaterialShaderGL::LIGHT_4_DIRECTIONAL,
				MaterialShaderGL::LIGHT_5_DIRECTIONAL,
				MaterialShaderGL::LIGHT_6_DIRECTIONAL,
				MaterialShaderGL::LIGHT_7_DIRECTIONAL
};
static int omni_light_conditionals[] = {
				MaterialShaderGL::LIGHT_0_OMNI,
				MaterialShaderGL::LIGHT_1_OMNI,
				MaterialShaderGL::LIGHT_2_OMNI,
				MaterialShaderGL::LIGHT_3_OMNI,
				MaterialShaderGL::LIGHT_4_OMNI,
				MaterialShaderGL::LIGHT_5_OMNI,
				MaterialShaderGL::LIGHT_6_OMNI,
				MaterialShaderGL::LIGHT_7_OMNI,
};

static int spot_light_conditionals[] = {
				MaterialShaderGL::LIGHT_0_SPOT,
				MaterialShaderGL::LIGHT_1_SPOT,
				MaterialShaderGL::LIGHT_2_SPOT,
				MaterialShaderGL::LIGHT_3_SPOT,
				MaterialShaderGL::LIGHT_4_SPOT,
				MaterialShaderGL::LIGHT_5_SPOT,
				MaterialShaderGL::LIGHT_6_SPOT,
				MaterialShaderGL::LIGHT_7_SPOT,
};

inline void Rasterizer_GL14::draw_surface_data(Surface_GL14 *s,Material_GL *m,int p_index_idx,int p_joints)  {

	int texture_count = 0;
	
	if (m->textures.diffuse.is_null()) {

		material_shader.set_conditional(MaterialShaderGL::USE_TEXTURE_BASE, false);
		glActiveTextureARB(GL_TEXTURE0 + texture_count);
		glDisable(GL_TEXTURE_2D);
	} else {
		material_shader.set_conditional(MaterialShaderGL::USE_TEXTURE_BASE, true);
		glActiveTextureARB(GL_TEXTURE0 + texture_count);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D,m->textures.diffuse->get_gl_tex_id());
		material_shader.set_uniform(MaterialShaderGL::TEXTURE_BASE, (int)texture_count++);
	}

	if (use_shaders) {
		
		if (m->textures.normalmap && !use_texture_light_pass) {
			glActiveTextureARB(GL_TEXTURE0 + texture_count);
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, m->textures.normalmap->get_gl_tex_id());
			material_shader.set_uniform(MaterialShaderGL::TEXTURE_NORMAL, (int)texture_count++);
			material_shader.set_conditional(MaterialShaderGL::USE_TEXTURE_NORMALMAP, true);
		} else {

			glActiveTextureARB(GL_TEXTURE0 + texture_count);
			glDisable(GL_TEXTURE_2D);
			material_shader.set_conditional(MaterialShaderGL::USE_TEXTURE_NORMALMAP, false);
		};

		if (m->textures.specular) {

			material_shader.set_conditional(MaterialShaderGL::USE_TEXTURE_SPECULAR, true);
			glActiveTextureARB(GL_TEXTURE0 + texture_count);
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D,m->textures.specular->get_gl_tex_id());
			material_shader.set_uniform(MaterialShaderGL::TEXTURE_SPECULAR, (int)texture_count++);
		} else {
			glActiveTextureARB(GL_TEXTURE0 + texture_count);
			glDisable(GL_TEXTURE_2D);
			material_shader.set_conditional(MaterialShaderGL::USE_TEXTURE_SPECULAR, false);
		};

		if (m->textures.emission) {

			material_shader.set_conditional(MaterialShaderGL::USE_TEXTURE_EMISSION, true);
			glActiveTextureARB(GL_TEXTURE0 + texture_count);
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D,m->textures.emission->get_gl_tex_id());
			material_shader.set_uniform(MaterialShaderGL::TEXTURE_EMISSION, (int)texture_count++);
			if (m->flags.emission_spherical) {
				material_shader.set_conditional(MaterialShaderGL::TEXTURE_EMISSION_SPHERE, true);
			} else {
				material_shader.set_conditional(MaterialShaderGL::TEXTURE_EMISSION_SPHERE, false);
			};					
				
		} else {
			glActiveTextureARB(GL_TEXTURE0 + texture_count);
			glDisable(GL_TEXTURE_2D);
			material_shader.set_conditional(MaterialShaderGL::USE_TEXTURE_EMISSION, false);
		};
		
		if (!m->textures.reflection.is_null()) {

			material_shader.set_conditional(MaterialShaderGL::USE_TEXTURE_REFLECTION, true);
			glActiveTextureARB(GL_TEXTURE0 + texture_count);
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D,m->textures.reflection->get_gl_tex_id());
			material_shader.set_uniform(MaterialShaderGL::TEXTURE_REFLECTION, (int)texture_count++);
		} else {
			material_shader.set_conditional(MaterialShaderGL::USE_TEXTURE_REFLECTION, false);
			glActiveTextureARB(GL_TEXTURE0 + texture_count);
			glDisable(GL_TEXTURE_2D);
		};

		if (m->textures.refmask) {

			material_shader.set_conditional(MaterialShaderGL::USE_TEXTURE_REFMASK, true);
			glActiveTextureARB(GL_TEXTURE0 + texture_count);
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D,m->textures.refmask->get_gl_tex_id());
			material_shader.set_uniform(MaterialShaderGL::TEXTURE_REFMASK, (int)texture_count++);
		} else {
			material_shader.set_conditional(MaterialShaderGL::USE_TEXTURE_REFMASK, false);
			glActiveTextureARB(GL_TEXTURE0 + texture_count);
			glDisable(GL_TEXTURE_2D);
		};
		
		if (m->textures.glowmap) {

			material_shader.set_conditional(MaterialShaderGL::USE_TEXTURE_GLOWMAP, true);
			glActiveTextureARB(GL_TEXTURE0 + texture_count);
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D,m->textures.glowmap->get_gl_tex_id());
			material_shader.set_uniform(MaterialShaderGL::TEXTURE_GLOWMAP, (int)texture_count++);
			if (m->flags.glow_spherical) {
				material_shader.set_conditional(MaterialShaderGL::TEXTURE_GLOW_SPHERE, true);
			} else {
				material_shader.set_conditional(MaterialShaderGL::TEXTURE_GLOW_SPHERE, false);
			};
		} else {
			glActiveTextureARB(GL_TEXTURE0 + texture_count);
			glDisable(GL_TEXTURE_2D);
			material_shader.set_conditional(MaterialShaderGL::USE_TEXTURE_GLOWMAP, false);
		};
	};

	if (use_shaders) {
		material_shader.bind();
	};

	for (int i=0;i<p_joints;i++) {

		glMatrixMode(GL_MODELVIEW);

		//GL_LOAD_MATRIX(p_visual->world_matrix);
		//GL_MULT_MATRIX(inv_camera_matrix);
		GL_LOAD_MATRIX(inv_camera_matrix);
		if (joint_draws[i]->world_matrix) {
			GL_MULT_MATRIX(*joint_draws[i]->world_matrix);
		};

		if (p_index_idx!=-1 && s->index_array_len>0) {

			if (s->use_VBO) {

				glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER,s->index_id);
				glDrawElements(gl_primitive[s->primitive],s->index_array_len, (s->index_array_len>(1<<16))?GL_UNSIGNED_INT:GL_UNSIGNED_SHORT,0);
			} else if (!s->index_mem.is_null()) {
				Memory::lock(s->index_mem);
				glDrawElements(gl_primitive[s->primitive],s->index_array_len, (s->index_array_len>(1<<16))?GL_UNSIGNED_INT:GL_UNSIGNED_SHORT,Memory::get(s->index_mem));
				Memory::unlock(s->index_mem);
			}


		} else {

			glDrawArrays(gl_primitive[s->primitive],0,s->array_len);
		}
	}

	for (int i=0; i<texture_count; i++) {
		glActiveTextureARB(GL_TEXTURE0+i);
		glDisable(GL_TEXTURE_2D);
	};
}


inline void Rasterizer_GL14::init_light(int p_idx, Light_GL* p_ptr)  {

	ERR_FAIL_INDEX(p_idx,MAX_GL_LIGHTS);
	ERR_FAIL_COND( gl_lights[p_idx].is_null() && p_ptr == NULL );


	const Light_GL *ld=p_ptr?p_ptr:gl_lights[p_idx].ptr();
	GLuint glid=GL_LIGHT0+p_idx;

	glEnable(glid);

	float rgba[4]={0,0,0,0};

	rgba[0]=(float)ld->ambient.r/255.0;
	rgba[1]=(float)ld->ambient.g/255.0;
	rgba[2]=(float)ld->ambient.b/255.0;
	rgba[3]=(float)ld->ambient.a/255.0;

	glLightfv(glid , GL_AMBIENT, rgba );

	rgba[0]=(float)ld->diffuse.r/255.0;
	rgba[1]=(float)ld->diffuse.g/255.0;
	rgba[2]=(float)ld->diffuse.b/255.0;
	rgba[3]=(float)ld->diffuse.a/255.0;

	glLightfv(glid, GL_DIFFUSE, rgba );

	rgba[0]=(float)ld->specular.r/255.0;
	rgba[1]=(float)ld->specular.g/255.0;
	rgba[2]=(float)ld->specular.b/255.0;
	rgba[3]=(float)ld->specular.a/255.0;


	glLightfv(glid, GL_SPECULAR, rgba );

	switch(ld->mode) {

		case Light::MODE_DIRECTIONAL: {
			/* This doesnt have attenuation */
			material_shader.set_conditional(directional_light_conditionals[p_idx], true);
			glMatrixMode(GL_MODELVIEW);
			glPushMatrix();
			glLoadIdentity();
			Vector3 v(0.0,0.0,-1.0); // directional lights point up by default
			ld->matrix.transform_no_translation( v );
			inv_camera_matrix.transform_no_translation( v );
			v.normalize(); // this sucks, so it will be optimized at some point
			v = -v;
			float lightpos[4]={v.get_x(),v.get_y(),v.get_z(),0.0};

			glLightfv(glid,GL_POSITION,lightpos); //at modelview
			
			glPopMatrix();

			glLightf(glid,GL_LINEAR_ATTENUATION,ld->radius); // for shadows

		} break;
		case Light::MODE_POINT: {

			material_shader.set_conditional(omni_light_conditionals[p_idx], true);
			glLightf(glid,GL_SPOT_CUTOFF,180.0);
			glLightf(glid,GL_CONSTANT_ATTENUATION,ld->attenuation);
			glLightf(glid,GL_LINEAR_ATTENUATION,ld->radius);
			glLightf(glid,GL_QUADRATIC_ATTENUATION,ld->intensity);


			glMatrixMode(GL_MODELVIEW);
			glPushMatrix();
			glLoadIdentity();
			Vector3 pos = ld->matrix.get_translation();
			inv_camera_matrix.transform(pos);
			float lightpos[4]={pos.x,pos.y,pos.z,1.0};
			glLightfv(glid,GL_POSITION,lightpos); //at modelview

			glPopMatrix();
		/*
			glLightf(glid,GL_SPOT_CUTOFF,180.0);
			glLightf(glid,GL_CONSTANT_ATTENUATION,ld->attenuation.constant);
			glLightf(glid,GL_LINEAR_ATTENUATION,ld->attenuation.linear);
			glLightf(glid,GL_QUADRATIC_ATTENUATION,ld->attenuation.quadratic);
			float lightpos[4]={0.0,0.0,0.0,1.0};

			glMatrixMode(GL_MODELVIEW);
			glPushMatrix();
			GL_LOAD_MATRIX(ld->matrix);
			GL_MULT_MATRIX((double*)inv_camera_matrix.elements);

			glLightfv(glid,GL_POSITION,lightpos); //at modelview

			glPopMatrix();
		*/

		} break;
		case Light::MODE_SPOT: {
			material_shader.set_conditional(spot_light_conditionals[p_idx], true);
			glLightf(glid,GL_SPOT_CUTOFF, ld->spot_angle);
			glLightf(glid,GL_SPOT_EXPONENT, ld->spot_intensity);
			glLightf(glid,GL_CONSTANT_ATTENUATION,ld->attenuation);
			glLightf(glid,GL_LINEAR_ATTENUATION,ld->radius);
			glLightf(glid,GL_QUADRATIC_ATTENUATION,ld->intensity);


			glMatrixMode(GL_MODELVIEW);
			glPushMatrix();
			glLoadIdentity();

			Vector3 pos(0, 0, -1);
			ld->matrix.transform_no_translation(pos);
			inv_camera_matrix.transform_no_translation(pos);
			pos.normalize();
			float lightdir[4]={pos.x,pos.y,pos.z,1.0};
			glLightfv(glid,GL_SPOT_DIRECTION,lightdir); //at modelview

			pos = ld->matrix.get_translation();
			inv_camera_matrix.transform(pos);
			float lightpos[4]={pos.x,pos.y,pos.z,1.0};
			glLightfv(glid,GL_POSITION,lightpos); //at modelview


			glPopMatrix();

			if (ld->shadow_texture) {

				material_shader.set_conditional(MaterialShaderGL::SPOT_SHADOW, true);
				glActiveTextureARB(GL_TEXTURE4);
				glEnable(GL_TEXTURE_2D);
				glMatrixMode(GL_TEXTURE);
				Matrix4 proj;
				proj.set_projection( ld->spot_angle, 1, 0, ld->radius );
					//GL_LOAD_MATRIX((inv_camera_matrix * ld->matrix).inverse() * proj);
				GL_LOAD_MATRIX( proj * ld->matrix.inverse() * inv_camera_matrix.inverse());
				glBindTexture(GL_TEXTURE_2D, ld->shadow_texture->get_gl_tex_id());
				material_shader.set_uniform(MaterialShaderGL::TEXTURE_SHADOW, (int)4);
			} else {
				
				material_shader.set_conditional(MaterialShaderGL::SPOT_SHADOW, false);
			};

			/*
			glMatrixMode(GL_MODELVIEW);
			glPushMatrix();
			GL_LOAD_MATRIX(ld->matrix);
			GL_MULT_MATRIX((double*)inv_camera_matrix.elements);

			glLightf(glid,GL_SPOT_CUTOFF,ld->cone_angle);
			set_direction=true;
			glLightf(glid,GL_CONSTANT_ATTENUATION,ld->attenuation.constant);
			glLightf(glid,GL_LINEAR_ATTENUATION,ld->attenuation.linear);
			glLightf(glid,GL_QUADRATIC_ATTENUATION,ld->attenuation.quadratic);

			Vector3 mpos = matrix.modelview.get_translation();
//			float lightpos[4]={0,0,0,1.0};
			float lightpos[4]={mpos.x,mpos.y,mpos.z,1.0};
			glLightfv(glid,GL_POSITION,lightpos); //at modelview

			Vector3 pos(0.0,1.0,0.0);
			lightpos[0]=pos.get_x();
			lightpos[1]=pos.get_y();
			lightpos[2]=pos.get_z();
			lightpos[3]=0;

			glLightfv(glid,GL_SPOT_DIRECTION,lightpos);

			glPopMatrix(); */

		} break;
		default: break;
	}
}

inline void Rasterizer_GL14::update_lights(Light_GL **p_lights,int p_light_count) {

	if (use_shaders) {

		if (use_texture_light_pass) {
		
			return;
		};
	};
	
	static unsigned int light_id=0;

	light_id++;

	/* prepare all (set lightid to current and recycled to false */
	for (int i=0;i<p_light_count;i++) {

		p_lights[i]->_pass_id=light_id;
		p_lights[i]->_recycled=false;
	}

	/* lights that have the same lightid in the gllights array are recycled, otherwise cleared */
	for (int i=0;i<MAX_GL_LIGHTS;i++) {

		if (!gl_lights[i].is_null() && gl_lights[i]->_pass_id==light_id) {
			gl_lights[i]->_recycled=true;
		} else {

			gl_lights[i].clear();
			glDisable(GL_LIGHT0+i);
			material_shader.set_conditional(directional_light_conditionals[i], false);
			material_shader.set_conditional(omni_light_conditionals[i], false);
			material_shader.set_conditional(spot_light_conditionals[i], false);
		}
	}

	/* allocate new lights as gl lights and initialize them*/

	int last_alloc=0;

	for (int i=0;i<p_light_count;i++) {

		if (p_lights[i]->_recycled)
			continue;

		for (int j=last_alloc;j<MAX_GL_LIGHTS;j++) {

			if (gl_lights[j].is_null()) {

				gl_lights[j]=p_lights[i];
				last_alloc=j+1;

				init_light(j);
				break;
			}
		}
	}

	material_shader.set_conditional(MaterialShaderGL::NO_LIGHTS, last_alloc==0?true:false);
}

inline void Rasterizer_GL14::setup_visual_element(RenderList_GL14::Visual *p_visual,bool p_setup_matrix)  {


	if (p_visual->has_alpha) {

		glEnable(GL_BLEND);
		glDepthMask(GL_FALSE);
	} else {

		glDisable(GL_BLEND);
		glDepthMask(GL_TRUE);
	}

	if (p_visual->lights) {


		update_lights(p_visual->lights,p_visual->light_count);

	}

	if (p_visual->world_matrix && p_setup_matrix) {

		glMatrixMode(GL_MODELVIEW);

		//GL_LOAD_MATRIX(p_visual->world_matrix);
		//GL_MULT_MATRIX(inv_camera_matrix);

		GL_LOAD_MATRIX(inv_camera_matrix);
		GL_MULT_MATRIX(*p_visual->world_matrix);

	}

}

void Rasterizer_GL14::do_light_pass(IRef<Light>* p_lights, int p_count) {

	Uint64 last_version = (((Uint64)1)<<63)-1L;

	real_t w, h;
	projection_matrix.get_viewport_size(w, h);
	Vector3 viewport(w, h, projection_matrix.get_z_near());
	
	light_shader.set_uniform(LightShaderGL::VIEWPORT, viewport);
	light_shader.set_uniform(LightShaderGL::ZFAR, projection_matrix.get_z_far());
	glActiveTextureARB(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, data_fbo.data);
	light_shader.set_uniform(LightShaderGL::NORMAL_TEXTURE, 0);

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, light_fbo.fbo);

	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);
	glDisable(GL_CULL_FACE );
	glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
	glDepthMask(GL_FALSE);
	
	
	//glDisable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	//glDepthMask(true);
	glClearColor(0,0.0,0,0.0);
	//glClearDepth(1.0f);
	glClear(/*GL_DEPTH_BUFFER_BIT|*/GL_COLOR_BUFFER_BIT);
	glDepthMask(false);

	static const GLenum deferred_buffers[1]={ GL_COLOR_ATTACHMENT0_EXT };
	glDrawBuffers(1,deferred_buffers);
	
	VideoMode vm = Main::get_singleton()->get_video_mode();

	for (int i=0; i<p_count; i++) {

		IRef<Light_GL> light = p_lights[i];
		Light_GL *ld=light.ptr();
		if (!ld) continue;
		init_light(0, ld);
		if (ld->shadow_texture) {
			glActiveTextureARB(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, ld->shadow_texture->get_gl_tex_id());
			light_shader.set_conditional(LightShaderGL::USE_SHADOW_MAP, true);
			light_shader.set_uniform(LightShaderGL::SHADOW_MAP, 1);

			glMatrixMode(GL_TEXTURE);
			GL_LOAD_MATRIX(ld->matrix.inverse() * inv_camera_matrix.inverse()); // directional light matrix should have its desired projection
		} else {
			
			light_shader.set_conditional(LightShaderGL::USE_SHADOW_MAP, false);
		};

		light_shader.set_conditional(LightShaderGL::LIGHT_MODE_DIRECTIONAL, false);
		light_shader.set_conditional(LightShaderGL::LIGHT_MODE_SPOT, false);
		light_shader.set_conditional(LightShaderGL::LIGHT_MODE_OMNI, false);

		switch(ld->mode) {
	
			case Light::MODE_DIRECTIONAL: {
				light_shader.set_conditional(LightShaderGL::LIGHT_MODE_DIRECTIONAL, true);
				if (last_version != light_shader.get_version()) {
					light_shader.bind();
				};
				glDisable(GL_DEPTH_TEST);
				copy_quad_using_viewport(0, 0, vm.width, vm.height, vm.width, vm.height);
				continue;
			} break;
			case Light::MODE_SPOT: {
				light_shader.set_conditional(LightShaderGL::LIGHT_MODE_SPOT, true);
			} break;
			case Light::MODE_POINT: {
				light_shader.set_conditional(LightShaderGL::LIGHT_MODE_OMNI, true);
			} break;
			default: ;
		};

		if (last_version != light_shader.get_version()) {
			light_shader.bind();
		};
		render_light_aabb(ld);
	};

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, compose_fbo.fbo);
	glDrawBuffers(1,deferred_buffers);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDepthFunc(GL_EQUAL);
};

static const float box[] = {

	// front
	0, 0, 0,
	1, 0, 0,
	1, 1, 0,
	0, 1, 0,


	// right
	1, 0, 0,
	1, 0, 1,
	1, 1, 1,
	1, 1, 0,
	
	// back
	1, 0, 1,
	0, 0, 1,
	0, 1, 1,
	1, 1, 1,
	
	// left
	0, 0, 1,
	0, 0, 0,
	0, 1, 0,
	0, 1, 1,

	// bottom
	0, 1, 0,
	1, 1, 0,
	1, 1, 1,
	0, 1, 1,

	
	// top
	0, 0, 0,
	0, 0, 1,
	1, 0, 1,
	1, 0, 0,

};
	
void Rasterizer_GL14::render_light_aabb(Light_GL* ld) {
	
	glMatrixMode(GL_PROJECTION);
	GL_LOAD_MATRIX(projection_matrix);

	AABB aabb = ld->get_matrix().xform( ld->get_AABB() );
	
	float znear = projection_matrix.get_z_near();

	Plane near_plane = inv_camera_matrix.xform_inv(Plane(Vector3(0, 0, 1), -znear));
	
	bool clip_near = aabb.intersects_plane(near_plane);

	static PolygonClipper near_clip;
	near_clip.clear();
	
	if (stencil_pass==0) {
		// first pass clears the stencil buffer.
		glClearStencil(0);
		glClear(GL_STENCIL_BUFFER_BIT);
		
	}
	
	
	stencil_pass++;
	
	if (clip_near) {
	
		float w, h;
		projection_matrix.get_viewport_size(w, h);
		float z = -(znear + 0.001);
		near_clip.add_point(inv_camera_matrix.xform_inv(Vector3(-w, h, z)));
		near_clip.add_point(inv_camera_matrix.xform_inv(Vector3(w, h, z)));
		near_clip.add_point(inv_camera_matrix.xform_inv(Vector3(w, -h, z)));
		near_clip.add_point(inv_camera_matrix.xform_inv(Vector3(-w, -h, z)));
		Plane p;

		for (int i=0; i<6; i++) {
			near_clip.clip(aabb.get_plane(i) );		
		};
	};
	
	Matrix4 aabb_matrix;
	aabb_matrix.scale( aabb.size );
	aabb_matrix.set_translation( aabb.pos );
	glEnable(GL_CULL_FACE);
	glDisable(GL_BACK);

	int from = 0;
	int to = 2;
	if (!use_stencil) {
		from = 3;
		to = 4;
	};

	glEnable(GL_DEPTH_TEST);
	
	for (int i=from;i<to;i++) {
	
	
		switch(i) {
		
			case 0: { // stencil clear
				glDepthMask( GL_FALSE );
				glDepthFunc( GL_LESS );
				glEnable( GL_STENCIL_TEST );				
				glEnable(GL_DEPTH_TEST);
				glEnable(GL_CULL_FACE);
				glCullFace(GL_FRONT); // draw backfaces
				glColorMask( GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE );
				glStencilFunc( GL_ALWAYS, stencil_pass, 0xFFFFFFFFL );
				glStencilOp( GL_KEEP, GL_REPLACE, GL_KEEP );
				
			} break;
			case 1: { // draw
				glCullFace(GL_BACK); // draw backfaces
				glStencilFunc( GL_EQUAL, stencil_pass, 0xFFFFFFFFL );
				glStencilOp( GL_KEEP, GL_KEEP, GL_KEEP );				
				glColorMask( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );
				
			} break;
			/*
			case 2: { // draw the light, testing stencil
				glStencilFunc( GL_EQUAL, 0, 0xFFFFFFFFL );				
				glStencilOp( GL_KEEP, GL_KEEP, GL_KEEP );
				glColorMask( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );
				glDisable(GL_DEPTH_TEST);
				glEnable(GL_CULL_FACE);
			} break; */
			case 3: {
					
				glEnable(GL_DEPTH_TEST);
				glEnable(GL_CULL_FACE);
				glDepthMask(GL_FALSE);
			};
		}
	
		glMatrixMode(GL_MODELVIEW);
		GL_LOAD_MATRIX(inv_camera_matrix * aabb_matrix);
	
		/*
		glEnable(GL_DEPTH_TEST);
		glDepthMask(GL_FALSE);
		glDepthFunc(GL_LESS);
		glEnable(GL_CULL_FACE );
		glCullFace(GL_BACK);
	*/
		/*
		int size = sizeof(box) / sizeof(float) / 3;
		glBegin(GL_QUADS);
		for (int i=0; i<size; i++) {
			glVertex3fv(&box[i*3]);
		};
		glEnd();
		*/
	
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(3,GL_FLOAT,0,(GLvoid*)box);
		glDrawArrays(GL_QUADS,0, sizeof(box) / sizeof(float) / 3);
		glDisableClientState(GL_VERTEX_ARRAY);
		
		if (clip_near && near_clip.get_point_count() > 0) {

			Vector3 prev;
			Vector3 last = near_clip.get_point(near_clip.get_point_count()-1);
			GL_LOAD_MATRIX(inv_camera_matrix);
			glBegin(GL_POLYGON);
			for (int i=0; i<near_clip.get_point_count(); i++) {
				Vector3 p = near_clip.get_point(i);
				glVertex3f(p.x, p.y, p.z);
			};
			glEnd();
		};
		
	}
	
	glDisable( GL_STENCIL_TEST );
	glEnable( GL_DEPTH_TEST );
	glDepthFunc( GL_LEQUAL );
	glDepthMask( GL_TRUE );	
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);	

	//glDepthMask(GL_TRUE);
	
};

/**
 *
 */
void Rasterizer_GL14::raster_list(RenderList_GL14 *p_render_list) {

	use_shaders = !GlobalVars::get_singleton()->get("use_fixed_pipeline");
	use_stencil = !GlobalVars::get_singleton()->get("disable_stencil");
	use_glow = use_shaders && !GlobalVars::get_singleton()->get("disable_glow");
	use_pre_light_pass = use_shaders && !GlobalVars::get_singleton()->get("disable_pre_light_pass");
	stencil_pass=0;
	// list going to be drawn should be ALWAYS closed.
	RenderList_GL14 &rl=*p_render_list;

	world_matrix.load_identity();
	inv_camera_matrix.load_identity();
	projection_matrix.load_identity();

	inside_scene = false;
	int scene_begin_index = -1;

	for (int i=0;i<MAX_GL_LIGHTS;i++)
		gl_lights[i].clear();

	for (int i=0;i<rl.get_element_count();i++) {

		RenderList_GL14::Element *re = rl.get_element(i);

		switch(re->type) {

			case RenderList_GL14::Element::TYPE_CLEAR: {

				glDepthMask(GL_TRUE);
				RenderList_GL14::Clear *re_c=static_cast<RenderList_GL14::Clear*>(re);
				Uint32 clear_mask=0;

				if (re_c->flags&Renderer::CLEAR_COLOR) {

					glClearColor( 	re_c->color.r/255.0,
							re_c->color.g/255.0,
					re_c->color.b/255.0,
					0.0 );
					clear_mask|=GL_COLOR_BUFFER_BIT;
				}

				if (re_c->flags&Renderer::CLEAR_DEPTH) {

					glClearDepth( re_c->depth);
					clear_mask|=GL_DEPTH_BUFFER_BIT;
				}

				if (re_c->flags&Renderer::CLEAR_STENCIL) {

					clear_mask|=GL_STENCIL_BUFFER_BIT;
				}

				glClear(clear_mask);

			} break;
			case RenderList_GL14::Element::TYPE_SET_LIGHTS: {

				RenderList_GL14::SetLights *re_l=static_cast<RenderList_GL14::SetLights*>(re);
				update_lights(re_l->lights,re_l->light_count);

			} break;
			case RenderList_GL14::Element::TYPE_SET_MATRIX: {

				RenderList_GL14::SetMatrix *re_sm=static_cast<RenderList_GL14::SetMatrix*>(re);
				switch(re_sm->mode) {

					case Renderer::MATRIX_WORLD: {

						world_matrix=*re_sm->matrix;

						glMatrixMode(GL_MODELVIEW);
						GL_LOAD_MATRIX(inv_camera_matrix);
						GL_MULT_MATRIX(world_matrix);
					} break;
					case Renderer::MATRIX_CAMERA: {

						inv_camera_matrix=re_sm->matrix->inverse();

						glMatrixMode(GL_MODELVIEW);
						GL_LOAD_MATRIX(inv_camera_matrix);
						GL_MULT_MATRIX(world_matrix);
					} break;
					case Renderer::MATRIX_PROJECTION: {

						projection_matrix=*re_sm->matrix;

						glMatrixMode(GL_PROJECTION);
						GL_LOAD_MATRIX(projection_matrix);
					} break;
				};

			} break;
			case RenderList_GL14::Element::TYPE_SET_TEXTURE: {

				RenderList_GL14::SetTexture *re_ct=static_cast<RenderList_GL14::SetTexture*>(re);
				glActiveTextureARB(GL_TEXTURE0);
				glEnable(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D,re_ct->texture->get_gl_tex_id());
				glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE );
				material_shader.set_conditional(MaterialShaderGL::USE_TEXTURE_BASE, true);
				material_shader.set_uniform(MaterialShaderGL::TEXTURE_BASE, (int)0);


			} break;
			case RenderList_GL14::Element::TYPE_SET_MATERIAL: {

				RenderList_GL14::SetMaterial *re_sm=static_cast<RenderList_GL14::SetMaterial*>(re);
				setup_material_simple(re_sm->material);

			} break;
			case RenderList_GL14::Element::TYPE_SET_FOG: {

				RenderList_GL14::SetFog *sf=static_cast<RenderList_GL14::SetFog*>(re);
				material_shader.set_conditional(MaterialShaderGL::FOG_TYPE_LINEAR, false);
				material_shader.set_conditional(MaterialShaderGL::FOG_TYPE_EXP, false);
				material_shader.set_conditional(MaterialShaderGL::FOG_TYPE_EXP2, false);

				if (sf->mode == Renderer::FOG_DISABLED) {

					glDisable(GL_FOG);
					material_shader.set_conditional(MaterialShaderGL::USE_FOG, false);
					break;
				};

				glEnable(GL_FOG);
				material_shader.set_conditional(MaterialShaderGL::USE_FOG, true);

				int val;
				switch (sf->mode) {

					default:
						case Renderer::FOG_LINEAR: {
							material_shader.set_conditional(MaterialShaderGL::FOG_TYPE_LINEAR, true);
							val = GL_LINEAR;
						}; break;
						case Renderer::FOG_EXP: {
							val = GL_EXP;
							material_shader.set_conditional(MaterialShaderGL::FOG_TYPE_EXP, true);
						}; break;
						case Renderer::FOG_EXP2: {
							val = GL_EXP2;
							material_shader.set_conditional(MaterialShaderGL::FOG_TYPE_EXP2, true);
						}; break;
				};
				float f = (float)val;
				glFogfv(GL_FOG_MODE, &f);

				glFogfv(GL_FOG_DENSITY, &sf->density);

				glFogfv(GL_FOG_START, &sf->start);

				glFogfv(GL_FOG_END, &sf->end);

				glFogfv(GL_FOG_INDEX, &sf->index);

				float c[4] = { (float)sf->color.r / 255.0, (float)sf->color.g / 255.0, (float)sf->color.b / 255.0, (float)sf->color.a / 255.0 };
				glFogfv(GL_FOG_COLOR, c);

				int src;
				if (sf->src == Renderer::SRC_FOG_COORD) src = GL_FRAGMENT_DEPTH;
				else src = GL_FRAGMENT_DEPTH;

				f = (int)src;
				glFogfv(GL_FOG_COORD_SRC, &f);
			} break;

			case RenderList_GL14::Element::TYPE_RENDER_TARGET_CHANGE: {

			} break;
			case RenderList_GL14::Element::TYPE_SCENE_BEGIN: {
//printf("scene begin\n");
				inside_scene = true;
				scene_begin_index = i;
				use_texture_light_pass = false;
				if (use_shaders) {
					
					check_resize_fbo();
					glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, compose_fbo.fbo);
					glColorMask(1, 1, 1, 1);
					glClear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);
				
					static const GLenum deferred_buffers[1]={ GL_COLOR_ATTACHMENT0_EXT };
					glDrawBuffers(1,deferred_buffers);
				};

			} break;
			case RenderList_GL14::Element::TYPE_SCENE_END: {

//printf("scene end\n");
				inside_scene = false;
				if (use_pre_light_pass || use_shaders) {

					glColorMask(1, 1, 1, 1);

					glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
					glActiveTextureARB(GL_TEXTURE0);
					//glEnable(GL_TEXTURE_2D);
					if (GlobalVars::get_singleton()->get("debug_light_pass")) {
					
						glBindTexture(GL_TEXTURE_2D, light_fbo.data);
					} else if (GlobalVars::get_singleton()->get("debug_normal_z")) {
					
						glBindTexture(GL_TEXTURE_2D, data_fbo.data);					
					} else {
						if (use_glow) {
							blur_compose();
						} else {
							post_process_shader.set_conditional(PostProcessShaderGL::BLUR_ENABLED, false);
						};
							
						glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

						if (use_glow) {
							glActiveTexture(GL_TEXTURE1);
							glBindTexture(GL_TEXTURE_2D, blur_fbo.color[0]);
							post_process_shader.set_uniform(PostProcessShaderGL::BLUR_TEXTURE, 1);
							post_process_shader.set_conditional(PostProcessShaderGL::BLUR_ENABLED, true);
						};
						
						if (!GlobalVars::get_singleton()->get("disable_ssao")) {
							glActiveTexture(GL_TEXTURE2);
							glBindTexture(GL_TEXTURE_2D, data_fbo.data);
							post_process_shader.set_uniform(PostProcessShaderGL::NORMAL_POS_TEXTURE, 2);
							post_process_shader.set_uniform(PostProcessShaderGL::SSAO_ENABLED, true);
						} else {
							post_process_shader.set_uniform(PostProcessShaderGL::SSAO_ENABLED, false);
						};
							
						glActiveTexture(GL_TEXTURE0);
						if (GlobalVars::get_singleton()->get("debug_blur_pass")) {
							glBindTexture(GL_TEXTURE_2D, blur_fbo.color[0]);
						} else {
							glBindTexture(GL_TEXTURE_2D, compose_fbo.data);
						};
					};
					VideoMode vm = Main::get_singleton()->get_video_mode();

					post_process_shader.set_uniform(PostProcessShaderGL::SCREEN_TEXTURE, 0);
					post_process_shader.set_uniform(PostProcessShaderGL::ZFAR, projection_matrix.get_z_far());
					post_process_shader.bind();
					glUniform1i(post_process_shader.get_uniform(PostProcessShaderGL::SCREEN_TEXTURE), 0);
					glUniform1i(post_process_shader.get_uniform(PostProcessShaderGL::BLUR_TEXTURE), 1);
					
					glUniform2f( post_process_shader.get_uniform( PostProcessShaderGL::PIXELSIZE), 1.0/vm.width, 1.0/vm.height);
					float vpw, vph;
					projection_matrix.get_viewport_size(vpw,vph);
					vpw/=projection_matrix.get_z_near();
					vph/=projection_matrix.get_z_near();
					
					glUniform2f( post_process_shader.get_uniform( PostProcessShaderGL::SCREEN_TO_VIEWPORT), vpw,vph);

					glDisable(GL_BLEND);
					copy_quad_using_viewport(0, 0, vm.width, vm.height, vm.width, vm.height);
				};

			} break;
			case RenderList_GL14::Element::TYPE_PRIMITIVE: {

					
				RenderList_GL14::PrimitiveElement *re_p=static_cast<RenderList_GL14::PrimitiveElement*>(re);
				if (use_pre_light_pass && inside_scene) {
					
					if (re_p->layer == 0 && !re_p->has_alpha) {
						
						int end = do_normal_pass(&rl, i);
						RenderList_GL14::SceneBegin *sb=static_cast<RenderList_GL14::SceneBegin*>(rl.get_element(scene_begin_index));
						do_light_pass(sb->visible_lights, sb->light_count);

						float w, h;
						projection_matrix.get_viewport_size(w, h);
						Vector3 viewport(w, h, projection_matrix.get_z_near());
						material_shader.set_uniform(MaterialShaderGL::VIEWPORT, viewport);
						use_texture_light_pass = true;
					} else {
						
						use_texture_light_pass = false;
					};
					
				};
					
				setup_visual_element(re_p);
				if (re_p->material) {

					setup_material_simple(re_p->material);
				}
				draw_primitive(re_p);
			} break;
			case RenderList_GL14::Element::TYPE_SURFACE: {
//printf("element %i surface, light pass %i\n", i, (int)use_texture_light_pass);
				RenderList_GL14::SurfaceElement *re_s=static_cast<RenderList_GL14::SurfaceElement*>(re);

				if (use_pre_light_pass && inside_scene) {
					
					if (re_s->layer == 0 && !re_s->has_alpha) {
						
						int end = do_normal_pass(&rl, i);
						RenderList_GL14::SceneBegin *sb=static_cast<RenderList_GL14::SceneBegin*>(rl.get_element(scene_begin_index));
						do_light_pass(sb->visible_lights, sb->light_count);

						float w, h;
						projection_matrix.get_viewport_size(w, h);
						Vector3 viewport(w, h, projection_matrix.get_z_near());
						material_shader.set_uniform(MaterialShaderGL::VIEWPORT, viewport);
						use_texture_light_pass = true;
					} else {
						
						use_texture_light_pass = false;
					};
					
				};
				
				int jcount=1;
				joint_draws[0]=re_s;;

				if (1) {
					for (int j=i+1;j<rl.get_element_count();j++) {

						if (jcount>=MAX_JOINT_DRAWS) {
							//printf("reached max\n");
							break;
						}
						RenderList_GL14::Element *re2 = rl.get_element(j);
						if (re2->type==RenderList_GL14::Element::TYPE_SURFACE) {

							RenderList_GL14::SurfaceElement *re_s2=static_cast<RenderList_GL14::SurfaceElement*>(re2);

							if (re_s2->surface!=re_s->surface ||
							re_s2->skeleton!=re_s->skeleton ||
							re_s2->extra_material!=re_s->extra_material	||
							re_s2->light_count!=re_s->light_count)
								break;

							bool add_ok=true;
							for (int k=0;k<re_s2->light_count;k++) {

								if (re_s2->lights[k]!=re_s->lights[k]) {

									add_ok=false;
									break;
								}
							}

							if (!add_ok)
								break;

							joint_draws[jcount++]=re_s2;
							//if (jcount>1)
							//	printf(

						} else {
							break;
						}

					}
				}

				i+=jcount-1;
				setup_visual_element(re_s,false);
				draw_surface(jcount);
				glColorMask(1, 1, 1, 1);
			} break;
			case RenderList_GL14::Element::TYPE_COPY_TO_TEXTURE: {

				RenderList_GL14::CopyScreenToTexture *cs_s=static_cast<RenderList_GL14::CopyScreenToTexture*>(re);

				Texture_GL *t = cs_s->texture;
				ERR_CONTINUE( !t->is_active() );

				DVector<Uint8> imgdata;
				float vp[4];
				//printf("vieport %f,%f,%f,%f\n",vp[0],vp[1],vp[2],vp[3]);
				glGetFloatv( GL_VIEWPORT, vp );

				imgdata.resize( vp[2]*vp[3]*3 );
				ERR_CONTINUE( imgdata.size() != (vp[2]*vp[3]*3) );

				imgdata.write_lock();
				Uint8 * data = imgdata.write();

				int format;
				switch (cs_s->source) {
					
				case Renderer::SOURCE_COLOR:
					format = GL_RGB;
					break;
				case Renderer::SOURCE_STENCIL:
					format = GL_STENCIL_INDEX;
					break;
				case Renderer::SOURCE_DEPTH:
					format = GL_DEPTH_COMPONENT;
					break;
				};
				
				glReadPixels(0,0,vp[2],vp[3],format,                    GL_UNSIGNED_BYTE, data );


				imgdata.write_unlock();

				Image image( vp[2], vp[3], Image::FORMAT_RGB, imgdata );

				imgdata.clear(); // non use anymore

				image.resize(t->get_width(), t->get_height() );

				image.flip_h();

				t->paste_area( 0, 0, image );
			} break;
			default: {};
		};
	}
}

void Rasterizer_GL14::blur_compose() {

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, blur_fbo.fbo[0]);
	glPushAttrib(GL_VIEWPORT_BIT);
	glViewport(0, 0, blur_fbo.width, blur_fbo.height);
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, compose_fbo.data);
	blur_copy_shader.set_uniform(BlurCopyShaderGL::SCREEN_TEXTURE, 0);
	blur_copy_shader.set_uniform(BlurCopyShaderGL::BLOOM, 0.0);
	//blur_copy_shader.set_uniform(BlurCopyShaderGL::PIXELSIZE, 0.4);
	blur_copy_shader.bind();
	VideoMode vm = Main::get_singleton()->get_video_mode();
	glUniform2f( blur_copy_shader.get_uniform( BlurCopyShaderGL::PIXELSIZE), 1.0/vm.width, 1.0/vm.height);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
	glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
	
	glBegin(GL_QUADS);
	glVertex2f(-1.0f,-1.0f);
	glVertex2f(1.0f,-1.0f);
	glVertex2f(1.0f, 1.0f);
	glVertex2f(-1.0f, 1.0f);
	glEnd();
	
	blur_shader.set_uniform(BlurShaderGL::SCREEN_TEXTURE, 0);
	
	for (int i=0;i<2;i++) {
	
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, blur_fbo.fbo[1]);
		glBindTexture(GL_TEXTURE_2D, blur_fbo.color[0]);
		blur_shader.set_conditional( BlurShaderGL::VERTICAL_PASS, true );
		blur_shader.set_uniform(BlurShaderGL::SCREEN_TEXTURE, 0);
		blur_shader.bind();
		glUniform2f( blur_shader.get_uniform( BlurShaderGL::PIXELSIZE), (1.0/vm.width)*4.0,(1.0/vm.height)*4.0);
		
		glBegin(GL_QUADS);
		glVertex2f(-1.0f,-1.0f);
		glVertex2f(1.0f,-1.0f);
		glVertex2f(1.0f, 1.0f);
		glVertex2f(-1.0f, 1.0f);
		glEnd();
	
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, blur_fbo.fbo[0]);
		glBindTexture(GL_TEXTURE_2D, blur_fbo.color[1]);
		blur_shader.set_conditional( BlurShaderGL::VERTICAL_PASS, false );
		blur_shader.bind();
		glUniform2f( blur_shader.get_uniform( BlurShaderGL::PIXELSIZE), (1.0/vm.width)*4.0,(1.0/vm.height)*4.0);
		
		glBegin(GL_QUADS);
		glVertex2f(-1.0f,-1.0f);
		glVertex2f(1.0f,-1.0f);
		glVertex2f(1.0f, 1.0f);
		glVertex2f(-1.0f, 1.0f);
		glEnd();
	
	}
	
	glPopAttrib();						
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);



}

void Rasterizer_GL14::copy_quad_using_viewport(float x, float y, float width, float height, float w_width, float w_height) {

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE );
	glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
	
	float srect[4] = {
		x / w_width,
		(w_height - (height + y)) / w_height,
		width / w_width,
		height / w_height
	};
	
	GLfloat viewport_coords[8]= {
		srect[0],srect[1],
		srect[0]+srect[2],srect[1],
		srect[0]+srect[2],srect[1]+srect[3],
		srect[0],srect[1]+srect[3],
	};
	
	GLfloat coords[8]={
		-1.0f,-1.0f,
		1.0f,-1.0f,
		1.0f, 1.0f,
		-1.0f, 1.0f
	};

	
	glBegin(GL_QUADS);
	for (int i=0;i<4;i++) {
		
		//glVertexAttrib2fv( 0, &screen_coords[i*2] );	
		glTexCoord2fv( &viewport_coords[i*2] );
		glVertex2fv( &coords[i*2] );
	}
	glEnd();
}


void Rasterizer_GL14::delete_fbo(Rasterizer_GL14::FBO* p_fbo) {
	
	if (p_fbo) {
		glDeleteRenderbuffersEXT(1,&p_fbo->fbo);
		glDeleteTextures(1,&p_fbo->data);
		p_fbo->fbo=0;
		p_fbo->width=0;
		p_fbo->height=0;
	} else {
		delete_fbo(&compose_fbo);
		delete_fbo(&light_fbo);
		delete_fbo(&data_fbo);
		glDeleteTextures(1, &depth_buffer);
		
	};
};

void Rasterizer_GL14::init_fbo(Rasterizer_GL14::FBO* p_fbo, int p_width, int p_height) {

	VideoMode vm = Main::get_singleton()->get_video_mode();

	p_fbo->active = false;

	if (p_width == -1)
		p_fbo->width = vm.width;
	else
		p_fbo->width = p_width;
	
	if (p_height == -1)
		p_fbo->height = vm.height;
	else
		p_fbo->height = p_height;

	glGenFramebuffersEXT(1, &p_fbo->fbo);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, p_fbo->fbo);

	glGenTextures(1, &p_fbo->data);
	
	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, depth_buffer);
	glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, depth_buffer);	
	glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_STENCIL_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, depth_buffer);	
		
	glGenTextures(1, &p_fbo->data);

	glBindTexture(GL_TEXTURE_2D, p_fbo->data);
	glTexImage2D(GL_TEXTURE_2D, 0, 4,  p_fbo->width, p_fbo->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F_ARB,  p_fbo->width, p_fbo->height, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, p_fbo->data, 0);


	static const GLenum deferred_buffers[3]={ GL_COLOR_ATTACHMENT0_EXT, GL_COLOR_ATTACHMENT1_EXT, GL_COLOR_ATTACHMENT2_EXT };
	glDrawBuffers(1,deferred_buffers);
	
	
	GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
	printf("%x\n",status);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0); // disable for now
	ERR_FAIL_COND( status != GL_FRAMEBUFFER_COMPLETE_EXT );
	p_fbo->active = true;
	
};

void Rasterizer_GL14::check_resize_fbo() {

	VideoMode vm = Main::get_singleton()->get_video_mode();
	if (data_fbo.width == vm.width &&
		data_fbo.height == vm.height )
		return; // nothing to do.
	if (data_fbo.fbo!=0) {
		
		WARN_PRINT("Resizing the screen after creation may reduce performance.");
	}
	delete_fbo(); // delete previous FBOs if they exist

	glGenRenderbuffersEXT(1, &depth_buffer);
	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, depth_buffer);
	glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_STENCIL_EXT, vm.width, vm.height);	
    

	init_fbo(&compose_fbo);
	init_fbo(&data_fbo);
	init_fbo(&light_fbo); //, nearest_power_of_2(vm.width)>>2, nearest_power_of_2(vm.height)>>2);
	
	blur_fbo.width = vm.width/4;
	blur_fbo.height = vm.height/4;
	
	for (int i=0;i<2;i++) {
	
		
		glGenFramebuffersEXT(1, &blur_fbo.fbo[i]);
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, blur_fbo.fbo[i]);
	
		glGenTextures(1, &blur_fbo.color[i]);
	
		glBindTexture(GL_TEXTURE_2D, blur_fbo.color[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,  blur_fbo.width, blur_fbo.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
		
		glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, blur_fbo.color[i], 0);
		
		int status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
		
		glClearColor(0,0,0,1.0);
		glClear(GL_COLOR_BUFFER_BIT);
		
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0); // disable for now
		
		ERR_FAIL_COND( status != GL_FRAMEBUFFER_COMPLETE_EXT );

	}
	
	
	
};

Rasterizer_GL14::Rasterizer_GL14() {

	skinned_array = memnew_arr( Vector3, MAX_SKINTRANSFORM_VERTICES );

	use_shaders = true;
	use_pre_light_pass = true;
	material_shader.init();
	if (!material_shader.test()) {
		GlobalVars::get_singleton()->set("use_fixed_pipeline", true);
		use_shaders = false;
		use_pre_light_pass = false;
	} else {
		check_resize_fbo();
		pre_light_shader.init();
		light_shader.init();
		post_process_shader.init();
		blur_copy_shader.init();
		blur_shader.init();
		glow_shader.init();
	};
	use_texture_light_pass = false;
}


Rasterizer_GL14::~Rasterizer_GL14() {

	memdelete_arr( skinned_array );
}


//*
int Rasterizer_GL14::do_normal_pass(RenderList_GL14 *p_render_list, int first_idx) {

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, data_fbo.fbo);
	static const GLenum deferred_buffers[1]={ GL_COLOR_ATTACHMENT0_EXT};
	glDrawBuffers(1,deferred_buffers);
	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glDepthMask(true);
	glDepthFunc(GL_LESS);
	glClearColor(0.0,0.0,1.0,1.0);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glClear(GL_COLOR_BUFFER_BIT);
	
	
	RenderList_GL14 &rl=*p_render_list;
	for (int i=first_idx; i<rl.get_element_count(); i++) {

		RenderList_GL14::Element *re = rl.get_element(i);
	
		switch(re->type) {

			case RenderList_GL14::Element::TYPE_PRIMITIVE: {
				
				RenderList_GL14::PrimitiveElement *re_p=static_cast<RenderList_GL14::PrimitiveElement*>(re);

				if (re_p->layer != 0 || re_p->has_alpha) {
					// end of normal pass
					return i;
				};

				bool use_texture = false;
				if (re_p->material) {
					
					if (!re_p->material->flags.shaded || !(re_p->flags&RenderList_GL14::PrimitiveElement::HAS_NORMALS))
						continue;
					
					if (re_p->material->flags.scissor_alpha && (re_p->flags & RenderList_GL14::PrimitiveElement::HAS_UVS)) {
						
						Texture_GL* tex = re_p->texture?re_p->texture:re_p->material->textures.diffuse.ptr();
						if (tex) {
							glActiveTexture(GL_TEXTURE0);
							glBindTexture(GL_TEXTURE_2D, tex->get_gl_tex_id());
							use_texture = true;

							pre_light_shader.set_conditional(PreLightShaderGL::ALPHA_TEST, true);
							pre_light_shader.set_uniform(PreLightShaderGL::ALPHA_THRESHHOLD, re_p->material->vars.scissor_alpha_treshold);
							pre_light_shader.set_uniform(PreLightShaderGL::TEXTURE_BASE, 0);

						} else {
							pre_light_shader.set_conditional(PreLightShaderGL::ALPHA_TEST, false);
						};
					};
					
					if (re_p->material->flags.double_sided)
						glDisable(GL_CULL_FACE);
					else {
						glEnable(GL_CULL_FACE);
						glCullFace( (re_p->material->flags.invert_normals)?GL_FRONT:GL_BACK);
					}
					
				};
				
				
				// render the primitive
				glDisable(GL_BLEND);
				glDepthMask(GL_TRUE);

				glMatrixMode(GL_MODELVIEW);
				GL_LOAD_MATRIX(inv_camera_matrix);
				GL_MULT_MATRIX(*re_p->world_matrix);

				glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);

				pre_light_shader.bind();
				
				glBegin( prim_type[re_p->flags&RenderList_GL14::PrimitiveElement::POINT_MASK] );
				
				int pcount = (re_p->flags&RenderList_GL14::PrimitiveElement::POINT_MASK)+1;
				
				for (int i=0;i<pcount; i++ ) {

					if (re_p->flags&RenderList_GL14::PrimitiveElement::HAS_NORMALS) {
			
						glNormal3f(
							   (float)re_p->normals[i*3+0]/127.0,
								(float)re_p->normals[i*3+1]/127.0,
								 (float)re_p->normals[i*3+2]/127.0
							  );
					}

					if (use_texture) {

						glTexCoord2f(re_p->uvs[i*2+0],re_p->uvs[i*2+1]  );
					}

					glVertex3f(re_p->vertices[i].x, re_p->vertices[i].y, re_p->vertices[i].z);

				};
			} break;

			
			case RenderList_GL14::Element::TYPE_SURFACE: {
				
				RenderList_GL14::SurfaceElement *re_s=static_cast<RenderList_GL14::SurfaceElement*>(re);
				if (re_s->layer != 0 || re_s->has_alpha) {
					// end of normal pass
					return i;
				};

				if (!re_s->surface->material->flags.shaded)
					continue;
				
				
				int jcount=1;
				joint_draws[0]=re_s;;

				if (1) {
					for (int j=i+1;j<rl.get_element_count();j++) {

						if (jcount>=MAX_JOINT_DRAWS) {
							//printf("reached max\n");
							break;
						}
						RenderList_GL14::Element *re2 = rl.get_element(j);
						if (!re2->type==RenderList_GL14::Element::TYPE_SURFACE)
							break;

						RenderList_GL14::SurfaceElement *re_s2=static_cast<RenderList_GL14::SurfaceElement*>(re2);

						if (re_s2->surface!=re_s->surface || re_s2->skeleton!=re_s->skeleton)
							break;

						joint_draws[jcount++]=re_s2;
						//if (jcount>1)
						//	printf(

					}
				}
				
				i+=jcount-1;

				glDisable(GL_BLEND);
				glDepthMask(GL_TRUE);
				
				draw_surface_normal_pass(jcount);
				
				
			};
			
			case RenderList_GL14::Element::TYPE_SCENE_END: {
			
				// end of normal pass
				return i;
			};
			default:
				break;
		};
	};
	// shouldn't reah here
	ERR_FAIL_V( -1 );
};


void Rasterizer_GL14::draw_surface_normal_pass(int p_joints) {

	RenderList_GL14::SurfaceElement *s=joint_draws[0];
	Surface_GL14 *surf = s->surface;
	ERR_FAIL_COND(!surf);
	ERR_FAIL_COND(!surf->configured);
	ERR_FAIL_COND( !surf->use_VBO && surf->vertex_mem.is_null());

	const Uint8 *vertex_memptr=NULL;
	if (surf->use_VBO) {

		glBindBufferARB(GL_ARRAY_BUFFER,surf->vertex_id);
	} else {
		Memory::lock( surf->vertex_mem );
		vertex_memptr=(const Uint8*)Memory::get( surf->vertex_mem );
	}

	int index_array_idx=-1;
	bool use_skinning;
	const Uint8* bones_ptr;
	
	for (int i=0;i<surf->array_count;i++) {

		const Surface_GL14::ArrayData &a=surf->array[i];

		if (!a.configured) {
			ERR_FAIL_COND( a.type==Surface::ARRAY_VERTEX );
			ERR_CONTINUE( !a.configured );
		}

		switch(a.type) {

			case Surface::ARRAY_VERTEX: {
				glEnableClientState(GL_VERTEX_ARRAY);
				if (surf->use_VBO)
					glVertexPointer(3,GL_FLOAT,surf->stride,(GLvoid*)a.ofs);
				else
					glVertexPointer(3,GL_FLOAT,surf->stride,(GLvoid*)&vertex_memptr[a.ofs]);

			} break;
			case Surface::ARRAY_NORMAL: {
				glEnableClientState(GL_NORMAL_ARRAY);
				if (surf->use_VBO)
					glNormalPointer(GL_FLOAT,surf->stride,(GLvoid*)a.ofs);
				else
					glNormalPointer(GL_FLOAT,surf->stride,(GLvoid*)&vertex_memptr[a.ofs]);

			} break;
			case Surface::ARRAY_BINORMAL: {

				glClientActiveTextureARB(GL_TEXTURE7);
				glEnableClientState(GL_TEXTURE_COORD_ARRAY);
				if (surf->use_VBO)
					glTexCoordPointer(3,GL_FLOAT,surf->stride,(GLvoid*)a.ofs);
				else
					glTexCoordPointer(3,GL_FLOAT,surf->stride,&vertex_memptr[a.ofs]);

			} break;

			case Surface::ARRAY_TANGENT: {

				glClientActiveTextureARB(GL_TEXTURE6);
				glEnableClientState(GL_TEXTURE_COORD_ARRAY);
				if (surf->use_VBO)
					glTexCoordPointer(4,GL_FLOAT,surf->stride,(GLvoid*)a.ofs);
				else
					glTexCoordPointer(4,GL_FLOAT,surf->stride,&vertex_memptr[a.ofs]);
			} break;
			case Surface::ARRAY_TEX_UV_0: {
				if ( (surf->material->textures.diffuse && surf->material->flags.scissor_alpha && surf->material->vars.scissor_alpha_treshold > 0) ||
						(surf->material->textures.normalmap && (surf->format & Surface::FORMAT_ARRAY_TANGENT)) ) {
				
					glClientActiveTextureARB(GL_TEXTURE0);
					glEnableClientState(GL_TEXTURE_COORD_ARRAY);
					if (surf->use_VBO)
						glTexCoordPointer(2,GL_FLOAT,surf->stride,(GLvoid*)a.ofs);
					else
						glTexCoordPointer(2,GL_FLOAT,surf->stride,&vertex_memptr[a.ofs]);
				};
			} break;
			case Surface::ARRAY_WEIGHTS: {

				// move the vertexpointer
				use_skinning=true;
				bones_ptr=&vertex_memptr[a.ofs];

				/* ? */
			} break;
			case Surface::ARRAY_CUSTOM_0:
			case Surface::ARRAY_CUSTOM_1:
			case Surface::ARRAY_CUSTOM_2:
				case Surface::ARRAY_CUSTOM_3: {

					/* */
				};

				case Surface::ARRAY_INDEX: {

				//index array must be configured to work
					index_array_idx=i;
					/* */
				} break;

			default:
				break;
		}
	}

	/* process skinning via software*/

	if (s->skeleton && s->skeleton->get_bones_ptr() && use_skinning && bones_ptr && surf->array_len < MAX_SKINTRANSFORM_VERTICES) {

		const Matrix4 * skeleton  = s->skeleton->get_bones_ptr();

		int skeleton_size = s->skeleton->get_bone_count();

		glVertexPointer(3,(sizeof(real_t)==4)?GL_FLOAT:GL_DOUBLE,0,skinned_array);

		int stride=surf->stride;
		Vector3 vtmp;

		for (int j=0;j<surf->array_len;j++) {

			const float *vtx=(float*)(&vertex_memptr[j*surf->stride]);
			skinned_array[j]=Vector3();

			Vector3 v( vtx[0], vtx[1], vtx[2] );
			const Uint8 *bone_ptr=&bones_ptr[j*stride];

#define _TRANSBONE(m_v)\
	ERR_CONTINUE( bone_ptr[m_v<<1] >= skeleton_size );\
	skeleton[ bone_ptr[m_v<<1] ].transform_copy( v, vtmp );\
	skinned_array[j]+=vtmp*(bone_ptr[(m_v<<1)+1]*(1/255.0));\
	if ( !bone_ptr[(m_v<<1)+1] )\
		continue;

			_TRANSBONE(0);
			_TRANSBONE(1);
			_TRANSBONE(2);
			_TRANSBONE(3);

		}

	}
#undef _TRANSBONE
	/* draw by material */
	Material_GL *material = surf->material.is_null() ? IRef<Material_GL>(Renderer::get_singleton()->get_default_material()).ptr() : surf->material.ptr();
	ERR_FAIL_COND(!material);

	
	
	if (material->flags.double_sided)
		glDisable(GL_CULL_FACE);
	else {
		glEnable(GL_CULL_FACE);
		glCullFace( (material->flags.invert_normals)?GL_FRONT:GL_BACK);
	}
	glDisable(GL_ALPHA_TEST);
	if (material->flags.scissor_alpha && material->textures.diffuse) {

		pre_light_shader.set_conditional(PreLightShaderGL::ALPHA_TEST, true);
		pre_light_shader.set_uniform(PreLightShaderGL::ALPHA_THRESHHOLD, material->vars.scissor_alpha_treshold);
		glActiveTextureARB(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, material->textures.diffuse->get_gl_tex_id());
		pre_light_shader.set_uniform(PreLightShaderGL::TEXTURE_BASE, (int)0);
	} else {

		pre_light_shader.set_conditional(PreLightShaderGL::ALPHA_TEST, false);
		glActiveTextureARB(GL_TEXTURE0);
		glDisable(GL_TEXTURE_2D);
	}

	
	if (material->textures.normalmap) {
		glActiveTextureARB(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, material->textures.normalmap->get_gl_tex_id());
		pre_light_shader.set_uniform(PreLightShaderGL::TEXTURE_NORMAL, (int)0);
		pre_light_shader.set_conditional(PreLightShaderGL::USE_TEXTURE_NORMALMAP, true);
	};
	

	glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
	pre_light_shader.bind();
	for (int i=0;i<p_joints;i++) {

		glMatrixMode(GL_MODELVIEW);

		//GL_LOAD_MATRIX(p_visual->world_matrix);
		//GL_MULT_MATRIX(inv_camera_matrix);
		GL_LOAD_MATRIX(inv_camera_matrix);
		if (joint_draws[i]->world_matrix) {
			GL_MULT_MATRIX(*joint_draws[i]->world_matrix);
		};

		if (index_array_idx!=-1 && surf->index_array_len>0) {

			if (surf->use_VBO) {

				glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER,surf->index_id);
				glDrawElements(gl_primitive[surf->primitive],surf->index_array_len, (surf->index_array_len>(1<<16))?GL_UNSIGNED_INT:GL_UNSIGNED_SHORT,0);
			} else if (!surf->index_mem.is_null()) {
				Memory::lock(surf->index_mem);
				glDrawElements(gl_primitive[surf->primitive],surf->index_array_len, (surf->index_array_len>(1<<16))?GL_UNSIGNED_INT:GL_UNSIGNED_SHORT,Memory::get(surf->index_mem));
				Memory::unlock(surf->index_mem);
			}


		} else {

			glDrawArrays(gl_primitive[surf->primitive],0,surf->array_len);
		}
	}
	
	
	for (int i=0; i<2; i++) {
		glActiveTextureARB(GL_TEXTURE0+i);
		glDisable(GL_TEXTURE_2D);
	};
	
	
	glDisableClientState(GL_EDGE_FLAG_ARRAY);
	glDisableClientState(GL_INDEX_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_SECONDARY_COLOR_ARRAY);

	if (surf->use_VBO && index_array_idx!=-1 && surf->index_array_len>0)
		glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER,0);

	if (!surf->use_VBO)
		Memory::unlock( surf->vertex_mem );
	else
		glBindBufferARB(GL_ARRAY_BUFFER,0);
	
};

//	*/

#endif // OPENGL_ENABLED
