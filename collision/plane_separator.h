//
// C++ Interface: plane_separator
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef PLANE_SEPARATOR_H
#define PLANE_SEPARATOR_H


#include "collision/volume_separator.h"
/**
	@author Juan Linietsky <reduzio@gmail.com>
*/
class PlaneSeparator : public VolumeSeparator {
public:

	virtual bool handles( const Volume& p_A, const Volume& p_B ) const;
	virtual void separate( const Volume& p_A,const Matrix4& p_A_transform, const Volume& p_B, const Matrix4 &p_B_transform, VolumeContactHandler *p_contact_handler) const;
};


#endif
