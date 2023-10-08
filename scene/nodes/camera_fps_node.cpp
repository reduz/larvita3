//
// C++ Implementation: camera_follow_node
//
// Description:
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "camera_fps_node.h"

#include "scene/nodes/scene_node.h"
#include "physics/physics.h"
#include "scene/main/scene_main_loop.h"

IRef<CameraNode> CameraFPSNode::get_camera() {

	return camera;
}

void CameraFPSNode::set_min_distance(float p_min_distance) {

	if (p_min_distance<=0)
		return;

	min_distance=p_min_distance;

	if (get_distance()<min_distance)
		set_distance(min_distance);
	
}

float CameraFPSNode::get_min_distance() const {

	return min_distance;
}

void CameraFPSNode::set_max_distance(float p_max_distance) {

	if (p_max_distance<=0)
		return;

	
	max_distance=p_max_distance;
	if (get_distance()>max_distance)
		set_distance(max_distance);

}

void CameraFPSNode::set_speed(float p_speed) {
	
	speed = p_speed;
};

float CameraFPSNode::get_speed() {
	
	return speed;
};

float CameraFPSNode::get_max_distance() const {

	return max_distance;
}

void CameraFPSNode::force_position(Vector3 p_pos) {
	
	Matrix4 mat = camera->get_global_matrix();
	mat.set_translation(p_pos);
	camera->set_global_matrix(mat);
};

void CameraFPSNode::process(float p_time) {

	angle_y = Math::fmod(angle_y,2*Math_PI);
	if (angle_y<0)
		angle_y+=2*Math_PI;

	if (angle_x < min_angle_x) angle_x = min_angle_x;
	if (angle_x > max_angle_x) angle_x = max_angle_x;

	Matrix4 thismat = Matrix4();
	thismat.rotate_y(angle_y);
	thismat.rotate_x(angle_x);

	thismat.set_translation( get_global_matrix().get_translation() );
		
	Matrix4 cammat = thismat;
	cammat.translate( 0,0, distance );
	
	Vector3 camera_pos = cammat.get_translation();
	Vector3 target_pos = thismat.get_translation()+rel_vector;

	Vector3 camera_inters=camera_pos;

	Vector3 prev_pos = camera->get_global_matrix().get_translation();
	Vector3 inters,normal;
	//if (SceneMainLoop::get_singleton()->get_physics()->intersect_segment(prev_pos,camera_pos,inters,normal,PhysicsBody::FLAG_STATIC)) {

	//	camera_pos = prev_pos;
	//};
	

	float min_distance = 1;	
	if (SceneMainLoop::get_singleton()->get_physics()->intersect_segment(target_pos,camera_pos,inters,normal,PhysicsBody::FLAG_STATIC)) {
		camera_inters=inters;

		if ((target_pos - camera_inters).length() > 0.1) {
			camera_inters = camera_inters + (target_pos - camera_inters).get_normalized() * 0.2;
		};
		
		if (target_pos.distance_to(camera_inters) < min_distance)  {
			
			camera_inters = target_pos + (camera_pos-target_pos).get_normalized() * min_distance;
		}
	};

	if ( /* speed != 0 && */ camera_inters != prev_pos) {
		
		Vector3 dif = camera_inters - prev_pos;
		if (dif.length() > Math::absf(speed * p_time)) {
			
			camera_inters = prev_pos + (dif.get_normalized() * speed * p_time);
			//camera_inters += dif.get_normalized() / (speed * time);
		};
	};

	Matrix4 mat;
	mat.set_lookat(camera_inters, target_pos, up_vector);
	camera->set_global_matrix(mat);
	

	/*
	Vector3 pos = get_global_matrix().get_translation();
	
	Vector3 camera_pos = pos + cam_vector;
	
	Matrix4 cam_mtx;
	
	cam_mtx.set_lookat( camera_pos, pos, up_vector);
	camera->set_global_matrix( cam_mtx );
	*/

	//printf("process, camera: %f,%f,%f - lookat %f,%f,%f\n",camera_pos.x,camera_pos.y,camera_pos.z,pos.x,pos.y,pos.z);
	//printf("camerafollow is at %f,%f,%f\n",get_global_matrix().get_translation().x,get_global_matrix().get_translation().y,get_global_matrix().get_translation().z);
	//printf("parent is at %f,%f,%f\n",get_parent()->get_global_matrix().get_translation().x,get_parent()->get_global_matrix().get_translation().y,get_parent()->get_global_matrix().get_translation().z);
	
	//
	//printf("camera is at %f,%f,%f\n",camera->get_global_matrix().get_translation().x,camera->get_global_matrix().get_translation().y,camera->get_global_matrix().get_translation().z);
	//printf("toplevel %i\n",has_toplevel_matrix());
}

void CameraFPSNode::rotate(float p_x, float p_y) {	
	
	angle_y += p_y;
	angle_x += p_x;
	


	/*
	Matrix4 rot;
	rot.rotate(up_vector,p_y);
	rot.rotate(right_vector, p_x);
	rot.transform(cam_vector);
	*/

}

void CameraFPSNode::set_distance(float p_dist) {
	
	
	if (p_dist<min_distance)
		p_dist=min_distance;
	
	if (p_dist>max_distance)
		p_dist=max_distance;

	distance = p_dist;	
}

float CameraFPSNode::get_distance() const {
	
	return distance;
}


void CameraFPSNode::set_lookat_adjust(Vector3 p_vec) {
	
	rel_vector = p_vec;
};

void CameraFPSNode::process_callback(Uint32 p_type, float p_time) {

	process(p_time);	
}


void CameraFPSNode::init() {
	
	if (!camera.is_null())
		return;
	
	camera = memnew(CameraNode);
	
	add_child(camera);
	camera->set_matrix_mode(Node::MATRIX_MODE_GLOBAL);

	//SceneMainLoop::get_singleton()->get_scene_tree()->add_child(camera);

};

CameraFPSNode::CameraFPSNode() {

	// to not fsck up lookat
	min_angle_x = -Math_PI/2 + Math_PI/8;
	max_angle_x = Math_PI/2 - Math_PI/8;
	
	angle_x = 0;
	angle_y = 0;
	
	max_distance=1.7;
	min_distance=0.5;
	set_distance(max_distance);

	rel_vector=Vector3(0,-1,0);
	up_vector=Vector3(0,1,0);
	
	speed = 0;
	
	process_signal.connect( this, &CameraFPSNode::process_callback );
	set_process_flags( MODE_FLAG_GAME | CALLBACK_FLAG_NORMAL );
}


CameraFPSNode::~CameraFPSNode() {

}


