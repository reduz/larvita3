#ifdef SDL_ENABLED
//
// C++ Interface: paintersdl
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef PIGUIPAINTERSDL_H
#define PIGUIPAINTERSDL_H

#include "base/painter.h"
#include <SDL.h>

namespace GUI {

/**
	@author Juan Linietsky <reduzio@gmail.com>
*/
class PainterSDL : public Painter {
	
	enum {
		
		MAX_BITMAPS=4096
	};
	
	SDL_Surface *surface;
	Rect rect;
	
	SDL_Surface * bitmaps[MAX_BITMAPS];
	
	Rect clip_rect;
	bool clip_rect_active;


	void draw_custom_bitmap(BitmapID p_bitmap,const Point &p_pos, const Rect& p_src_rect, const Color&p_color,Direction p_dir );
	
	Size normal_mode;
	
	int rate;
	int delay;
public:
	
	virtual void set_clip_rect(bool p_enabled, const Rect& p_rect=Rect());
	virtual bool has_clip_rect();
	virtual Rect get_clip_rect();
	
	
	virtual void set_local_rect(const Rect& p_rect);
	virtual Rect get_local_rect();

	
	virtual void draw_rect(const Point & p_from,const Size & p_size,const Color& p_color);
	
	virtual void draw_fill_rect(const Point & p_from,const Size & p_size,const Color& p_color);
	
	virtual BitmapID load_bitmap(String p_file);
	virtual BitmapID create_bitmap(const Size& p_size,BitmapMode p_mode=MODE_PIXMAP,bool p_with_alpha=false);
	virtual Color get_bitmap_pixel(BitmapID p_bitmap,const Point& p_pos);
	virtual void set_bitmap_pixel(BitmapID p_bitmap,const Point& p_pos,const Color& p_color,unsigned char p_alpha=255);
	
	virtual Size get_bitmap_size(BitmapID p_bitmap);
	
	virtual bool is_bitmap_valid(BitmapID p_bitmap);
	
	
	virtual void draw_line(const Point& p_from, const Point& p_to, const Color& p_color,int p_width=1,unsigned char p_alpha=255);	
	virtual void draw_bitmap(BitmapID p_bitmap,const Point &p_pos,const Color&p_color=Color() );
	virtual void draw_bitmap(BitmapID p_bitmap,const Point &p_pos, const Rect& p_src_rect,const Color&p_color=Color());
	
	virtual void draw_bitmap(BitmapID p_bitmap,const Point &p_pos,Direction p_dir ,const Color&p_color=Color(255,255,255) );
	virtual void draw_bitmap(BitmapID p_bitmap,const Point &p_pos, const Rect& p_src_rect,Direction p_dir,const Color&p_color=Color(255,255,255) );
	
	virtual void draw_tiled_bitmap(BitmapID p_bitmap,const Rect& p_rect,const Point& p_origin,const Color&p_color=Color());
	
	virtual void remove_bitmap(BitmapID p_bitmap);
	/* SDL SPECIFIC */
	
	BitmapID register_bitmap(SDL_Surface *p_bitmap);
	
	
	/** screen update **/
	
	void update_screen();
	void update_screen_rect(const Rect& p_rect);
	Size get_display_size();
	
	void set_cursor_pos(const Point& p_pos,bool p_global=false);	
	
	void set_display_title(String p_title, String p_icon_title);	
	
	int get_fullscreen_mode_count();
	Size get_fullscreen_mode(int p_mode);
	void set_fullscreen(bool,int p_mode=-1);
	bool is_fullscreen();

	void set_key_repeat(int p_delay_ms,int p_rate_ms);
	int get_key_repeat_rate();
	int get_key_repeat_delay();
	
	void set_window_icon(BitmapID p_bitmap);
		
	virtual void set_cursor_visible(bool p_visible);		
	virtual void set_default_cursor_bitmap(BitmapID p_bitmap,const Point& p_hotspot);		
	virtual void set_cursor_bitmap(BitmapID p_bitmap,const Point& p_hotspot);		
	
	PainterSDL(SDL_Surface *p_surf);
	~PainterSDL();

};

}

#endif

#endif
