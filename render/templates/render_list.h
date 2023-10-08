//
// C++ Interface: render_list
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef RENDER_LIST_H
#define RENDER_LIST_H


#include "global_vars.h"
#include "render/renderer.h"
#include "sort.h"


/**
	@author Juan Linietsky <reduzio@gmail.com>
*/
	
enum RenderListSortMethod {
	
	RENDER_LIST_SORT_MODE_NONE,
	RENDER_LIST_SORT_LAYER_ALPHA_DEPTH
};

template<class LightT, class TextureT, class SurfaceT, class SkeletonT,class MaterialT,int _MAX_ELEMENT_LIGHTS,RenderListSortMethod sort_method=RENDER_LIST_SORT_LAYER_ALPHA_DEPTH>
class RenderList {	
public:	
	
	enum {
		
		MAX_ELEMENT_LIGHTS=_MAX_ELEMENT_LIGHTS,
		GUARD_SIZE=1024
	};
	struct Element {
	
		enum Type {
		
			TYPE_CLEAR,
			TYPE_SET_LIGHTS, // change matrix, invalid on render list
			TYPE_SET_MATRIX, // change matrix, invalid on render list
			TYPE_SET_MATERIAL, // change material, invalid on render list
   			TYPE_SET_FOG,
			TYPE_RENDER_TARGET_CHANGE, // change render target, invalid on render list
			TYPE_SCENE_BEGIN, // begin of a render list
   			TYPE_SCENE_END, // end of a render list
			TYPE_SET_TEXTURE, // change texture on layer 0
			TYPE_PRIMITIVE, // render a primitive
			TYPE_SURFACE, // render a surface
			TYPE_COPY_TO_TEXTURE, // copy screen to a texture
			TYPE_MAX
		};
		
		Type type;
		
		virtual ~Element() {};
	};
private:	
	int MAX_ELEMENTS;
	int BUFFER_SIZE;
	
	Uint8 *element_data_buffer;
	Element **elements;
	
	int element_count;
	Uint32 buffer_pos;
	bool inside_scene;
		
	
	struct IRefCache {
		
		IAPIRef ref;
		IRefCache *prev;
		IRefCache(const IAPIRef& p_iapiref) : ref( p_iapiref ) { prev=NULL; }
	};
	
	IRefCache *iref_last;
	
	template<class T>
	_FORCE_INLINE_ T* allocate_iref_cache(const IRef<T>& p_iref) {
	
		if (p_iref.is_null())
			return NULL;
		ERR_FAIL_COND_V(sizeof(IRefCache)>(BUFFER_SIZE-buffer_pos),NULL);
		
		IRefCache * ic= memnew_placement( &element_data_buffer[ buffer_pos ] , IRefCache(p_iref)  );
		buffer_pos+=sizeof(IRefCache);
		
		ic->prev=iref_last;
		iref_last=ic;
		
		return (T*)p_iref.ptr();
	}
		
	template<class T> 
	_FORCE_INLINE_ T* allocate_mem(int p_count=1) {
		// allocates mem of some type, constructor/destructor will not be called if exist, so use with care.
		// also, make sure it's aligned to the platform
		unsigned int to_alloc=sizeof(T)*p_count;
		if (to_alloc&(sizeof(void*)-1))
			to_alloc+=sizeof(void*)-(to_alloc&(sizeof(void*)-1));
		
		ERR_FAIL_COND_V(to_alloc>(BUFFER_SIZE-buffer_pos),NULL);
		
		T*area=(T*)&element_data_buffer[ buffer_pos ];
		buffer_pos+=to_alloc;
		return area;
	}
	
	LightT** light_cache;
	Uint8 light_cache_count;
	Matrix4 *matrix_cache[3];
	MaterialT* material_cache;
	TextureT* texture_cache;
	
	
	template<class T>
	_FORCE_INLINE_ T * allocate_element() {
		
		ERR_FAIL_COND_V(sizeof(T)>(BUFFER_SIZE-buffer_pos-GUARD_SIZE),NULL);
		ERR_FAIL_COND_V(element_count>MAX_ELEMENTS,NULL);

		T * element = memnew_placement( &element_data_buffer[ buffer_pos ], T);
		elements[element_count]=element;
		buffer_pos+=sizeof(T);
		element_count++;	
			
		return element;
	}
		
	_FORCE_INLINE_ bool is_render_list_full() const { 
		
		return (BUFFER_SIZE-buffer_pos < GUARD_SIZE );
	}
	
public:
	
	Matrix4 get_matrix_cache(Renderer::MatrixMode p_mode) const {
		
		return matrix_cache?*matrix_cache[p_mode]:Matrix4();
	}

	struct Clear : public Element {
	
		Uint32 flags;
		float depth;
		Color color;
		Clear() { Element::type=Element::TYPE_CLEAR; }
	};
	
	struct SetLights : public Element {
	
		Uint8 light_count;
		LightT** lights;
		SetLights() { Element::type=Element::TYPE_SET_LIGHTS; }
	};
	
	struct SetMatrix : public Element {
	
		Renderer::MatrixMode mode;
		Matrix4 *matrix;
		SetMatrix() { Element::type=Element::TYPE_SET_MATRIX; }
	};
	
	struct SetMaterial : public Element {
	
		MaterialT* material;
		SetMaterial() { Element::type=Element::TYPE_SET_MATERIAL; }
	};
	
	struct SetTexture : public Element {
	
		TextureT* texture;
		SetTexture() { Element::type=Element::TYPE_SET_TEXTURE; }
	};
	
	struct SetFog : public Element {
		
		Renderer::FogMode mode;
		float density;
		float start;
		float end;
		float index;
		Color color;
		Renderer::FogCoordSrc src;
		
		SetFog() { Element::type=Element::TYPE_SET_FOG; }
	};

	
	struct SceneBegin : public Element {

		IRef<Light>* visible_lights;
		int light_count;
		
		SceneBegin() { Element::type=Element::TYPE_SCENE_BEGIN; }
	};
	
	struct SceneEnd : public Element {
	
		SceneEnd() { Element::type=Element::TYPE_SCENE_END; }
	};
	
	struct Visual : public Element {
	
	
		Matrix4* world_matrix; // if not null, uses a world matrix
		LightT **lights;
		Uint8 light_count;
		bool has_alpha; // enable alpha blend, do alphasort
		int layer;
		float z; // for usage in render list
	
		Visual() { has_alpha=false; lights=0; world_matrix=0; z=0; layer=0; light_count=0; }
	};
	
	struct CopyScreenToTexture : public Element {
		
		TextureT *texture;
		int source;
	
		CopyScreenToTexture() { Element::type=Element::TYPE_COPY_TO_TEXTURE; texture=NULL; source = 0; }
	};
	
	struct PrimitiveElement : public Visual {
	
		enum {
		
			HAS_NORMALS=4,
			HAS_BINORMAL_TANGENT=8,
			HAS_COLORS=16,
			HAS_UVS=32,
			POINT_MASK=0x3
		};
	
		Uint32 flags;
		Vector3 *vertices;
		Sint8 *normals; // make them take less space
		Color* colors;
		float* uvs;		
		MaterialT *material;
		TextureT* texture; // may user texture or not..
		
		PrimitiveElement() { Element::type=Element::TYPE_PRIMITIVE; flags=0; vertices=0; normals=0; colors=0; uvs=0; uvs=0; material=0; texture=0; }
	};
	
	struct SurfaceElement : public Visual {
	
		AABB *aabb; // for usage in occlusion culling
		SurfaceT* surface;
		SkeletonT* skeleton;
		MaterialT* extra_material;
		SurfaceElement() { Element::type=Element::TYPE_SURFACE; aabb=0; surface=0; skeleton=0; extra_material=0; }
	};
	
	struct _LAD_Comparator { // layer, alpha, depth

		inline bool operator()(Visual *a, Visual* b) const { 
			
			if (a->layer==b->layer) {
							
				if (a->has_alpha == b->has_alpha) {
					
					if (a->has_alpha)
						return a->z > b->z;
					else
						return a->z < b->z;
				} else
					return (!a->has_alpha);	
			} else
				return a->layer<b->layer;		
		}
	};
	
	
	_FORCE_INLINE_ int get_element_count() {
			
		return element_count;		
	}
	_FORCE_INLINE_ Element* get_element(int p_index)  {
		
		ERR_FAIL_INDEX_V(p_index,element_count,NULL);
		return elements[p_index];
	}
		
	
	void cmd_clear(Uint32 p_flags,float p_depth,Color p_color) {
		
		ERR_FAIL_COND( is_render_list_full() );
		Clear *clear = allocate_element<Clear>();
		ERR_FAIL_COND(!clear);
		clear->flags=p_flags;
		clear->color=p_color;
		clear->depth=p_depth;				
	}
	
	void cmd_set_light_profile(Renderer::LightProfile p_lights) {

		LightT** lights = (LightT**)p_lights.lights;
		light_cache = lights;
		light_cache_count = p_lights.light_count;

		if (!inside_scene) {
			
			SetLights *set_lights = allocate_element<SetLights>();
			ERR_FAIL_COND(!set_lights);
			
			set_lights->lights=lights;
			set_lights->light_count=p_lights.light_count;			
		}
	};
	
	LightT** cmd_set_lights(IRef<LightT> *p_lights,int p_light_count) {
				
		ERR_FAIL_COND_V( is_render_list_full(), NULL );
		LightT **lights = allocate_mem<LightT*>(p_light_count);			
		ERR_FAIL_COND_V(!lights, NULL);
		
		for (int i=0;i<p_light_count;i++) {
			
			lights[i]=allocate_iref_cache(p_lights[i]);
		}
				
		light_cache=lights;
		light_cache_count=p_light_count;
		
		if (!inside_scene) {
			
			SetLights *set_lights = allocate_element<SetLights>();
			ERR_FAIL_COND_V(!set_lights, NULL);
			
			set_lights->lights=lights;
			set_lights->light_count=light_cache_count;			
		}
		
		return lights;
	}
	
	void cmd_set_matrix(Renderer::MatrixMode p_matrix_mode,const Matrix4& p_matrix) {
	
		ERR_FAIL_COND( is_render_list_full() );
		ERR_FAIL_COND(inside_scene && p_matrix_mode!=Renderer::MATRIX_WORLD); // this is forbidden
		Matrix4 *matptr = allocate_mem<Matrix4>();
		ERR_FAIL_COND(!matptr);
		*matptr=p_matrix;
		
		if (!inside_scene) {
			SetMatrix * set_matrix = allocate_element<SetMatrix>();
			ERR_FAIL_COND(!set_matrix); // didn't fit

			set_matrix->mode=p_matrix_mode;
			set_matrix->matrix=matptr;
		}
		
		/* actualize caches */
		
		matrix_cache[p_matrix_mode]=matptr;
		
	}
	
	void cmd_set_material(const IRef<MaterialT>& p_material) {
		
		
		if (p_material.is_null()) 
			return;
		ERR_FAIL_COND( is_render_list_full() );
				
		material_cache=allocate_iref_cache(p_material);
		
		if (!inside_scene) {
			SetMaterial * set_material = allocate_element<SetMaterial>();
			ERR_FAIL_COND(!set_material); // didn't fit
			set_material->material=material_cache;
		}
		
	}
	
	void cmd_set_texture(const IRef<TextureT>& p_texture) {
		
		ERR_FAIL_COND(p_texture.is_null());		
		ERR_FAIL_COND( is_render_list_full() );
		
		texture_cache=allocate_iref_cache(p_texture);
		
		if (!inside_scene) {
			SetTexture * set_texture = allocate_element<SetTexture>();
			ERR_FAIL_COND(!set_texture); // didn't fit
			set_texture->texture=texture_cache;
		}
	}
	
	void cmd_set_fog(Renderer::FogMode p_mode, float p_density, float p_start, float p_end, float p_index, Color p_color, Renderer::FogCoordSrc p_src) {
		
		ERR_FAIL_COND(inside_scene);	
		SetFog * csetfog = allocate_element<SetFog>();
		ERR_FAIL_COND(!csetfog); // didn't fit
		
		csetfog->mode=p_mode;
		csetfog->density=p_density;
		csetfog->start=p_start;
		csetfog->end=p_end;
		csetfog->index=p_index;
		csetfog->color=p_color;
		csetfog->src=p_src;		
	}

	void cmd_copy_to_texture( const IRef<TextureT>& p_texture, int p_source ) {
		
		ERR_FAIL_COND(inside_scene);	
		ERR_FAIL_COND(p_texture.is_null());
		ERR_FAIL_COND( is_render_list_full() );		
		
		CopyScreenToTexture * csttexture = allocate_element<CopyScreenToTexture>();
		ERR_FAIL_COND(!csttexture); // didn't fit
		
		csttexture->texture=allocate_iref_cache(p_texture);
		csttexture->source = p_source;
	}
	
	void cmd_scene_begin(IRef<Light>*p_visible_lights, int p_light_count) {
		
		ERR_FAIL_COND(inside_scene);
		ERR_FAIL_COND( is_render_list_full() );
		
		SceneBegin * scene_begin = allocate_element<SceneBegin>();
		scene_begin->visible_lights = p_visible_lights;
		scene_begin->light_count = p_light_count;
		ERR_FAIL_COND( !scene_begin );
		inside_scene=true;
		// will i add something to scene begin?				
	}
	
	void cmd_scene_end() {
		
		ERR_FAIL_COND(!inside_scene);
		ERR_FAIL_COND( is_render_list_full() );
						
		SceneEnd * scene_end = allocate_element<SceneEnd>();
		ERR_FAIL_COND( !scene_end );
		inside_scene=false;
		
		if (sort_method==RENDER_LIST_SORT_LAYER_ALPHA_DEPTH) {
			int sort_to=element_count-2;
			int sort_from=sort_to;
			
			
			bool render_list_valid=true;
			int alpha_count=0;
			while(elements[sort_from]->type!=Element::TYPE_SCENE_BEGIN) {
				
				if (elements[sort_from]->type!=Element::TYPE_PRIMITIVE && elements[sort_from]->type!=Element::TYPE_SURFACE) {
					
					render_list_valid=false;
					break;
				}
				
				if (((Visual*)elements[sort_from])->has_alpha)
					alpha_count++;
				
				sort_from--;
				if (sort_from<0) {
					render_list_valid=false;
					break;
				}
			}
			
			ERR_FAIL_COND(!render_list_valid);
			sort_from++;
			if (sort_from>sort_to)
				return; // render list empty
			
			Visual **v=(Visual**)(&elements[sort_from]);
			int len=(sort_to-sort_from)+1;
			SortArray<Visual*,_LAD_Comparator> sorter;
			sorter.sort(v,len);
			
			//printf("drawing %i elements, %i with alpha\n",len,alpha_count);
		}
				
		
		// will i add something to scene end?		
	}
		
	void cmd_add_surface(const IRef<SurfaceT>& p_surface,const IRef<SkeletonT>& p_skeleton,IRef<MaterialT> p_extra_material) {
		
		ERR_FAIL_COND( is_render_list_full() );
		
		SurfaceElement *surface_element = allocate_element<SurfaceElement>();
		ERR_FAIL_COND(!surface_element);
		surface_element->surface=allocate_iref_cache(p_surface);
		surface_element->skeleton=allocate_iref_cache(p_skeleton);
		if (p_extra_material)
			surface_element->extra_material=allocate_iref_cache(p_extra_material);
		//@TODO transform AABB by matrix
		
		if (inside_scene) {

			surface_element->world_matrix=matrix_cache[Renderer::MATRIX_WORLD];
			surface_element->lights=light_cache;
			surface_element->light_count=light_cache_count;
			
			surface_element->has_alpha=p_surface->has_alpha();
			IRef<Material> mat = p_surface->get_material();
			if (!mat.is_null())
				surface_element->layer=mat->get_layer();
			
			surface_element->aabb=allocate_mem<AABB>();
			ERR_FAIL_COND(!surface_element->aabb);
			*surface_element->aabb=p_surface->get_AABB();			
			/* approx. center of object */
			Vector3 center=surface_element->aabb->pos+surface_element->aabb->size/2.0;
			if (matrix_cache[Renderer::MATRIX_WORLD])
				matrix_cache[Renderer::MATRIX_WORLD]->transform(center);
			if (matrix_cache[Renderer::MATRIX_CAMERA]) {
				center-=matrix_cache[Renderer::MATRIX_CAMERA]->get_translation();
				Vector3 cam_z = matrix_cache[Renderer::MATRIX_CAMERA]->get_z();
			
				surface_element->z=-cam_z.dot(center);
			}
			
		}
	}
	
	void cmd_add_primitive(int p_points,const Vector3 *p_vertices, const Vector3 *p_normals,const Vector3 *p_binormals_and_tangents,const Color *p_colors,const float *p_uv) {

		ERR_FAIL_COND( is_render_list_full() );
		
		ERR_FAIL_COND(p_points<1 || p_points>4);
		ERR_FAIL_COND(!p_vertices);
		PrimitiveElement * re = allocate_element<PrimitiveElement>();
		ERR_FAIL_COND(!re); // didn't fit

		re->flags=p_points-1;
		re->vertices=allocate_mem<Vector3>(p_points);
		ERR_FAIL_COND(!re->vertices);
		
		for (int i=0;i<p_points;i++) {
			re->vertices[i]=p_vertices[i];
		}

#define _PACK_NORMAL(m_addr,m_v)\
		{\
		int n=(int)(m_v*127);\
		if (n<-127)\
		n=-127;\
		if (n>127)\
		n=127;\
		m_addr=(Sint8)n;\
	}

		if (p_normals) {

			re->flags|=PrimitiveElement::HAS_NORMALS;
			re->normals=allocate_mem<Sint8>(p_points*3);
			ERR_FAIL_COND(!re->normals);
			for (int i=0;i<p_points;i++) {
				_PACK_NORMAL(re->normals[i*3+0],p_normals[i].x);
				_PACK_NORMAL(re->normals[i*3+1],p_normals[i].y);
				_PACK_NORMAL(re->normals[i*3+2],p_normals[i].z);
			}
		}
/*
		if (p_binormals_and_tangents) {

			re->flags|=PrimitiveElement::HAS_BINORMAL_TANGENT;
			
			for (int i=0;i<p_points*2;i++) {
				_PACK_NORMAL(re->normals[p_points*3+i*3+0],p_binormals_and_tangents[i].x);
				_PACK_NORMAL(re->normals[p_points*3+i*3+1],p_binormals_and_tangents[i].y);
				_PACK_NORMAL(re->normals[p_points*3+i*3+2],p_binormals_and_tangents[i].z);
			}
		}
*/
#undef _PACK_NORMAL

		if (p_colors) {

			re->flags|=PrimitiveElement::HAS_COLORS;
			re->colors=allocate_mem<Color>(p_points);
			ERR_FAIL_COND(!re->colors);
			for (int i=0;i<p_points;i++) {
				re->colors[i]=p_colors[i];
				if (p_colors[i].a<255)
					re->has_alpha=true;
			}

		}

		if (p_uv) {

			re->flags|=PrimitiveElement::HAS_UVS;
			re->uvs=allocate_mem<float>(p_points*2);
			ERR_FAIL_COND(!re->uvs);
			for (int i=0;i<p_points*2;i++) {
				re->uvs[i]=p_uv[i];
			}
		}



		re->material=material_cache;

		if (material_cache && material_cache->has_alpha())
			re->has_alpha=true;
		
		if (p_uv) {
			if (texture_cache && texture_cache->has_alpha() && !(material_cache && material_cache->has_scissor_enabled()))
				re->has_alpha=true;
		} else {
			texture_cache=NULL;
		}

		if (inside_scene) {

			re->material=material_cache;
			if (re->material)
				re->layer=re->material->get_layer();
			re->texture=texture_cache;
			re->world_matrix=matrix_cache[Renderer::MATRIX_WORLD];

			re->lights=light_cache;
			re->light_count=light_cache_count;
			
			/* compute center and z */
			Vector3 center;
			for (int i=0;i<p_points;i++)
				center+=p_vertices[i];
			center/=(real_t)p_points;
			
			if (matrix_cache[Renderer::MATRIX_WORLD])
				matrix_cache[Renderer::MATRIX_WORLD]->transform(center);
			if (matrix_cache[Renderer::MATRIX_CAMERA]) {
				center-=matrix_cache[Renderer::MATRIX_CAMERA]->get_translation();
				Vector3 cam_z = matrix_cache[Renderer::MATRIX_CAMERA]->get_z();
			
				re->z=-cam_z.dot(center);
			}
				
		}
				
	}
	
	void clear() {
		
		for (int i=0;i<element_count;i++) {
			
			elements[i]->~Element();
			elements[i]=NULL;
		}
		
		element_count=0;
		buffer_pos=0;
		inside_scene=false;
		
		light_cache=NULL;
		light_cache_count=0;
		matrix_cache[0]=NULL;
		matrix_cache[1]=NULL;
		matrix_cache[2]=NULL;
		material_cache=NULL;
		texture_cache=NULL;
		
		/* erase iref cache */
		while(iref_last) {
			
			IRefCache *aux=iref_last;
			iref_last=aux->prev;
			aux->~IRefCache();
		}
		
	}
	
	RenderList() {
		
		MAX_ELEMENTS=GlobalVars::get_singleton()->get("render/render_list_elements").get_int();
		BUFFER_SIZE=GlobalVars::get_singleton()->get("render/render_list_size_kb").get_int()*1024;
		//printf("render list buffer size %i, elements %i\n",BUFFER_SIZE,MAX_ELEMENTS);
		element_data_buffer=NULL;
		elements=NULL;
		element_data_buffer = memnew_arr( Uint8, BUFFER_SIZE );
		elements = memnew_arr( Element*, MAX_ELEMENTS );
		
		element_count=0; 
		buffer_pos=0; 	
		inside_scene=false;	
		iref_last=NULL;
	}
	
	~RenderList() { clear(); }
};




#endif
