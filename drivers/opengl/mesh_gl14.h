#ifdef OPENGL_ENABLED
//
// C++ Interface: mesh_gl
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef MESH_GL14_H
#define MESH_GL14_H

#include "render/mesh.h"
#include "drivers/opengl/surface_gl14.h"
#include "vector.h"

/**
	@author Juan Linietsky <reduzio@gmail.com>
*/
class Mesh_GL14 : public Mesh {
	
	IAPI_TYPE(Mesh_GL14,Mesh);
	IAPI_INSTANCE_FUNC(Mesh_GL14);
	IAPI_PERSIST_AS(Mesh);
	
friend class Renderer_GL14;	
	Vector< IRef<Surface_GL14> > surfaces;

	bool skip_presave;
public:
		
		
	virtual void set(String p_path,const Variant& p_value);
	virtual Variant get(String p_path) const;
	virtual void get_property_list( List<PropertyInfo> *p_list ) const;
	virtual PropertyHint get_property_hint(String p_path) const;
	virtual Variant call(String p_method, const List<Variant>& p_params=List<Variant>());
		
	void set_surface_count(int p_count);
	int get_surface_count() const;
	void add_surface(IRef<Surface> p_surface);
	
	void set_surface(int p_index,IRef<Surface> p_surface);
	IRef<Surface> get_surface(int p_index) const;
	
	
	Mesh_GL14();
	~Mesh_GL14();

};

#endif
#endif // OPENGL_ENABLED
