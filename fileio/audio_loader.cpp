//
// C++ Implementation: audio_loader
//
// Description: 
//
//
// Author: ,,, <red@lunatea>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "audio_loader.h"
#include "os/memory.h"


AudioFormatLoader::AudioFormatLoader()
{
}


AudioFormatLoader::~AudioFormatLoader()
{
}


AudioFormatLoader *AudioLoader::loader[MAX_LOADERS];
	
int AudioLoader::loader_count=0;

AudioSampleID AudioLoader::load_sample(String p_path, FileAccess *p_custom) {

	FileAccess *f = p_custom?p_custom:FileAccess::create();

	for(int i=0;i<loader_count;i++) {
	
		AudioSampleID id = loader[i]->load_sample(p_path,f);
		if (id != AUDIO_SAMPLE_INVALID_ID) {
			if (!p_custom)
				memdelete(f);
			return id;
		}
	}
	
	if (!p_custom)
		memdelete(f);
	
	return AUDIO_SAMPLE_INVALID_ID;
}
void AudioLoader::add_audio_format_loader(AudioFormatLoader *p_loader) {

	ERR_FAIL_COND(loader_count==MAX_LOADERS);
	loader[loader_count++]=p_loader;
}
