//
// C++ Implementation: plane_separator
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "plane_separator.h"
#include "plane_volume.h"

bool PlaneSeparator::handles( const Volume& p_A, const Volume& p_B ) const {
	
	return (( p_A.get_volume_type()==Volume::PLANE || p_B.get_volume_type()==Volume::PLANE ) && p_A.get_volume_type()!=p_B.get_volume_type());

}

void PlaneSeparator::separate( const Volume& p_A,const Matrix4& p_A_transform, const Volume& p_B, const Matrix4 &p_B_transform, VolumeContactHandler *p_contact_handler) const {



	ERR_FAIL_COND(!(( p_A.get_volume_type()==Volume::PLANE || p_B.get_volume_type()==Volume::PLANE ) && p_A.get_volume_type()!=p_B.get_volume_type()));	
	
	const PlaneVolume *A;
	const Volume *B;
	const Matrix4* B_transform;
	const Matrix4* A_transform;
	bool swap=false;
	
	if (p_A.get_volume_type()==Volume::PLANE) {
		A=SAFE_CAST<const PlaneVolume*>(&p_A);
		B=&p_B;
		A_transform=&p_A_transform;
		B_transform=&p_B_transform;
	} else {
	
		A=SAFE_CAST<const PlaneVolume*>(&p_B);
		B=&p_A;
		A_transform=&p_B_transform;
		B_transform=&p_A_transform;
		swap=true;
	
	}

	Plane p = A->get_plane();
	
	A_transform->transform(p);
	
	
	Vector3 supports[Volume::MAX_SUPPORTS];
	int support_count;
	
	B->get_support(-p.normal,*B_transform,supports,&support_count,Volume::MAX_SUPPORTS);
	
	
	//printf("plane %f,%f,%f - %f\n",p.normal.x,p.normal.y,p.normal.z,p.d);
	//printf("support points %i\n",S.get_point_count());
	for (int i=0;i<support_count;i++) {
	
		float d = p.distance_to_point(supports[i]);
		if (d>0)
			continue; //useless
						
		VolumeContact vc;
		vc.A=supports[i]-p.normal*d;
		vc.B=supports[i];
	
		if (swap)
			vc.invert();
			
		p_contact_handler->add_contact(vc,true);
	}
	

}



