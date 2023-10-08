//
// C++ Interface: texture
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef TEXTURE_H
#define TEXTURE_H

#include "iapi.h"
#include "image/image.h"
/**
	@author Juan Linietsky <reduzio@gmail.com>
*/
class Texture : public IAPI {
	
	IAPI_TYPE(Texture,IAPI);
public:
	
	enum StorageMode {
		
		STORAGE_FILE,
  		STORAGE_BUILT_IN
	};
	
	enum Flags {
		FLAG_RESERVED=1, /// reserved for now
		FLAG_MIPMAPS=2, /// Enable automatic mipmap generation - when available
		FLAG_REPEAT=4, /// Repeat texture (Tiling), otherwise Clamping
		FLAG_COMPRESS=8, /// Attempt compression of texture
		FLAG_FILTER=16, /// Create texure with linear (or available) filter
		FLAG_USE_GRAYSCALE_AS_INTENSITY=32,
		FLAG_USE_GRAYSCALE_AS_NORMALMAP=64, /// convert a grayscale texture (bumpmap) to RGB (normalmap)
		FLAGS_DEFAULT=FLAG_REPEAT|FLAG_MIPMAPS|FLAG_FILTER
	};	
	
	
	/* Texture must implement the following properties, in the following
	   order:
		-creation params:
		"flags":int[flags] - flags to load the texture from
		IF storage is STORAGE_FILE:
		"file":String[file] - file where texture was loaded from, (if built_in_store is false)
		ELSE
		"width":int - texture width
		"height":int - texture height
		"format":string - image format ["grayscale"|"grayscale_alpha"|"rgb"|"rgba"|"indexed"|"indexed_alpha"]
		END
		
		-normal params (non persistent)
		(if storage is STORAGE_FILE)
		area[x][y]:Image - image at area x,y
	*/
	  
	/* Texture must implement the following methods also:
		int load(String,int)
		int load(Image,int)
	*/
	
	
	virtual Error load(String p_path,Uint32 p_flags=FLAGS_DEFAULT)=0;
	virtual Error load(const Image& p_image,Uint32 p_flags=FLAGS_DEFAULT)=0;
	
	virtual Error create(int p_width,int p_height,Image::Format p_format,Uint32 p_flags=FLAGS_DEFAULT)=0;
	virtual Error paste_area(int p_x,int p_y,const Image& p_image)=0;
	virtual Error copy_area(int p_x,int p_y,int p_width,int p_height,Image& r_image) const=0;
	
	virtual String get_file() const=0;
	virtual Uint32 get_flags() const=0;
	virtual Image::Format get_format() const=0;
	
	virtual int get_width() const=0;
	virtual int get_height() const=0;
	
	virtual void set_storage_mode(StorageMode p_mode)=0;
	virtual StorageMode get_storage_mode() const=0;
	virtual void set_storage_file(String p_file)=0;
	
	virtual bool has_alpha() const=0;	
	
	Texture();
	~Texture();

};

#endif
