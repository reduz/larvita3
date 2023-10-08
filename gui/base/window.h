//
// C++ Interface: window
//
// Description:
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef PIGUIINSTANCE_H
#define PIGUIINSTANCE_H

#include "base/painter.h"
#include "base/skin.h"
#include "base/timer.h"
#include "signals/signals.h"

namespace GUI {

class Frame;
class Container;
class Label;

/**
	@author Juan Linietsky <reduzio@gmail.com>
*/


class Window : public SignalTarget {

public:

	enum SizeMode {

		SIZE_NORMAL, ///<just set pos/size normally
		SIZE_CENTER, ///< Center of parent window (or desktop)
		SIZE_TOPLEVEL_CENTER ///< center of the toplevel screen, taking half of the space
	};

	enum Mode {

		MODE_NORMAL, ///< Normal window
		MODE_MODAL, ///< Modal Window (Dialog)
		MODE_POPUP, ///< Popup window (modal, but dissapears if you click somewhere else)

	};

private:

	struct ModalStack {

		ModalStack *next;
		Window *window;

		ModalStack() { next=0; window=0; }
	};

	struct UpdateRectList {

		Rect rect;
		UpdateRectList *next;

		UpdateRectList() { next=0; }
	};

	struct RootWindowData {


		Skin *skin;
		ModalStack *modal_stack;
		Painter *painter;
		Timer *timer;
		UpdateRectList *update_rect_list;
		bool update_rect_list_locked;
		
		Window *tooltip;
		Label * tooltip_label;
		TimerID tooltip_timer;
		Frame *tooltipped_frame;	
		int tooltip_cbk_count;
		Point last_mouse_pos;
		bool stop_event;
		
		RootWindowData() { skin=0; modal_stack=0; painter=0; timer=0; update_rect_list=0; update_rect_list_locked=false; tooltipped_frame=0; tooltip_cbk_count=0; stop_event=true; }

	};

	RootWindowData *root_data;


	Frame *root_frame;
	Frame *last_under_cursor;
	Frame *focus_child; //has keyboard focus
	struct Drag {
		Frame *child;
		int button_mask;
		Point pos;
	} drag;


	bool visible;
	Point pos;
	Size size;

	SizeMode size_mode;
	Mode mode;

	Window *parent; //in case we have a parent;

	//bool queued_delete;

	Frame *size_update_from;
	bool size_update_needed;

	Window *childs;
	Window *next;


	Window *focus; //focused window , only the tree root can use this
	Window *root; //pointer to tree root

	void tooltip_timer_cbk();

	Frame* find_frame_at_pos(Point p_pos,Point *local_pos);

	void check_tooltip_and_cursor_at_pos(const Point& p_pos,bool p_update_tooltip_timer=true);
	

	void update_rect_merge(UpdateRectList **p_rect);
	void add_update_rect(const Rect& p_rect);


	void remove_from_modal_stack();
	void raise_window(Window *p_child);
	Window *find_window_at_pos(const Point& p_point);
	void initialize();

	void redraw_screen_over_area(const Rect &p_rect);
	void redraw_contents_over_area(const Rect& p_rect);

	void top_frame_resized(Size p_size);

	void adjust_size_type();

	virtual void set_root_frame_internal(Container *p_root_frame) {}

	bool no_local_updates;
	bool no_stretch_root_frame;

	void check_size_updates();
public:

	Signal<> popup_cancel_signal;
	Signal< Method5<unsigned long,unsigned long,bool,bool,int> > key_unhandled_signal;
	Signal<> skin_changed_signal;
	Signal<> update_signal; //< called after check_for_updates and gets cleared after every call
	Signal<> frame_signal; //< called for every frame/event (when update is called)

	void set_no_local_updates(bool p_disable);
	void set_no_stretch_root_frames(bool p_disable);

	void update(const Rect& p_rect);
	void update();


	void set_tree_size_changed();
	Painter *get_painter();
	Timer *get_timer();

	void set_root_frame(Container *p_root);
	Frame * get_root_frame();
	Window *get_root();
	Window *get_parent();

	void set_pos(const Point &p_pos);
	Point get_pos();
	void set_size(const Size& p_size,bool p_force=false);
	Size get_size();

	Point get_global_pos();
	void redraw_all(Rect p_custom_area=Rect());
	void check_for_updates();


	void raise();
	void show();
	void hide();


	void mouse_button(const Point& p_pos, int p_button,bool p_press,int p_modifier_mask);
	void mouse_motion(const Point& p_pos, const Point& p_rel, int p_button_mask);
	void mouse_doubleclick(const Point& p_pos,int p_modifier_mask);
	void key(unsigned long p_unicode, unsigned long p_scan_code,bool p_press,bool p_repeat,int p_modifier_mask);

	void stop_event();
	
	bool is_visible();

	Frame * get_focused_child();
	void set_focus(Frame *p_frame);
	void clear_focus();

	void frame_request_resize(Frame *p_frame);
	void frame_deleted_notify(Frame *p_frame);

	Skin *get_skin();
	virtual void skin_changed();

	Mode get_mode();
	//void queue_delete(); //queue deletion until next "check for updates"

	//void check_for_deletes();

	void frame_hide_notify(Frame *p_frame);

	Window(Window *p_parent,Mode p_mode=MODE_NORMAL, SizeMode p_size_mode=SIZE_NORMAL);
	Window(Painter *p_painter,Timer *p_timer,Skin *p_skin);

	virtual ~Window();
};

}

#endif
