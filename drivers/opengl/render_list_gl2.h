#ifdef OPENGL2_ENABLED
//
// C++ Interface: render_list_gl
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef RENDER_LIST_GL2_H
#define RENDER_LIST_GL2_H

/**
	@author Juan Linietsky <reduzio@gmail.com>
*/
#include "render/templates/render_list.h"
#include "drivers/opengl/material_gl.h"
#include "drivers/opengl/texture_gl.h"
#include "drivers/opengl/surface_gl2.h"
#include "drivers/opengl/mesh_gl2.h"
#include "drivers/opengl/skeleton_gl.h"
#include "drivers/opengl/light_gl.h"
		
typedef RenderList<Light_GL,Texture_GL,Surface_GL2,Skeleton_GL,Material_GL,8> RenderList_GL2;

#endif
#endif // OPENGL2_ENABLED
