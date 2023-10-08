//
// C++ Implementation: audio_server
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "audio_server.h"

AudioServer *AudioServer::singleton=NULL;

AudioServer *AudioServer::get_singleton() {

	return singleton;
}

void AudioServer::set_music_volume(float p_volume) {
	
	_music_volume=p_volume;
}
float AudioServer::get_music_volume() const {
	
	return _music_volume;	
}
void AudioServer::set_sfx_volume(float p_volume) {
	
	_sfx_volume=p_volume;
}
float AudioServer::get_sfx_volume() const {
	
	return _sfx_volume;
}

AudioServer::AudioServer() {

	_music_volume=1.0;
	_sfx_volume=1.0;
	
	singleton=this;
}


AudioServer::~AudioServer()
{
}


