#ifdef SDL_ENABLED
//
// C++ Implementation: paintersdl
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "painter_sdl.h"
//#include <SDL_gfxPrimitives.h>
#include "base/defs.h"
namespace GUI {


Color SDL_GetPixel(SDL_Surface *surface, int x, int y)
{
	if (x<0 || x>=surface->w)
		return Color();
	if (y<0 || y>=surface->h)
		return Color();
	
	
	int bpp = surface->format->BytesPerPixel;
	/* Here p is the address to the pixel we want to retrieve */
	Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

	Uint8 r,g,b;
	switch(bpp) {
		case 1:
			return Color(*p);

		case 2:
			SDL_GetRGB(*(Uint16*)p,surface->format,&r,&g,&b);
			return Color(r,g,b);

		case 3:
			if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
				SDL_GetRGB(p[0] << 16 | p[1] << 8 | p[2],surface->format,&r,&g,&b);
			else
				SDL_GetRGB(p[0] | p[1] << 8 | p[2] << 16,surface->format,&r,&g,&b);

			return Color(r,g,b);
			
		case 4:

			SDL_GetRGB(*(Uint32*)p,surface->format,&r,&g,&b);
			return Color(r,g,b);

	}

	return Color(0);
}


static inline void SDL_PutPixel(SDL_Surface *surface, int x, int y, const Color& p_color,Uint8 p_alpha=255)
{

	if (x<0 || x>=surface->w)
		return;
	if (y<0 || y>=surface->h)
		return;
	

	int bpp = surface->format->BytesPerPixel;
	/* Here p is the address to the pixel we want to set */
	Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

	switch(bpp) {
		case 1:
			*p = ((Uint32)p_color.r+(Uint32)p_color.g+(Uint32)p_color.b)/3;
			break;

		case 2:
			*(Uint16 *)p = SDL_MapRGBA( surface->format,p_color.r, p_color.g, p_color.b, p_alpha);
			break;

		case 3: {
			Uint16 pixel=SDL_MapRGB( surface->format,p_color.r, p_color.g, p_color.b);
			if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
				p[0] = (pixel >> 16) & 0xff;
				p[1] = (pixel >> 8) & 0xff;
				p[2] = pixel & 0xff;
			} else {
				p[0] = pixel & 0xff;
				p[1] = (pixel >> 8) & 0xff;
				p[2] = (pixel >> 16) & 0xff;
			}
		}	break;

		case 4:

			*(Uint32 *)p = SDL_MapRGBA( surface->format,p_color.r, p_color.g, p_color.b, p_alpha);
			break;
	}
}

void PainterSDL::draw_line(const Point& p_from, const Point& p_to, const Color& p_color,int p_width,unsigned char p_alpha) {

	/* not the fastest thing on earth but.. will likely optimize someday */
	
	if (SDL_MUSTLOCK(surface)) {
	
		int res=SDL_LockSurface(surface);
		
		if (res==-1)
			return;
	}
		
	
	for (int i=0;i<p_width;i++) {
	
		int x1=p_from.x+rect.pos.x;
		int x2=p_to.x+rect.pos.x;
		int y1=p_from.y+rect.pos.y;
		int y2=p_to.y+rect.pos.y;
		
		if (i>0) {
		
			int add = (i>>1)*((i&1)?-1:1);
			if (ABS(x1-x2) > ABS(y1-y2) ) {
			
				y1+=add;
				y2+=add;
			} else {
			
				x1+=add;
				x2+=add;			
			}
		}
		
		int lg_delta, sh_delta, cycle, lg_step, sh_step;
		
		lg_delta = x2 - x1;
		sh_delta = y2 - y1;
		lg_step = SGN(lg_delta);
		lg_delta = ABS(lg_delta);
		sh_step = SGN(sh_delta);
		sh_delta = ABS(sh_delta);
		if (sh_delta < lg_delta) {
			cycle = lg_delta >> 1;
			while (x1 != x2) {
				SDL_PutPixel(surface,x1,y1,p_color);
				cycle += sh_delta;
				if (cycle > lg_delta) {
					cycle -= lg_delta;
					y1 += sh_step;
				}
				x1 += lg_step;
			}
			SDL_PutPixel(surface,x1,y1,p_color);
		}
		cycle = sh_delta >> 1;
		while (y1 != y2) {
			SDL_PutPixel(surface,x1,y1,p_color);
			cycle += lg_delta;
			if (cycle > sh_delta) {
				cycle -= sh_delta;
				x1 += lg_step;
			}
			y1 += sh_step;
		}
		
		SDL_PutPixel(surface,x1,y1,p_color);
	}
		
	if (SDL_MUSTLOCK(surface)) {
	
		SDL_UnlockSurface(surface);
		
	}
		

}

void PainterSDL::set_clip_rect(bool p_enabled, const Rect& p_rect) {
	
	if (!p_enabled || p_rect.has_no_area()) {
		
		SDL_SetClipRect(surface, 0);
		clip_rect_active=false;
		clip_rect=Rect();
		return;
	}
	
	Rect global_rect=p_rect;
	
	SDL_Rect dst_rect;
	
	dst_rect.x=global_rect.pos.x;
	dst_rect.y=global_rect.pos.y;
	dst_rect.w=global_rect.size.width;
	dst_rect.h=global_rect.size.height;


	SDL_SetClipRect(surface, &dst_rect);
	
	clip_rect=global_rect;
	clip_rect_active=true;

}

bool PainterSDL::has_clip_rect() {
	
	return clip_rect_active;	
}

Rect PainterSDL::get_clip_rect() {
	
	return clip_rect;
}

void PainterSDL::set_local_rect(const Rect& p_rect) {

	rect=p_rect;
}

Rect PainterSDL::get_local_rect() {
	
	return rect;
}

void PainterSDL::draw_rect(const Point & p_from,const Size & p_size,const Color& p_color) {
	
	
	if (p_size.x <1 || p_size.y<1)
		return;
	
	//Point from=p_from+rect.pos;

	draw_fill_rect( p_from, Size( p_size.width, 1 ) , p_color);
	draw_fill_rect( p_from+Size(0,p_size.height-1), Size( p_size.width, 1 ) , p_color);
	draw_fill_rect( p_from, Size( 1, p_size.height ) , p_color);
	draw_fill_rect( p_from+Size(p_size.width-1,0), Size( 1, p_size.height ) , p_color);
//	rectangleRGBA(surface, from.x,from.y,from.x+p_size.width-1, from.y+p_size.height-1,p_color.r,p_color.g,p_color.b,255);
}

void PainterSDL::draw_fill_rect(const Point & p_from,const Size & p_size,const Color& p_color) {
	if (p_size.x <0 || p_size.y<0)
		return;
	Rect rct;
	
	rct.pos=p_from+rect.pos;
	rct.size=p_size;

	rct=rect.clip(rct);
	
	
	SDL_Rect dst_rect;
	
	dst_rect.x=rct.pos.x;
	dst_rect.y=rct.pos.y;
	dst_rect.w=rct.size.width;
	dst_rect.h=rct.size.height;
	
	SDL_FillRect(surface,&dst_rect, SDL_MapRGB( surface->format, p_color.r,p_color.g,p_color.b) );
	
	
}

void PainterSDL::draw_bitmap(BitmapID p_bitmap,const Point &p_pos,const Color&p_color) {
	
	if (!is_bitmap_valid(p_bitmap)) {

		PRINT_ERROR("Invalid BitmapID");
		return;
	};	


	
	draw_bitmap( p_bitmap, p_pos, Rect( Point(), get_bitmap_size( p_bitmap )),p_color );

}

void PainterSDL::draw_bitmap(BitmapID p_bitmap,const Point &p_pos,Direction p_dir ,const Color&p_color ) {
	
	draw_bitmap( p_bitmap, p_pos, Rect( Point(), get_bitmap_size( p_bitmap )),p_dir,p_color );
	
}


BitmapID PainterSDL::load_bitmap(String p_file) {
	
	SDL_Surface *s=SDL_LoadBMP(p_file.utf8().get_data());
	if (s==0)
		return -1;
	if (s->format->BitsPerPixel>=24) {
		
		SDL_SetColorKey(s,SDL_SRCCOLORKEY,0xFF00FF);
	}
	
	return register_bitmap( s );
		
	
}
BitmapID PainterSDL::create_bitmap(const Size& p_size,BitmapMode p_mode,bool p_alpha) {

	if (p_size.width<=0 || p_size.height<=0)
		return -1;
	

	SDL_Surface *s=0;

	if (p_mode==MODE_PIXMAP) {

		if (p_alpha) { 
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
			Uint32 rmask = 0xff000000;
			Uint32 gmask = 0x00ff0000;
			Uint32 bmask = 0x0000ff00;
			Uint32 amask = 0x000000ff;
#else
			Uint32 rmask = 0x000000ff;
			Uint32 gmask = 0x0000ff00;
			Uint32 bmask = 0x00ff0000;
			Uint32 amask = 0xff000000;
#endif			
			s= SDL_CreateRGBSurface((surface->flags&SDL_HWSURFACE)?SDL_HWSURFACE:SDL_SWSURFACE, p_size.width, p_size.height, 32,rmask,gmask,bmask,amask);
			
		} else {
			s= SDL_CreateRGBSurface((surface->flags&SDL_HWSURFACE)?SDL_HWSURFACE:SDL_SWSURFACE, p_size.width, p_size.height, surface->format->BitsPerPixel, surface->format->Rmask,surface->format->Gmask,surface->format->Bmask,surface->format->Amask);
			
			if (s)
				SDL_SetColorKey(s,SDL_SRCCOLORKEY,0xFF00FF);
		}
		
	} if (p_mode==MODE_ALPHA_MASK) {

		s= SDL_CreateRGBSurface(SDL_SWSURFACE, p_size.width, p_size.height, 8, 0,0,0,0xFF);

		if (s) {

			SDL_Color colors;
					/* Fill colors with color information */
			for(int i=0;i<256;i++){
				colors.r=i;
				colors.g=i;
				colors.b=i;
				/* Set palette */
				SDL_SetPalette(s, SDL_LOGPAL, &colors, i, 1);					
			}
		}

	}

	
	if (!s)
		return -1;

	return register_bitmap( s );	
}

Color PainterSDL::get_bitmap_pixel(BitmapID p_bitmap,const Point& p_pos) {

	if (!is_bitmap_valid( p_bitmap) )
		return (Color(0));

	if (SDL_MUSTLOCK(bitmaps[p_bitmap])) {
		int res=SDL_LockSurface(bitmaps[p_bitmap]);

		if (res==-1)
			return Color(0);
	}	
	Color c=SDL_GetPixel(bitmaps[p_bitmap],p_pos.x,p_pos.y);

	if (SDL_MUSTLOCK(bitmaps[p_bitmap])) {
		SDL_UnlockSurface(bitmaps[p_bitmap]);
	}
	return c;
	
}

void PainterSDL::set_bitmap_pixel(BitmapID p_bitmap,const Point& p_pos,const Color& p_color,unsigned char p_alpha) {

	if (!is_bitmap_valid( p_bitmap) )
		return ;

	if (SDL_MUSTLOCK(bitmaps[p_bitmap])) {
	
		int res=SDL_LockSurface(bitmaps[p_bitmap]);
		if (res==-1)
			return ;
	}
	
	SDL_PutPixel(bitmaps[p_bitmap],p_pos.x,p_pos.y,p_color,p_alpha);

	if (SDL_MUSTLOCK(bitmaps[p_bitmap])) {
	
		SDL_UnlockSurface(bitmaps[p_bitmap]);
	}
	
}

void PainterSDL::draw_bitmap(BitmapID p_bitmap,const Point &p_pos, const Rect& p_src_rect,const Color&p_color) {

	
	draw_bitmap(p_bitmap,p_pos,p_src_rect,RIGHT,p_color);
}

void PainterSDL::draw_bitmap(BitmapID p_bitmap,const Point &p_pos, const Rect& p_src_rect,Direction p_dir,const Color&p_color) {
	
	if (!is_bitmap_valid(p_bitmap)) {

		PRINT_ERROR("Invalid BitmapID");
		return;
	};	

	
	if (bitmaps[p_bitmap]->format->BytesPerPixel==1 || p_dir!=RIGHT) {

		draw_custom_bitmap( p_bitmap, p_pos, p_src_rect,p_color,p_dir);
		return;
	}

	
	Point pos=p_pos+rect.pos;
	
	/* Convert all to same coordinate space */
	Rect write_rect=Rect(pos,p_src_rect.size);
	Rect src_surface_rect=Rect(pos-p_src_rect.pos, get_bitmap_size( p_bitmap) ); //source rect in write coordinates
	Rect dst_surface_rect=Rect(Point(), Size( surface->w, surface->h ) );

	/* clip by both surfaces */

	write_rect=src_surface_rect.clip( write_rect );
	write_rect=rect.clip( write_rect );
	write_rect=dst_surface_rect.clip( write_rect );
	if (clip_rect_active)
		write_rect=clip_rect.clip( write_rect );
	

	if (write_rect.has_no_area()) {
		
		return;
	} 

	Rect read_rect( write_rect.pos-src_surface_rect.pos, write_rect.size );
	
	

	SDL_Rect src_rect;
	src_rect.x=read_rect.pos.x;
	src_rect.y=read_rect.pos.y;
	src_rect.w=read_rect.size.width;
	src_rect.h=read_rect.size.height;
	
	
	SDL_Rect dst_rect;
	dst_rect.x=write_rect.pos.x;
	dst_rect.y=write_rect.pos.y;
	dst_rect.w=dst_rect.h=0;
			
	SDL_BlitSurface(bitmaps[p_bitmap], &src_rect, surface, &dst_rect);
	
	
}

void PainterSDL::draw_custom_bitmap(BitmapID p_bitmap,const Point &p_pos, const Rect& p_src_rect, const Color&p_color, Direction p_dir ) {

	
	Uint32 color=SDL_MapRGB( surface->format,p_color.r, p_color.g, p_color.b);
	SDL_Surface *src_surface=bitmaps[p_bitmap];

	bool is_alphamask=src_surface->format->BytesPerPixel==1;
	
	if (!is_alphamask) {
		
		PRINT_ERROR("Only alphamask pixmaps supported as for now");
		return;
	}
		
			
	Uint32 Rmask = surface->format->Rmask;
	Uint32 Gmask = surface->format->Gmask;
	Uint32 Bmask = surface->format->Bmask;
	Uint32 Amask = surface->format->Amask;
	
	Uint32 R = 0, G = 0, B = 0, A = 0;

	
	/* CLIPPING, STEP 1 */
	Rect global_rect=Rect( p_pos + rect.pos , p_src_rect.size ); //convert src rect to global screen coords
	
	
	/* CLIPPING, STEP2, Apply Rotation */
	if (p_dir==RIGHT) {
	
		//global_rect=Rect(rect.pos+p_pos-p_src_rect.pos, Size( src_surface->w, src_surface->h ) ).clip( global_rect ); // clip by src_surface
	
	} else if (p_dir==DOWN) {
		
		global_rect.size.swap_xy();
		
		Rect clipsrc = Rect(rect.pos+p_pos-p_src_rect.pos, Size( src_surface->w, src_surface->h ) );
		
		global_rect=clipsrc.clip( global_rect ); // clip by src_surface
	
	} else if (p_dir==LEFT || p_dir==UP) {
		
		PRINT_ERROR("Unsupported Direction in bitmap draw - only RIGHT and DOWN work ATM");
		return;
	} // RIGHT is default
		
	/* CLIPPING Step 3 CLIP IT */
	
	
	global_rect = rect.clip( global_rect ); //clip by local rect
	if (clip_rect_active) // clip by clip rect, if enabled
		global_rect=clip_rect.clip( global_rect );
	
		
	global_rect =  Rect(Point(), Size( surface->w, surface->h ) ).clip ( global_rect ); // clip by screen
	
	if (global_rect.has_no_area()) {
		
		return; //nothing to do, no area!
	}
	
	
	
	/* CLIPPING Step 4 , Transform to local again */
	//Rect local_rect=Rect( global_rect.pos-(p_pos + rect.pos) , global_rect.size ); // first create, in local coords
	Rect write_rect=global_rect;
	Rect read_rect=Rect( global_rect.pos-(p_pos + rect.pos)+p_src_rect.pos, global_rect.size );
	/* CLIPPING Step 5, Deapply Rotation */
	
	if (p_dir==DOWN) {
		
		read_rect.size.swap_xy(); //adjust size
		/*
		read_rect.pos-=p_src_rect.pos;
		read_rect.pos.swap_xy();
		read_rect.pos+=p_src_rect.pos;*/

	}
		
	/* Compute read/write rects */


	

//	draw_fill_rect(p_pos,p_src_rect.size,Color(233,233,233));
//	draw_fill_rect(write_rect.pos-rect.pos,write_rect.size,Color(233,233,233));

	if (SDL_MUSTLOCK(src_surface)) {
		
		int res=SDL_LockSurface(src_surface);
		if (res==-1)
			return;
	}
		
	if (SDL_MUSTLOCK(surface)) {
		int res=SDL_LockSurface(surface);

		if (res==-1)
			return;
	}	
	
	switch (surface->format->BytesPerPixel) {
		case 2: {

			for (int iy=0;iy<read_rect.size.height;iy++) {

				Uint16 *dst_line;
				
				switch (p_dir) {
					
					case RIGHT: {
						dst_line=(Uint16 *)surface->pixels + (write_rect.pos.y+iy) * surface->pitch / 2 + write_rect.pos.x;
					} break;
					case DOWN: {
						dst_line=(Uint16 *)surface->pixels + (write_rect.pos.y) * surface->pitch / 2 + write_rect.pos.x+(read_rect.size.height-iy-1);

					} break;
					default: {};
				}
					
				Uint8 *src_line=(Uint8 *)src_surface->pixels + (read_rect.pos.y+iy) * src_surface->pitch  + read_rect.pos.x;

				int todo=read_rect.size.width;

				while (todo--) {

					Uint8 alpha=*src_line;
					if ( alpha == 255) {
						
						*dst_line = color;
					} else if (alpha>0) {
						
						
						Uint32 dc = *dst_line;

						R = ((dc & Rmask) + (((color & Rmask) - (dc & Rmask)) * alpha >> 8)) & Rmask;
						G = ((dc & Gmask) + (((color & Gmask) - (dc & Gmask)) * alpha >> 8)) & Gmask;
						B = ((dc & Bmask) + (((color & Bmask) - (dc & Bmask)) * alpha >> 8)) & Bmask;
						if (Amask)
							A = ((dc & Amask) + (((color & Amask) - (dc & Amask)) * alpha >> 8)) & Amask;

						*dst_line = R | G | B | A;
					}
					src_line++;
					switch (p_dir) {
					
						case RIGHT: {

							dst_line++;
						} break;
						case DOWN: {

							dst_line+=surface->pitch / 2;
						} break;
					}
				}
			}
		
		} break;



		case 4:{

			for (int iy=0;iy<read_rect.size.height;iy++) {

				
				Uint32 *dst_line;
				
				switch (p_dir) {
					
					case RIGHT: {
						dst_line=(Uint32 *)surface->pixels + (write_rect.pos.y+iy) * surface->pitch / 4 + write_rect.pos.x;
					} break;
					case DOWN: {
						dst_line=(Uint32 *)surface->pixels + (write_rect.pos.y) * surface->pitch / 4 + write_rect.pos.x+(read_rect.size.height-iy-1);
					} break;
					default: {};
				}
				
				Uint8 *src_line=(Uint8 *)(src_surface->pixels) + (read_rect.pos.y+iy) * src_surface->pitch  + read_rect.pos.x;

				int todo=read_rect.size.width;

				while (todo--) {
					Uint8 alpha=*src_line;
					
					if (alpha == 255) {
						*dst_line = color;
					} else if (alpha>0) {
						
						Uint32 dc = *dst_line;
		
						R = ((dc & Rmask) + (((color & Rmask) - (dc & Rmask)) * alpha >> 8)) & Rmask;
						G = ((dc & Gmask) + (((color & Gmask) - (dc & Gmask)) * alpha >> 8)) & Gmask;
						B = ((dc & Bmask) + (((color & Bmask) - (dc & Bmask)) * alpha >> 8)) & Bmask;
						if (Amask)
							A = ((dc & Amask) + (((color & Amask) - (dc & Amask)) * alpha >> 8)) & Amask;
		
						*dst_line = R | G | B | A;
					}

					src_line++;
					switch (p_dir) {
					
						case RIGHT: {

							dst_line++;
						} break;
						case DOWN: {

							dst_line+=surface->pitch / 4;
						} break;
					}
					

				}
			}
		} break;

	}
	
	if (SDL_MUSTLOCK(src_surface)) {
		
		SDL_UnlockSurface(src_surface);
	}
	
	if (SDL_MUSTLOCK(surface)) {
		
		SDL_UnlockSurface(surface);
	}
	
}
	
void PainterSDL::draw_tiled_bitmap(BitmapID p_bitmap,const Rect& p_rect,const Point& p_origin,const Color&p_color ) {
		
	if (!is_bitmap_valid(p_bitmap))
		return;
	
	if (p_rect.size.width<=0 || p_rect.size.height<=0)
		return;
	
	Size bm_size=get_bitmap_size( p_bitmap );
	
	Size ofs=p_rect.pos;
	
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
		
		Point ofsh=ofs;
		
		while(ofsh.x<(p_rect.pos.x+p_rect.size.width)) {
			
			
			Point point=ofsh;
			Rect lrect=Rect(Point(),bm_size);
			
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
	
bool PainterSDL::is_bitmap_valid(BitmapID p_bitmap) {
	
	return (p_bitmap>=0 && p_bitmap<MAX_BITMAPS && bitmaps[p_bitmap]);
}

Size PainterSDL::get_bitmap_size(BitmapID p_bitmap) {
	
	if (!is_bitmap_valid(p_bitmap)) {

		PRINT_ERROR("Invalid BitmapID");
		return Size();
	};
	
	return Size(bitmaps[p_bitmap]->w,bitmaps[p_bitmap]->h);
}


void PainterSDL::remove_bitmap(BitmapID p_bitmap) {

	if (!is_bitmap_valid(p_bitmap))
		return;
	
	SDL_FreeSurface(bitmaps[p_bitmap]);
	bitmaps[p_bitmap]=0;
}
BitmapID PainterSDL::register_bitmap(SDL_Surface *p_bitmap) {
	
	for (int i=0;i<MAX_BITMAPS;i++) {
	
		if (bitmaps[i])
			continue;
		
		bitmaps[i]=p_bitmap;
		return i;
	}
	
	return -1; //no bitmap found
}

void PainterSDL::update_screen() {
	
	SDL_UpdateRect(surface, 0, 0, 0, 0); //updates all

}
void PainterSDL::update_screen_rect(const Rect& p_rect) {
	
	Rect screen_rect=Rect( Point() , Size( surface->w, surface->h ) );
	Rect update_rect=screen_rect.clip( p_rect );
//	update_rect.pos+=rect.pos;
	
	//update_rect = update_rect.clip( rect );
	SDL_UpdateRect(surface, update_rect.pos.x, update_rect.pos.y, update_rect.size.width, update_rect.size.height);

}

void PainterSDL::set_cursor_pos(const Point& p_pos,bool p_global) {
	
	Point pos=p_pos;
	
	if (!p_global)
		pos+=rect.pos;
	
	SDL_WarpMouse(pos.x,pos.y);
}

Size PainterSDL::get_display_size() {
	
	return Size( surface->w, surface->h );
}


int PainterSDL::get_fullscreen_mode_count() {
	
	
	SDL_Rect **modes;
	int i;
	
	/* Get available fullscreen/hardware modes */
	modes=SDL_ListModes(NULL, SDL_FULLSCREEN|SDL_SWSURFACE);
	
	if (modes == (SDL_Rect **)0)
		    return 0;

	/* Check if our resolution is restricted */
	if(modes == (SDL_Rect **)-1) 
		return 0;
	
	int count=0;
	for(i=0;modes[i];++i)
		count++;
	
	return count;
	
}
Size PainterSDL::get_fullscreen_mode(int p_mode) {
	
	SDL_Rect **modes;
	int i;
	
	/* Get available fullscreen/hardware modes */
	modes=SDL_ListModes(NULL, SDL_FULLSCREEN|SDL_SWSURFACE);
	
	if (modes == (SDL_Rect **)0)
		    return Size();

	/* Check if our resolution is restricted */
	if(modes == (SDL_Rect **)-1) 
		return Size();
	
	int count=0;
	for(i=0;modes[i];++i) {
		if (count==p_mode)
			return Size(modes[i]->w, modes[i]->h);
		
		count++;
	}
	
	return Size();	
}
void PainterSDL::set_fullscreen(bool p_full,int p_mode) {
	
	
	if (p_full) {
		
		Size size=get_fullscreen_mode( p_mode );
		if (size.height==0 || size.width==0)
			return;
		if (!is_fullscreen())
			normal_mode=Size( surface->w, surface->h );
		Uint32 new_flags=surface->flags;
		new_flags|=SDL_FULLSCREEN;
		surface=SDL_SetVideoMode( size.width,size.height, surface->format->BitsPerPixel,new_flags); 
		
	} else {
		
		
		if (normal_mode.width==0 || normal_mode.height==0) {
			normal_mode=Size( surface->w, surface->h );
		}
		Uint32 new_flags=surface->flags;
		new_flags&=~SDL_FULLSCREEN;
		new_flags|=SDL_RESIZABLE;
		surface=SDL_SetVideoMode( normal_mode.width,normal_mode.height, surface->format->BitsPerPixel,new_flags); 
		
	}
	
}

bool PainterSDL::is_fullscreen() {
	
	return surface->flags&SDL_FULLSCREEN;	
}

void PainterSDL::set_display_title(String p_title, String p_icon_title) {
	
	SDL_WM_SetCaption(p_title.utf8().get_data(), p_icon_title.utf8().get_data());
}

void PainterSDL::set_key_repeat(int p_delay_ms,int p_rate_ms) {
	
	rate=p_rate_ms;
	delay=p_delay_ms;
	
	SDL_EnableKeyRepeat(p_delay_ms,p_rate_ms);
	
}
int PainterSDL::get_key_repeat_rate() {
	
	return rate;	
}
int PainterSDL::get_key_repeat_delay() {
	
	return delay;
}

void PainterSDL::set_window_icon(BitmapID p_bitmap) {
	
	if (!is_bitmap_valid(p_bitmap))
		return;
	
	SDL_WM_SetIcon(bitmaps[p_bitmap],0);
	
}

void PainterSDL::set_cursor_visible(bool p_visible) {


}

void PainterSDL::set_default_cursor_bitmap(BitmapID p_bitmap,const Point& p_hotspot) {


}

void PainterSDL::set_cursor_bitmap(BitmapID p_bitmap,const Point& p_hotspot) {


}

PainterSDL::PainterSDL(SDL_Surface *p_surf) {
	
	
	surface=p_surf;
	
	for (int i=0;i<MAX_BITMAPS;i++) //anihilate bitmaps
		bitmaps[i]=0;
	
#include "default_font.inc"
	
	
	BitmapID default_font = create_bitmap(Size( _SDL_builtin_font_texture_width, _SDL_builtin_font_texture_height ), MODE_ALPHA_MASK);
	
	for(int i=0;i<_SDL_builtin_font_texture_height;i++) {
	
		for(int j=0;j<_SDL_builtin_font_texture_width;j++) {
		
			set_bitmap_pixel( default_font,Point( j,i ), Color(_SDL_builtin_font_texture[i*_SDL_builtin_font_texture_width+j]));

		}
	}
	

	GUI::FontID dfid=create_font( _SDL_builtin_font_height, _SDL_builtin_font_ascent );

	for (int i=0;i<_SDL_builtin_font_charcount;i++) {

		unsigned int character=_SDL_builtin_font_charrects[i][0];
		GUI::Rect frect;
		frect.pos.x=_SDL_builtin_font_charrects[i][1];
		frect.pos.y=_SDL_builtin_font_charrects[i][2];
		frect.size.x=_SDL_builtin_font_charrects[i][3];
		frect.size.y=_SDL_builtin_font_charrects[i][4];
		int valign=_SDL_builtin_font_charrects[i][5];

		font_add_char( dfid, character, 0, frect, valign );
	}
	
	
	
	
	set_key_repeat( SDL_DEFAULT_REPEAT_DELAY,SDL_DEFAULT_REPEAT_INTERVAL);
	
}


PainterSDL::~PainterSDL()
{
}



}
#endif
