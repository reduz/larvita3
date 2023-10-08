//
// C++ Interface: surface_tool
//
// Description: 
//
//
// Author: ,,, <red@lunatea>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef SURFACE_TOOL_H
#define SURFACE_TOOL_H


#include "render/surface.h"
#include "vector.h"

/**
	@author ,,, <red@lunatea>
*/

class SurfaceTool {

public:
	struct Vertex {
	
		Vector3 vertex;
		Color color;
		Vector3 normal; // normal, binormal, tangent
		Vector3 binormal;
		Vector3 tangent;		
		Vector< Vector3 > uv; // u,v,w
		Vector< Variant > custom;
		Uint8 weight[Surface::MAX_WEIGHT_BYTES];
		
		bool same_as(const Vertex& p_vertex) const;
				
		Vertex() { for (int i=0;i<Surface::MAX_WEIGHT_BYTES;i++) weight[i]=0; }
	};

private:
	//format
	Surface::PrimitiveType primitive;
	int format;
	IRef<Material> material;
	//arrays
	DVector< Vertex > vertex_array;
	Vector< int > index_array;
	Vector< int > weight_bone_idx_array;
	
	//memory
	Color last_color;
	Vector3 last_normal;
	Vector< Vector3 > last_uv;
	Uint8 last_weight[8];
	Vector3 last_binormal;
	Vector3 last_tangent;
	Vector< Variant > last_custom;

	void create(Surface::PrimitiveType p_primitive,int p_format,const Uint8* p_custom_array_fmt=NULL);

public:

	DVector< Vertex > get_vertex_array() const;

	void add_vertex( const Vector3& p_vertex);
	void add_color( Color p_color );
	void add_normal( const Vector3& p_normal);
	void add_tangent( const Vector3& p_tangent );
	void add_binormal( const Vector3& p_binormal );
	void add_uv( int p_layer, const Vector3& p_uv);
	void add_weight( const Uint8 *p_weight);
	void add_custom( int p_layer, const Variant& p_custom);

	void set_weigh_bone_index(int p_layer, int p_bone);

	void index();
	void deindex();
	void generate_tangents();

	Surface::PrimitiveType get_primitive_type() const { return this->primitive; };
	int get_format() const { return this->format; }

	IRef<Surface> commit();
	SurfaceTool(IRef<Surface> p_surface);
	SurfaceTool(Surface::PrimitiveType p_primitive,int p_format,
			const Uint8* p_custom_array_fmt=NULL,
			const DVector<Vertex>& p_vertex_array=DVector<Vertex>());
	SurfaceTool() { format = 0; }
	~SurfaceTool();

};

#endif
