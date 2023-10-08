//
// C++ Implementation: png_mem_compress
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "png_mem_compress.h"
#include "libpng/png.h"

static void _PNG_user_write_flush(png_structp png_ptr) {
	
}

 static void _PNG_user_write_data(png_structp png_ptr, png_bytep data, png_size_t length) {
	
	DVector<Uint8> * u8vecptr= (DVector<Uint8>*)png_ptr->io_ptr;
	
	int from = u8vecptr->size();
	
	u8vecptr->resize( from + length );
	
	u8vecptr->write_lock();
	Uint8 *u8vec = u8vecptr->write();
	
	for (int i=0;i<(int)length;i++)
		u8vec[from+i]=data[i];
	
	u8vecptr->write_unlock();
	
 }

#define abort_(m_err) {\
	ERR_PRINT(m_err);\
	ERR_FAIL_V( DVector<Uint8>() ) }
	
DVector<Uint8> PNG_MemCompress::compress( const Image& p_image ) {
	
	/* initialize stuff */
	png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	
	
	DVector<Uint8> dv;	
	
	if (!png_ptr)
		abort_("[write_png_file] png_create_write_struct failed");

	png_infop info_ptr = png_create_info_struct(png_ptr);
	
	if (!info_ptr)
		abort_("[write_png_file] png_create_info_struct failed");

	if (setjmp(png_jmpbuf(png_ptr)))
		abort_("[write_png_file] Error during init_io");

	png_set_write_fn( png_ptr, (void*)&dv, _PNG_user_write_data, _PNG_user_write_flush );
	

	/* write header */
	//if (setjmp(png_jmpbuf(png_ptr)))
	//	abort_("[write_png_file] Error during writing header");

	Uint32 format;
	int components=1;
	switch(p_image.get_format()) {
	
		
		case Image::FORMAT_GRAYSCALE: {
		
			format=PNG_COLOR_TYPE_GRAY;
		} break;
		case Image::FORMAT_GRAYSCALE_ALPHA: {
		
			format=PNG_COLOR_TYPE_GRAY_ALPHA;
			components=2;
		} break;
		case Image::FORMAT_INDEXED: {
		
			ERR_FAIL_V( DVector<Uint8>() );
		} break;
		case Image::FORMAT_RGB: {
		
			format=PNG_COLOR_TYPE_RGB;
			components=3;
		} break;
		case Image::FORMAT_RGBA: {
		
			format=PNG_COLOR_TYPE_RGB_ALPHA;
			components=4;

		} break;
		default: {
		
			ERR_FAIL_V( DVector<Uint8>() );
		} break;
	}
	
	png_set_IHDR(png_ptr, info_ptr, p_image.get_width(), p_image.get_height(),
		     8, format, PNG_INTERLACE_NONE,
       PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
	
	 png_write_info(png_ptr, info_ptr);
		
	const Uint8 **row_ptr = memnew_arr( const Uint8* , p_image.get_height() );

	DVector<Uint8> img = p_image.get_data();
	img.read_lock();
	const Uint8 * imgdata=img.read();
	
	for (int i=0;i<p_image.get_height();i++) {
		
		row_ptr[i]=&imgdata[ i* p_image.get_width() * components ];
	}
	
	png_write_image(png_ptr, (png_bytep*)row_ptr);
	png_write_end(png_ptr, info_ptr);

	
		
	memdelete_arr(row_ptr);	
	png_destroy_write_struct(&png_ptr,&info_ptr);

	return dv;
}	
	
	
struct _PNG_ReadStr {
		
	const DVector<Uint8> * u8vecptr;
	unsigned int ofs;
	
	_PNG_ReadStr() { ofs=0; }
};
	
static void _PNG_user_read_data(png_structp png_ptr, png_bytep data, png_size_t length) {

	_PNG_ReadStr * _readstr= (_PNG_ReadStr*)png_ptr->io_ptr;
	
	int len = _readstr->u8vecptr->size();
	
	ERR_FAIL_COND( (_readstr->ofs + length) > len );
	
	_readstr->u8vecptr->read_lock();
			
	const Uint8* u8=_readstr->u8vecptr->read();
	
	for (unsigned int i=0;i<length;i++) {
		
		data[i]=u8[_readstr->ofs+i];
	}
	
	_readstr->ofs+=length;
	_readstr->u8vecptr->read_unlock();

}

Image PNG_MemCompress::uncompress( const DVector<Uint8>& p_data) {
	
	png_structp png;
	png_infop info;

	png = png_create_read_struct(PNG_LIBPNG_VER_STRING, (png_voidp)NULL, NULL, NULL);
	ERR_FAIL_COND_V(!png, Image(););

	info = png_create_info_struct(png);
	if (!info) {
		png_destroy_read_struct(&png,NULL,NULL);

		return Image();;
	}

	_PNG_ReadStr read_str;
	read_str.u8vecptr=&p_data;
	
	png_set_read_fn(png,&read_str,_PNG_user_read_data);

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
		return Image();;
	}

	Image::Format fmt; 
	int components=1;
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
		
			ERR_PRINT("INVALID PNG TYPE");
			png_destroy_read_struct(&png, &info, NULL);
			return Image();;
		} break;
	}

	int rowsize = png_get_rowbytes(png, info);

	DVector<Uint8> dstbuff;
	
	dstbuff.resize( rowsize * height );
		
	dstbuff.write_lock();
	
	Uint8* data = dstbuff.write();
	
	Uint8 **row_p = memnew_arr( Uint8*, height );
	
	for (unsigned int i = 0; i < height; i++) {
		row_p[i] = &data[width*i*components];
	}

	png_read_image(png, (png_bytep*)row_p);
	
	memdelete_arr( row_p );
	
	dstbuff.write_unlock();

	Image image( width, height, fmt, dstbuff );
	
	png_destroy_read_struct(&png, &info, NULL );
	
	return image;

}
	
