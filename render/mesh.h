//
// C++ Interface: mesh
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef MESH_H
#define MESH_H

#include "iapi.h"
#include "render/surface.h"
#include "face3.h"

/**
	@author Juan Linietsky <reduzio@gmail.com>
*/
class Mesh : public IAPI {
	
	IAPI_TYPE(Mesh,IAPI);

public:
	
	/* properties for a mesh

		surface_count:int amount of surfaces
		surfaces/1:IOBJ(surface) surface 1
		surfaces/2:IOBJ(surface) surface 2
		[ .. ]
		surfaces/N:IOBJ(surface) surface N (up to surface_count)
	*/


	virtual void set_surface_count(int p_count)=0;
	virtual int get_surface_count() const=0;
	virtual void add_surface(IRef<Surface> p_surface)=0;
	
	virtual void set_surface(int p_index,IRef<Surface> p_surface)=0;
	virtual IRef<Surface> get_surface(int p_index) const=0;
	
	virtual AABB get_AABB() const;

	DVector<Face3> get_faces() const;
	
	
	
	Mesh();
	~Mesh();
};

#endif
