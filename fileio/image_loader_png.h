//
// C++ Interface: image_loader_png
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef IMAGE_LOADER_PNG_H
#define IMAGE_LOADER_PNG_H

#include "fileio/image_loader.h"
#include "libpng/png.h"

/**
	@author Juan Linietsky <reduzio@gmail.com>
*/
class ImageLoader_PNG : public ImageFormatLoader {

	static void _read_png_data(png_structp png_ptr,png_bytep data, png_size_t p_length);

	virtual Error load_image(String p_file,Image *p_image,FileAccess *f);

public:
	ImageLoader_PNG();
	~ImageLoader_PNG();

};

#endif
