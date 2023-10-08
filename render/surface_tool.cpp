//
// C++ Implementation: surface_tool
//
// Description: 
//
//
// Author: ,,, <red@lunatea>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "surface_tool.h"
#include "render/renderer.h"

#define _VERTEX_SNAP 0.0001
#define EQ_VERTEX_DIST 0.00001

bool SurfaceTool::Vertex::same_as(const Vertex& p_vertex) const {
	
	if (vertex.distance_to(p_vertex.vertex)>EQ_VERTEX_DIST)
		return false;
	
	for (int i=0;i<uv.size();i++) {
		if (uv[i].distance_to(p_vertex.uv[i])>EQ_VERTEX_DIST)
			return false;
	}
	
	if (normal.distance_to(p_vertex.normal)>EQ_VERTEX_DIST)
		return false;

	
	return true;
}

void SurfaceTool::add_vertex( const Vector3& p_vertex) {

	Vertex vtx;
	vtx.vertex=p_vertex;
	vtx.vertex.snap(_VERTEX_SNAP);
	vtx.color=last_color;
	vtx.normal=last_normal;
	vtx.uv=last_uv;
	for(int i=0;i<Surface::MAX_WEIGHT_BYTES;i++)
		vtx.weight[i]=last_weight[i];
	vtx.binormal=last_binormal;
	vtx.tangent=last_tangent;
	vtx.custom=last_custom;
	vertex_array.push_back(vtx);

}
void SurfaceTool::add_color( Color p_color ) {

	last_color=p_color;
}
void SurfaceTool::add_normal( const Vector3& p_normal) {


	last_normal=p_normal;
}


void SurfaceTool::add_tangent( const Vector3& p_tangent) {

	
	last_tangent=p_tangent;

}


void SurfaceTool::add_binormal( const Vector3& p_binormal) {

	
	last_binormal=p_binormal;

}


void SurfaceTool::add_uv( int p_layer, const Vector3& p_uv) {

	ERR_FAIL_INDEX( p_layer, last_uv.size() );
	last_uv.set(p_layer,p_uv);
	
}
void SurfaceTool::add_weight( const Uint8 *p_weight) {

	for (int i=0;i<Surface::MAX_WEIGHT_BYTES;i++) {
		
		last_weight[i]=p_weight[i];
	}
}
void SurfaceTool::add_custom( int p_layer, const Variant& p_custom) {
	
	ERR_FAIL_INDEX( p_layer, last_custom.size() );
	last_custom.set(p_layer,p_custom);
}

IRef<Surface> SurfaceTool::commit() {


	printf("commit begin\n");
	IRef<Surface> surface = Renderer::get_singleton()->create_surface();

	ERR_FAIL_COND_V( surface.is_null(), surface );
	surface->create(primitive,format,0,vertex_array.size(),index_array.size());

	int varr_len=vertex_array.size();
	vertex_array.read_lock();
	const Vertex *varr = vertex_array.read();

	for (int i=0;i<32;i++) {
	
		switch(format&(1<<i)) {
		
			case Surface::FORMAT_ARRAY_VERTEX: 
			case Surface::FORMAT_ARRAY_NORMAL: {
			
				Variant array( Variant::REAL_ARRAY, vertex_array.size()*3 );
				
				
				for (int v=0;v<varr_len;v++) { 
				
					float vec[3];
					switch(format&(1<<i)) {
						case Surface::FORMAT_ARRAY_VERTEX: { 
							vec[0]=varr[v].vertex.x;
							vec[1]=varr[v].vertex.y;
							vec[2]=varr[v].vertex.z;
						} break;
						case Surface::FORMAT_ARRAY_NORMAL: { 
							vec[0]=varr[v].normal.x;
							vec[1]=varr[v].normal.y;
							vec[2]=varr[v].normal.z;
						} break;
					}
					
					array.array_set_real(v*3+0,vec[0]);
					array.array_set_real(v*3+1,vec[1]);
					array.array_set_real(v*3+2,vec[2]);
				}
			
				surface->set_array( (Surface::ArrayType)i, array );
			} break;
			case Surface::FORMAT_ARRAY_TANGENT: {
			
				
				Variant array( Variant::REAL_ARRAY, vertex_array.size()*4 );
				
				
				for (int v=0;v<varr_len;v++) { 
				
					float vec[4];
					
					vec[0]=varr[v].tangent.x;
					vec[1]=varr[v].tangent.y;
					vec[2]=varr[v].tangent.z;
					
					Vector3 fake_bn = Vector3::cross(varr[v].normal,varr[v].tangent);
					
					vec[3]=fake_bn.dot(varr[v].binormal)<0 ? -1.0 : 1.0;
					
					array.array_set_real(v*4+0,vec[0]);
					array.array_set_real(v*4+1,vec[1]);
					array.array_set_real(v*4+2,vec[2]);
					array.array_set_real(v*4+3,vec[3]);
				}
				printf("setting tangent array\n");
				surface->set_array( (Surface::ArrayType)i, array );
				
			} break;
			case Surface::FORMAT_ARRAY_COLOR:  {
				
				Variant array( Variant::INT_ARRAY, vertex_array.size() );
								
				for (int v=0;v<varr_len;v++) { 
				
					array.array_set_int(v,varr[v].color.to_32());
				}
			
				surface->set_array( (Surface::ArrayType)i, array );
			
			} break;
			case Surface::FORMAT_ARRAY_TEX_UV_0:
			case Surface::FORMAT_ARRAY_TEX_UV_1:
			case Surface::FORMAT_ARRAY_TEX_UV_2:
			case Surface::FORMAT_ARRAY_TEX_UV_3:
			case Surface::FORMAT_ARRAY_TEX_UV_4:
			case Surface::FORMAT_ARRAY_TEX_UV_5:
			case Surface::FORMAT_ARRAY_TEX_UV_6:
			case Surface::FORMAT_ARRAY_TEX_UV_7: {
			
				
				Variant array( Variant::REAL_ARRAY, vertex_array.size()*2 );
								
				for (int v=0;v<varr_len;v++) { 
				
					array.array_set_real(v*2+0,varr[v].uv.get(i-Surface::ARRAY_TEX_UV_0).x);
					array.array_set_real(v*2+1,varr[v].uv.get(i-Surface::ARRAY_TEX_UV_0).y);
				}
			
				surface->set_array( (Surface::ArrayType)i, array );
			
			} break;
			case Surface::FORMAT_ARRAY_WEIGHTS: {
			
				Variant array( Variant::BYTE_ARRAY, vertex_array.size()*Surface::MAX_WEIGHT_BYTES );
								
				for (int v=0;v<varr_len;v++) { 
				
					for (int w=0;w<Surface::MAX_WEIGHT_BYTES;w++)  
					
						array.array_set_byte(v*Surface::MAX_WEIGHT_BYTES+w,varr[v].weight[w]);
				}
			
				surface->set_array( (Surface::ArrayType)i, array );
			
			} break;
			case Surface::FORMAT_ARRAY_CUSTOM_0:  break;
			case Surface::FORMAT_ARRAY_CUSTOM_1:  break;
			case Surface::FORMAT_ARRAY_CUSTOM_2:  break;
			case Surface::FORMAT_ARRAY_CUSTOM_3:  break;
			case Surface::FORMAT_ARRAY_INDEX: {
				
				ERR_FAIL_COND_V(index_array.size()==0,IRef<Surface>());
				Variant iarray( Variant::INT_ARRAY, index_array.size() );
								
				for (int v=0;v<iarray.size();v++) { 
				
					iarray.array_set_int(v,index_array[v]);
				}
			
				surface->set_array( (Surface::ArrayType)i, iarray );
				
			} break;
			
			default: {}
		}
	
	}

	vertex_array.read_unlock();
	surface->set_material(material);
	printf("commit end\n");
	
	return surface;
}


void SurfaceTool::create(Surface::PrimitiveType p_primitive,int p_format,const Uint8* p_custom_array_fmt) {

	primitive=p_primitive;
	format=p_format;
	for (int i=0;i<32;i++) {
		
		switch(format&(1<<i)) {
		
			case Surface::FORMAT_ARRAY_VERTEX:  break;
			case Surface::FORMAT_ARRAY_NORMAL:  break;
			case Surface::FORMAT_ARRAY_COLOR:  break;
			case Surface::FORMAT_ARRAY_TEX_UV_0: last_uv.resize(1); break;
			case Surface::FORMAT_ARRAY_TEX_UV_1: last_uv.resize(2);	break;
			case Surface::FORMAT_ARRAY_TEX_UV_2: last_uv.resize(3);  break;
			case Surface::FORMAT_ARRAY_TEX_UV_3: last_uv.resize(4); break;
			case Surface::FORMAT_ARRAY_TEX_UV_4: last_uv.resize(5); break;
			case Surface::FORMAT_ARRAY_TEX_UV_5: last_uv.resize(6); break;
			case Surface::FORMAT_ARRAY_TEX_UV_6: last_uv.resize(7); break;
			case Surface::FORMAT_ARRAY_TEX_UV_7: last_uv.resize(8); break;
			case Surface::FORMAT_ARRAY_CUSTOM_0: last_custom.resize(1); break;
			case Surface::FORMAT_ARRAY_CUSTOM_1: last_custom.resize(2); break;
			case Surface::FORMAT_ARRAY_CUSTOM_2: last_custom.resize(3); break;
			case Surface::FORMAT_ARRAY_CUSTOM_3: last_custom.resize(4); break;
			default: {}
		}
	}
	/*
	if (format & Surface::FORMAT_ARRAY_BINORMAL) {
		format = format ^ Surface::FORMAT_ARRAY_BINORMAL;
	};
	*/

}

void SurfaceTool::index() {
	
	
	printf("indexing..\n");
	ERR_FAIL_COND( format & Surface::FORMAT_ARRAY_INDEX ); // already indexed
	
	index_array.clear();
	DVector< Vertex > indexed_vertex_array;
	
	int vertex_array_len = vertex_array.size();
	vertex_array.read_lock();
	const Vertex*vertex_array_ptr = vertex_array.read();
	
	for (int i=0;i<vertex_array_len;i++) {
				
		int index_pos=-1;
		
		int indexed_vertex_array_len=indexed_vertex_array.size();
		
		if (indexed_vertex_array_len) {
		
			indexed_vertex_array.read_lock();
			const Vertex* indexed_vertex_array_ptr=indexed_vertex_array.read();
			
			for (int j=0;j<indexed_vertex_array_len;j++) {
				
				if (vertex_array_ptr[i].same_as(indexed_vertex_array_ptr[j])) {
					
					index_pos=j;
					break;
				}
			}
			
			indexed_vertex_array.read_unlock();
		}
		
		if (index_pos==-1) {
			
			index_pos=indexed_vertex_array.size();
			indexed_vertex_array.push_back(vertex_array_ptr[i]);
		} else {
		
			indexed_vertex_array.write_lock();
			indexed_vertex_array.write()[index_pos].normal+=vertex_array_ptr[i].normal;
			indexed_vertex_array.write()[index_pos].binormal+=vertex_array_ptr[i].binormal;
			indexed_vertex_array.write()[index_pos].tangent+=vertex_array_ptr[i].tangent;
			indexed_vertex_array.write_unlock();
		}
		
		index_array.push_back(index_pos);
	}
	
	int idxvertsize=indexed_vertex_array.size();
	indexed_vertex_array.write_lock();
	Vertex* idxvert=indexed_vertex_array.write();
	for (int i=0;i<idxvertsize;i++) {
	
		idxvert[i].normal.normalize();
		idxvert[i].tangent.normalize();
		idxvert[i].binormal.normalize();
	}
	indexed_vertex_array.write_unlock();
	
	vertex_array.read_unlock();
	
	format|=Surface::FORMAT_ARRAY_INDEX;
	vertex_array=indexed_vertex_array;
	
	printf("indexing.. end\n");
	
}

void SurfaceTool::deindex() {
	
	
}

void SurfaceTool::generate_tangents() {

	if (!(format&Surface::FORMAT_ARRAY_TEX_UV_0))
		return;
		
	int len=vertex_array.size();
	vertex_array.write_lock();
	Vertex *vertexptr=vertex_array.write();
	
	for (int i=0;i<len/3;i++) {
	

		Vector3 v1 = vertexptr[i*3+0].vertex;
		Vector3 v2 = vertexptr[i*3+1].vertex;
		Vector3 v3 = vertexptr[i*3+2].vertex;
		
		Vector3 w1 = vertexptr[i*3+0].uv[0];
		Vector3 w2 = vertexptr[i*3+1].uv[0];
		Vector3 w3 = vertexptr[i*3+2].uv[0];
		

		float x1 = v2.x - v1.x;
		float x2 = v3.x - v1.x;
		float y1 = v2.y - v1.y;
		float y2 = v3.y - v1.y;
		float z1 = v2.z - v1.z;
		float z2 = v3.z - v1.z;
		
		float s1 = w2.x - w1.x;
		float s2 = w3.x - w1.x;
		float t1 = w2.y - w1.y;
		float t2 = w3.y - w1.y;
		
		float r  = (s1 * t2 - s2 * t1);
		
		Vector3 binormal,tangent;
		
		if (r==0) {
			binormal=Vector3(0,0,0);
			tangent=Vector3(0,0,0);
		} else {
			tangent = Vector3((t2 * x1 - t1 * x2) * r, (t2 * y1 - t1 * y2) * r,
			(t2 * z1 - t1 * z2) * r);
			binormal = Vector3((s1 * x2 - s2 * x1) * r, (s1 * y2 - s2 * y1) * r,
			(s1 * z2 - s2 * z1) * r);
		}
						
		tangent.normalize();
		binormal.normalize();
		Vector3 normal=Plane( v1, v2, v3 ).normal;
			
		Vector3 tangentp = tangent - normal * normal.dot( tangent );
		Vector3 binormalp = binormal - normal * (normal.dot(binormal)) - tangent * (tangent.dot(binormal));
		
		tangentp.normalize();
		binormalp.normalize();
	
		
		for (int j=0;j<3;j++) {
			vertexptr[i*3+j].normal=normal;
			vertexptr[i*3+j].binormal=binormalp;				
			vertexptr[i*3+j].tangent=tangentp;				
		}
	}
	
	format|=Surface::FORMAT_ARRAY_TANGENT;
	printf("adding tangents to the format\n");
	
	vertex_array.write_unlock();
		
}

SurfaceTool::SurfaceTool(IRef<Surface> p_surface) {

			
	Uint32 format=0;
	
	Variant vertices;
	Variant normals;
	Variant tangents;
	Variant colors;
	Variant tex_uv[8];
	Variant weights;
	Variant indices;
	
		
	for (int i=0;i<32;i++) {
		
		Surface::ArrayType type = (Surface::ArrayType)i;
		if (i!=Surface::ARRAY_INDEX && p_surface->get_format()&(1<<i))
			format|=(1<<i);
		switch(p_surface->get_format()&(1<<i)) {
		
			case Surface::FORMAT_ARRAY_VERTEX:  vertices=p_surface->get_array(type); break;
			case Surface::FORMAT_ARRAY_NORMAL: normals=p_surface->get_array(type);break;
			case Surface::FORMAT_ARRAY_COLOR: colors=p_surface->get_array(type); break;
			case Surface::FORMAT_ARRAY_TANGENT: tangents=p_surface->get_array(type); break;
			case Surface::FORMAT_ARRAY_TEX_UV_0:
 			case Surface::FORMAT_ARRAY_TEX_UV_1:
			case Surface::FORMAT_ARRAY_TEX_UV_2:
			case Surface::FORMAT_ARRAY_TEX_UV_3:
			case Surface::FORMAT_ARRAY_TEX_UV_4:
			case Surface::FORMAT_ARRAY_TEX_UV_5:
			case Surface::FORMAT_ARRAY_TEX_UV_6:
			case Surface::FORMAT_ARRAY_TEX_UV_7:
				tex_uv[Surface::ARRAY_TEX_UV_0-i]=p_surface->get_array(type); break;
			case Surface::FORMAT_ARRAY_WEIGHTS:
				weights=p_surface->get_array(type);
				break;
			case Surface::FORMAT_ARRAY_INDEX:
				indices=p_surface->get_array(type);
				break;
			default: {}
		}
	}
	
	create(p_surface->get_primitive_type(),format);
	material=p_surface->get_material();
	
	int array_len=p_surface->get_array_len();
	int index_array_len=p_surface->get_index_array_len();
	ERR_FAIL_COND( index_array_len>=0 && indices.get_type() == Variant::NIL );
	int count=(index_array_len > 0) ? index_array_len : array_len;
	
	
	for (int i=0;i<count;i++) {
	
		int idx=(index_array_len > 0) ? indices.array_get_int(i) : i;
		
		if (weights.get_type()!=Variant::NIL) {
		
			Uint8 w[8];
			for (int j=0;j<8;j++) {
			
				w[j]=weights.array_get_byte(idx*8+j);
			}
			add_weight(w);
		}
		
		for (int j=0;j<8;j++) {
			
			if (tex_uv[j].get_type()!=Variant::NIL) {
			
				Vector3 uv( tex_uv[j].array_get_real(idx*2+0), tex_uv[j].array_get_real(idx*2+1),0.0 );
				add_uv(j,uv);
			}
		}
		
		if (colors.get_type()!=Variant::NIL) {
		
			Uint32 col=colors.array_get_int(idx);
			Color c;
			c.from_32(col);
			add_color(c);
		
		}
	
		Vector3 normal;
		if (normals.get_type()!=Variant::NIL) {
		
			normal =Vector3( 
				normals.array_get_real( idx*3+0 ), 			
				normals.array_get_real( idx*3+1 ), 			
				normals.array_get_real( idx*3+2 )
				);
					
			add_normal(normal);
		}
	
		if (tangents.get_type()!=Variant::NIL) {
		
			float w[4];
			for (int j=0;j<4;j++) {
			
				w[j]=tangents.array_get_real(idx*4+j);
			}
			
			Vector3 tangent(w[0],w[1],w[2]);
			add_tangent(tangent);
			Vector3 binormal=Vector3::cross(normal,tangent)*w[3];
			binormal.normalize();
			add_binormal(binormal);
		}
	
	
	
		if (vertices.get_type()!=Variant::NIL) {
		
			Vector3 vertex( 
				vertices.array_get_real( idx*3+0 ), 			
				vertices.array_get_real( idx*3+1 ), 			
				vertices.array_get_real( idx*3+2 )
				);
				
			add_vertex(vertex);
		}
	
	}
	
	
}

SurfaceTool::SurfaceTool(Surface::PrimitiveType p_primitive,int p_format,
		const Uint8* p_custom_array_fmt, const DVector<Vertex>& p_vertex_array) {

	create(p_primitive,p_format,p_custom_array_fmt);		

	this->vertex_array = p_vertex_array;
}


void SurfaceTool::set_weigh_bone_index(int p_layer, int p_bone) {

	ERR_FAIL_INDEX(p_layer,weight_bone_idx_array.size());
   	weight_bone_idx_array.set(p_layer,p_bone); 
}

DVector< SurfaceTool::Vertex > SurfaceTool::get_vertex_array() const {

   return vertex_array;
}

SurfaceTool::~SurfaceTool() {
}


