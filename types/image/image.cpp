//
// C++ Implementation: image
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "image.h"



int Image::get_width() const {

	return width;
}
int Image::get_height() const{

	return height;
}
Color Image::get_pixel(int p_x,int p_y) const{

	ERR_FAIL_INDEX_V(p_x,width,Color());
	ERR_FAIL_INDEX_V(p_y,height,Color());

	Color result;
	
	data.read_lock();
	const unsigned char*data_ptr=data.read();
	int ofs=p_y*width+p_x;
	
	switch(format) {
		
		case FORMAT_GRAYSCALE_ALPHA: {
		
			result=Color(data_ptr[ofs*2],data_ptr[ofs*2+1]);		
		
		} break;
		case FORMAT_RGB: {
		
			result=Color(data_ptr[ofs*3],data_ptr[ofs*3+1],data_ptr[ofs*3+2]);		
		
		} break;
		case FORMAT_RGBA: {
		
			result=Color(data_ptr[ofs*4],data_ptr[ofs*4+1],data_ptr[ofs*4+2],data_ptr[ofs*4+3]);		
		} break;
		case FORMAT_GRAYSCALE:
		case FORMAT_INDEXED_ALPHA:
		case FORMAT_INDEXED: {
		
			result=Color(data_ptr[ofs]);		
		} break;
		
	}
	
	data.read_unlock();
	
	return result;

}

void Image::convert( Format p_new_format ){

	Image copy(get_width(), get_height(), p_new_format);
	for (int x=0; x<get_width(); x++) {
		for (int y=0; y<get_height(); y++) {
			copy.put_pixel(x, y, get_pixel(x, y));
		};
	};
	*this = copy;
}

Image::Format Image::get_format() const{

	return format;
}

void Image::resize( int p_width, int p_height, Interpolation p_interpolation ) {

	ERR_FAIL_COND(p_width<=0);
	ERR_FAIL_COND(p_height<=0);
	ERR_FAIL_COND(p_width>MAX_WIDTH);
	ERR_FAIL_COND(p_height>MAX_HEIGHT);
	
	/* to save memory, resizing should be done in-place, however, since this function
	   will most likely either not be used much, or in critical areas, for now it wont, because
	   it's a waste of time. */

	if (p_width==width && p_height==height)
		return;
		
	Image dst( p_width, p_height, format );
	 
	if (format==FORMAT_INDEXED)
		p_interpolation=INTERPOLATE_NEAREST;
	 
	for (int y=0;y<p_height;y++) {
	
		for (int x=0;x<p_width;x++) {
	
			
			switch(p_interpolation) {
				case INTERPOLATE_NEAREST: {
				
					dst.put_pixel( x, y, get_pixel( x * width / p_width , y * height / p_height ) );
				
				} break;
				case INTERPOLATE_BILINEAR: {
				
					float x_src=(float)x * (float)width / (float)p_width;
					float y_src=(float)y * (float)height / (float)p_height;
					int x_src_i=(int)x_src;
					int y_src_i=(int)y_src;
					float x_src_f=x_src-(float)x_src_i;
					float y_src_f=y_src-(float)y_src_i;
					
					Color current=get_pixel( x_src_i, y_src_i ); 
					Color next_x=get_pixel( (x_src_i < (width-1)) ? x_src_i+1 : x_src_i, y_src_i  ); 
					Color next_y=get_pixel( x_src_i, (y_src_i < (height-1)) ? y_src_i+1 : y_src_i  ); 
					
					/* wathever the format is, will work */
					
#define _INTERP(m_res,m_src,m_dst,m_coeff)\
	float m_res = (float)m_src + ((float)m_dst-(float)m_src)*m_coeff;
#define _RESULT(m_res,m_src,m_dst)\
	float m_res = (m_src+m_dst)/2.0;\
	if (m_res<0)\
		m_res=0;\
	if (m_res>255)\
		m_res=255;
	
					_INTERP(x_r,current.r,next_x.r,x_src_f);
					_INTERP(x_g,current.g,next_x.g,x_src_f);
					_INTERP(x_b,current.b,next_x.b,x_src_f);
					_INTERP(x_a,current.a,next_x.a,x_src_f);
				
					_INTERP(y_r,current.r,next_y.r,y_src_f);
					_INTERP(y_g,current.g,next_y.g,y_src_f);
					_INTERP(y_b,current.b,next_y.b,y_src_f);
					_INTERP(y_a,current.a,next_y.a,y_src_f);
					
					_RESULT(r,x_r,y_r);
					_RESULT(g,x_g,y_g);
					_RESULT(b,x_b,y_b);
					_RESULT(a,x_a,y_a);
					
					Color result((Uint8)r,(Uint8)g,(Uint8)b,(Uint8)a);
					
					dst.put_pixel( x,y,result);				
					
				} break;
			}
		}
	}
	
	*this=dst;
}
void Image::crop( int p_width, int p_height ) {

	ERR_FAIL_COND(p_width<=0);
	ERR_FAIL_COND(p_height<=0);
	ERR_FAIL_COND(p_width>MAX_WIDTH);
	ERR_FAIL_COND(p_height>MAX_HEIGHT);
	
	/* to save memory, cropping should be done in-place, however, since this function
	   will most likely either not be used much, or in critical areas, for now it wont, because
	   it's a waste of time. */

	if (p_width==width && p_height==height)
		return;
		
	Image dst( p_width, p_height, format );
	
	for (int y=0;y<p_height;y++) {
	
		for (int x=0;x<p_width;x++) {

			Color col = (x>=width || y>=height)? Color() : get_pixel(x,y);
			dst.put_pixel(x,y,col);
		}
	}
	
	*this=dst;
	
}

void Image::flip_h() {
	
	for (int y=0;y<(height/2);y++) {
	
		for (int x=0;x<width;x++) {

			Color up = get_pixel(x,y);
			Color down = get_pixel(x,height-y-1);
			
			put_pixel(x,y,down);
			put_pixel(x,height-y-1,up);
		}
	}
}

void Image::make_mipmap( Image& p_mipmap ) const {

	ERR_FAIL_COND( height<=1 );
	ERR_FAIL_COND( width<=1 );
	
	p_mipmap.create( width / 2, height / 2 , format );
	
	/* could also be faster... but, i care for it working for now */
	
	for (int y=0;y<(height/2);y++) {
	
		for (int x=0;x<(width/2);x++) {

			Uint16 ravg=0;
			Uint16 gavg=0;
			Uint16 bavg=0;
			Uint16 aavg=0;
			
			for (int i=0;i<4;i++) {
			
				Color c=get_pixel( x*2 + (i&1) , y*2 + (i>>1) );
				ravg+=c.r;
				gavg+=c.g;
				bavg+=c.b;
				aavg+=c.a;
			}
			
			ravg>>=2;
			gavg>>=2;
			bavg>>=2;
			aavg>>=2;
			
			p_mipmap.put_pixel( x, y, Color(ravg,gavg,bavg,aavg));
		}
	}

}

void Image::make_normal_map(float p_height_scale) {

	ERR_FAIL_COND( empty() );
	
	Image normalmap(width,height,FORMAT_RGB);
	/*
	for (int y=0;y<height;y++) {
		for (int x=0;x<width;x++) {
		
			float center=get_pixel(x,y).gray()/255.0;
			float up=(y>0)?get_pixel(x,y-1).gray()/255.0:center;
			float down=(y<(height-1))?get_pixel(x,y+1).gray()/255.0:center;
			float left=(x>0)?get_pixel(x-1,y).gray()/255.0:center;
			float right=(x<(width-1))?get_pixel(x+1,y).gray()/255.0:center;
		
		
			// uhm, how do i do this? ....
			
			Color result( (Uint8)((normal.x+1.0)*127.0), (Uint8)((normal.y+1.0)*127.0), (Uint8)((normal.z+1.0)*127.0) );
			
			normalmap.put_pixel( x, y, result );
		}
		
	}
	*/
	*this=normalmap;
}

bool Image::empty() const {

	return (data.size()==0);
}

DVector<Uint8> Image::get_data() const {
	
	return data;
}
Image::Image() {

	width=0;
	height=0;
}

void Image::create(int p_width, int p_height, Format p_format) {


	int csize=0; //component size
	int extra=0;
	
	switch(p_format) {
	
		case FORMAT_GRAYSCALE: csize = 1; break;
		case FORMAT_GRAYSCALE_ALPHA: csize = 2; break;
		case FORMAT_RGB: csize = 3; break;
		case FORMAT_RGBA: csize = 4; break;
		case FORMAT_INDEXED: csize = 1; extra=256*3; break;
		case FORMAT_INDEXED_ALPHA: csize = 1; extra=256*4;break;
	}
	
	data.resize( csize * p_width * p_height + extra);
	width=p_width;
	height=p_height;
	format=p_format;

}

void Image::create(int p_width, int p_height, Format p_format, const DVector<Uint8>& p_data) {
	
	width=0;
	height=0;
	format=FORMAT_GRAYSCALE;
	ERR_FAIL_INDEX(p_width-1,MAX_WIDTH);
	ERR_FAIL_INDEX(p_height-1,MAX_HEIGHT);
	
	height=p_height;
	width=p_width;
	format=p_format;
	data=p_data;	
}

Image::Image(int p_width, int p_height, Format p_format) {

	create(p_width,p_height,p_format);

}

Image::Image(int p_width, int p_height, Format p_format, const DVector<Uint8>& p_data) {

	create(p_width,p_height,p_format,p_data);

}



Image::~Image() {

}


