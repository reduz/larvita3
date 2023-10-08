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
#ifndef CAMERA_FPS_NODE_H
#define CAMERA_FPS_NODE_H

#include "scene/nodes/node.h"
#include "scene/nodes/camera_node.h"
/**
	@author Juan Linietsky <reduzio@gmail.com>
*/
class CameraFPSNode : public Node {

	IAPI_TYPE( CameraFPSNode, Node );
	IAPI_INSTANCE_FUNC( CameraFPSNode );
	
	IRef<CameraNode> camera;
	
	Vector3 rel_vector;
	Vector3 up_vector;
	
	float distance;	
	float max_distance;
	float min_distance;
	
	float speed;
	
	float angle_x;
	float min_angle_x;
	float max_angle_x;

	float angle_y;
		
	void process_callback(Uint32 p_type, float p_time);

	void init();
	
public:

	virtual void process(float p_time);

	void set_speed(float p_speed);
	float get_speed();

	
	void set_max_distance(float max_distance);
	float get_max_distance() const;
	
	void set_min_distance(float min_distance);
	float get_min_distance() const;
		
	void rotate(float p_x, float p_y);
	void set_distance(float p_dist);
	float get_distance() const;

	void force_position(Vector3 p_pos);
	
	void set_lookat_adjust(Vector3 p_vec);
	
	IRef<CameraNode> get_camera();
	CameraFPSNode();
	~CameraFPSNode();

};

#endif
