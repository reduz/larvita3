//
// C++ Interface: editor_viewport
//
// Description: 
//
//
// Author: ,,, <red@lunatea>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef EDITOR_VIEWPORT_H
#define EDITOR_VIEWPORT_H

#include "base/widget.h"
#include "scene/nodes/camera_node.h"

/**
	@author ,,, <red@lunatea>
*/
class EditorViewport : public GUI::Widget {		
public:
	
	enum PointerMode {
		MODE_SELECT,
		MODE_MOVE,
		MODE_ROTATE,
	};
		
private:	
	struct Position {
		
		Vector3 cursor;
		float cam_rot_y;
		float cam_rot_x;
		float cam_dist;		
		
	} pos;
	
	struct Action {
	
		bool panning;
	
	} action;
	
	IRef<CameraNode> camera;
	PointerMode pointer_mode;	
		
	void update_camera_matrix();	
		
	void mouse_button(const GUI::Point& p_pos, int p_button,bool p_press,int p_modifier_mask); 	
	void mouse_motion(const GUI::Point& p_pos, const GUI::Point& p_rel, int p_button_mask); ///< Overridable Method to notify when the mouse moved 	
	virtual void draw(const GUI::Point& p_global,const GUI::Size& p_size,const GUI::Rect& p_exposed);
public:

	Signal< Method1< List<IAPIRef>* > > selection_signal;

	void setup(IRef<CameraNode> p_camera);

	EditorViewport();
	~EditorViewport();

};

#endif
