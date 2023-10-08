//
// C++ Interface: camera_follow_node
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef CAMERA_FOLLOW_NODE_H
#define CAMERA_FOLLOW_NODE_H

#include "scene/nodes/node.h"
#include "scene/nodes/camera_node.h"
/**
	@author Juan Linietsky <reduzio@gmail.com>
*/
class CameraFollowNode : public Node {

	IAPI_TYPE( CameraFollowNode, Node );
	IAPI_INSTANCE_FUNC( CameraFollowNode );
	
	CameraNode *camera;
	
	Vector3 pos; // target position (global coords)
	
	float min_hdist; // minimum h-distance to player
	float max_hdist; // maximum h-distance to player
	
	float vadjust_speed;
	
	Vector3 up_vector;
	
	Vector3 lookat_adjust;
	
	void process_callback(Uint32 p_type, float p_time);	

public:

	virtual void process(float p_time);
	
	void set_max_distance(float max_distance);
	float get_max_distance() const;
	
	void set_min_distance(float min_distance);
	float get_min_distance() const;
		
	void set_vadjust_speed(float p_speed);
	float get_vadjust_speed() const;
	
	void set_up_vector(Vector3 p_up_vector);
	Vector3 get_up_vector() const;
	
	void set_lookat_adjust(Vector3 p_lookat_adjust);
	Vector3 get_lookat_adjust() const;
	
	void rotate(float p_phi);
	
	CameraNode *get_camera();
	
	CameraFollowNode();
	~CameraFollowNode();

};

#endif
