//
// C++ Implementation: painter_renderer
//
// Description:
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "painter_renderer.h"

#include "fileio/image_loader.h"

void PainterRenderer::set_local_rect(const GUI::Rect& p_rect) {

	rect=p_rect;
}
GUI::Rect PainterRenderer::get_local_rect() {

	return rect;

}

void PainterRenderer::set_clip_rect(bool p_enabled, const GUI::Rect& p_rect) {


	clip_enabled=p_enabled;
	clip=p_rect;



}
bool PainterRenderer::has_clip_rect() {

	return clip_enabled;

}
GUI::Rect PainterRenderer::get_clip_rect() {


	return clip;
}

void PainterRenderer::draw_rect(const GUI::Point & p_from,const GUI::Size & p_size,const GUI::Color& p_color) {


	if (p_size.x <1 || p_size.y<1)
		return;

	//Point from=p_from+rect.pos;

	draw_fill_rect( p_from, GUI::Size( p_size.width, 1 ) , p_color);
	draw_fill_rect( p_from+GUI::Size(0,p_size.height-1), GUI::Size( p_size.width, 1 ) , p_color);
	draw_fill_rect( p_from, GUI::Size( 1, p_size.height ) , p_color);
	draw_fill_rect( p_from+GUI::Size(p_size.width-1,0), GUI::Size( 1, p_size.height ) , p_color);



}

void PainterRenderer::draw_fill_rect(const GUI::Point & p_from,const GUI::Size & p_size,const GUI::Color& p_color) {

	GUI::Rect r=GUI::Rect(p_from,p_size);
	r.pos+=rect.pos;

	r=rect.clip(r);
	if (clip_enabled)
		r=clip.clip(r);
	r=GUI::Rect(GUI::Point(),screen_size).clip(r);


	if (r.has_no_area())
		return;

	GUI::Point from=r.pos;
	GUI::Point size=r.size;
	size.height+=1;
	size.width+=1;

	Vector3 points[4];
	Color color[4];

	for (int i=0;i<4;i++)
		color[i]=Color(p_color.r,p_color.g,p_color.b);


	points[0]=Vector3( (float)from.x, (float)from.y , 0.0f);
	points[1]=Vector3( (float)from.x, (float)from.y+(float)size.y-1.0f , 0.0f);
	points[2]=Vector3( (float)from.x+(float)size.x-1.0f, (float)from.y+(float)size.y-1.0f , 0.0f);
	points[3]=Vector3( (float)from.x+(float)size.x-1.0f, (float)from.y , 0.0f);

	Renderer::get_singleton()->render_primitive( 4, points, NULL, NULL, color );

}

GUI::BitmapID PainterRenderer::load_bitmap(String p_file) {

	/* check cache */
	int exists=-1;
	
	if (p_file!="") {
		for (int i=0;i<MAX_BITMAPS;i++) {
			
			if (bitmaps[i].texture.is_null())
				continue;
			
			if (bitmaps[i].file==p_file) {
				exists=i;
				break;
			}
				
		}
	}
	
	if (exists!=-1) {
		
		for (int i=0;i<MAX_BITMAPS;i++) {
		
			if (bitmaps[i].texture.is_null()) {
				//printf("reusing bitmap %s\n",bitmaps[exists].file.ascii().get_data());
				bitmaps[i]=bitmaps[exists];
				return i;
			}
			
		}
		
		return GUI::INVALID_BITMAP_ID;
	}
	
	Image img;

	Error err=ImageLoader::load_image(p_file,&img);
	if (err != OK ) {
		ERR_PRINT(String("can't load: "+p_file ).ascii().get_data() );
	}
	ERR_FAIL_COND_V(err != OK, GUI::INVALID_BITMAP_ID);

	if (img.get_format()==Image::FORMAT_GRAYSCALE) {
		
		DVector<Uint8> gsalpha;
		gsalpha.resize( img.get_width() * img.get_height() * 2);
		gsalpha.write_lock();
		Uint8 *gsalpha_ptr=gsalpha.write();
		
		for (int i=0;i<img.get_width();i++) {
			
			for (int j=0;j<img.get_height();j++) {
			
				gsalpha_ptr[ (img.get_height()*j+i)*2 ]=255;
				gsalpha_ptr[ (img.get_height()*j+i)*2+1 ]=img.get_pixel(i,j).r;
			}
		}
		
		gsalpha.write_unlock();
				
		Image img_alpha_gray(  img.get_width(), img.get_height(), Image::FORMAT_GRAYSCALE_ALPHA,gsalpha );
		
		return create_bitmap_from_image(img_alpha_gray,p_file);
	}
	return create_bitmap_from_image(img,p_file);
}

GUI::BitmapID PainterRenderer::create_bitmap_from_image(const Image& p_image,String p_file) {

	Image img = p_image;

	Error err;

	int orig_width = img.get_width();
	int orig_height = img.get_height();
	if ((int)nearest_power_of_2(orig_width) != orig_width || (int)nearest_power_of_2(orig_height) != orig_height) {

		img.crop(nearest_power_of_2(orig_width), nearest_power_of_2(orig_height));
	};


	IRef<Texture> texture=Renderer::get_singleton()->create_texture();
	ERR_FAIL_COND_V( !texture, GUI::INVALID_BITMAP_ID );

	Uint32 flags=0; /// no filter, no repeat, no mipmaps
	if (img.get_format()==Image::FORMAT_GRAYSCALE)
		flags=Texture::FLAG_USE_GRAYSCALE_AS_INTENSITY;

	err = texture->load(img, flags);
	ERR_FAIL_COND_V( err != OK, GUI::INVALID_BITMAP_ID );

	int id = create_bitmap_from_texture(texture, orig_width, orig_height);
	 
	if (id>=0)
		bitmaps[id].file=p_file;
	
	return id;
};


GUI::BitmapID PainterRenderer::create_bitmap(const GUI::Size& p_size,GUI::BitmapMode p_mode,bool p_with_alpha) {

	GUI::Size size = p_size;
	IRef<Texture> texture=Renderer::get_singleton()->create_texture();

	//*
	int orig_w = p_size.x;
	int orig_h = p_size.y;
	if ((int)nearest_power_of_2(orig_w) != orig_w) size.x = nearest_power_of_2(orig_w);
	if ((int)nearest_power_of_2(orig_h) != orig_h) size.y = nearest_power_of_2(orig_h);

	Error err = texture->create(size.x,size.y, p_with_alpha?Image::FORMAT_RGBA:Image::FORMAT_RGB);
	ERR_FAIL_COND_V( err, GUI::INVALID_BITMAP_ID );

	// kind of disabled for now....
	return create_bitmap_from_texture(texture, orig_w, orig_h);
}

GUI::BitmapID PainterRenderer::create_bitmap_from_texture(IRef<Texture> p_texture, int width, int height,String p_file) {

	if (width < 0) width = p_texture->get_width();
	if (height < 0) height = p_texture->get_height();

	Bitmap bm(p_texture, width, height);
	bm.file=p_file;
	
	for (int i=0;i<MAX_BITMAPS;i++) {

		if (bitmaps[i].texture)
			continue;

		bitmaps[i]=bm;
		return i;
	}

	return -1;
}

GUI::Color PainterRenderer::get_bitmap_pixel(GUI::BitmapID p_bitmap,const GUI::Point& p_pos) {

	return GUI::Color();

}
void PainterRenderer::set_bitmap_pixel(GUI::BitmapID p_bitmap,const GUI::Point& p_pos,const GUI::Color& p_color,unsigned char p_alpha) {

	ERR_FAIL_COND(!is_bitmap_valid(p_bitmap));
	IRef<Texture> texture = bitmaps[p_bitmap].texture;

	Image image(1,1,texture->get_format());
	image.put_pixel( 0,0 , Color(p_color.r,p_color.g, p_color.b, p_alpha ) );
	texture->paste_area( p_pos.x, p_pos.y, image );
}

bool PainterRenderer::is_bitmap_valid(GUI::BitmapID p_bitmap) {

	if (p_bitmap == -1) return false;
	ERR_FAIL_INDEX_V(p_bitmap,MAX_BITMAPS,false);

	return bitmaps[p_bitmap].texture;
}

void PainterRenderer::draw_bitmap(GUI::BitmapID p_bitmap,const GUI::Point &p_pos,const GUI::Color&p_color ) {

	ERR_FAIL_COND(!is_bitmap_valid(p_bitmap));

	draw_bitmap( p_bitmap, p_pos, GUI::Rect( GUI::Point(), get_bitmap_size( p_bitmap ) ), p_color );
}
void PainterRenderer::draw_bitmap(GUI::BitmapID p_bitmap,const GUI::Point &p_pos, const GUI::Rect& p_src_rect,const GUI::Color &p_color ) {


	ERR_FAIL_COND(!is_bitmap_valid(p_bitmap));


	GUI::Rect dst_rect( p_pos+rect.pos, p_src_rect.size );
	
	dst_rect = rect.clip( dst_rect );
	dst_rect = GUI::Rect(GUI::Point(), screen_size ).clip(dst_rect );
	if (clip_enabled) {
		dst_rect = clip.clip( dst_rect );
	}
		
	if (dst_rect.has_no_area())
		return;
		
	
	GUI::Rect src_rect(p_src_rect.pos + (dst_rect.pos - (rect.pos+p_pos)), dst_rect.size );

	//GUI::Size bm_size=get_bitmap_size( p_bitmap );
	GUI::Size bm_size = GUI::Size(bitmaps[p_bitmap].texture->get_width(), bitmaps[p_bitmap].texture->get_height());


	Vector3 rect_beg( (float)src_rect.pos.x/(float)bm_size.width, (float)src_rect.pos.y/(float)bm_size.height,0 );

	Vector3 rect_end( (float)(src_rect.pos.x+src_rect.size.x)/(float)bm_size.width, (float)(src_rect.pos.y+src_rect.size.y)/(float)bm_size.height,0 );


	Vector3 dst_pos( (float)dst_rect.pos.x, (float)dst_rect.pos.y, 0.0f);
	Vector3 dst_size( (float)src_rect.size.x, (float)src_rect.size.y, 0.0f);

	//printf("uv is %g,%g - size %g,%g\n",(float)src_rect.pos.x/(float)bm_size.width, (float)src_rect.pos.y/(float)bm_size.height,(float)src_rect.size.x/(float)bm_size.width, (float)src_rect.size.y/(float)bm_size.height);

	Vector3 rect[4];
	float uv[8];
	Color color[4];

	for (int i=0;i<4;i++)
		color[i]=Color(p_color.r,p_color.g,p_color.b);



	uv[0]=rect_beg.x;
	uv[1]=rect_beg.y;
	rect[0]=dst_pos;

	uv[2]=rect_end.x;
	uv[3]=rect_beg.y;
	rect[1]=dst_pos + Vector3( dst_size.x, 0, 0);

	uv[4]=rect_end.x;
	uv[5]=rect_end.y;
	rect[2]=dst_pos + dst_size;

	uv[6]=rect_beg.x;
	uv[7]=rect_end.y;
	rect[3]=dst_pos + Vector3( 0, dst_size.y, 0);

	Renderer::get_singleton()->render_primitive_change_texture( bitmaps[p_bitmap].texture );
	Renderer::get_singleton()->render_primitive( 4, rect, 0, 0, color, uv );

}

void PainterRenderer::draw_bitmap(GUI::BitmapID p_bitmap,const GUI::Point &p_pos,GUI::Direction p_dir ,const GUI::Color&p_color ) {

	ERR_FAIL();
}
void PainterRenderer::draw_bitmap(GUI::BitmapID p_bitmap,const GUI::Point &p_pos, const GUI::Rect& p_src_rect,GUI::Direction p_dir,const GUI::Color&p_color ) {

	ERR_FAIL();
}


void PainterRenderer::draw_tiled_bitmap(GUI::BitmapID p_bitmap,const GUI::Rect& p_rect,const GUI::Point& p_origin,const GUI::Color&p_color) {

	ERR_FAIL_COND(!is_bitmap_valid(p_bitmap));

	if (p_rect.size.width<=0 || p_rect.size.height<=0)
		return;

	GUI::Size bm_size=get_bitmap_size( p_bitmap );

	GUI::Size ofs=p_rect.pos;

	if (p_origin.x < p_rect.pos.x) {
		ofs.x-=(p_rect.pos.x-p_origin.x)%bm_size.width;
	} else if (p_origin.x > p_rect.pos.x) {

		ofs.x-=bm_size.width-(p_origin.x-p_rect.pos.x)%bm_size.width;
	}

	if (p_origin.y < p_rect.pos.y) {
		ofs.y-=(p_rect.pos.y-p_origin.y)%bm_size.height;
	} else if (p_origin.y > p_rect.pos.y) {

		ofs.y-=bm_size.height-(p_origin.y-p_rect.pos.y)%bm_size.height;
	}



	while (ofs.y<(p_rect.pos.y+p_rect.size.height)) {

		GUI::Point ofsh=ofs;

		while(ofsh.x<(p_rect.pos.x+p_rect.size.width)) {


			GUI::Point point=ofsh;
			GUI::Rect lrect=GUI::Rect(GUI::Point(),bm_size);

			if (ofsh.x<p_rect.pos.x) {

				point.x=p_rect.pos.x;
				lrect.pos.x=p_rect.pos.x-ofsh.x;
				lrect.size.width-=lrect.pos.x;
			}

			if ( (ofsh.x+bm_size.width) > (p_rect.pos.x+p_rect.size.width) ) {

				lrect.size.width-=(ofsh.x+bm_size.width)-(p_rect.pos.x+p_rect.size.width);
			}

			if (ofsh.y<p_rect.pos.y) {

				point.y=p_rect.pos.y;
				lrect.pos.y=p_rect.pos.y-ofsh.y;
				lrect.size.height-=lrect.pos.y;
			}

			if ( (ofsh.y+bm_size.height) > (p_rect.pos.y+p_rect.size.height) ) {

				lrect.size.height-=(ofsh.y+bm_size.height)-(p_rect.pos.y+p_rect.size.height);
			}


			draw_bitmap( p_bitmap, point, lrect,p_color );

			ofsh.x+=bm_size.width;

		}

		ofs.y+=bm_size.height;

	}


}

GUI::Size PainterRenderer::get_bitmap_size(GUI::BitmapID p_bitmap) {

	if (!is_bitmap_valid(p_bitmap)) return GUI::Size(0, 0);
	//ERR_FAIL_COND_V(!is_bitmap_valid(p_bitmap),GUI::Size(0,0));

	//return GUI::Size( bitmaps[p_bitmap].texture->get_width(), bitmaps[p_bitmap].texture->get_height() );
	return GUI::Size( bitmaps[p_bitmap].width, bitmaps[p_bitmap].height );
}

void PainterRenderer::remove_bitmap(GUI::BitmapID p_bitmap) {


	ERR_FAIL_COND(!is_bitmap_valid( p_bitmap ));
	bitmaps[p_bitmap] = Bitmap();

}

void PainterRenderer::update_screen() {



}
void PainterRenderer::update_screen_rect(const GUI::Rect& p_rect) {



}

GUI::Size PainterRenderer::get_display_size() {


	return screen_size;
}
void PainterRenderer::set_cursor_pos(const GUI::Point& p_pos,bool p_global) {

	// ?

}


int PainterRenderer::get_fullscreen_mode_count() {

	return 0;

}
GUI::Size PainterRenderer::get_fullscreen_mode(int p_mode) {


	return GUI::Size();
}
void PainterRenderer::set_fullscreen(bool,int p_mode) {



}
bool PainterRenderer::is_fullscreen() {

	return true;

}

void PainterRenderer::set_key_repeat(int p_delay_ms,int p_rate_ms) {



}
int PainterRenderer::get_key_repeat_rate() {


	return 0;
}
int PainterRenderer::get_key_repeat_delay() {


	return 0;

}

void PainterRenderer::set_window_icon(GUI::BitmapID p_bitmap) {



}

void PainterRenderer::set_display_title(String p_title, String p_icon_title) {



}

void PainterRenderer::set_screen_size(GUI::Size p_size) {

	screen_size=p_size;
}



void PainterRenderer::set_cursor_visible(bool p_visible) {

	cursor_visible=p_visible;
}

void PainterRenderer::set_default_cursor_bitmap(GUI::BitmapID p_bitmap,const GUI::Point& p_hotspot) {

	if (p_bitmap==GUI::INVALID_BITMAP_ID) {
	
		default_cursor=1;
		default_cursor_hotspot=GUI::Point();
				
	} else {
	
		default_cursor=p_bitmap;
		default_cursor_hotspot=p_hotspot;
	}
}

void PainterRenderer::set_cursor_bitmap(GUI::BitmapID p_bitmap,const GUI::Point& p_hotspot) {

	cursor=p_bitmap;
	cursor_hotspot=p_hotspot;
}

void PainterRenderer::draw_cursor(const GUI::Point& p_pos) {

	if (!cursor_visible)
		return;

	if (is_bitmap_valid( cursor ))
		draw_bitmap( cursor, p_pos-cursor_hotspot );
	else {
	
		draw_bitmap( default_cursor, p_pos-default_cursor_hotspot );	
	}
}

#include "default_font.inc"

#define DEFAULT_CURSOR_WIDTH 32
#define DEFAULT_CURSOR_HEIGHT 32

static const char* _default_cursor[DEFAULT_CURSOR_HEIGHT]={
"X                               ",
"XX                              ",
"X.X                             ",
"X..X                            ",
"X...X                           ",
"X....X                          ",
"X.....X                         ",
"X......X                        ",
"X.......X                       ",
"X........X                      ",
"X.....XXXXX                     ",
"X..X..X                         ",
"X.X X..X                        ",
"XX  X..X                        ",
"X    X..X                       ",
"     X..X                       ",
"      X..X                      ",
"      X..X                      ",
"       XX                       ",
"                                ",
"                                ",
"                                ",
"                                ",
"                                ",
"                                ",
"                                ",
"                                ",
"                                ",
"                                ",
"                                ",
"                                ",
"                                "};

PainterRenderer::PainterRenderer() {


	clip_enabled=false;

	/* Create Default Font */

	DVector<Uint8> texdata;
	texdata.resize(builtin_font_texture_width*builtin_font_texture_height*2);
	int tdlen=texdata.size();
	for (int i=0;i<tdlen/2;i++) {
		texdata.set(i*2,255);
		texdata.set(i*2+1,builtin_font_texture[i]);
	}
	Image img(builtin_font_texture_width,builtin_font_texture_height,Image::FORMAT_GRAYSCALE_ALPHA,texdata);

	IRef<Texture> font_tex=Renderer::get_singleton()->create_texture();

	font_tex->load( img );

	bitmaps[0].texture=font_tex;
	bitmaps[0].width=font_tex->get_width();
	bitmaps[0].height=font_tex->get_height();

	GUI::FontID dfid=create_font( builtin_font_height, builtin_font_ascent );

	for (int i=0;i<builtin_font_charcount;i++) {

		unsigned int character=builtin_font_charrects[i][0];
		GUI::Rect frect;
		frect.pos.x=builtin_font_charrects[i][1];
		frect.pos.y=builtin_font_charrects[i][2];
		frect.size.x=builtin_font_charrects[i][3];
		frect.size.y=builtin_font_charrects[i][4];
		int valign=builtin_font_charrects[i][5];

		font_add_char( dfid, character, 0, frect, valign );
	}
	
	Image default_cursor_image( DEFAULT_CURSOR_WIDTH, DEFAULT_CURSOR_HEIGHT , Image::FORMAT_RGBA );
	
	for (int i=0;i<DEFAULT_CURSOR_WIDTH;i++) {
	
		for (int j=0;j<DEFAULT_CURSOR_HEIGHT;j++) {
	
			Color c;
			switch(_default_cursor[j][i]) {
			
				case 'X': c=Color(0,0,0,255); break;
				case '.': c=Color(255,255,255,255); break;
				default: c=Color(0,0,0,0); break;			
			}			
			
			default_cursor_image.put_pixel(i,j,c);
		}
	}
	
	IRef<Texture> cursor_tex=Renderer::get_singleton()->create_texture();

	cursor_tex->load( default_cursor_image );

	bitmaps[1].texture=cursor_tex;
	bitmaps[1].width=cursor_tex->get_width();
	bitmaps[1].height=cursor_tex->get_height();
	
	default_cursor=1;
	cursor_visible=true;
}


PainterRenderer::~PainterRenderer()
{

	for(int i=0;i<MAX_BITMAPS;i++) {

		if (bitmaps[i].texture) {
			bitmaps[i] = Bitmap();
		}
	}
}
