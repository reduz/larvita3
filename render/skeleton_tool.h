//
// C++ Interface: skeleton_tool
//
// Description: 
//
//
// Author: Tomas Neme <lacrymology@gmail.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef SKELETON_TOOL_H
#define SKELETON_TOOL_H

#if 0
/**
	@author Tomas Neme <lacrymology@gmail.com>, (C) 2008
*/

#include "render/surface_tool.h"

class SkeletonTool {

public:
	struct Face {
		SurfaceTool::Vertex vertices[3];
		Vector<int> affected_bones;
	} ;

private:
	//surfaces
	const SurfaceTool* surface_tool;

	Vector<Face> faces;

	Vector<int> skeleton;

	void eliminate_extra_bones() ; 	/// Eliminates the smallest weights in any
									/// surfaces with over 8 bones
	// I don't think these can be used
	void eliminate_bone(int p_bone) ;
	void eliminate_zero_bones() ; /// Effectively eliminates from the arrays
									/// all bones that have no influence over
									/// any vertices
	Vector<int> get_faces_from_bone(int p_bone,int* p_faces) ; /// this will change
						/// p_faces so all the faces that are affected by p_bone
						/// point to it
	int count_bone_vertices(int p_bone) ;
	bool check_retval(const DVector<SurfaceTool>& retval);

public:


	void set_skeleton( Vector<int> p_skeleton );
	void add_weight( int p_vertex_index, int p_weight_index, int p_bone_index, Uint8 p_weight);

	SkeletonTool(const SurfaceTool* p_st);

	DVector<SurfaceTool> commit();
};

#endif
#endif