#ifdef OPENGL2_ENABLED

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
#include "rasterizer_forward_gl2.h"

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


void RasterizerForward_GL2::setup_material_base(const Material_GL *p_material) {

	/* Global First */

	glDisable(GL_COLOR_MATERIAL); /* unused, unless color array */

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

	glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,p_material->vars.shininess*255.0);

	if (p_material->flags.double_sided)
		glDisable(GL_CULL_FACE);
	else {
		glEnable(GL_CULL_FACE);
		glCullFace( (p_material->flags.invert_normals)?GL_FRONT:GL_BACK);
	}

	if (p_material->flags.shaded)
		glEnable(GL_LIGHTING);
	else {
		glDisable(GL_LIGHTING);
	}

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

	/*
	if (p_material->flags.draw_z) {
		glDepthMask(GL_TRUE);
	} else {
		glDepthMask(GL_FALSE);
	};
	*/

	if (p_material->flags.wireframe)
		glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);

	/* texture */
}


void RasterizerForward_GL2::setup_material_simple(const Material_GL *p_material) {

	setup_material_base(p_material);

}

inline void RasterizerForward_GL2::draw_primitive(RenderList_GL2::PrimitiveElement *p) {

	static const GLenum prim_type[4]={GL_POINTS,GL_LINES,GL_TRIANGLES,GL_QUADS};


	int pcount = (p->flags&RenderList_GL2::PrimitiveElement::POINT_MASK)+1;

	if (!(p->flags&RenderList_GL2::PrimitiveElement::HAS_UVS)) {

		glDisable(GL_TEXTURE_2D);
		
	} else {
		if (p->texture) {
			
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D,p->texture->get_gl_tex_id());
			
		}
	}
	
	if (p->has_alpha)
		glEnable(GL_BLEND);
	else
		glDisable(GL_BLEND);

	if (!(p->flags&RenderList_GL2::PrimitiveElement::HAS_COLORS))
		glColor4f(255,255,255,255);

	glBegin( prim_type[p->flags&RenderList_GL2::PrimitiveElement::POINT_MASK] );

	for (int i=0;i<pcount; i++ ) {

		if (p->flags&RenderList_GL2::PrimitiveElement::HAS_BINORMAL_TANGENT) {

		/* uh?
			*/	ERR_PRINT("Unimplemented");

		} else if (p->flags&RenderList_GL2::PrimitiveElement::HAS_NORMALS) {

			glNormal3f(
				   (float)p->normals[i*3+0]/127.0,
				    (float)p->normals[i*3+1]/127.0,
				     (float)p->normals[i*3+2]/127.0
				  );
		}

		if (p->flags&RenderList_GL2::PrimitiveElement::HAS_COLORS) {

			glColor4f(
				  (float)p->colors[i].r/255.0,
				   (float)p->colors[i].g/255.0,
				    (float)p->colors[i].b/255.0,
				     (float)p->colors[i].a/255.0
				 );
		}

		if (p->flags&RenderList_GL2::PrimitiveElement::HAS_UVS) {

			glTexCoord2f(p->uvs[i*2+0],p->uvs[i*2+1]  );
		}

		glVertex3f(p->vertices[i].x, p->vertices[i].y, p->vertices[i].z
			  );


	}


	glEnd();
	if (p->texture) {
		
		glDisable(GL_TEXTURE_2D);
	}		
}

inline void RasterizerForward_GL2::draw_surface(RenderList_GL2::SurfaceElement *s) {

	Surface_GL2 *surf = s->surface;
	ERR_FAIL_COND(!surf);
	ERR_FAIL_COND(!surf->configured);
	ERR_FAIL_COND(surf->vertex_id==0);

	glBindBuffer(GL_ARRAY_BUFFER,surf->vertex_id);
	int index_array_idx=-1;

	for (int i=0;i<surf->array_count;i++) {

		const Surface_GL2::ArrayData &a=surf->array[i];

		if (!a.configured) {
			ERR_FAIL_COND( a.type==Surface::ARRAY_VERTEX );
			ERR_CONTINUE( !a.configured );
		}

		switch(a.type) {

			case Surface::ARRAY_VERTEX: {
				glEnableClientState(GL_VERTEX_ARRAY);
				glVertexPointer(3,GL_FLOAT,surf->stride,(GLvoid*)a.ofs);

			} break;
			case Surface::ARRAY_NORMAL: {
				glEnableClientState(GL_NORMAL_ARRAY);
				glNormalPointer(GL_FLOAT,surf->stride,(GLvoid*)a.ofs);

			} break;
			case Surface::ARRAY_BINORMAL:
				case Surface::ARRAY_TANGENT: {
				} break;
				case Surface::ARRAY_COLOR: {
					glEnableClientState(GL_COLOR_ARRAY);
					glColorPointer(4,GL_UNSIGNED_BYTE,surf->stride,(GLvoid*)a.ofs);
				} break;
			case Surface::ARRAY_TEX_UV_0:
			case Surface::ARRAY_TEX_UV_1:
			case Surface::ARRAY_TEX_UV_2:
			case Surface::ARRAY_TEX_UV_3:
			case Surface::ARRAY_TEX_UV_4:
			case Surface::ARRAY_TEX_UV_5:
			case Surface::ARRAY_TEX_UV_6:
				case Surface::ARRAY_TEX_UV_7: {
					glActiveTexture(GL_TEXTURE0 + (i-Surface::ARRAY_TEX_UV_0));
					glEnableClientState(GL_TEXTURE_COORD_ARRAY);
					glTexCoordPointer(2,GL_FLOAT,surf->stride,(GLvoid*)a.ofs);
				} break;
			case Surface::ARRAY_WEIGHTS: {

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

	Material_GL *material = surf->material.is_null() ? IRef<Material_GL>(Renderer::get_singleton()->get_default_material()).ptr() : surf->material.ptr();
	ERR_FAIL_COND(!material);

	while(material) {

		setup_material_base( material );
		
		// check if shader must be enabled
		Shader_GL2 *shader=NULL;
		if (!material->shader.is_null()) {
		
			shader=material->shader.ptr();
		}
	
		if (shader && shader->active) {
			
			glUseProgram(shader->program_id);
		}
		
		draw_surface_data( surf, material,index_array_idx );
		material=!material->next_pass.is_null()?material->next_pass.ptr():NULL;
		
		// disable shader
		if (shader && shader->active) {
		
			glUseProgram(0);
		}
		
	}

	glDisableClientState(GL_EDGE_FLAG_ARRAY);
	glDisableClientState(GL_INDEX_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_SECONDARY_COLOR_ARRAY);

	if (index_array_idx!=-1 && surf->index_array_len>0 && surf->index_id)
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);

	glBindBuffer(GL_ARRAY_BUFFER,0);

}

void RasterizerForward_GL2::draw_surface_data(Surface_GL2 *s,Material_GL *m,int p_index_idx) {

	/* TODO pixel shaders */

	if (m->textures.diffuse.is_null()) {

		glDisable(GL_TEXTURE_2D);
	} else {
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D,m->textures.diffuse->get_gl_tex_id());

	}


	static const GLenum gl_primitive[7]={ GL_POINTS, GL_LINES, GL_LINE_STRIP,GL_LINE_LOOP, GL_TRIANGLES, GL_TRIANGLE_STRIP, GL_TRIANGLE_FAN };

	if (p_index_idx!=-1 && s->index_array_len>0 && s->index_id) {

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,s->index_id);
		glDrawElements(gl_primitive[s->primitive],s->index_array_len, (s->index_array_len>(1<<16))?GL_UNSIGNED_INT:GL_UNSIGNED_SHORT,0);

	} else {

		glDrawArrays(gl_primitive[s->primitive],0,s->array_len);
	}

}


void RasterizerForward_GL2::init_light(int p_idx) {

	ERR_FAIL_INDEX(p_idx,MAX_GL_LIGHTS);
	ERR_FAIL_COND( gl_lights[p_idx].is_null() );


	const Light_GL *ld=gl_lights[p_idx].ptr();
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
			glMatrixMode(GL_MODELVIEW);
			glPushMatrix();
			glLoadIdentity();
			Vector3 v(0.0,1.0,0.0); // directional lights point up by default
			Matrix4 auxm=(ld->matrix * inv_camera_matrix); // this could be cached
			auxm.transform_no_translation( v );
			v.normalize(); // this sucks, so it will be optimized at some point

			float lightpos[4]={v.get_x(),v.get_y(),v.get_z(),0.0};

			glLightfv(glid,GL_POSITION,lightpos); //at modelview

			glPopMatrix();

		} break;
		case Light::MODE_POINT: {
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

	}
}

inline void RasterizerForward_GL2::update_lights(Light_GL **p_lights,int p_light_count) {

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

}

void RasterizerForward_GL2::setup_visual_element(RenderList_GL2::Visual *p_visual) {


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

	if (p_visual->world_matrix) {

		glMatrixMode(GL_MODELVIEW);

		//GL_LOAD_MATRIX(p_visual->world_matrix);
		//GL_MULT_MATRIX(inv_camera_matrix);

		GL_LOAD_MATRIX(inv_camera_matrix);
		GL_MULT_MATRIX(*p_visual->world_matrix);

	}

}
/**
 *
 */
void RasterizerForward_GL2::raster_list(RenderList_GL2 *p_render_list) {

	// list going to be drawn should be ALWAYS closed.
	RenderList_GL2 &rl=*p_render_list;

	world_matrix.load_identity();
	inv_camera_matrix.load_identity();
	projection_matrix.load_identity();
	
	for (int i=0;i<MAX_GL_LIGHTS;i++)
		gl_lights[i].clear();


	for (int i=0;i<rl.get_element_count();i++) {

		RenderList_GL2::Element *re = rl.get_element(i);

		switch(re->type) {

			case RenderList_GL2::Element::TYPE_CLEAR: {

				RenderList_GL2::Clear *re_c=static_cast<RenderList_GL2::Clear*>(re);
				Uint32 clear_mask=0;

				if (re_c->flags&Renderer::CLEAR_COLOR) {

					glClearColor( 	re_c->color.r/255.0,
							re_c->color.g/255.0,
							re_c->color.b/255.0,
							re_c->color.a/255.0 );
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
			case RenderList_GL2::Element::TYPE_SET_LIGHTS: {

				RenderList_GL2::SetLights *re_l=static_cast<RenderList_GL2::SetLights*>(re);
				update_lights(re_l->lights,re_l->light_count);

			} break;
			case RenderList_GL2::Element::TYPE_SET_MATRIX: {

				RenderList_GL2::SetMatrix *re_sm=static_cast<RenderList_GL2::SetMatrix*>(re);
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
			case RenderList_GL2::Element::TYPE_SET_TEXTURE: {

				RenderList_GL2::SetTexture *re_ct=static_cast<RenderList_GL2::SetTexture*>(re);
				glEnable(GL_TEXTURE_2D);
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D,re_ct->texture->get_gl_tex_id());
				glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE );


			} break;
			case RenderList_GL2::Element::TYPE_SET_MATERIAL: {

				RenderList_GL2::SetMaterial *re_sm=static_cast<RenderList_GL2::SetMaterial*>(re);
				setup_material_simple(re_sm->material);

			} break;
			case RenderList_GL2::Element::TYPE_RENDER_TARGET_CHANGE: {

			} break;
			case RenderList_GL2::Element::TYPE_SCENE_BEGIN: {

			} break;
			case RenderList_GL2::Element::TYPE_SCENE_END: {

			} break;
			case RenderList_GL2::Element::TYPE_PRIMITIVE: {

				RenderList_GL2::PrimitiveElement *re_p=static_cast<RenderList_GL2::PrimitiveElement*>(re);

				if (re_p->material) {

					setup_material_simple(re_p->material);
				}
				setup_visual_element(re_p);
				draw_primitive(re_p);
			} break;
			case RenderList_GL2::Element::TYPE_SURFACE: {

				RenderList_GL2::SurfaceElement *re_s=static_cast<RenderList_GL2::SurfaceElement*>(re);

				setup_visual_element(re_s);
				draw_surface(re_s);

			} break;
			default:{};
		};
	}


}


RasterizerForward_GL2::RasterizerForward_GL2() {
	
}


RasterizerForward_GL2::~RasterizerForward_GL2()
{
}


#endif // OPENGL2_ENABLED
