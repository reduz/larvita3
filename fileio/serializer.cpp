//
// C++ Implementation: serializer
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "serializer.h"


void Serializer::add_vector3(const Vector3& p_v) {
	
	add_float(p_v.x);	
	add_float(p_v.y);	
	add_float(p_v.z);	
}
void Serializer::add_matrix4(const Matrix4& p_m) {
	
	for (int i=0;i<4;i++) 
		for (int j=0;j<4;j++) 
			add_double(p_m.elements[i][j]);			
}
void Serializer::add_plane(const Plane& p_p) {
	
	add_double(p_p.normal.x);	
	add_double(p_p.normal.y);	
	add_double(p_p.normal.z);	
	add_double(p_p.d);	
	
}
void Serializer::add_rect3(const Rect3& p_r3) {
	
	add_vector3(p_r3.pos);
	add_vector3(p_r3.size);
	
}
void Serializer::add_quat(const Quat& p_q) {
	
	add_double(p_q.x);
	add_double(p_q.y);
	add_double(p_q.z);
	add_double(p_q.w);
	
}
void Serializer::add_color(const Color& p_c) {
	
	add_uint8(p_c.r);
	add_uint8(p_c.g);
	add_uint8(p_c.b);
	add_uint8(p_c.a);
}


Serializer::Serializer()
{
}


Serializer::~Serializer()
{
}


