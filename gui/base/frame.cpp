

#include "frame.h"
#include <stdio.h>

#include "base/container.h"
#include "base/window.h"
#include "base/skin.h"
namespace GUI {


struct _StyleOverride {

	int style;
	int with;
	_StyleOverride *next;
	
	int get(int p_style) {
	
		if (p_style==style)
			return with;
		else if (next)
			return next->get( p_style);
		else
			return p_style;
	}
	
	_StyleOverride(int p_style=-1,int p_with=-1,_StyleOverride *p_next=0) { style=p_style; with=p_with; next=p_next; }
	~_StyleOverride() { if (next) GUI_DELETE( next ); }
};

struct FramePrivate {
	
	bool can_fill_vertical;	
	bool can_fill_horizontal;	
	Container *parent;	
	bool has_update_rect;
	Window *window;
	FocusMode focus_mode;
	bool visible;
	bool clipping;
	bool bg_on_updates;
	Skin *skin;	
	Size size_cache;
	String tooltip;
	BitmapID cursor;
	Point cursor_hotspot;
	_StyleOverride *stylebox_overrides;
	_StyleOverride *font_overrides;
	_StyleOverride *bitmap_overrides;
	_StyleOverride *constant_overrides;
	_StyleOverride *color_overrides;
	
	FramePrivate() { can_fill_vertical=true; can_fill_horizontal=true; parent=0; window=0; focus_mode=FOCUS_NONE; visible=true; skin=0;  clipping=false; bg_on_updates=true; 
		stylebox_overrides=0; font_overrides=0; constant_overrides=0; color_overrides=0; bitmap_overrides=0; cursor=INVALID_BITMAP_ID; }
	
	~FramePrivate() {
	
		if (stylebox_overrides) GUI_DELETE( stylebox_overrides );
		if (font_overrides) GUI_DELETE( font_overrides );
		if (constant_overrides) GUI_DELETE( constant_overrides );
		if (color_overrides) GUI_DELETE( color_overrides );
		if (bitmap_overrides) GUI_DELETE( bitmap_overrides );
	}
};

void Frame::add_stylebox_override(int p_style, int p_with_style) {

	_fp->stylebox_overrides = GUI_NEW( _StyleOverride( p_style, p_with_style, _fp->stylebox_overrides ) );
}
void Frame::add_font_override(int p_style, int p_with_style) {

	_fp->font_overrides = GUI_NEW( _StyleOverride( p_style, p_with_style, _fp->font_overrides ) );

}
void Frame::add_bitmap_override(int p_style, int p_with_style) {

	_fp->bitmap_overrides = GUI_NEW( _StyleOverride( p_style, p_with_style, _fp->bitmap_overrides ) );

}
void Frame::add_constant_override(int p_style, int p_with_style) {

	_fp->constant_overrides = GUI_NEW( _StyleOverride( p_style, p_with_style, _fp->constant_overrides ) );


}
void Frame::add_color_override(int p_style, int p_with_style) {

	_fp->color_overrides = GUI_NEW( _StyleOverride( p_style, p_with_style, _fp->color_overrides ) );


}

void Frame::window_hid() {}


void Frame::set_tooltip(String p_text) {
	
	_fp->tooltip=p_text;
}

BitmapID Frame::get_cursor(const Point& p_point,Point *p_hotspot) {

	if (p_hotspot)
		*p_hotspot=_fp->cursor_hotspot;
		
	return _fp->cursor;
}

void Frame::set_cursor(BitmapID p_bitmap, const Point& p_hotspot) {

	_fp->cursor=p_bitmap;
	_fp->cursor_hotspot=p_hotspot;
}

String Frame::get_tooltip(const Point& p_point) {
	
	return _fp->tooltip;	
}


const StyleBox& Frame::stylebox(int p_which) {
	
	static StyleBox none;
	if (!_fp->skin)
		return none;

	if (_fp->stylebox_overrides)
		p_which=_fp->stylebox_overrides->get(p_which);

	return _fp->skin->get_stylebox( p_which );
	
}
FontID Frame::font(int p_which){
	
	if (!_fp->skin)
		return INVALID_FONT_ID;
	
	if (_fp->font_overrides)
		p_which=_fp->font_overrides->get(p_which);
	
	return _fp->skin->get_font( p_which );
	
}
BitmapID Frame::bitmap(int p_which){
	
	
	if (!_fp->skin)
		return INVALID_BITMAP_ID;
	
	if (_fp->bitmap_overrides) {
		p_which=_fp->bitmap_overrides->get(p_which);
	}
	
	return _fp->skin->get_bitmap( p_which );
	
}
int Frame::constant(int p_which){
	
	if (!_fp->skin)
		return 0;
	
	
	if (_fp->constant_overrides)
		p_which=_fp->constant_overrides->get(p_which);
	
	return _fp->skin->get_constant( p_which );
	
}
const Color& Frame::color(int p_which){
	
	static Color none;
	if (!_fp->skin)
		return none;
	
	if (_fp->color_overrides)
		p_which=_fp->color_overrides->get(p_which);
	
	return _fp->skin->get_color( p_which );
	
}

void Frame::set_clipping(bool p_clipping) {
	
	_fp->clipping=p_clipping;
}
bool Frame::is_clipping() {
	
	
	return _fp->clipping;
}

void Frame::set_in_window() {


}

void Frame::skin_changed() {
	
	
}


void Frame::set_bg_on_updates(bool p_draw) {
	
	_fp->bg_on_updates=p_draw;
}
bool Frame::has_bg_on_updates() {
	
	return _fp->bg_on_updates;
}

void Frame::update() {
	
	update(false);	
}

void Frame::update(bool p_only_rect,const Rect& p_rect) {

	if (!get_window()) {
		
		return;
	}
	
	Rect update_rect;
	
	if (p_only_rect && !p_rect.has_no_area()) {
		
		Rect r=p_rect;
		r.pos+=get_global_pos();
		update_rect=r;
	} else {
		
		
		update_rect=Rect( get_global_pos() , _fp->size_cache );
	}
	
	if (update_rect.has_no_area())
		return; //pointless
	
	/* check if this widget is REALLY visible */
	Frame *p = this;
	
	while (p) {
	
		if (!p->_fp->visible)
			return;
		p=p->_fp->parent;
	}
	
	_fp->window->update( update_rect );
}


FocusMode Frame::get_focus_mode() {
	
	return _fp->focus_mode;
}
void Frame::set_focus_mode(FocusMode p_focus_mode) {
	
	_fp->focus_mode=p_focus_mode;
}

void Frame::get_focus() {


	if (get_window() && _fp->focus_mode!=FOCUS_NONE) {

		
		get_window()->set_focus( this );
	}
	
}

bool Frame::has_focus() {
	
	if (!_fp->window)
		return false;
	return (_fp->window->get_focused_child()==this);
}

void Frame::set_fill_vertical(bool p_fill) {
	
	_fp->can_fill_vertical=p_fill;
}
void Frame::set_fill_horizontal(bool p_fill) {
	
	_fp->can_fill_horizontal=p_fill;
}	
bool Frame::can_fill_vertical() {
	return _fp->can_fill_vertical;
}

bool Frame::can_fill_horizontal() {
	
	return _fp->can_fill_horizontal;
	
}
	
void Frame::adjust_minimum_size() {

	if (!_fp->window)
		return;
	_fp->window->frame_request_resize(this);

}
	
void Frame::check_minimum_size() {
	
	if (!_fp->window)
		return;
	
	if (!_fp->parent) {
	
		_fp->window->frame_request_resize(this);
	} else {
	
		_fp->parent->check_minimum_size();	
	}			
}
	
void Frame::set_parent(Container *p_container) {
	
	_fp->parent=p_container;
}

Container *Frame::get_parent() {

	return _fp->parent;
}
	
Frame* Frame::get_child_at_pos(const Point& p_point,const Size &p_size,Point *p_frame_local_pos) {
		
	if (p_point.x < 0)
		return 0;
	if (p_point.y < 0)
		return 0;
	
	if (p_point.x >= p_size.width)
		return 0;
	if (p_point.y >= p_size.height)
		return 0;
	
	if (p_frame_local_pos)
		*p_frame_local_pos=p_point;
	return this;
}
void Frame::mouse_enter() {}
void Frame::mouse_leave() {}

void Frame::focus_enter() {
	
}
void Frame::focus_leave() {
	
}

void Frame::set_window(Window *p_window) {
	
	if (p_window && _fp->window) {
		//PRINT_ERROR("Setting frame in window more than once!"); -- probably normal, as long as check exists, all is ok
		return; 
	}
		
	_fp->window=p_window;
	if (p_window)
		_fp->skin=p_window->get_skin();
	set_in_window();
}


Window *Frame::get_window() {
	
	return _fp->window;
}

void Frame::show() {
	
	if (_fp->visible)
		return;
	
	_fp->visible=true;
	
	if (_fp->parent)
		_fp->parent->adjust_minimum_size();
	check_minimum_size();
	update();
	if (get_window())
		get_window()->set_tree_size_changed();
}
void Frame::hide() {
	
	if (!_fp->visible)
		return;

	_fp->visible=false;
	if (_fp->parent)
		_fp->parent->adjust_minimum_size();
	
	/* Set hide notify, to avoid focus to continue sending to a hdiden widget */
	if (get_window())
		get_window()->frame_hide_notify(this);
	
	check_minimum_size();
}
bool Frame::is_visible() {
	
	return _fp->visible;
}
	
Frame::Frame() {
	
	_fp = GUI_NEW( FramePrivate );
}

Frame *Frame::next_in_focus( Frame *p_from ) {

	
	if (get_focus_mode()==FOCUS_ALL)
		return this;

	return 0;
}
Frame *Frame::prev_in_focus( Frame *p_from ) {


	if (get_focus_mode()==FOCUS_ALL)
		return this;

	return 0;
	
}

Size Frame::get_assigned_size() {
	
	if (_fp->parent) {
		
		return _fp->parent->get_child_size( this );
	} 
	
	return Size();
	
}

Point Frame::get_global_pos() {

	if (_fp->parent) {


		return _fp->parent->get_child_pos( this )+_fp->parent->get_global_pos();
	} 

	return Point();

}

Size Frame::get_size_cache() {
	
	return _fp->size_cache;
}
void Frame::resize_tree(const Size& p_new_size) {
	
	_fp->size_cache=p_new_size;
	resize(p_new_size);
}

void Frame::resize(const Size& p_new_size) {
	
	//left empty
}

bool Frame::is_far_parent_of(Frame *p_frame) {
	
	while (p_frame) {
		
		if (p_frame==this)
			return true;
		p_frame=p_frame->get_parent();
	}
	
	return false;
}
String Frame::get_type() {

	
	return "Frame";
}

Frame::~Frame() {
	
	if (_fp->parent && _fp->parent->is_child(this))
		_fp->parent->remove_frame(this);
	
	deleted_signal.call();
	
	if (_fp->window)
		_fp->window->frame_deleted_notify( this );
	
	GUI_DELETE( _fp );
}

}; //end of namespace
