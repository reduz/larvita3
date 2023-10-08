//
// C++ Implementation: volume_separator
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "volume_separator.h"
#include "polygon_clipper.h"


CollisionDebugger* CollisionDebugger::singleton=NULL;



/**********************/


bool VolumeSeparator::test_projection(const Vector3& p_axis,float& r_best_depth,Vector3& r_best_axis,float minA,float maxA, float minB, float maxB) const {


	// 1D minkowski difference
	
	minB -= ( maxA - minA ) * 0.5;
	maxB += ( maxA - minA ) * 0.5;
	
	float dmin = minB - ( minA + maxA ) * 0.5;
	float dmax = maxB - ( minA + maxA ) * 0.5;
	
	
	if (dmin > 0.0 || dmax < 0.0)
		return false; // doesn't contain 0

	//use the smallest depth
	
	dmin = ABS(dmin);
			
	if ( dmax < dmin )
	{
		if ( dmax < r_best_depth ) 
		{			
			r_best_depth=dmax;
			r_best_axis=p_axis;
		} 
	}
	else
	{
		if ( dmin < r_best_depth )			
		{
			r_best_depth=dmin;
			r_best_axis=-p_axis; // keep it as A axis
		}
	}
			
	return true;

}




VolumeSeparator *VolumeSeparatorManager::find_separator( const Volume& p_A, const Volume& p_B ) const {

	for (int i=0;i<volume_separator_count;i++) {
	
		if (volume_separators[i]->handles(p_A,p_B))
			return volume_separators[i];
		
	}
	
	return NULL;

}

void VolumeSeparatorManager::register_volume_separator(VolumeSeparator * p_separator) {

	ERR_FAIL_COND( volume_separator_count >=MAX_VOLUME_SEPARATORS );

	volume_separators[ volume_separator_count++ ] = p_separator;
	
}

void VolumeSeparatorManager::separate(const Volume& p_A,const Matrix4& p_A_transform, const Volume& p_B, const Matrix4 &p_B_transform, VolumeContactHandler *p_contact_handler) const {
	
	for (int i=0;i<volume_separator_count;i++) {
		
		if (!volume_separators[ i ]->handles(p_A,p_B))
			continue;
		
		volume_separators[ i ]->separate(p_A,p_A_transform,p_B,p_B_transform,p_contact_handler);
		return;
	}
	
	ERR_PRINT("No Separator Found");
	
}


VolumeSeparatorManager::VolumeSeparatorManager() {

	volume_separator_count=0;
}

