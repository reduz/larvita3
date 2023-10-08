//
// C++ Implementation: audio_sample_manager
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "audio_sample_manager.h"

AudioSampleManager * AudioSampleManager::singleton=NULL;
AudioSampleManager *AudioSampleManager::get_singleton() {

	return singleton;
}


AudioSampleManager::AudioSampleManager(){
	singleton=this;
}


AudioSampleManager::~AudioSampleManager()
{
}


