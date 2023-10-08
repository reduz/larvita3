//
// C++ Implementation: sphere_separator
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "sphere_separator.h"
#include "sphere_volume.h"
#include "contact_solver.h"

bool SphereSeparator::handles( const Volume& p_A, const Volume& p_B ) const {
	
	return ( p_A.get_volume_type()==Volume::SPHERE && p_B.get_volume_type()==Volume::SPHERE );

}

void SphereSeparator::separate( const Volume& p_A,const Matrix4& p_A_transform, const Volume& p_B, const Matrix4 &p_B_transform, VolumeContactHandler *p_contact_handler) const {


	ERR_FAIL_COND( p_A.get_volume_type()!=Volume::SPHERE || p_B.get_volume_type()!=Volume::SPHERE);
	
	//const SphereVolume *A=SAFE_CAST<const SphereVolume*>(&p_A);
	//const SphereVolume *B=SAFE_CAST<const SphereVolume*>(&p_B);
	
	
	Vector3 axis=(p_B_transform.get_translation()-p_A_transform.get_translation()).get_normalized();
	
	float minA,maxA,minB,maxB;
	
	p_A.project_range(axis,p_A_transform,minA,maxA);
	p_B.project_range(axis,p_B_transform,minB,maxB);
	
	if (!projection_overlaps(minA,maxA,minB,maxB))
		return; // no collision
		
	
	Vector3 As;
	int c;
	p_A.get_support(axis,p_A_transform,&As,&c,1);
	ERR_FAIL_COND(c!=1);
	
	Vector3 Bs;
	p_B.get_support(-axis,p_B_transform,&Bs,&c,1);
	ERR_FAIL_COND(c!=1);
	
	ContactSolver::solve(&As,1,&Bs,1,p_contact_handler);


}



