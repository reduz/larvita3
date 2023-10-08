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
#include "camera_follow_node.h"

#include "scene/main/scene_main_loop.h"
//#include "scene/nodes/scene_node.h"
#include "physics/physics.h"

CameraNode *CameraFollowNode::get_camera() {

	return camera;
}

void CameraFollowNode::set_max_distance(float max_distance) {
	
	max_hdist=max_distance;	
}

float CameraFollowNode::get_max_distance() const {
	
	return max_hdist;	
}


void CameraFollowNode::set_min_distance(float min_distance) {
	
	min_hdist=min_distance;	
}

float CameraFollowNode::get_min_distance() const {
	
	return min_hdist;
}

	
void CameraFollowNode::set_vadjust_speed(float p_speed) {
	
	vadjust_speed=p_speed;	
}

float CameraFollowNode::get_vadjust_speed() const {
	
	return vadjust_speed;
}


void CameraFollowNode::set_up_vector(Vector3 p_up_vector) {
	
	up_vector=p_up_vector;	
}

Vector3 CameraFollowNode::get_up_vector() const {
	
	return up_vector;	
}


void CameraFollowNode::set_lookat_adjust(Vector3 p_lookat_adjust) {
	
	lookat_adjust=p_lookat_adjust;	
}

Vector3 CameraFollowNode::get_lookat_adjust() const {
	
	return lookat_adjust;	
}


void CameraFollowNode::rotate(float p_phi) {
	
	Matrix4 rot_mtx;
	rot_mtx.load_rotate(up_vector,p_phi);
	Vector3 from=get_global_matrix().get_translation();
	
	Vector3 rel_pos=pos-from; // from -> pos
	rot_mtx.transform(rel_pos);
	
	pos=from+rel_pos;			
}



void CameraFollowNode::process(float p_time) {

	Vector3 from=get_global_matrix().get_translation();
	
	Vector3 rel_pos=pos-from; // from -> pos
	
	float rel_vpos_len=up_vector.dot(rel_pos);
	Vector3 rel_vpos = up_vector * rel_vpos_len;
	Vector3 rel_hpos=rel_pos - rel_vpos; // hvector
	
	
	/* adjust vpos */	
	float rel_hpos_len = rel_hpos.length();
	if ( rel_hpos_len > max_hdist )
		rel_hpos=rel_hpos.get_normalized() * max_hdist;
	else if ( rel_hpos_len < min_hdist )
		rel_hpos=rel_hpos.get_normalized() * min_hdist;
	
	/* adjust hpos */	
	
	rel_vpos_len-=rel_vpos_len*vadjust_speed*p_time;	
	rel_vpos=up_vector*rel_vpos_len;		
	
	pos = from+rel_vpos+rel_hpos;
	
	Vector3 target_pos=from+lookat_adjust;
	Vector3 camera_pos=pos;	
	
	Vector3 inters,normal;
	if (SceneMainLoop::get_singleton()-> get_physics()->intersect_segment(target_pos,camera_pos,inters,normal,PhysicsBody::FLAG_STATIC)) {
		camera_pos=inters;
	}
	
	Matrix4 cam_mtx;
	cam_mtx.set_lookat( camera_pos, target_pos, up_vector);
	camera->set_global_matrix( cam_mtx );

}

void CameraFollowNode::process_callback(Uint32 p_type, float p_time) {

	process(p_time);
	
}

CameraFollowNode::CameraFollowNode() {

	camera = memnew(CameraNode);
	camera->set_name("follower"); // lakitu bros. camera :)
	add_child(camera);
	
	pos=Vector3(0,0,0.01); // will be adjusted
	
	min_hdist=4.0;
	max_hdist=7.0; // maximum h-distance to player
	
	vadjust_speed=1.0;
	
	up_vector=Vector3(0.0,1.0,0.0);
	
	lookat_adjust=Vector3(0,0,0);
	
	process_signal.connect( this, &CameraFollowNode::process_callback );
	set_process_flags( MODE_FLAG_GAME | CALLBACK_FLAG_NORMAL );
}


CameraFollowNode::~CameraFollowNode() {
	memdelete(camera);
}


