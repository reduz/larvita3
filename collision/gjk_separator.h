//
// C++ Interface: gjk_separator
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef GJK_SEPARATOR_H
#define GJK_SEPARATOR_H

#include "collision/volume_separator.h"
#include "list.h"

/**
	@author Juan Linietsky <reduzio@gmail.com>
*/

struct GJK_Debug {
	
	struct Step {
			
		int order;
		Vector3 simplex[5];
		Vector3 ray;
		Vector3 support;
	};
	
	List<Step> steps;
};

struct MPR_Debug {
		
	Vector3 ray;

};


class GJK_Separator : public VolumeSeparator {

public:	
	GJK_Debug *debug;
	MPR_Debug *mpr_debug;
	
	virtual bool handles( const Volume& p_A, const Volume& p_B ) const;
	virtual void separate( const Volume& p_A,const Matrix4& p_A_transform, const Volume& p_B, const Matrix4 &p_B_transform, VolumeContactHandler *p_contact_handler) const;
	
	GJK_Separator() { debug=NULL; mpr_debug=NULL; }
};

#endif
