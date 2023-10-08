//
// C++ Interface: audio_loader
//
// Description: 
//
//
// Author: ,,, <red@lunatea>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef AUDIO_LOADER_H
#define AUDIO_LOADER_H


#include "audio/audio_sample_manager.h"
#include "os/file_access.h"

/**
	@author ,,, <red@lunatea>
*/


class AudioFormatLoader {
public:

	virtual AudioSampleID load_sample(String p_path,FileAccess *p_file)=0;

	AudioFormatLoader();
	virtual ~AudioFormatLoader();
};


class AudioLoader {

	enum {
		MAX_LOADERS=8
	};

	static AudioFormatLoader *loader[MAX_LOADERS];
	static int loader_count;

protected:


public:
	
	static AudioSampleID load_sample(String p_path, FileAccess *p_custom=NULL);	
	static void add_audio_format_loader(AudioFormatLoader *p_loader);

};


#endif
