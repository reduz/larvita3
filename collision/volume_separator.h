//
// C++ Interface: volume_separator
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef VOLUME_SEPARATOR_H
#define VOLUME_SEPARATOR_H

#include "collision/volume.h"
#include "signals/signals.h"
/**
	@author Juan Linietsky <reduzio@gmail.com>
*/


struct VolumeContact {


	Vector3 A,B; //point in A, point in B, both in world coordinates
	
	void invert() {
	
		SWAP(A,B);
	}
	
	VolumeContact inverted() const {
	
		VolumeContact v=*this;
		v.invert();
		return v;
	
	}

	inline Vector3 middle_point() const {
	
		return (A+B)/2.0;
	}

	VolumeContact(const Vector3& p_A,const Vector3& p_B) { A=p_A; B=p_B; }
	VolumeContact() {}

};

class ContactGenerator;

class VolumeContactHandler {
public:
	
	virtual void add_contact(const VolumeContact& p_contact,bool p_collided)=0;
	virtual ~VolumeContactHandler() {}
};


class CollisionDebugger {

public:
	static CollisionDebugger * singleton;
	virtual void plot_contact(const Vector3& p_contact)=0;
	virtual ~CollisionDebugger() {}
};

class VolumeSeparator : public SignalTarget {
friend class VolumeSeparatorManager;
	ContactGenerator *_cg;
protected:

	inline const ContactGenerator *get_contact_generator() const { return _cg; }
	inline bool projection_overlaps(float minA,float maxA, float minB, float maxB) const {
	
		minB -= ( maxA - minA ) * 0.5;
		maxB += ( maxA - minA ) * 0.5;
	
		float dmin = minB - ( minA + maxA ) * 0.5;
		float dmax = maxB - ( minA + maxA ) * 0.5;
	
	
		if (dmin > 0.0 || dmax < 0.0)
			return false; // doesn't contain 0
		else
			return true;		
	}
	bool test_projection(const Vector3& p_axis,float& r_best_depth,Vector3& r_best_axis,float minA,float maxA, float minB, float maxB) const;

public:

	virtual bool handles( const Volume& p_A, const Volume& p_B ) const=0;
	virtual void separate( const Volume& p_A,const Matrix4& p_A_transform, const Volume& p_B, const Matrix4 &p_B_transform, VolumeContactHandler *p_contact_handler) const=0;
	
	VolumeSeparator() { _cg=NULL; }
	virtual ~VolumeSeparator() {}
};

class VolumeSeparatorManager {

	enum {
		MAX_VOLUME_SEPARATORS=8
	};	
	

	VolumeSeparator *volume_separators[MAX_VOLUME_SEPARATORS];
	int volume_separator_count;

public:

	VolumeSeparator *find_separator( const Volume& p_A, const Volume& p_B ) const;
	void register_volume_separator(VolumeSeparator * p_separator);
	
	void separate(const Volume& p_A,const Matrix4& p_A_transform, const Volume& p_B, const Matrix4 &p_B_transform, VolumeContactHandler *p_contact_handler) const; /* Warning, this is slower*/
	
	VolumeSeparatorManager();
};




#endif
