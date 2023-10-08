//
// C++ Implementation: file_access_wrapper
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "file_access.h"
#include "global_vars.h"

FileAccess* (*FileAccess::create_func)()=0;

FileAccess *FileAccess::create(){

	ERR_FAIL_COND_V( !create_func, 0 );

	return create_func();
}

String FileAccess::fix_path(String p_path) const {
	
	return GlobalVars::get_singleton()->translate_path(p_path);
}

/* these are all implemented for ease of porting, then can later be optimized */

Uint16 FileAccess::get_16() {
	
	Uint16 res;
	Uint8 a,b;
	
	a=get_8();
	b=get_8();
	
	if (endian_swap) {
		
		SWAP( a,b );
	}
	
	res=b;
	res<<=8;
	res|=a;
	
	return res;
}
Uint32 FileAccess::get_32() {
	
	Uint32 res;
	Uint16 a,b;
	
	a=get_16();
	b=get_16();
	
	if (endian_swap) {
		
		SWAP( a,b );
	}
	
	res=b;
	res<<=16;
	res|=a;
	
	return res;	
}
Uint64 FileAccess::get_64() {
	
	Uint64 res;
	Uint32 a,b;
	
	a=get_32();
	b=get_32();
	
	if (endian_swap) {
		
		SWAP( a,b );
	}
	
	res=b;
	res<<=32;
	res|=a;
	
	return res;	
	
}

void FileAccess::get_buffer(Uint8 *p_dst,int p_length) {
	
	for (int i=0;i<p_length;i++)
		p_dst[i]=get_8();
	
}

void FileAccess::store_16(Uint16 p_dest) {
	
	Uint8 a,b;
	
	a=p_dest&0xFF;
	b=p_dest>>8;
	
	if (endian_swap) {
		
		SWAP( a,b );
	}
		
	store_8(a);
	store_8(b);

}
void FileAccess::store_32(Uint32 p_dest) {
	
	
	Uint16 a,b;
	
	a=p_dest&0xFFFF;
	b=p_dest>>16;
	
	if (endian_swap) {
		
		SWAP( a,b );
	}
		
	store_16(a);
	store_16(b);
	
}
void FileAccess::store_64(Uint64 p_dest) {
	
	Uint32 a,b;
	
	a=p_dest&0xFFFFFFFF;
	b=p_dest>>32;
	
	if (endian_swap) {
		
		SWAP( a,b );
	}
		
	store_32(a);
	store_32(b);
	
}
void FileAccess::store_buffer(const Uint8 *p_src,int p_length) {
	
	for (int i=0;i<p_length;i++)
		store_8(p_src[i]);
}
