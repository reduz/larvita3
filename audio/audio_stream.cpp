//
// C++ Implementation: audio_stream
//
// Description: 
//
//
// Author: ,,, <red@lunatea>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "audio_stream.h"

void AudioStream::play() {

	set_status(PLAY);
}
void AudioStream::pause() {

	set_status(PAUSE);

}
void AudioStream::stop() {

	set_status(STOP);

}

Variant AudioStream::call(String p_method, const List<Variant>& p_params) {

	if (p_method=="play")
		play();
	if (p_method=="pause")
		pause();
	if (p_method=="stop")
		stop();
	if  (p_method=="needs_presave")
		return true;

	return Variant();
}
void AudioStream::get_method_list( List<MethodInfo> *p_list ) const {

	MethodInfo mi;
	mi.name="play";
	p_list->push_back(mi);
	mi.name="pause";
	p_list->push_back(mi);
	mi.name="stop";
	p_list->push_back(mi);


}

void AudioStream::set_loop(bool p_loop) {

	loop=p_loop;
}
bool AudioStream::has_loop() const {

	return loop;
}


void AudioStream::set_audio_type(Type p_type) {
	
	type=p_type;
}
AudioStream::Type AudioStream::get_audio_type() const {
	
	return type;	
}

AudioStream::AudioStream() {
	
	loop=true;
	type=MUSIC;
}


AudioStream::~AudioStream()
{
}


