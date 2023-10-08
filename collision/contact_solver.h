//
// C++ Interface: contact_solver
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef CONTACT_SOLVER_H
#define CONTACT_SOLVER_H

#include "collision/volume_separator.h"
/**
	@author Juan Linietsky <reduzio@gmail.com>
*/
class ContactSolver { 
public:

	static void solve(const Vector3 * p_A_points,int p_A_count, const Vector3 * p_B_points,int p_B_count,VolumeContactHandler *p_contact_handler);

};

#endif
