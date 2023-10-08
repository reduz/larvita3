//
// C++ Interface: skeleton
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef SKELETON_H
#define SKELETON_H

#include "iapi.h"
/**
	@author Juan Linietsky <reduzio@gmail.com>
*/
class Skeleton : public IAPI {

	IAPI_TYPE(Skeleton,IAPI);

public:

	/* IAPI Description
		-creation params:
			bone_couunt:int amount of bones
		-iapi
			bone[0]:Matrix4 
			bone[1]:Matrix4 
			..
			bone[N]:Matrix4 (N == bone_count) bone matrices
	*/
	
	virtual void set_bone_matrix(int p_bone_idx,const Matrix4& p_matrix)=0;
	virtual Matrix4 get_bone_matrix(int p_bone_idx) const=0;
	virtual int get_bone_count() const=0;

	virtual void create(int p_bone_count)=0;
	Skeleton();
	~Skeleton();

};

#endif
