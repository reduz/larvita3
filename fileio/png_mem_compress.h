//
// C++ Interface: png_mem_compress
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef PNG_MEM_COMPRESS_H
#define PNG_MEM_COMPRESS_H

#include "dvector.h"
#include "image.h"

/**
	@author Juan Linietsky <reduzio@gmail.com>
*/
class PNG_MemCompress{
public:
	
	static DVector<Uint8> compress( const Image& p_image );
	static Image uncompress( const DVector<Uint8>& p_data );
	

};

#endif
