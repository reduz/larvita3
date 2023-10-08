//
// C++ Interface: painter_renderer
//
// Description:
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef PAINTER_RENDERER_H
#define PAINTER_RENDERER_H

#include "gui/base/painter.h"
#include "render/renderer.h"


/**
	@author Juan Linietsky <red@lunatea>
*/
class PainterRenderer : public GUI::Painter {

public:
	enum {
		MAX_BITMAPS=4096
	};

private:

	GUI::Rect rect;
	GUI::Rect clip;
	bool clip_enabled;
	GUI::Size screen_size;
	GUI::BitmapID cursor;
	GUI::Point cursor_hotspot;
	GUI::BitmapID default_cursor;
	GUI::Point default_cursor_hotspot;
	bool cursor_visible;


	struct Bitmap {

		IRef<Texture> texture;
		int width;
		int height;
		String file;
		Bitmap() {
			width = -1;
			height = -1;
		};

		Bitmap(IRef<Texture> p_tex, int p_width, int p_height) {

			texture = p_tex;
			width = p_width;
			height = p_height;
		};
	};
	Bitmap bitmaps[MAX_BITMAPS]; //rid for textures

public:

	virtual void set_local_rect(const GUI::Rect& p_rect);
	virtual GUI::Rect get_local_rect();

	virtual void set_clip_rect(bool p_enabled, const GUI::Rect& p_rect=GUI::Rect());
	virtual bool has_clip_rect();
	virtual GUI::Rect get_clip_rect();

	virtual void draw_rect(const GUI::Point & p_from,const GUI::Size & p_size,const GUI::Color& p_color);

    virtual void draw_line(const GUI::Point& p_from, const GUI::Point& p_to, const GUI::Color& p_color,int p_width=1,unsigned char p_alpha=255){};

	virtual void draw_fill_rect(const GUI::Point & p_from,const GUI::Size & p_size,const GUI::Color& p_color);

	virtual GUI::BitmapID load_bitmap(String p_file);
	virtual GUI::BitmapID create_bitmap(const GUI::Size& p_size,GUI::BitmapMode p_mode=GUI::MODE_PIXMAP,bool p_with_alpha=false);
	virtual GUI::BitmapID create_bitmap_from_texture(IRef<Texture> p_texture, int width = -1, int height = -1,String p_file="");
	virtual GUI::BitmapID create_bitmap_from_image(const Image& p_image,String p_file="");

	virtual GUI::Color get_bitmap_pixel(GUI::BitmapID p_bitmap,const GUI::Point& p_pos);
	virtual void set_bitmap_pixel(GUI::BitmapID p_bitmap,const GUI::Point& p_pos,const GUI::Color& p_color,unsigned char p_alpha=255);


	virtual bool is_bitmap_valid(GUI::BitmapID p_bitmap);
	virtual void draw_bitmap(GUI::BitmapID p_bitmap,const GUI::Point &p_pos,const GUI::Color&p_color=GUI::Color(255,255,255) );
	virtual void draw_bitmap(GUI::BitmapID p_bitmap,const GUI::Point &p_pos, const GUI::Rect& p_src_rect,const GUI::Color&p_color=GUI::Color(255,255,255) );

	virtual void draw_bitmap(GUI::BitmapID p_bitmap,const GUI::Point &p_pos,GUI::Direction p_dir ,const GUI::Color&p_color=GUI::Color(255,255,255) );
	virtual void draw_bitmap(GUI::BitmapID p_bitmap,const GUI::Point &p_pos, const GUI::Rect& p_src_rect,GUI::Direction p_dir,const GUI::Color&p_color=GUI::Color(255,255,255) );

	virtual void draw_tiled_bitmap(GUI::BitmapID p_bitmap,const GUI::Rect& p_rect,const GUI::Point& p_origin,const GUI::Color&p_color=GUI::Color(255,255,255));

	virtual GUI::Size get_bitmap_size(GUI::BitmapID p_bitmap);

	virtual void remove_bitmap(GUI::BitmapID p_bitmap);

	/** Font Management */

	/** The default Font Manager just uses bitmapfonts, taken from previously registered bitmaps,
		 *  if you want to use something else (freetype/SDL_TTf/System Native, etc) you can override the following
	 *  functions, but keep in mind that portability is up to you.
	 */

	/** Updating **/

	virtual void update_screen();
	virtual void update_screen_rect(const GUI::Rect& p_rect);

	virtual GUI::Size get_display_size();
	virtual void set_cursor_pos(const GUI::Point& p_pos,bool p_global=false);

	virtual int get_fullscreen_mode_count();
	virtual GUI::Size get_fullscreen_mode(int p_mode);
	virtual void set_fullscreen(bool,int p_mode=-1);
	virtual bool is_fullscreen();

	virtual void set_key_repeat(int p_delay_ms,int p_rate_ms);
	virtual int get_key_repeat_rate();
	virtual int get_key_repeat_delay();

	virtual void set_window_icon(GUI::BitmapID p_bitmap);


	virtual void set_display_title(String p_title, String p_icon_title);

	void set_screen_size(GUI::Size p_size);


	virtual void set_cursor_visible(bool p_visible);		
	virtual void set_default_cursor_bitmap(GUI::BitmapID p_bitmap,const GUI::Point& p_hotspot);
	virtual void set_cursor_bitmap(GUI::BitmapID p_bitmap,const GUI::Point& p_hotspot);

	void draw_cursor(const GUI::Point& p_pos);

	PainterRenderer();

	~PainterRenderer();

};



#endif
