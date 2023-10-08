//
// C++ Interface: audio_sample
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef AUDIO_SAMPLE_H
#define AUDIO_SAMPLE_H

#include "iapi.h"
#include "audio/audio_sample_manager.h"
#include "audio/audio_server.h"
/**
	@author Juan Linietsky <reduzio@gmail.com>
*/

class AudioSample : public IAPI {
	
	IAPI_TYPE( AudioSample, IAPI );
	
	AudioSFXID sfxid;
	AudioSampleID id;
	String file;
public:
	
	virtual Variant call(String p_method, const List<Variant>& p_params=List<Variant>());	
	virtual void get_method_list( List<MethodInfo> *p_list ) const; 
	
	static IRef<IAPI> IAPI_DEFAULT_INSTANCE_FUNC(const String& p_type,const  IAPI::CreationParams& p_params);
	
	virtual void set(String p_path, const Variant& p_value);
	virtual Variant get(String p_path) const;
	virtual void get_property_list( List<PropertyInfo> *p_list ) const; 
	virtual PropertyHint get_property_hint(String p_path);
	
	bool is_loaded() const;
	AudioSampleID get_sample_ID();
	
	void play(AudioServer::FxParams *p_params=NULL);
	void change(AudioServer::FxParams *p_params);
	void play_multi(AudioServer::FxParams *p_params=NULL);
	bool is_playing() const;	
	void stop();
	
	Error load(String p_path);
	
	AudioSample();
	~AudioSample();

};

#endif
