//
// C++ Interface: audio_loader_wav
//
// Description: 
//
//
// Author: ,,, <red@lunatea>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef AUDIO_LOADER_WAV_H
#define AUDIO_LOADER_WAV_H


#include "fileio/audio_loader.h"

/**
	@author ,,, <red@lunatea>
*/
class AudioFormatLoaderWAV : public AudioFormatLoader {
public:

	virtual AudioSampleID load_sample(String p_path,FileAccess *p_file);
	
	AudioFormatLoaderWAV();	
	~AudioFormatLoaderWAV();

};

#endif
