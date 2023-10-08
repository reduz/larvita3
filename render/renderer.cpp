//
// C++ Implementation: renderer
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "renderer.h"

Renderer * Renderer::singleton=NULL;
Renderer*(*Renderer::create_func)()=NULL;	

IRef<Texture> Renderer::get_error_texture() {

	if (!error_texture.is_null())
		return error_texture;
		
	error_texture = create_texture();
	
#define ERROR_TEXTURE_SIZE 256
	
	Image data(ERROR_TEXTURE_SIZE,ERROR_TEXTURE_SIZE,Image::FORMAT_RGB);
	
	for (int x=0;x<ERROR_TEXTURE_SIZE;x++) {
		
		for (int y=0;y<ERROR_TEXTURE_SIZE;y++) {
		
			Color c;
			int r=255-(x+y)/2;
			
			if ((x%(ERROR_TEXTURE_SIZE/8))<2 ||(y%(ERROR_TEXTURE_SIZE/8))<2) {
				
				c.r=y;
				c.g=r;
				c.b=x;
				
			} else {
				
				c.r=r;
				c.g=x;
				c.b=y;
			}
			
			data.put_pixel(x, y, c);
		}
	}
	
	
	error_texture->load( data );
	
	return error_texture;

}


IRef<Material> Renderer::get_default_material() {


	if (!default_material.is_null())
		return default_material;
		
	default_material = create_material();
	
	default_material->set("color/ambient",Color(50,50,50));
	default_material->set("color/diffuse",Color(190,190,190));
	default_material->set("color/specular",Color(255,255,255));
	default_material->set("color/emission",Color(0,0,0));
	default_material->set("flags/visible",true);
	default_material->set("flags/double_sided",false);
	default_material->set("flags/invert_normals",false);
	default_material->set("flags/shaded",true);
	default_material->set("flags/draw_z",true);
	default_material->set("flags/test_z",Renderer::DEPTH_TEST_LESS);
	default_material->set("flags/wireframe",false);
	default_material->set("vars/transparency",0.0f);
	default_material->set("vars/shininess",0.2f);
	default_material->set("vars/line_thickness",1.0f);
	default_material->set("vars/normalmap_depth",0.5f);
	
	return default_material;
}

IRef<Material> Renderer::get_gui_material() {


	if (!gui_material.is_null())
		return gui_material;
		
	gui_material = create_material();
	
	gui_material->set("color/ambient",Color(255,255,255));
	gui_material->set("color/diffuse",Color(255,255,255));
	gui_material->set("color/specular",Color(255,255,255));
	gui_material->set("color/emission",Color(0,0,0));
	gui_material->set("flags/visible",true);
	gui_material->set("flags/double_sided",true);
	gui_material->set("flags/invert_normals",false);
	gui_material->set("flags/shaded",false);
	gui_material->set("flags/draw_z",false);
	gui_material->set("flags/test_z",false);
	gui_material->set("flags/wireframe",false);
	gui_material->set("vars/transparency",0.0f);
	gui_material->set("vars/shininess",0.2f);
	gui_material->set("vars/line_thickness",1.0f);
	gui_material->set("vars/normalmap_depth",0.5f);
	
	return gui_material;
}

IRef<Material> Renderer::get_indicator_material() {
	
	if (indicator_material) {
		return indicator_material;
	};
	
	indicator_material = create_material();

	indicator_material->set("flags/visible",true);
	indicator_material->set("flags/double_sided",true);
	indicator_material->set("flags/invert_normals",false);
	indicator_material->set("flags/shaded",false);
	indicator_material->set("flags/draw_z",true);
	indicator_material->set("flags/wireframe",false);

	indicator_material->set("vars/transparency",0.0f);
	indicator_material->set("vars/shininess",0.0f);
	indicator_material->set("vars/line_thickness",1.0f);

	return indicator_material;
};


Renderer* Renderer::get_singleton() {

	return singleton;
}


IRef<Mesh> Renderer::get_test_cube() {

	if (!test_cube.is_null())
		return test_cube;

	/* cube Mesh */
		
		
	IRef<Surface> surface = Renderer::get_singleton()->create_surface();
	surface->create( Surface::PRIMITIVE_TRIANGLES, Surface::FORMAT_ARRAY_INDEX|Surface::FORMAT_ARRAY_VERTEX|Surface::FORMAT_ARRAY_NORMAL|Surface::FORMAT_ARRAY_TEX_UV_0,0,36,36);
		
	Variant vertices(Variant::REAL_ARRAY,36*3);
	Variant normals(Variant::REAL_ARRAY,36*3);
	Variant uvs(Variant::REAL_ARRAY,36*2);
								
	int vtx_idx=0;
#define ADD_VTX(m_idx);\
	vertices.array_set_real(vtx_idx*3+0,face_points[m_idx].x);	\
	vertices.array_set_real(vtx_idx*3+1,face_points[m_idx].y);	\
	vertices.array_set_real(vtx_idx*3+2,face_points[m_idx].z);	\
	normals.array_set_real(vtx_idx*3+0,normal_points[m_idx].x);	\
	normals.array_set_real(vtx_idx*3+1,normal_points[m_idx].y);	\
	normals.array_set_real(vtx_idx*3+2,normal_points[m_idx].z);	\
	uvs.array_set_real(vtx_idx*2+0,uv_points[m_idx*2+0]);		\
	uvs.array_set_real(vtx_idx*2+1,uv_points[m_idx*2+1]);		\
	vtx_idx++;
	
	for (int i=0;i<6;i++) {
	
		
		Vector3 face_points[4];
		Vector3 normal_points[4];
		float uv_points[8]={0,0,0,1,1,1,1,0};
		
		for (int j=0;j<4;j++) {
			
			float v[3];
			v[0]=1.0;
			v[1]=1-2*((j>>1)&1);
			v[2]=v[1]*(1-2*(j&1));
			
			for (int k=0;k<3;k++) {
				
				if (i<3)
					face_points[j].set((i+k)%3,v[k]*(i>=3?-1:1));
				else
					face_points[3-j].set((i+k)%3,v[k]*(i>=3?-1:1));
			}
			normal_points[j]=Vector3();
			normal_points[j].set(i%3,(i>=3?-1:1));
		}
		
		//for (int j=0;j<4;j++)
		//	printf("%i - %f,%f,%f , %f,%f,%f\n",j,face_points[j].x,face_points[j].y,face_points[j].z,normal_points[j].x,normal_points[j].y,normal_points[j].z);
		
		//tri 1
		ADD_VTX(0);
		ADD_VTX(1);
		ADD_VTX(2);
		//tri 2
		ADD_VTX(2);
		ADD_VTX(3);
		ADD_VTX(0);
		
	}

	Variant indices(Variant::INT_ARRAY, vertices.size() / 3);
	for (int i=0; i<vertices.size() / 3; i++) {
		
		indices.array_set_int(i, i);
	};
	
	surface->set_array( Surface::ARRAY_VERTEX, vertices );
	surface->set_array( Surface::ARRAY_NORMAL, normals );
	surface->set_array( Surface::ARRAY_TEX_UV_0, uvs );
	surface->set_array( Surface::ARRAY_INDEX, indices );
	
	IRef<Material> cube_material = create_material();
	cube_material->set("textures/diffuse",IObjRef(get_error_texture()));
	surface->set_material(cube_material);
	
	test_cube = Renderer::get_singleton()->create_mesh();
	test_cube->add_surface( surface );
	return test_cube;
#undef ADD_VTX	
	
}

IRef<Mesh> Renderer::make_sphere(int p_lats,int p_lons,float p_scale) {


	
	int vcount = (p_lats+1) * (p_lons+1) *6-6;
	IRef<Surface> surface = Renderer::get_singleton()->create_surface();
	surface->create( Surface::PRIMITIVE_TRIANGLES, Surface::FORMAT_ARRAY_VERTEX|Surface::FORMAT_ARRAY_NORMAL,0,vcount);
	Variant vertices(Variant::REAL_ARRAY,vcount*3);
	Variant normals(Variant::REAL_ARRAY,vcount*3);
		
	int vtx_idx=0;
#define ADD_VTX(m_vtx);\
	vertices.array_set_real(vtx_idx*3+0,m_vtx.x);	\
	vertices.array_set_real(vtx_idx*3+1,m_vtx.y);	\
	vertices.array_set_real(vtx_idx*3+2,m_vtx.z);	\
	normals.array_set_real(vtx_idx*3+0,m_vtx.x);	\
	normals.array_set_real(vtx_idx*3+1,m_vtx.y);	\
	normals.array_set_real(vtx_idx*3+2,m_vtx.z);	\
	vtx_idx++;
		
	Vector3 last_vtx_1;
	Vector3 last_vtx_2;

	int i, j;
	for(i = 0; i <= p_lats; i++) {
		double lat0 = Math_PI * (-0.5 + (double) (i - 1) / p_lats);
		double z0  = Math::sin(lat0);
		double zr0 =  Math::cos(lat0);
	
		double lat1 = Math_PI * (-0.5 + (double) i / p_lats);
		double z1 = Math::sin(lat1);
		double zr1 = Math::cos(lat1);
	
		for(j = p_lons; j >= 0; j--) {
			double lng = 2 * Math_PI * (double) (j - 1) / p_lons;
			double x = Math::cos(lng);
			double y = Math::sin(lng);
			
			Vector3 vtx_1(x * zr0, z0,y * zr0);
			Vector3 vtx_2(x * zr1, z1,y * zr1);
			vtx_1*=p_scale;
			vtx_2*=p_scale;
			
			if (i>0 || j<p_lons) {
				ADD_VTX(last_vtx_1);
				ADD_VTX(vtx_1);
				ADD_VTX(last_vtx_2);
				
				ADD_VTX(vtx_1);
				ADD_VTX(last_vtx_1);
				ADD_VTX(vtx_2);
			}
			
			last_vtx_1=vtx_2;
			last_vtx_2=vtx_1;
		}
	}	
	
	surface->set_array( Surface::ARRAY_VERTEX, vertices );
	surface->set_array( Surface::ARRAY_NORMAL, normals );
	
	IRef<Mesh> sphere = Renderer::get_singleton()->create_mesh();
	sphere->add_surface( surface );
	printf("added being %i\n",vtx_idx);
	return sphere;	
#undef ADD_VTX	

}

Renderer* Renderer::create() {

	ERR_FAIL_COND_V(singleton,NULL); //already created
	ERR_FAIL_COND_V(!create_func,NULL);
	return create_func();

}


Renderer::Renderer() {

	ERR_FAIL_COND(singleton && singleton!=this);
	singleton=this;
}


Renderer::~Renderer() {

	singleton=NULL;
}


