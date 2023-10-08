//
// C++ Interface: image_loader
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef IMAGE_LOADER_H
#define IMAGE_LOADER_H

#include "image.h"
#include "rstring.h"
#include "os/file_access.h"

/**
	@author Juan Linietsky <reduzio@gmail.com>
*/


/**
 * @class ImageScanLineLoader
 * @author Juan Linietsky <reduzio@gmail.com>
 * ScanLine loader for images.
 * Image loaders can load images scanline by scanline, to save on memory. proper usage is:
 *
 * ImageScanLineLoader * loader = ImageLoader::open( "someimage.png" );
 * // if loader is null, then image couldn't open.
 * if (!loader) return someerror();
 * int line;
 * Image img;
 *
 * while( loader->get_scanline( &img, &line ) ) {
 *	// "img" containing scanline number "line" was fetched
 *	// lines can arrive in any order, since many images are stored interleaced, for example.
 * }
 * 
 * memdelete( loader ); // delete when done using it
 
 */
class ImageLoader;
 
class ImageScanLineLoader {
friend class ImageLoader;
	ImageLoader * owner;
public:	
	virtual int get_width()=0;
	virtual int get_height()=0;
	virtual Image::Format get_format()=0;
	virtual bool get_scanline(Image *p_scanline,int *p_y)=0;


	virtual ~ImageScanLineLoader() {}
};


/**
 * @class ImageLoader
 * Base Class and singleton for loading images from disk
 * Can load images in one go, or by scanline
 */


class ImageFormatLoader {
friend class ImageLoader;
protected:
	virtual Error load_image(String p_file,Image *p_image,FileAccess *p_fileaccess)=0; ///< Load image in one go
	virtual ImageScanLineLoader * open_image(String p_dst,FileAccess *p_fileaccess); ///< Open image using scanline loader. A default implementation using _load_image internally is provided for testing or unability of using it, but it's not recommended, because it wastes memory.
public:
	virtual ~ImageFormatLoader() {}
};

class ImageLoader {

	enum {
		MAX_LOADERS=8
	};

	static ImageFormatLoader *loader[MAX_LOADERS];
	static int loader_count;

protected:


public:
	
	static Error load_image(String p_file,Image *p_image, FileAccess *p_custom=NULL);
	static ImageScanLineLoader* open_image(String p_image, FileAccess *p_custom=NULL);

	static void add_image_format_loader(ImageFormatLoader *p_loader);

	ImageLoader();
	virtual ~ImageLoader();

};

#endif
