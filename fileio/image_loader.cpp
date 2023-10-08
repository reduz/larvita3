//
// C++ Implementation: image_loader
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "image_loader.h"

#include "types/global_vars.h"

ImageScanLineLoader* ImageFormatLoader::open_image(String p_image, FileAccess *p_custom) {

	return NULL;
}


ImageScanLineLoader* ImageLoader::open_image(String p_image, FileAccess *p_custom) {
	
	return NULL;
};

Error ImageLoader::load_image(String p_file,Image *p_image, FileAccess *p_custom) {

	FileAccess *f=p_custom;
	if (!f)
		f=FileAccess::create();
		
	/* shouldn't be here
	if (p_file[0] != '/' && (p_file.size() > 1 && p_file[1] != ':')) {
	
		p_file = GlobalVars::get_singleton()->get("resource_path").get_string() + "/" + p_file;
		printf("file is now %ls\n", p_file.c_str());
	}; */
		
	for (int i=0;i<loader_count;i++) {
	
		Error err = loader[i]->load_image(p_file,p_image,f);
		if (err!=ERR_FILE_UNRECOGNIZED)
			return err;
			
		
	}
	
	if (!p_custom)
		memdelete(f);
		
	return ERR_FILE_UNRECOGNIZED;

}

ImageFormatLoader *ImageLoader::loader[MAX_LOADERS];
int ImageLoader::loader_count=0;

void ImageLoader::add_image_format_loader(ImageFormatLoader *p_loader) {

	ERR_FAIL_COND(loader_count >=MAX_LOADERS );
	loader[loader_count++]=p_loader;
}


ImageLoader::ImageLoader()
{
}


ImageLoader::~ImageLoader()
{
}



