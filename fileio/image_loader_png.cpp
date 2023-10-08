//
// C++ Implementation: image_loader_png
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "image_loader_png.h"

void ImageLoader_PNG::_read_png_data(png_structp png_ptr,png_bytep data, png_size_t p_length) {

	FileAccess *f = (FileAccess*)png_ptr->io_ptr;
	f->get_buffer( (Uint8*)data, p_length );

}


Error ImageLoader_PNG::load_image(String p_file,Image *p_image,FileAccess *f) {


	Error err = f->open(p_file, FileAccess::READ);
	if (err != OK) {
		ERR_PRINT((String("loading image ")+ p_file).ascii().get_data());
		ERR_FAIL_COND_V(err,err);
	};
	png_structp png;
	png_infop info;

	png = png_create_read_struct(PNG_LIBPNG_VER_STRING, (png_voidp)NULL, NULL, NULL);
	ERR_FAIL_COND_V(!png, ERR_OUT_OF_MEMORY);

	info = png_create_info_struct(png);
	if (!info) {
		png_destroy_read_struct(&png,NULL,NULL);
		f->close();
		ERR_PRINT("Out of Memory");
		return ERR_OUT_OF_MEMORY;
	}


	png_set_read_fn(png,(void*)f,_read_png_data);

	// Read png header into struct
	png_uint_32 width, height;
	int depth, color;


	png_read_info(png, info);
	png_get_IHDR(png, info, &width, &height, &depth, &color, NULL, NULL, NULL);

	/*
	printf("Image width:%i\n", width);
	printf("Image Height:%i\n", height);
	printf("Bit depth:%i\n", depth);
	printf("Color type:%i\n", color);
	*/

	if (depth!=8) { //only bit dept 8 per channel is handled
	
		png_destroy_read_struct(&png,NULL,NULL);
		f->close();
		ERR_PRINT("Unavailable Depth");
		return ERR_UNAVAILABLE;
	}

	Image::Format fmt; 
	int components;
	switch(color) {
	
		
		case PNG_COLOR_TYPE_GRAY: {
		
			fmt=Image::FORMAT_GRAYSCALE;
			components=1;
		} break;
		case PNG_COLOR_TYPE_GRAY_ALPHA: {
		
			fmt=Image::FORMAT_GRAYSCALE_ALPHA;
			components=2;
		} break;
		case PNG_COLOR_TYPE_PALETTE: {
		
			fmt=Image::FORMAT_INDEXED;
			components=1;
		} break;
		case PNG_COLOR_TYPE_RGB: {
		
			fmt=Image::FORMAT_RGB;
			components=3;
		} break;
		case PNG_COLOR_TYPE_RGB_ALPHA: {
		
			fmt=Image::FORMAT_RGBA;
			components=4;
		} break;
		default: {
		
			f->close();
			ERR_PRINT("INVALID PNG TYPE");
			png_destroy_read_struct(&png, &info, NULL);
			return ERR_UNAVAILABLE;
		} break;
	}

	int rowsize = png_get_rowbytes(png, info);

	DVector<Uint8> dstbuff;
	
	dstbuff.resize( rowsize * height );
		
	dstbuff.write_lock();
	
	Uint8* data = dstbuff.write();
	
	Uint8 **row_p = memnew_arr( Uint8*, height );
	
	for (unsigned int i = 0; i < height; i++) {
		row_p[i] = &data[components*width*i];
	}

	png_read_image(png, (png_bytep*)row_p);
	
	memdelete_arr( row_p );
	
	dstbuff.write_unlock();
	
	p_image->create( width, height, fmt, dstbuff );
	
	f->close();
	png_destroy_read_struct(&png, &info, NULL );



	return OK;
}


ImageLoader_PNG::ImageLoader_PNG()
{
}


ImageLoader_PNG::~ImageLoader_PNG()
{
}


