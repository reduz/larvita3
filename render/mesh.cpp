//
// C++ Implementation: mesh
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "mesh.h"

AABB Mesh::get_AABB() const {
	
	AABB aabb;
	
	for (int i=0;i<get_surface_count();i++) {
		
		IRef<Surface> surf=get_surface(i);
		
		if (i==0)
			aabb=surf->get_AABB();
		else 
			aabb.merge_with( surf->get_AABB() );
	}
	
	return aabb;
}

DVector<Face3> Mesh::get_faces() const {
	
	DVector<Face3> faces;
	
	for (int i=0;i<get_surface_count();i++) {
		
		IRef<Surface> surf=get_surface(i);
		
		if (surf->get_primitive_type()!=Surface::PRIMITIVE_TRIANGLES)
			continue;
		bool use_indices = surf->get_format()&Surface::FORMAT_ARRAY_INDEX;
		int len = use_indices?surf->get_index_array_len():surf->get_array_len();
		printf("index array len %i, vertex array len %i\n",surf->get_index_array_len(),surf->get_array_len());
		Variant vertices=surf->get_array(Surface::ARRAY_VERTEX);
		Variant indices=use_indices?surf->get_array(Surface::ARRAY_INDEX):Variant();
				
		Face3 face;
		printf("vertices size %i\n",vertices.size());
		for (int j=0;j<len;j++) {
			
			Vector3 v;
			int idx=use_indices?indices.array_get_int(j):j;
			
			v.x=vertices.array_get_real(idx*3+0);
			v.y=vertices.array_get_real(idx*3+1);
			v.z=vertices.array_get_real(idx*3+2);
			
			face.vertex[j%3]=v;
			if ((j%3)==2)
				faces.push_back(face);
		}

		printf("surface %i - %i vertices - %i faces\n",i,len,faces.size());
	}
	
	return faces;
}

Mesh::Mesh()
{
}


Mesh::~Mesh()
{
}


