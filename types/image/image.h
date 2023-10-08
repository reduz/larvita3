//
// C++ Interface: image
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef IMAGE_H
#define IMAGE_H

#include "dvector.h"

/**
 *	@author Juan Linietsky <reduzio@gmail.com>
 *
 * Image storage class. This is used to store an image in user memory, as well as
 * providing some basic methods for image manipulation.
 * Images can be loaded from a file, or registered into the Render object as textures.
*/

SCRIPT_BIND_BEGIN;

class Image {

	enum { 
		MAX_WIDTH=4096, // force a limit somehow
		MAX_HEIGHT=4096 // force a limit somehow
	};
public:

	enum Format {
		FORMAT_GRAYSCALE, ///< one byte per pixel, 0-255 
		FORMAT_GRAYSCALE_ALPHA, ///< two bytes per pixel, 0-255. alpha 0-255 
		FORMAT_RGB, ///< one byte R, one byte G, one byte B 
		FORMAT_RGBA, ///< one byte R, one byte G, one byte B, one byte A 
		FORMAT_INDEXED, ///< index byte 0-256, and after image end, 256*3 bytes of palette 
		FORMAT_INDEXED_ALPHA ///< index byte 0-256, and after image end, 256*4 bytes of palette (alpha) 
		
	};

	enum Interpolation {
	
		INTERPOLATE_NEAREST,
		INTERPOLATE_BILINEAR,
		/* INTERPOLATE GAUSS */
	};

private:
	Format format;
	DVector<Uint8> data;
	int width,height;
	
public:


	int get_width() const; ///< Get image width
	int get_height() const; ///< Get image height
	
	/**
	 * Get a pixel from the image. for grayscale or indexed formats, use Color::gray to obtain the actual
	 * value.
	 */
	Color get_pixel(int p_x,int p_y) const;  
	/**
	 * Set a pixel into the image. for grayscale or indexed formats, a suitable Color constructor.
	 */
	inline void put_pixel(int p_x,int p_y, const Color& p_color); /* alpha and index are averaged */
	
	/**
	 * Convert the image to another format, as close as it can be done.
	 */
	void convert( Format p_new_format );
	
	/**
	 * Get the current image format.
	 */
	Format get_format() const;
	
	/**
	 * Resize the image, using the prefered interpolation method.
	 * Indexed-Color images always use INTERPOLATE_NEAREST.
	 */ 
	void resize( int p_width, int p_height, Interpolation p_interpolation=INTERPOLATE_BILINEAR );
	/**
	 * Crop the image to a specific size, if larger, then the image is filled by black
	 */
	void crop( int p_width, int p_height );
	
	
	void flip_h();
	/**
	 * Generate a mipmap to an image (creates an image 1/4 the size, with averaging of 4->1) 
	 */
	void make_mipmap( Image& p_mipmap ) const; 
	
	/**
	 * Generate a normal map from a grayscale image
	 */
	 
	void make_normal_map(float p_height_scale=1.0);

	/**
	 * Create a new image of a given size and format. Current image will be lost
	 */
	void create(int p_width, int p_hight, Format p_format);
	void create(int p_width, int p_hight, Format p_format, const DVector<Uint8>& p_data); 
	
	/**
	 * returns true when the image is empty (0,0) in size
	 */
	bool empty() const;
	
	DVector<Uint8> get_data() const;
	
	/** 
	 * create an empty image
	 */
	Image();
	/** 
	 * create an empty image of a specific size and format
	 */
	Image(int p_width, int p_hight, Format p_format); 
	/** 
	 * import an image of a specific size and format from a pointer
	 */
	Image(int p_width, int p_hight, Format p_format, const DVector<Uint8>& p_data); 
	
	~Image();

};

SCRIPT_BIND_END;
		
inline void Image::put_pixel(int p_x,int p_y, const Color& p_color){

	ERR_FAIL_INDEX(p_x,width);
	ERR_FAIL_INDEX(p_y,height);
		
	data.write_lock();
	unsigned char *data_ptr=data.write();
	
	int ofs=p_y*width+p_x;
	
	switch(format) {
		case FORMAT_GRAYSCALE: {
		
			data_ptr[ofs]=p_color.gray();		
		} break;
		case FORMAT_GRAYSCALE_ALPHA: {
		
			data_ptr[ofs*2]=p_color.gray();		
			data_ptr[ofs*2+1]=p_color.a;
		
		} break;
		case FORMAT_RGB: {
		
			data_ptr[ofs*3+0]=p_color.r;
			data_ptr[ofs*3+1]=p_color.g;
			data_ptr[ofs*3+2]=p_color.b;
		
		} break;
		case FORMAT_RGBA: {
		
			data_ptr[ofs*4+0]=p_color.r;
			data_ptr[ofs*4+1]=p_color.g;
			data_ptr[ofs*4+2]=p_color.b;
			data_ptr[ofs*4+3]=p_color.a;
			
		} break;
		case FORMAT_INDEXED:
		case FORMAT_INDEXED_ALPHA: {
		
			data_ptr[ofs]=p_color.gray();		
		} break;
		
	}

	data.write_unlock();
}

#endif
