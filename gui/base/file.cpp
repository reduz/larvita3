//
// C++ Implementation: file
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "file.h"

namespace GUI {


File* (*File::create_func)()=0;

File *File::create(){

	if ( !create_func)
		return 0;

	return create_func();
}


/* these are all implemented for ease of porting, then can later be optimized */

unsigned short File::get_16() {
	
	unsigned short res;
	unsigned char a,b;
	
	a=get_8();
	b=get_8();
	
	if (endian_swap) {
		unsigned char aux=a;
		a=b;
		b=aux;
	}
	
	res=b;
	res<<=8;
	res|=a;
	
	return res;
}
unsigned int File::get_32() {
	
	unsigned int res;
	unsigned short a,b;
	
	a=get_16();
	b=get_16();
	
	if (endian_swap) {
		
		unsigned short aux=a;
		a=b;
		b=aux;
	}
	
	res=b;
	res<<=16;
	res|=a;
	
	return res;	
}
gui_u64 File::get_64() {
	
	gui_u64 res;
	unsigned int a,b;
	
	a=get_32();
	b=get_32();
	
	if (endian_swap) {
		
		unsigned int aux=a;
		a=b;
		b=aux;
	}
	
	res=b;
	res<<=32;
	res|=a;
	
	return res;	
	
}

void File::get_buffer(unsigned char *p_dst,int p_length) {
	
	for (int i=0;i<p_length;i++)
		p_dst[i]=get_8();
	
}

void File::store_16(unsigned short p_dest) {
	
	unsigned char a,b;
	
	a=p_dest&0xFF;
	b=p_dest>>8;
	
	if (endian_swap) {
		
		unsigned char aux=a;
		a=b;
		b=aux;
	}
		
	store_8(a);
	store_8(b);

}
void File::store_32(unsigned int p_dest) {
	
	
	unsigned short a,b;
	
	a=p_dest&0xFFFF;
	b=p_dest>>16;
	
	if (endian_swap) {
		
		unsigned short aux=a;
		a=b;
		b=aux;
	}
		
	store_16(a);
	store_16(b);
	
}
void File::store_64(gui_u64 p_dest) {
	
	unsigned int a,b;
	
	a=p_dest&0xFFFFFFFF;
	b=p_dest>>32;
	
	if (endian_swap) {
		
		unsigned int aux=a;
		a=b;
		b=aux;
	}
		
	store_32(a);
	store_32(b);
	
}
void File::store_buffer(const unsigned char *p_src,int p_length) {
	
	for (int i=0;i<p_length;i++)
		store_8(p_src[i]);
}







}
