//
// C++ Implementation: deserializer
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "deserializer.h"

 Vector3 Deserializer::get_vector3() {
	
	Vector3 v;
	v.x=get_float();
	v.y=get_float();
	v.z=get_float();
	return v;
}
 Matrix4 Deserializer::get_matrix4() {
	
	Matrix4 m;
	for (int i=0;i<4;i++) 
		for (int j=0;j<4;j++) 
			m.elements[i][j]=get_double();			
	
	return m;	
}
 Plane Deserializer::get_plane() {
	
	Plane p;
	p.normal.x=get_double();
	p.normal.y=get_double();
	p.normal.z=get_double();
	p.d=get_double();
	return p;
}
 Rect3 Deserializer::get_rect3() {
	
	Rect3 r3;
	r3.pos=get_vector3();
	r3.size=get_vector3();
	return r3;
}
 Quat Deserializer::get_quat() {
	
	Quat q;
	q.x=get_double();
	q.y=get_double();
	q.z=get_double();
	q.w=get_double();
	return q;
}
 Color Deserializer::get_color() {
	
	Color c;
	c.r=get_uint8();
	c.g=get_uint8();
	c.b=get_uint8();
	c.a=get_uint8();
	return c;
}


Deserializer::Deserializer()
{
}


Deserializer::~Deserializer()
{
}


