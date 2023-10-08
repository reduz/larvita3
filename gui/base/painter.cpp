


#include "painter.h"

#include "defs.h"
#include <stdio.h>
#include <math.h>
namespace GUI {

struct PainterPrivate {


	enum {

		MAX_FONTS=32,
		MAX_LOCAL_RECT_STACK=128,
  		MAX_CLIP_RECT_STACK=128,
	};

	struct Font {

		enum {
			HASHTABLE_BITS=8,
			HASHTABLE_SIZE=(1<<HASHTABLE_BITS),
			HASHTABLE_MASK=HASHTABLE_SIZE-1
		};

		struct Character {

			unsigned int valign;
			BitmapID bitmap;
			unsigned int unicode;
			Rect rect; //rect from the bitmap
			Character *next;
			Character() { next=0; unicode=0; bitmap=0; valign=0; }
		};

		Character *characters[HASHTABLE_SIZE];

		int height;
		int ascent;
		bool in_use;

		void cleanup() {
			for (int i=0;i<HASHTABLE_SIZE;i++) {

				while(characters[i]) {
					Character *c=characters[i];
					characters[i]=c->next;
					GUI_DELETE(c);
				}
			}
			height=0;
			ascent=0;
			in_use=false;
		}

		void add_char(unsigned int p_char,BitmapID p_bitmap,const Rect& p_rect,unsigned int p_valign=0) {

			Character *c=find_char( p_char);

			if ( !c ) {

				c = GUI_NEW( Character );
				int bucket=p_char&HASHTABLE_MASK;
				c->next=characters[bucket]; //should be added at the end for speed...
				characters[bucket]=c;
			}


			c->bitmap=p_bitmap;
			c->unicode=p_char;
			c->rect=p_rect;
			c->valign=p_valign;
		}

		Font() { in_use=false; ascent=0; height=0; for (int i=0;i<HASHTABLE_SIZE;i++) characters[i]=0; }


		inline Character * find_char(unsigned int p_unicode) {


			unsigned int bucket=p_unicode&HASHTABLE_MASK;

			if (bucket>HASHTABLE_SIZE) {

				printf("MUUH\n");
			}
			Character *c=characters[bucket];

			while (c) {

				if (c->unicode==p_unicode)
					return c;
				c=c->next;
			}

			return 0; // should change this to some default
		}
	};

	Font fonts[MAX_FONTS];
	Rect clip_rect_stack[MAX_CLIP_RECT_STACK];
	int clip_rect_count;
	Rect local_rect_stack[MAX_CLIP_RECT_STACK];
	int local_rect_count;

	PainterPrivate() {
		clip_rect_count=0;
		local_rect_count=0;
	}

	~PainterPrivate() {

		for (int i=0;i<MAX_FONTS;i++)
			fonts[i].cleanup();
	}
};



void Painter::push_clip_rect(const Rect &p_rect) {

	if (p->clip_rect_count>=PainterPrivate::MAX_CLIP_RECT_STACK) {

		PRINT_ERROR("Clip Rect stack is Full!");
		return;
	}

	Rect local_rect = p->local_rect_count ? p->local_rect_stack[ p->local_rect_count -1 ] : Rect( Point() , get_display_size() );

	Rect clip_rect = p_rect;
	clip_rect.pos += local_rect.pos;
	clip_rect = local_rect.clip( clip_rect );

	if (p->clip_rect_count>0)
		clip_rect=p->clip_rect_stack[ p->clip_rect_count-1 ].clip(clip_rect);
		
	set_clip_rect(true, clip_rect);

	p->clip_rect_stack[ p->clip_rect_count++ ] = clip_rect;

}
void Painter::pop_clip_rect()  {

	if (p->clip_rect_count==0) {

		PRINT_ERROR("Clip Rect stack is Empty!");
		return;
	}

	p->clip_rect_count--;

	if ( p->clip_rect_count ) {

		Rect clip_rect = p->clip_rect_stack[ p->clip_rect_count - 1 ];
		set_clip_rect(true, clip_rect);

	} else {

		set_clip_rect(false);
	}

}

void Painter::reset_clip_rect_stack()  {

	p->clip_rect_count=0;
	set_clip_rect(false);
}

void Painter::push_local_rect(const Rect &p_rect)  {

	if (p->local_rect_count>=PainterPrivate::MAX_LOCAL_RECT_STACK) {

		PRINT_ERROR("Clip Rect stack is Full!");
		return;
	}

	Rect local_rect = p->local_rect_count ? p->local_rect_stack[ p->local_rect_count -1 ] : Rect( Point() , get_display_size() );

	Rect new_local_rect=p_rect;
	new_local_rect.pos+=local_rect.pos;


	set_local_rect( new_local_rect );

	p->local_rect_stack[ p->local_rect_count++ ] = new_local_rect;

}
void Painter::pop_local_rect()  {

	if (p->local_rect_count==0) {

		PRINT_ERROR("Local Rect stack is Empty!");
		return;
	}

	p->local_rect_count--;

	if ( p->local_rect_count ) {

		Rect local_rect = p->local_rect_stack[ p->local_rect_count - 1 ];
		set_local_rect(local_rect);

	} else {

		set_local_rect(Rect( Point() , get_display_size() ));
	}

}
void Painter::reset_local_rect_stack()  {

	p->local_rect_count=0;
	set_local_rect(Rect( Point() , get_display_size() ));
}



int Painter::get_font_string_width(FontID p_font,const String& p_string) {

	if (!is_font_valid( p_font )) {

		PRINT_ERROR("Invalid Font");
		return 0;
	}

	int width=0;

	for (int i=0;i<p_string.length();i++) {

		PainterPrivate::Font::Character * c = p->fonts[p_font].find_char(p_string[i]);

		if (!c)
			continue;

		width+=c->rect.size.width;
	}

	return width;
}



void Painter::draw_char(FontID p_font,const Point & p_pos,String::CharType &p_char,const Color&p_color) {

	if (!is_font_valid( p_font )) {

		PRINT_ERROR("Invalid Font");
		return ;
	}

	PainterPrivate::Font::Character * c = p->fonts[p_font].find_char(p_char);

	draw_bitmap( c->bitmap, p_pos + Point(0,-p->fonts[p_font].ascent+c->valign), c->rect, p_color );
}


void Painter::draw_text(FontID p_font,const Point & p_pos,const String &p_string,const Color&p_color,int p_clip_w) {

	draw_text(p_font,p_pos,p_string,RIGHT,p_color,p_clip_w);
}

void Painter::draw_text(FontID p_font,const Point & p_pos,const String &p_string,Direction p_dir,const Color&p_color,int p_clip_w) {

	if (!is_font_valid( p_font )) {

		PRINT_ERROR("Invalid Font");
		return ;
	}
	Point pos=p_pos;
	int ofs=0;
	for (int i=0;i<p_string.length();i++) {

		PainterPrivate::Font::Character * c = p->fonts[p_font].find_char(p_string[i]);

		if (!c)
			continue;
		if (p_clip_w>=0 && (ofs+c->rect.size.width)>(p_clip_w))
			break; //width exceeded

		switch (p_dir) {

			case RIGHT: {
				Point cpos=pos;
				cpos.x+=ofs;
				cpos.y-=p->fonts[p_font].ascent;
				cpos.y+=c->valign;
				draw_bitmap( c->bitmap, cpos, c->rect, p_color );
			} break;
			case DOWN: {
				Point cpos=pos;
				cpos.y+=ofs;
				cpos.x-=c->rect.size.height;
				cpos.x+=p->fonts[p_font].ascent;
				cpos.x-=c->valign;
								
				draw_bitmap( c->bitmap, cpos, c->rect, DOWN, p_color );
			} break;
			default: {

				PRINT_ERROR("Only drawing text right and down is supported as for now");
			} break;
		}
		ofs+=c->rect.size.width;


	}
}

int Painter::get_font_char_width(FontID p_font,unsigned int p_char) {

	if (!is_font_valid(p_font))
		return -1;
	PainterPrivate::Font::Character * c = p->fonts[p_font].find_char(p_char);
	if (!c)
		return 0;
	return c->rect.size.width;

}
int Painter::get_font_height(FontID p_font) {

	if (!is_font_valid( p_font )) {

		PRINT_ERROR("Invalid Font");
		return -1;
	}

	return p->fonts[p_font].height;

}
int Painter::get_font_ascent(FontID p_font) {

	if (!is_font_valid( p_font )) {

		PRINT_ERROR("Invalid Font");
		return -1;
	}

	return p->fonts[p_font].ascent;

}
int Painter::get_font_descent(FontID p_font) {

	if (!is_font_valid( p_font )) {

		PRINT_ERROR("Invalid Font");
		return -1;
	}

	return p->fonts[p_font].height-p->fonts[p_font].ascent;
}

FontID Painter::create_font(int p_height,int p_ascent)  {

	for (int i=0;i<PainterPrivate::MAX_FONTS;i++) {

		if (p->fonts[i].in_use)
			continue;

		p->fonts[i].cleanup();
		p->fonts[i].in_use=true;
		p->fonts[i].height=p_height;
		p->fonts[i].ascent=p_ascent;

		return i;
	}

	return -1;
}

bool Painter::is_font_valid(FontID p_font) {

	return (p_font>=0 && p_font <PainterPrivate::MAX_FONTS && p->fonts[p_font].in_use );
}

void Painter::erase_font_and_bitmaps(FontID p_font) {

	if (!is_font_valid( p_font ))
		return;
	for (int i=0;i<PainterPrivate::Font::HASHTABLE_SIZE;i++) {

		PainterPrivate::Font::Character *c=p->fonts[p_font].characters[i];

		while (c) {

			if (is_bitmap_valid(c->bitmap))
				remove_bitmap(  c->bitmap );

			c=c->next;
		}
	}

	p->fonts[p_font].cleanup();

}

void Painter::erase_font(FontID p_font)  {

	if (!is_font_valid( p_font ))
		return;
	p->fonts[p_font].cleanup();


}
void Painter::font_add_char(FontID p_font,unsigned int p_char,BitmapID p_bitmap,const Rect& p_rect,unsigned int p_valign) {

	if (!is_font_valid( p_font )) {
		PRINT_ERROR("Invalid font");
		return;
	}
	if (!is_bitmap_valid( p_bitmap )) {
		PRINT_ERROR("Invalid bitmap");
		return;
	}

	p->fonts[p_font].add_char(p_char,p_bitmap,p_rect,p_valign);
//	printf("adding ucode: %i\n",p_char);
}


void Painter::draw_circle( const Point& p_from, const Point& p_to, const Color& p_color ) {

	int from_y=p_from.y,to_y=p_to.y;
	int from_x=p_from.x,to_x=p_to.x;
	int aux;
	
	if (from_x>to_x) {
		aux=from_x;
		from_x=to_x;
		to_x=aux;
	}
		
	if (from_y>to_y) {
		aux=from_y;
		from_y=to_y;
		to_y=aux;
	}
		
	float y_range=(to_y-from_y);
		
	for (int i=from_x;i<=to_x;i++) {
	
		float x=(float)(i-from_x) / (float)(to_x-from_x);
		x=x*2-1.0;
		if (x<-1.0)
			x=-1.0;
		if (x>1.0)
			x=1.0;
			
		float y=sqrtf( 1.0 - x*x );
		
		float beg_y=from_y+(1.0-y)*y_range/2.0;
		float size_y=y*y_range;
		
		draw_fill_rect( Point( i, (int)beg_y), Size( 1, (int)size_y), p_color );				
	}
	
}


void Painter::draw_fill_rect(const Point & p_from,const Size & p_size,const Color& p_color,const Rect& p_clip) {

	Rect final = p_clip.clip( Rect( p_from, p_size ) );
	if (final.size.width<=0 ||final.size.height<=0 )
		return;
	draw_fill_rect( final.pos, final.size, p_color );
}


void Painter::draw_tiled_bitmap(BitmapID p_bitmap,const Rect& p_rect,const Color&p_color) {

	draw_tiled_bitmap( p_bitmap,p_rect,p_rect.pos,p_color );

}


void Painter::draw_tiled_bitmap_region(BitmapID p_bitmap,const Rect& p_region,const Rect& p_rect,const Point& p_origin,const Color&p_color) {


	if (!is_bitmap_valid(p_bitmap))
		return;

	if (p_rect.size.width<=0 || p_rect.size.height<=0)
		return;

	Size bm_size=p_region.size;

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

			Rect r=lrect;
			r.pos+=p_region.pos;
			draw_bitmap( p_bitmap, point, r,p_color );

			ofsh.x+=bm_size.width;

		}

		ofs.y+=bm_size.height;

	}

}

void Painter::draw_stylebox(const StyleBox& p_stylebox,const Point& p_pos, const Size& p_size) {

	draw_stylebox( p_stylebox, p_pos, p_size, Rect( p_pos, p_size ));
}
void Painter::draw_stylebox(const StyleBox& p_stylebox,const Point& p_pos, const Size& p_size,const Rect &p_clip) {


	switch(p_stylebox.mode) {

		case StyleBox::MODE_FLAT: {

			Rect r( p_pos, p_size );

			for (int i=0;i<p_stylebox.flat.margin;i++) {



				Color color_upleft=p_stylebox.flat.border_upleft;
				Color color_downright=p_stylebox.flat.border_downright;

				if (p_stylebox.flat.blend) {

					color_upleft.r=(p_stylebox.flat.margin-i)*(int)color_upleft.r/p_stylebox.flat.margin + i*(int)p_stylebox.flat.center.r/p_stylebox.flat.margin;
					color_upleft.g=(p_stylebox.flat.margin-i)*(int)color_upleft.g/p_stylebox.flat.margin + i*(int)p_stylebox.flat.center.g/p_stylebox.flat.margin;
					color_upleft.b=(p_stylebox.flat.margin-i)*(int)color_upleft.b/p_stylebox.flat.margin + i*(int)p_stylebox.flat.center.b/p_stylebox.flat.margin;

					color_downright.r=(p_stylebox.flat.margin-i)*(int)color_downright.r/p_stylebox.flat.margin + i*(int)p_stylebox.flat.center.r/p_stylebox.flat.margin;
					color_downright.g=(p_stylebox.flat.margin-i)*(int)color_downright.g/p_stylebox.flat.margin + i*(int)p_stylebox.flat.center.g/p_stylebox.flat.margin;
					color_downright.b=(p_stylebox.flat.margin-i)*(int)color_downright.b/p_stylebox.flat.margin + i*(int)p_stylebox.flat.center.b/p_stylebox.flat.margin;

				}

				draw_fill_rect( Point( r.pos.x,r.pos.y+r.size.y-1), Size(r.size.x ,1 ), color_downright,p_clip);
				draw_fill_rect( Point( r.pos.x+r.size.x-1,r.pos.y ), Size( 1 ,r.size.y ), color_downright,p_clip);

				draw_fill_rect( r.pos, Size(r.size.x ,1 ), color_upleft,p_clip);
				draw_fill_rect( r.pos, Size( 1 ,r.size.y ), color_upleft,p_clip);


				r.pos.x++;
				r.pos.y++;
				r.size.x-=2;
				r.size.y-=2;
			}

			if (p_stylebox.draw_center)
				draw_fill_rect( r.pos, r.size , p_stylebox.flat.center, p_clip );

		} break;
		case StyleBox::MODE_FLAT_BITMAP:
		case StyleBox::MODE_BITMAP: {

			if (!is_bitmap_valid(p_stylebox.bitmap))
				break;

			const int *m=p_stylebox.bitmap_margins;
			Point btopleft=Point( m[MARGIN_LEFT] , m[MARGIN_TOP] );
			Point bbottomright=Point( m[MARGIN_RIGHT] , m[MARGIN_BOTTOM] );

			Size bsize = get_bitmap_size( p_stylebox.bitmap );

			Rect rect_topleft=Rect( Point(), btopleft);
			Rect rect_center=Rect( btopleft, bsize-btopleft-bbottomright );
			Rect rect_bottomright=Rect( bsize-bbottomright, bbottomright );
			Rect rect_topright=Rect( Point( bsize.x-m[MARGIN_RIGHT], 0 ), Size( m[MARGIN_RIGHT], m[MARGIN_TOP] ) );
			Rect rect_bottomleft=Rect( Point( 0,bsize.y-m[MARGIN_BOTTOM]), Size( m[MARGIN_LEFT], m[MARGIN_BOTTOM] ) );
			Rect rect_left = Rect( Point( 0, m[MARGIN_TOP]), Size( m[MARGIN_LEFT], rect_center.size.y ));
			Rect rect_top = Rect( Point(m[MARGIN_LEFT],0), Size( rect_center.size.x,m[MARGIN_TOP] ));
			Rect rect_right = Rect( Point(rect_center.size.x+m[MARGIN_LEFT],m[MARGIN_TOP]), Size( m[MARGIN_RIGHT],rect_center.size.y ));
			Rect rect_bottom = Rect( Point(m[MARGIN_LEFT],rect_center.size.y+m[MARGIN_TOP]), Size( rect_center.size.x,m[MARGIN_BOTTOM] ));

			if (!rect_topleft.has_no_area()) {

				draw_bitmap(p_stylebox.bitmap,p_pos, rect_topleft);
			}
			//fixed

			if (!rect_topright.has_no_area()) {

				Point pos=p_pos;
				pos.x+=p_size.x-m[MARGIN_RIGHT];
				draw_bitmap(p_stylebox.bitmap,pos, rect_topright);

			}


			if (!rect_bottomleft.has_no_area()) {

				Point pos=p_pos;
				pos.y+=p_size.y-m[MARGIN_BOTTOM];
				draw_bitmap(p_stylebox.bitmap,pos, rect_bottomleft);

			}

			if (!rect_bottomright.has_no_area()) {

				Point pos=p_pos+p_size-Size(m[MARGIN_RIGHT],m[MARGIN_BOTTOM]);
				draw_bitmap(p_stylebox.bitmap,pos, rect_bottomright);
			}


			if (!rect_top.has_no_area()) {

				Point pos=p_pos;
				pos.x+=m[MARGIN_LEFT];

				Size size=p_size;

				size.width-=m[MARGIN_LEFT]+m[MARGIN_RIGHT];
				size.height=m[MARGIN_TOP];

				Rect dest=Rect(pos,size).clip(p_clip);

				draw_tiled_bitmap_region(p_stylebox.bitmap,rect_top,dest,pos);
			}

			if (!rect_bottom.has_no_area()) {

				Point pos=p_pos;
				pos.y+=p_size.y-m[MARGIN_BOTTOM];
				pos.x+=m[MARGIN_LEFT];
				Size size=p_size;

				size.width-=m[MARGIN_LEFT]+m[MARGIN_RIGHT];
				size.height=m[MARGIN_BOTTOM];

				Rect dest=Rect(pos,size).clip(p_clip);

				draw_tiled_bitmap_region(p_stylebox.bitmap,rect_bottom,dest,pos);
			}

			if (!rect_left.has_no_area()) {

				Point pos=p_pos;
				pos.y+=m[MARGIN_TOP];

				Size size=p_size;

				size.height-=m[MARGIN_TOP]+m[MARGIN_BOTTOM];
				size.width=m[MARGIN_LEFT];

				Rect dest=Rect(pos,size).clip(p_clip);

				draw_tiled_bitmap_region(p_stylebox.bitmap,rect_left,dest,pos);
			}

			if (!rect_right.has_no_area()) {

				Point pos=p_pos;
				pos.x+=p_size.x-m[MARGIN_RIGHT];
				pos.y+=m[MARGIN_TOP];

				Size size=p_size;

				size.height-=m[MARGIN_TOP]+m[MARGIN_BOTTOM];
				size.width=m[MARGIN_RIGHT];

				Rect dest=Rect(pos,size).clip(p_clip);

				draw_tiled_bitmap_region(p_stylebox.bitmap,rect_right,dest,pos);
			}

			if (!rect_center.has_no_area()) {

				Point pos=p_pos+rect_topleft.size;
				Size size=p_size;
				size-=rect_topleft.size+rect_bottomright.size;

				Rect dest=Rect(pos,size).clip(p_clip);

				draw_tiled_bitmap_region(p_stylebox.bitmap,rect_center,dest,pos);

			}

			/*
			//expandable
			if (p_draw_center && p_stylebox.draw_center && p_stylebox.mode==StyleBox::MODE_BITMAP && is_bitmap_valid( p_stylebox.bitmaps[StyleBox::POS_CENTER] )) {


				Point pos=p_pos+get_bitmap_size( p_stylebox.bitmaps[StyleBox::POS_TOPLEFT] );
				Size size=p_size;
				size-=get_bitmap_size( p_stylebox.bitmaps[StyleBox::POS_TOPLEFT] )+get_bitmap_size( p_stylebox.bitmaps[StyleBox::POS_BOTTOMRIGHT] );

				Rect dest=Rect(pos,size).clip(p_clip);
				draw_tiled_bitmap( p_stylebox.bitmaps[StyleBox::POS_CENTER] , dest,pos );

			}

			if (p_draw_center && p_stylebox.draw_center && p_stylebox.mode==StyleBox::MODE_FLAT_BITMAP) {


				Point pos=p_pos+get_bitmap_size( p_stylebox.bitmaps[StyleBox::POS_TOPLEFT] );
				Size size=p_size;
				size-=get_bitmap_size( p_stylebox.bitmaps[StyleBox::POS_TOPLEFT] )+get_bitmap_size( p_stylebox.bitmaps[StyleBox::POS_BOTTOMRIGHT] );


				draw_fill_rect( pos, size, p_stylebox.flat.center, p_clip );

			}

			if (is_bitmap_valid( p_stylebox.bitmaps[StyleBox::POS_LEFT] )) {


				Point pos=p_pos;
				pos.y+=get_bitmap_size( p_stylebox.bitmaps[StyleBox::POS_TOPLEFT] ).height;

				Size size=p_size;

				size.height-=get_bitmap_size( p_stylebox.bitmaps[StyleBox::POS_TOPLEFT] ).height+get_bitmap_size( p_stylebox.bitmaps[StyleBox::POS_BOTTOMRIGHT] ).height;
				size.width=get_bitmap_size( p_stylebox.bitmaps[StyleBox::POS_LEFT] ).width;

				Rect dest=Rect(pos,size).clip(p_clip);
				draw_tiled_bitmap( p_stylebox.bitmaps[StyleBox::POS_LEFT] ,dest,pos );


			}

			if (is_bitmap_valid( p_stylebox.bitmaps[StyleBox::POS_TOP] )) {


				Point pos=p_pos;
				pos.x+=get_bitmap_size( p_stylebox.bitmaps[StyleBox::POS_TOPLEFT] ).width;

				Size size=p_size;

				size.width-=get_bitmap_size( p_stylebox.bitmaps[StyleBox::POS_TOPLEFT] ).width+get_bitmap_size( p_stylebox.bitmaps[StyleBox::POS_BOTTOMRIGHT] ).width;
				size.height=get_bitmap_size( p_stylebox.bitmaps[StyleBox::POS_TOP] ).height;

				Rect dest=Rect(pos,size).clip(p_clip);
				draw_tiled_bitmap( p_stylebox.bitmaps[StyleBox::POS_TOP] ,dest,pos );

			}
			if (is_bitmap_valid( p_stylebox.bitmaps[StyleBox::POS_RIGHT] )) {


				Point pos=p_pos;
				pos.x+=p_size.x-get_bitmap_size( p_stylebox.bitmaps[StyleBox::POS_RIGHT] ).width;
				pos.y+=get_bitmap_size( p_stylebox.bitmaps[StyleBox::POS_TOPRIGHT] ).height;
				Size size=p_size;

				size.height-=get_bitmap_size( p_stylebox.bitmaps[StyleBox::POS_TOPRIGHT] ).height+get_bitmap_size( p_stylebox.bitmaps[StyleBox::POS_BOTTOMRIGHT] ).height;
				size.width=get_bitmap_size( p_stylebox.bitmaps[StyleBox::POS_RIGHT] ).width;

				Rect dest=Rect(pos,size).clip(p_clip);
				draw_tiled_bitmap( p_stylebox.bitmaps[StyleBox::POS_RIGHT] ,dest,pos );

			}
			if (is_bitmap_valid( p_stylebox.bitmaps[StyleBox::POS_BOTTOM] )) {


				Point pos=p_pos;
				pos.y+=p_size.y-get_bitmap_size( p_stylebox.bitmaps[StyleBox::POS_BOTTOM] ).height;
				pos.x+=get_bitmap_size( p_stylebox.bitmaps[StyleBox::POS_BOTTOMLEFT] ).width;
				Size size=p_size;

				size.width-=get_bitmap_size( p_stylebox.bitmaps[StyleBox::POS_BOTTOMRIGHT] ).width+get_bitmap_size( p_stylebox.bitmaps[StyleBox::POS_BOTTOMLEFT] ).width;
				size.height=get_bitmap_size( p_stylebox.bitmaps[StyleBox::POS_BOTTOM] ).height;

				Rect dest=Rect(pos,size).clip(p_clip);
				draw_tiled_bitmap( p_stylebox.bitmaps[StyleBox::POS_BOTTOM] ,dest,pos );

			}

			*/
		} break;

		default: {}
	}


}

Point Painter::get_stylebox_offset(const StyleBox& p_stylebox) {

	return Point( 
		get_stylebox_margin( p_stylebox, MARGIN_LEFT ), 
		get_stylebox_margin( p_stylebox, MARGIN_TOP ) );
}

int Painter::get_stylebox_margin(const StyleBox& p_stylebox,const Margin& p_margin) {


	if (p_stylebox.margins[p_margin]>=0)
		return p_stylebox.margins[p_margin];
	else {

		int res=0;

		switch(p_stylebox.mode) {

			case StyleBox::MODE_FLAT: {

				res=p_stylebox.flat.margin;
			} break;
			case StyleBox::MODE_FLAT_BITMAP:
			case StyleBox::MODE_BITMAP: {
				res=p_stylebox.bitmap_margins[p_margin];
			} break;
			default: res=0;

		}

		if (res<0)
			res=0;

		return res;
	}

}


Size Painter::get_stylebox_min_size(const StyleBox& p_stylebox,bool p_with_center) {

	Size min=Size( 	get_stylebox_margin( p_stylebox, MARGIN_LEFT) +
			get_stylebox_margin( p_stylebox, MARGIN_RIGHT),
			get_stylebox_margin( p_stylebox, MARGIN_TOP) +
			get_stylebox_margin( p_stylebox, MARGIN_BOTTOM ) );

	if (p_stylebox.mode==StyleBox::MODE_BITMAP && p_with_center) {

		Size center_size=get_bitmap_size( p_stylebox.bitmap);
		center_size-=Point( p_stylebox.bitmap_margins[MARGIN_LEFT]+p_stylebox.bitmap_margins[MARGIN_RIGHT],p_stylebox.bitmap_margins[MARGIN_TOP]+p_stylebox.bitmap_margins[MARGIN_BOTTOM]);
		min+=center_size;
	}


	return min;
}

struct __Painter__ColorMap {

    String chr;
    Color color;
    unsigned char alpha;
    __Painter__ColorMap() { alpha=255; }
};

BitmapID Painter::load_xpm(const char **p_xpm) {

	Size size;
	int pixelchars=0;
	bool has_alpha=false;
	BitmapID bitmap=-1;

	enum Status {
		READING_HEADER,
  		READING_COLORS,
    		READING_PIXELS,
      		DONE
	};

	Status status = READING_HEADER;
	int line=0;


	__Painter__ColorMap * colormap;
	int colormap_size;

	while (status!=DONE) {

		const char * line_ptr = p_xpm[line];


		switch (status) {

			case READING_HEADER: {

				String line_str=line_ptr;
				line_str.replace("\t"," ");

				size.width=line_str.get_slice(" ",0).to_int();
				size.height=line_str.get_slice(" ",1).to_int();
				colormap_size=line_str.get_slice(" ",2).to_int();
				pixelchars=line_str.get_slice(" ",3).to_int();
				if (colormap_size > 32766) {
					PRINT_ERROR("XPM data wth colors > 32766 not supported");
					return -1;
				}
				if (pixelchars > 5) {
					PRINT_ERROR("XPM data with characters per pixel > 5 ");
					return -1;
				}
				if (size.width > 32767) {
					PRINT_ERROR("Image width > 32767 pixels for data\n");
					return -1;
				}
				if (size.height > 32767) {
					PRINT_ERROR("Image height > 32767 pixels for data\n");
					return -1;
				}
				colormap = GUI_NEW_ARRAY( __Painter__ColorMap, colormap_size );

				if (!colormap) {
					return -1;
				}
				status=READING_COLORS;
			} break;
			case READING_COLORS: {

				for (int i=0;i<pixelchars;i++) {

					colormap[line-1].chr+=*line_ptr;
					line_ptr++;
				}
				//skip spaces
				while (*line_ptr==' ' ||  *line_ptr=='\t' || *line_ptr==0) {
					if (*line_ptr==0)
						break;
					line_ptr++;
				}
				if (*line_ptr=='c') {

					line_ptr++;
					while (*line_ptr==' ' ||  *line_ptr=='\t' || *line_ptr==0) {
						if (*line_ptr==0)
							break;
						line_ptr++;
					}

					if (*line_ptr=='#') {
						line_ptr++;
						Color col;
						for (int i=0;i<6;i++) {

							char v = line_ptr[i];

							if (v>='0' && v<='9')
								v-='0';
							else if (v>='A' && v<='F')
								v=(v-'A')+10;
							else if (v>='a' && v<='f')
								v=(v-'a')+10;
							else
								break;

							switch(i) {
								case 0: col.r=v<<4; break;
								case 1: col.r|=v; break;
								case 2: col.g=v<<4; break;
								case 3: col.g|=v; break;
								case 4: col.b=v<<4; break;
								case 5: col.b|=v; break;
							};

							// magenta mask
							if (col.r==255 && col.g==0 && col.b==255) {

								colormap[line-1].color=Color(0);
								colormap[line-1].alpha=0;
								has_alpha=true;
							} else {

								colormap[line-1].color=col;
							}
						}
					}
				}
				if (line==colormap_size) {

					status=READING_PIXELS;
					bitmap=create_bitmap(size,MODE_PIXMAP,has_alpha);
					if (bitmap<0) {

						GUI_DELETE_ARRAY( colormap );
						return bitmap;
					}
				}
			} break;
			case READING_PIXELS: {

				int y=line-colormap_size-1;
				for (int x=0;x<size.width;x++) {

					char pixelstr[6]={0,0,0,0,0,0};
					for (int i=0;i<pixelchars;i++)
						pixelstr[i]=line_ptr[x*pixelchars+i];

					Color pixel; unsigned char alpha=255;
					for (int i=0;i<colormap_size;i++) {

						if (colormap[i].chr==pixelstr) {

							pixel=colormap[i].color;
							alpha=colormap[i].alpha;
							break;
						}
					}

					 set_bitmap_pixel(bitmap,Point(x,y), pixel, alpha );

				}

				if (y==(size.height-1))
					status=DONE;
			} break;
		}

		line++;
	};

	GUI_DELETE_ARRAY(colormap);

	return bitmap;
}

void Painter::draw_arrow( const Point& p_pos, const Size& p_size, Direction p_dir, const Color& p_color,bool p_trianglify) {

	if (p_size.width<=0 || p_size.height<=0)
		return;

	Point pos=p_pos;
	Size size=p_size;

	if (p_trianglify) {

		if (p_dir==UP || p_dir==DOWN ) {

			int desired_height=(p_size.width+1)/2;


			if (desired_height < p_size.height ) {
				//good, let's do it

				pos.y+=(p_size.height-desired_height)/2;
				size.height=desired_height;

			}
		}

		if (p_dir==LEFT || p_dir==RIGHT ) {

			int desired_width=(p_size.height+1)/2;


			if (desired_width < p_size.width ) {
				//good, let's do it

				pos.x+=(p_size.width-desired_width)/2;
				size.width=desired_width;

			}
		}
	}
	size.height &= ~1;
	size.width &= ~1;
	for (int i=0;i<size.height;i++) {

		int begin,end;
		switch (p_dir) {

			case UP:
			case DOWN: {

				int idx=i;

				if (p_dir==UP) {

					idx=size.height-i-1;
				}

				int ofsm=idx*(size.width/2)/size.height;
				begin=ofsm;
				end=size.width-ofsm-1;


			} break;
			case RIGHT:
			case LEFT: {

				int ofs=(i%(size.height/2))*(size.width)/(size.height/2);
				end=size.width-1;
				begin=(i>=(size.height/2))?ofs:(size.width-ofs);

				if (p_dir==RIGHT) {

					int auxbeg=size.width-end;
					end=size.width-begin;
					begin=auxbeg;
				}

			} break;

		};

		Point from( pos.x + begin , pos.y + i );
		Size rsize( end-begin , 1 );
		draw_fill_rect( from, rsize, p_color );
	}
}

Painter::Painter() {

	p = GUI_NEW( PainterPrivate );
}
Painter::~Painter() {

	GUI_DELETE( p );
}

};
