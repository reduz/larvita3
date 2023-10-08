#ifdef OPENGL_ENABLED
//
// C++ Implementation: texture_gl
//
// Description:
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "texture_gl.h"

#include "fileio/image_loader.h"
#include "os/dir_access.h"
#include "types/iapi/iapi_db.h"
/* Texture must implement the following properties, in the following
	order:
	-creation params:
	"flags":int[flags] - flags to load the texture from
	"file":String[file] - file where texture was loaded from, (if loaded from file)

	-normal params (non persistent)
	"width":int - texture width
	"height":int - texture height
*/

/* Texture must implement the following methods also:
	int load(String,int)
	int load(Image,int)
*/

IRef<IAPI> Texture_GL::IAPI_DEFAULT_INSTANCE_FUNC(const String& p_type,const  IAPI::CreationParams& p_params) {

	Texture_GL *tgl = memnew( Texture_GL );

	if (p_params.has("file") && p_params.get("file").get_string()!="") {

		tgl->load(p_params.get("file").get_string(),p_params.get("flags").get_int());
	} else if (p_params.has("format")) {

		int w = p_params.get("width").get_int();
		int h = p_params.get("height").get_int();
		int flg = p_params.get("flags").get_int();
		String fmts = p_params.get("format").get_string();
		Image::Format fmt=Image::FORMAT_RGBA;

		if (fmts=="grayscale")
			fmt=Image::FORMAT_GRAYSCALE;
		if (fmts=="grayscale_alpha")
			fmt=Image::FORMAT_GRAYSCALE_ALPHA;
		if (fmts=="rgb")
			fmt=Image::FORMAT_RGB;
		if (fmts=="rgba")
			fmt=Image::FORMAT_RGBA;
		if (fmts=="indexed")
			fmt=Image::FORMAT_INDEXED;
		if (fmts=="indexed_alpha")
			fmt=Image::FORMAT_INDEXED_ALPHA;

		tgl->create(w,h,fmt,flg);

	}

	return tgl;
}

void Texture_GL::set(String p_path,const Variant& p_value) {

	ERR_FAIL(); //needless calling
}
Variant Texture_GL::get(String p_path) const {

	if (p_path=="file")
		return file;
	else if (p_path=="flags")
		return flags;
	else if (p_path=="width")
		return width;
	else if (p_path=="height")
		return height;
	else if (p_path=="format") {

		if (format==Image::FORMAT_GRAYSCALE)
			return "grayscale";
		if (format==Image::FORMAT_GRAYSCALE_ALPHA)
			return "grayscale_alpha";
		if (format==Image::FORMAT_RGB)
			return "rgb";
		if (format==Image::FORMAT_RGBA)
			return "rgba";
		if (format==Image::FORMAT_INDEXED)
			return "indexed";
		if (format==Image::FORMAT_INDEXED_ALPHA)
			return "indexed_alpha";

	}

	ERR_FAIL_V(Variant()); // unknown property
}
void Texture_GL::get_property_list( List<PropertyInfo> *p_list ) const {

	if (!active)
		return;

	if (storage_mode==STORAGE_FILE) {

		p_list->push_back( PropertyInfo( Variant::STRING, "file", PropertyInfo::USAGE_CREATION_PARAM ) );
		p_list->push_back( PropertyInfo( Variant::INT, "width",PropertyInfo::USAGE_EDITING ) );
		p_list->push_back( PropertyInfo( Variant::INT, "height",PropertyInfo::USAGE_EDITING ) );

	} else {
		p_list->push_back( PropertyInfo( Variant::INT, "width",PropertyInfo::USAGE_EDITING|PropertyInfo::USAGE_CREATION_PARAM ) );
		p_list->push_back( PropertyInfo( Variant::INT, "height",PropertyInfo::USAGE_EDITING|PropertyInfo::USAGE_CREATION_PARAM ) );
		p_list->push_back( PropertyInfo( Variant::STRING, "format",PropertyInfo::USAGE_EDITING|PropertyInfo::USAGE_CREATION_PARAM ) );
	}
	p_list->push_back( PropertyInfo( Variant::INT, "flags", PropertyInfo::USAGE_CREATION_PARAM ) );

}

IAPI::PropertyHint Texture_GL::get_property_hint(String p_path) const {

	if (p_path=="width" || p_path=="height")
		return IAPI::PropertyHint( IAPI::PropertyHint::HINT_RANGE , "0,2048");

	if (p_path=="flags")
		return IAPI::PropertyHint(IAPI::PropertyHint::HINT_FLAGS,
	      "Reserved (not in use), Mipmaps, Repeat, Compress, Filters, Intensity, Normalmap", Texture::FLAGS_DEFAULT);

	return IAPI::PropertyHint();
}

Variant Texture_GL::call(String p_method, const List<Variant>& p_params) {

	if  (p_method=="needs_presave") {
		return true;
	} else if (p_method == "make_local_files") {

		if (storage_mode == STORAGE_FILE) {

			String path = file;
			if (file.find("://") != 3) {

				if (file.find_last("\\")!=-1) {

					file = file.right( file.find_last("\\") );
				} else if (file.find_last("/")!=-1) {

					file = file.right( file.find_last("/") );
				}

				DirAccess *da = DirAccess::create();
				da->copy(path,"tex://"+file);
				memdelete(da);

				String fname = file;
				file = "tex://"+file;

				IAPI_DB * db = IAPI_DB::get_singleton();
				ERR_FAIL_COND_V(!db, Variant());
				db->save(this, "textures/"+fname);
			} else {
				if (file.find("tex://") == 0) {
					IAPI_DB * db = IAPI_DB::get_singleton();
					ERR_FAIL_COND_V(!db, Variant());
					String fname = file.right( file.find_last("/") );

					db->save(this, "textures/"+fname);
				};
			};

		};
	} else if (p_method=="import") {

		ERR_FAIL_COND_V(p_params.size()<2,Variant());
		return (int)load(p_params[0].get_string(),p_params[1].get_int());

	}

	return Variant();
}


void Texture_GL::get_method_list( List<MethodInfo> *p_list ) const {

	MethodInfo mi("import");
	mi.parameters.push_back( MethodInfo::ParamInfo( Variant::STRING, "path", PropertyHint( PropertyHint::HINT_FILE,"*.png;*.tga") ) );
	mi.parameters.push_back( MethodInfo::ParamInfo( Variant::INT, "flags", PropertyHint( PropertyHint::HINT_FLAGS,"Scissor Alpha,Use Mipmaps,Repeat,Compress,Filter,Use as Intensity,Use as Normalmap",(int)FLAGS_DEFAULT) ) );
	p_list->push_back( mi );

}


Error Texture_GL::load(String p_file,Uint32 p_flags) {

	Image img;


	Error err=ImageLoader::load_image(p_file,&img);
	ERR_FAIL_COND_V(err != OK, err);

	err = load(img,p_flags);
	ERR_FAIL_COND_V(err != OK, err);

	file=p_file;

	return err;

}
Error Texture_GL::load(const Image& p_image,Uint32 p_flags) {

	Error err = create(p_image.get_width(),p_image.get_height(),p_image.get_format(),p_flags);
	ERR_FAIL_COND_V(err,err);
	err=paste_area(0,0,p_image);
	ERR_FAIL_COND_V(err,err);

	return OK;
}

void Texture_GL::get_gl_format(Image::Format p_format, Uint32 p_flags,GLenum& r_gl_format,int &r_gl_components,bool &r_has_alpha_cache) {

	r_has_alpha_cache=false;

	switch(p_format) {

		case Image::FORMAT_GRAYSCALE: {


			if (p_flags&FLAG_USE_GRAYSCALE_AS_INTENSITY) {
				r_has_alpha_cache=true;
				r_gl_components=GL_INTENSITY; r_gl_format=GL_LUMINANCE;
			} else {

				r_gl_components=1;
				r_gl_format=GL_LUMINANCE;
			}
		} break;
		case Image::FORMAT_GRAYSCALE_ALPHA: {

			r_gl_components=2; r_gl_format=GL_LUMINANCE_ALPHA; r_has_alpha_cache=true;

		} break;
		/*
		case Image::FORMAT_INDEXED: {

			alternate=p_image;
			img=&alternate;
			alternate.convert( Image::FORMAT_RGB );

		};  */
		case Image::FORMAT_RGB: {

			r_gl_components=3; r_gl_format=GL_RGB;
		} break;
		/*
		case Image::FORMAT_INDEXED_ALPHA: {

			alternate=p_image;
			img=&alternate;
			alternate.convert( Image::FORMAT_RGBA );

		}; */
		case Image::FORMAT_RGBA: {

			r_gl_components=4; r_gl_format=GL_RGBA; r_has_alpha_cache=true;
		} break;
		default: {

			ERR_FAIL();
		}
	}
}


Error Texture_GL::create(int p_width,int p_height,Image::Format p_format,Uint32 p_flags) {

	int components;
	GLenum format;

	get_gl_format(p_format,p_flags,format,components,has_alpha_cache);

	if (!active) {
		glGenTextures(1, &tex_id);
		active=true;
	}

	gl_components_cache=components;
	gl_format_cache=format;
	has_alpha_cache=has_alpha_cache;

	glBindTexture(GL_TEXTURE_2D, tex_id);

	if (p_flags&FLAG_MIPMAPS) {
		glTexParameteri( GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE );
	}

	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);	// Linear Filtering

	glTexImage2D(GL_TEXTURE_2D, 0, components, p_width, p_height, 0, format, GL_UNSIGNED_BYTE,NULL);

	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);	// Linear Filtering

	if (p_flags&FLAG_FILTER) {

		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);	// Linear Filtering
		if (p_flags&FLAG_MIPMAPS)
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);

	}

	if (p_flags&FLAG_REPEAT) {

		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
	} else {
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );

	}

	//img->lock();
	//gluBuild2DMipmaps(GL_TEXTURE_2D, format, img->get_width(), img->get_height(), format, GL_UNSIGNED_BYTE, img->get_data());
	//img->unlock();

	flags=p_flags;
	width=p_width;
	height=p_height;
	this->format=p_format;
	return OK;
}

Error Texture_GL::paste_area(int p_x,int p_y,const Image& p_image) {

	ERR_FAIL_COND_V(!active,ERR_UNCONFIGURED);

	DVector<Uint8> data = p_image.get_data();

	data.read_lock();

	glBindTexture(GL_TEXTURE_2D, tex_id);
	glTexSubImage2D( GL_TEXTURE_2D, 0, p_x,p_y,p_image.get_width(),p_image.get_height(),gl_format_cache,GL_UNSIGNED_BYTE,data.read() );

	data.read_unlock();

	return OK;
}

Error Texture_GL::copy_area(int p_x,int p_y,int p_width,int p_height,Image& r_image) const {

	ERR_FAIL_COND_V(!active,ERR_UNCONFIGURED);

	printf("COPYAREA IS AT WORK! WHEEE\n");
	DVector<Uint8> imgdata;

	imgdata.resize( width*height*3 );
	ERR_FAIL_COND_V( imgdata.size() != (width*height*3), ERR_OUT_OF_MEMORY );

	imgdata.write_lock();
	Uint8 * data = imgdata.write();

	glBindTexture(GL_TEXTURE_2D, tex_id);
	glGetTexImage( GL_TEXTURE_2D, 0, GL_RGB,                                         GL_UNSIGNED_BYTE, data );

	imgdata.write_unlock();

	Image image( width, height, Image::FORMAT_RGB, imgdata );

	imgdata.clear(); // non use anymore

	r_image=image;


	return OK;
}



String Texture_GL::get_file() const {

	return file;
}
Uint32 Texture_GL::get_flags() const {

	return flags;
}

void Texture_GL::set_storage_mode(StorageMode p_mode) {

	storage_mode=p_mode;
}

Texture::StorageMode Texture_GL::get_storage_mode() const {

	return storage_mode;
}

void Texture_GL::set_storage_file(String p_file) {

	file=p_file;
}

int Texture_GL::get_width() const {

	return width;
}
int Texture_GL::get_height() const {

	return height;
}

Image::Format Texture_GL::get_format() const {

	return format;
}

bool Texture_GL::has_alpha() const {

	return has_alpha_cache;
}

Texture_GL::Texture_GL() {


	flags=0;
	tex_id=0;
	active=false;
	storage_mode=STORAGE_FILE;
	width=0;
	height=0;
	format=Image::FORMAT_GRAYSCALE;
	has_alpha_cache=false;
}


Texture_GL::~Texture_GL() {

	if (active)
		glDeleteTextures( 1,&tex_id );
}


#endif // OPENGL_ENABLED
