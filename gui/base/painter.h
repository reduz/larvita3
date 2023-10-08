#ifndef PIGUI_PAINTER_H
#define PIGUI_PAINTER_H

#include "base/geometry.h"
#include "base/defs.h"
#include "base/style_box.h"
namespace GUI {



class PainterPrivate;



class Painter {

	
	
	PainterPrivate * p;
protected:
	virtual void set_local_rect(const Rect& p_rect)=0;
	virtual Rect get_local_rect()=0;

	virtual void set_clip_rect(bool p_enabled, const Rect& p_rect=Rect())=0;
	virtual bool has_clip_rect()=0;
	virtual Rect get_clip_rect()=0;
	
public:

	void push_clip_rect(const Rect &p_rect); /// Pushes a clip rect, relative to the local rect
	void pop_clip_rect(); /// pops a clip rect, reverts to previous one
	void reset_clip_rect_stack();  /// reset the clip rect stack

	void push_local_rect(const Rect &p_rect); /// push a local rect, relative to the previous one, all drawing will be performed relative tot his rect
	void pop_local_rect(); /// pop a local rect, revert to previous one
	void reset_local_rect_stack(); /// reset the local rect stack
	
	virtual void draw_rect(const Point & p_from,const Size & p_size,const Color& p_color)=0;
	
	virtual void draw_fill_rect(const Point & p_from,const Size & p_size,const Color& p_color)=0;
	virtual void draw_fill_rect(const Point & p_from,const Size & p_size,const Color& p_color,const Rect& p_clip);	

	virtual BitmapID load_bitmap(String p_file)=0;
	BitmapID load_xpm(const char **p_xpm);
	
	virtual BitmapID create_bitmap(const Size& p_size,BitmapMode p_mode=MODE_PIXMAP,bool p_with_alpha=false)=0;
	virtual Color get_bitmap_pixel(BitmapID p_bitmap,const Point& p_pos)=0;
	virtual void set_bitmap_pixel(BitmapID p_bitmap,const Point& p_pos,const Color& p_color,unsigned char p_alpha=255)=0;
	
	virtual bool is_bitmap_valid(BitmapID p_bitmap)=0;
	virtual void draw_bitmap(BitmapID p_bitmap,const Point &p_pos,const Color&p_color=Color(255,255,255) )=0;
	virtual void draw_bitmap(BitmapID p_bitmap,const Point &p_pos, const Rect& p_src_rect,const Color&p_color=Color(255,255,255) )=0;
	
	virtual void draw_line(const Point& p_from, const Point& p_to, const Color& p_color,int p_width=1,unsigned char p_alpha=255)=0;
	virtual void draw_circle( const Point& p_from, const Point& p_to, const Color& p_color );
	
	virtual void draw_bitmap(BitmapID p_bitmap,const Point &p_pos,Direction p_dir ,const Color&p_color=Color(255,255,255) )=0;
	virtual void draw_bitmap(BitmapID p_bitmap,const Point &p_pos, const Rect& p_src_rect,Direction p_dir,const Color&p_color=Color(255,255,255) )=0;
	
	virtual void draw_tiled_bitmap_region(BitmapID p_bitmap,const Rect& p_region,const Rect& p_rect,const Point& p_origin,const Color&p_color=Color(255,255,255));
		
	virtual void draw_tiled_bitmap(BitmapID p_bitmap,const Rect& p_rect,const Point& p_origin,const Color&p_color=Color(255,255,255))=0;
	virtual void draw_tiled_bitmap(BitmapID p_bitmap,const Rect& p_rect,const Color&p_color=Color(255,255,255));
	
	virtual Size get_bitmap_size(BitmapID p_bitmap)=0;
	
	virtual void remove_bitmap(BitmapID p_bitmap)=0;
	/** Font Management */
		
	/** The default Font Manager just uses bitmapfonts, taken from previously registered bitmaps,
	 *  if you want to use something else (freetype/SDL_TTf/System Native, etc) you can override the following
	 *  functions, but keep in mind that portability is up to you.
	 */
	
	virtual void draw_text(FontID p_font,const Point & p_pos,const String &p_string,const Color&p_color=Color(),int p_clip_w=-1);
	virtual void draw_char(FontID p_font,const Point & p_pos,String::CharType &p_char,const Color&p_color=Color());
	virtual void draw_text(FontID p_font,const Point & p_pos,const String &p_string,Direction p_dir,const Color&p_color=Color(),int p_clip_w=-1);
	
	virtual int get_font_char_width(FontID p_font,unsigned int p_char);
	virtual int get_font_string_width(FontID p_font,const String& p_string);
	virtual int get_font_height(FontID p_font);
	virtual int get_font_ascent(FontID p_font);
	virtual int get_font_descent(FontID p_font);
	

	virtual FontID create_font(int p_height,int p_ascent);
	virtual bool is_font_valid(FontID p_font);
	virtual void erase_font(FontID p_font);
	virtual void erase_font_and_bitmaps(FontID p_font);
	virtual void font_add_char( FontID,unsigned int p_char,BitmapID p_bitmap,const Rect& p_rect,unsigned int p_valign=0 );
	
	/** Style Box Painting **/		
	
	void draw_stylebox(const StyleBox& p_stylebox,const Point& p_pos, const Size& p_size);
	void draw_stylebox(const StyleBox& p_stylebox,const Point& p_pos, const Size& p_size,const Rect &p_clip);
	
	int get_stylebox_margin(const StyleBox& p_stylebox,const Margin& p_margin);
	Size get_stylebox_min_size(const StyleBox& p_stylebox,bool p_with_center=false);
	Point get_stylebox_offset(const StyleBox& p_stylebox);
	
	void draw_arrow( const Point& p_pos, const Size& p_size, Direction p_dir, const Color& p_color,bool p_trianglify=true);
	/** Updating **/
	
	virtual void update_screen()=0;
	virtual void update_screen_rect(const Rect& p_rect)=0;
	
	virtual Size get_display_size()=0;
	virtual void set_cursor_pos(const Point& p_pos,bool p_global=false)=0;
	Painter();
	
	
	virtual int get_fullscreen_mode_count()=0;
	virtual Size get_fullscreen_mode(int p_mode)=0;
	virtual void set_fullscreen(bool,int p_mode=-1)=0;
	virtual bool is_fullscreen()=0;
	
	virtual void set_key_repeat(int p_delay_ms,int p_rate_ms)=0;
	virtual int get_key_repeat_rate()=0;
	virtual int get_key_repeat_delay()=0;

	virtual void set_window_icon(BitmapID p_bitmap)=0;
	
	virtual void set_cursor_visible(bool p_visible)=0;
	virtual void set_default_cursor_bitmap(BitmapID p_bitmap,const Point& p_hotspot)=0; // use INVALID_BITMAP_ID to reset
	virtual void set_cursor_bitmap(BitmapID p_bitmap,const Point& p_hotspot)=0; // use INVALID_BITMAP_ID to reset
	
	
	virtual void set_display_title(String p_title, String p_icon_title)=0;
	virtual ~Painter();


};


}

#endif
