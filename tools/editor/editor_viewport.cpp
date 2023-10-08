//
// C++ Implementation: editor_viewport
//
// Description: 
//
//
// Author: ,,, <red@lunatea>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "editor_viewport.h"
#include "editor_skin.h"
#include "os/keyboard.h"

void EditorViewport::draw(const GUI::Point& p_global,const GUI::Size& p_size,const GUI::Rect& p_exposed) {
	
	if (has_focus()) {
		
		get_painter()->draw_stylebox( stylebox( SB_EDITOR_VIEWPORT_FOCUS ), GUI::Point(), p_size );
	}
}


void EditorViewport::update_camera_matrix() {

	Matrix4 mat;
	mat.translate(pos.cursor);
	mat.rotate_y( pos.cam_rot_y );
	mat.rotate_x( pos.cam_rot_x );
	mat.translate( 0, 0, pos.cam_dist );
	
	camera->set_global_matrix( mat );

}

void EditorViewport::mouse_button(const GUI::Point& p_pos, int p_button,bool p_press,int p_modifier_mask) {
	 	
	if (p_press) {
	 	
		if (p_button==GUI::BUTTON_WHEEL_UP) {
		
			pos.cam_dist*=0.92;
			update_camera_matrix();
		}
		
		if (p_button==GUI::BUTTON_WHEEL_DOWN) {
		
			pos.cam_dist/=0.92;
			update_camera_matrix();
		}
	
		if ( p_button==GUI::BUTTON_MIDDLE) {
		
			action.panning=p_modifier_mask&KEY_MASK_SHIFT;
		} else {
		
			action.panning=false;
		
		}
		
		if (p_button==GUI::BUTTON_LEFT) {
		
			switch(pointer_mode) {
			
				case MODE_SELECT: {
				
				} break;
				case MODE_MOVE: {
				
				} break;
				case MODE_ROTATE: {
				
				} break;
			}
		
		}
		
	} else {
		
		if ( p_button==GUI::BUTTON_MIDDLE) {
		
			action.panning=false;
		}
		
	}
	
}


void EditorViewport::mouse_motion(const GUI::Point& p_pos, const GUI::Point& p_rel, int p_button_mask) {
	
			
	if (p_button_mask&GUI::BUTTON_MASK_MIDDLE) {
	
		if (!has_focus())
			get_focus();
	
		if (action.panning) {
			Matrix4 mat;
			mat.translate(pos.cursor);
			mat.rotate_y( pos.cam_rot_y );
			mat.rotate_x( pos.cam_rot_x );
			mat.translate( Vector3( -p_rel.x/64.0 * pos.cam_dist , p_rel.y/64.0 * pos.cam_dist, 0 ) );
			pos.cursor=mat.get_translation();	
		} else {
		
			pos.cam_rot_y+=-p_rel.x/64.0;
			pos.cam_rot_x+=p_rel.y/64.0;
		}
		
		update_camera_matrix();
	}
	
	
	
}

void EditorViewport::setup(IRef<CameraNode> p_camera) {
	
	camera=p_camera;
	Matrix4 pos;
	pos.translate( 0,0, 20 );
	camera->set_global_matrix( pos );
	printf("setting up camera node: %p - %s\n",camera.ptr(),Variant(camera->get_global_matrix( ).get_translation()).get_string(). ascii().get_data());
}

EditorViewport::EditorViewport() {
	
	
	set_focus_mode(GUI::FOCUS_CLICK);
	pos.cam_rot_y=0;
	pos.cam_rot_x=0;
	pos.cam_dist=1;
	action.panning=false;
	pointer_mode=MODE_SELECT;
}


EditorViewport::~EditorViewport()
{
}


