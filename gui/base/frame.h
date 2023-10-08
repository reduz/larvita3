
#ifndef PIGUI_FRAME_H
#define PIGUI_FRAME_H

#include "base/geometry.h"
#include "base/defs.h"
#include "signals/signals.h"

namespace GUI {

class Window;
struct FramePrivate;
class Container;
struct StyleBox;
class Skin;

class Frame : public SignalTarget {


	FramePrivate *_fp;
protected:	
	
	/* skin subsystem */
	
	virtual const StyleBox& stylebox(int p_which); ///< Get a certain stylebox, from the enum. Override to create specific custom looking widgets.
	virtual FontID font(int p_which); ///< Get a certain font, from the enum. Override to create specific custom looking widgets.
	virtual BitmapID bitmap(int p_which);///< Get a certain bitmap, from the enum. Override to create specific custom looking widgets.
	virtual int constant(int p_which); ///< Get a certain constant, from the enum. Override to create specific custom looking widgets.
	virtual const Color& color(int p_which); ///< Get a certain color, from the enum. Override to create specific custom looking widgets.
	

	virtual void set_in_window();
	
public:
	
	virtual void check_minimum_size();
		
	void add_stylebox_override(int p_style, int p_with_style);
	void add_font_override(int p_style, int p_with_style);
	void add_bitmap_override(int p_style, int p_with_style);
	void add_constant_override(int p_style, int p_with_style);
	void add_color_override(int p_style, int p_with_style);
	
	virtual BitmapID get_cursor(const Point& p_point=Point(),Point *p_hotspot=NULL);
	virtual void set_cursor(BitmapID p_bitmap, const Point& p_hotspot);
	
	void set_tooltip(String p_text);
	virtual String get_tooltip(const Point& p_point=Point());
	
	virtual void adjust_minimum_size();
	
	virtual	void update();
	void update(bool p_only_rect,const Rect& p_rect=Rect()); ///< Call update. This means the frame will be redraw when IDLE (not when update called!). Alternative you can ask to update only a region.
	
	void set_fill_vertical(bool p_fill=true); ///< This sets wether the frame will fill up it's assigned horizontal space, or will just remain as small as possible inside a bigger space.
	void set_fill_horizontal(bool p_fill=true); ///< This sets wether the frame will fill up it's assigned vertical space, or will just remain as small as possible inside a bigger space.
	bool can_fill_vertical(); ///< Return wether the frame will fill it's assigned vertical stretchable space.
	bool can_fill_horizontal(); ///< Return wether the frame will fill it's assigned horizontal stretchable space.
	
	FocusMode get_focus_mode(); ///< Get the way this frame can gain focus.
	void set_focus_mode(FocusMode p_focus_mode); ///< Alter the way in which this frame will gain focus. check FocusMode enum in defs.h . By default a frame can't aquire focus.
	bool has_focus(); ///< Return wether this frame is focused
	void get_focus(); ///< Force the frame to aquire focus.
	
	void set_clipping(bool p_clipping); /// Clipping is an optimization of wether children frames can draw outside this frame's area. Default is DISABLED, since it's very difficult for this to happen by itself, but clipping can be enabled in the case of, for example, scrollable containers.
	
	bool is_clipping(); ///< Return wether clipping is enabled (see set_clipping)
	
	
	virtual void set_minimum_size(const Size & p_size )=0; ///force a CUSTOM mimimum size for a frame. Almost all the time, frames already compute their minimum size by themselves, but this is just to override it.
	
	virtual Size get_minimum_size()=0; ///< return the minimum size for a frame.
	virtual void resize(const Size& p_new_size); ///< This is just an event to mention that the frame was resized. The size of the frame will not change with this function. Remember, that a frame will not store position/size inside, so this is just a "notification" (size is stored at the parent, in the container.
	Size get_size_cache();
		
	virtual Frame* get_child_at_pos(const Point& p_point,const Size &p_size,Point *p_frame_local_pos=0); ///< Get a Child at a specific position, closest to the pointer

	virtual void draw_tree(const Point& p_global,const Size& p_size,const Rect& p_exposed)=0; ///< Draw frame and child subrames, at a global pos and size
	virtual void resize_tree(const Size& p_new_size); ///< resize tree frame and child subframes -- call back this on override
	
	
	/* Events */
	
	virtual void draw(const Point& p_global,const Size& p_size,const Rect& p_exposed)=0; ///< Overridable Method to draw the frame. p_global is the global position of the frame, while p_size is the function it should be drawn (if you issue a get_painter(), the painter will have local coords for this frame). Rect is the region that needs to be redrawn.
	virtual void mouse_button(const Point& p_pos, int p_button,bool p_press,int p_modifier_mask)=0; ///< Overridable Method to notify the frame when it was clicked. p_pos is the click position in local coordinates, while p_button is one of the values from the MouseButton enum at defs.h. Modifier mask contains OR-ed bits from Keyboard modifiers (ctrl/alt/shift/etc)
	virtual void mouse_doubleclick(const Point& p_pos,int p_modifier_mask)=0; ///< Overridable Method to notify when the frame was double clicked. p_pos is the click position in local coords, Modifier mask contains OR-ed bits from Keyboard modifiers (ctrl/alt/shift/etc)
	virtual void mouse_motion(const Point& p_pos, const Point& p_rel, int p_button_mask)=0; ///< Overridable Method to notify when the mouse moved over a frame. p_pos is in local coordinates, p_rel is the relative motion and p_button mask contains bits of MouseButton ORed together, minus one. (ie, (1<<BUTTON_LEFT) -1). MouseButtonMask enum provides this. WARNING, if the user drags a frame (click on it, hols the left mouse button and move the mouse pointer away from it) it will keep receiving events from the mouse, until mouse button has been release.
	virtual bool key(unsigned long p_unicode, unsigned long p_scan_code,bool p_press,bool p_repeat,int p_modifier_mask)=0; ///< Overridable Method to notify when a key was pressed. p_unicode is an unicode char, p_scan_code is a value from keyboard.h , p_repeat means it is an echo key, and p_modifier mask holds the value of the keyboard modifiers. p_modifier_mask and p_scan_code can be OR'ed together to form a unique keybind. returning true asks for the key to be eaten (not propagated)
	virtual void mouse_enter(); ///< Overridable Method to notify when mouse enters the frame area.
	virtual void mouse_leave(); ///< Overridable Method to notify when mouse leaves the frame area.
	virtual void focus_enter();  ///< Overridable Method to notify when the frame gains focus (set_focus_mode must be enabled)
	virtual void focus_leave(); ///< Overridable Method to notify when the frame loses focus (set_focus_mode must be enabled)
	virtual void window_hid(); ///< Overridable Method to notify the focused widget if the window where it is located hid

	virtual void skin_changed(); ///< Overridable Method to notify the frame that the skin changed, and that constants/data from it should be reloaded.
	
	virtual Point get_global_pos(); ///< Convenience method to obtain the global pos of a frame. Dont abuse it, it's slow.
	Size get_assigned_size(); ///< Convenience method to find out the size of the widget assigned by the parent.
	
	
	virtual Frame *next_in_focus( Frame *p_from=0 ); ///< This method is used by containers to obtain the next focusable frame child (tab)
	virtual Frame *prev_in_focus( Frame *p_from=0 ); ///< This method is used by containers to obtain the previous focusable frame child (shift-tab)
	
	virtual void set_window(Window *p_window); ///< Set the window on which this frame resides
	Window *get_window(); ///< Get the window on which this frame resides
	virtual void set_parent(Container *p_container); ///< Set the parent of this frame. Parents are always containers!

	virtual Container *get_parent(); ///< Get the parent of this frame. Parents are always containers!

	virtual String get_type(); ///< Get the type of the frame. This is only used for convenience or debugging for the user, because the GUI library wont use it.
	
	void show(); ///< Show this frame, if already visible has no effect
	void hide(); ///< Hide this frame, if already hidden has no effect
	bool is_visible(); ///< Return wether the frame is visible. As a WARNING, even if the frame has visibility flag, an upper parent may be invisible, thus hidding the rest of the tree
	
	void set_bg_on_updates(bool p_draw); ///< If this flag is on, the background will be redrawn automatically before draw() is called on this frame. The default behavior is p_draw=true
	bool has_bg_on_updates(); ///< Check wether this widget will draw the background when updated (check set_bg_on_updates )
	
	bool is_far_parent_of(Frame *p_frame); ///< Check wether a certain frame is parent of this, somewhere above in the hierarchy tree.
	
	virtual Frame* find_closest_focusable_to(Point p_point,Direction p_dir,int &mindist)=0; ///< Find the closes focusable frame in a certain direction. This is used for arrow key focusing.
//	virtual void set_parent( Frame *p_parent )=0;
	
	/* Signals */	
	
	Signal<> deleted_signal; ///< This signal is called upon destruction (from the constructor). Be VERY, VERY careful when using this, because if you try to access the inherited classes of this object, they will be INVALID at the time the signal is called, but their memory wont be freed yet!!
	
	Frame();
	virtual ~Frame();
};

} //end of namespace


#endif
