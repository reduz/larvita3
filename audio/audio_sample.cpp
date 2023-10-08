//
// C++ Implementation: audio_sample
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "audio_sample.h"
#include "fileio/audio_loader.h"



IRef<IAPI> AudioSample::IAPI_DEFAULT_INSTANCE_FUNC(const String& p_type,const  IAPI::CreationParams& p_params) {
	
	AudioSample *as = memnew( AudioSample );
//printf("loading audio_sample\n");	
	if (p_params.has("file")) {
		
		as->load( p_params.get("file") );
//printf("has file %ls\n", p_params.get("file").get_string().c_str());
	}
	
	return as;
}


void AudioSample::set(String p_path, const Variant& p_value) {

	
}

Variant AudioSample::get(String p_path) const {
	
	if (!is_loaded())
		return Variant();
	if (p_path=="length")
		return AudioSampleManager::get_singleton()->get_size(id);
	if (p_path=="rate (hz)")
		return AudioSampleManager::get_singleton()->get_c5_freq(id);
	if (p_path=="format") {
		switch (AudioSampleManager::get_singleton()->get_format(id)) {
			
			case AUDIO_SAMPLE_FORMAT_PCM8: return "PCM8";
			case AUDIO_SAMPLE_FORMAT_PCM16: return "PCM16";
		}
	}
	if (p_path=="stereo")
		return AudioSampleManager::get_singleton()->is_stereo(id);
	
	if (p_path=="loop_type") {
		return (int)AudioSampleManager::get_singleton()->get_loop_type(id);
	}
	if (p_path=="loop_begin") {
	
		return AudioSampleManager::get_singleton()->get_loop_begin(id);
	}
	if (p_path=="loop_end") {
	
		return AudioSampleManager::get_singleton()->get_loop_end(id);
	}
	
	if (p_path=="file") {
		
		return file;
	};
	
	return Variant();	
}
void AudioSample::get_property_list( List<PropertyInfo> *p_list ) const {
	
	if (file!="")
		p_list->push_back( PropertyInfo(Variant::STRING,"file",PropertyInfo::USAGE_CREATION_PARAM));
	// info properties (can't edit)
	p_list->push_back( PropertyInfo( Variant::INT, "length" ) );
	p_list->push_back( PropertyInfo( Variant::INT, "rate (hz)" ) );
	p_list->push_back( PropertyInfo( Variant::STRING, "format" ) );
	p_list->push_back( PropertyInfo( Variant::BOOL, "stereo" ) );
	p_list->push_back( PropertyInfo( Variant::INT, "loop_type" ) );
	p_list->push_back( PropertyInfo( Variant::INT, "loop_begin" ) );
	p_list->push_back( PropertyInfo( Variant::INT, "loop_end" ) );
}

Variant AudioSample::call(String p_method, const List<Variant>& p_params) {
	
	if (p_method=="load") {
		
		ERR_FAIL_COND_V(p_params.size()!=1,Variant());
		load(p_params[0].get_string());
		
	} else if (p_method=="play") {
		
		play();
	} else if (p_method=="play_multi") {
		
		play_multi();
		
	} else if (p_method=="stop") {
		
		stop();
	}
	
	return Variant();
}
void AudioSample::get_method_list( List<MethodInfo> *p_list ) const {
	 
	MethodInfo mi("load");
	mi.parameters.push_back( MethodInfo::ParamInfo( Variant::STRING, "file", PropertyHint( PropertyHint::HINT_FILE, ".wav;.ogg" ) ) );
	
	p_list->push_back(mi);
	
	p_list->push_back( MethodInfo("play") );
	p_list->push_back( MethodInfo("play_multi") );
	p_list->push_back( MethodInfo("stop") );
}

IAPI::PropertyHint AudioSample::get_property_hint(String p_path) {
	
	if (p_path=="file")
		return PropertyHint( PropertyHint::HINT_FILE, ".wav;.ogg" );
	if (p_path=="length")
		return PropertyHint( PropertyHint::HINT_RANGE, "0,10000000" );
	if (p_path=="loop_begin")
		return PropertyHint( PropertyHint::HINT_RANGE, "0,10000000" );
	if (p_path=="loop_end")
		return PropertyHint( PropertyHint::HINT_RANGE, "0,10000000" );
	if (p_path=="loop_type")
		return PropertyHint( PropertyHint::HINT_ENUM, "Disabled,Forward,Ping-Poing" );	
	if (p_path=="rate")
		return PropertyHint( PropertyHint::HINT_RANGE, "1,192000" );
	return IAPI::PropertyHint();	
}

bool AudioSample::is_loaded() const {
	
	return AudioSampleManager::get_singleton()->check( id );
}
AudioSampleID AudioSample::get_sample_ID() {
	
	return id;
}

void AudioSample::play(AudioServer::FxParams *p_params) {
	
	ERR_FAIL_COND(id==AUDIO_SAMPLE_INVALID_ID);
	
	stop();
	
	sfxid=AudioServer::get_singleton()->play_fx(id,p_params);	
	
}

void AudioSample::change(AudioServer::FxParams *p_params) {
	
	ERR_FAIL_COND( sfxid==AUDIO_FX_INVALID_ID );
	ERR_FAIL_COND(!p_params);
	
	AudioServer::get_singleton()->change_fx( sfxid, p_params );
}

void AudioSample::play_multi(AudioServer::FxParams *p_params) {
	
	ERR_FAIL_COND(id==AUDIO_SAMPLE_INVALID_ID);
	
	sfxid=AudioServer::get_singleton()->play_fx(id,p_params);		
}


bool AudioSample::is_playing() const {
		
	return AudioServer::get_singleton()->is_fx_active( sfxid );	
}
void AudioSample::stop() {
	
	if (sfxid==AUDIO_FX_INVALID_ID)
		return;
	
	AudioServer::get_singleton()->stop_fx( sfxid );
	sfxid=AUDIO_FX_INVALID_ID;
}
Error AudioSample::load(String p_path) {
	
	if (id!=AUDIO_SAMPLE_INVALID_ID) {
		
		AudioSampleManager::get_singleton()->destroy( id );
	}
	
	file="";
	
	id=AudioLoader::load_sample(p_path);
	
	if (id==AUDIO_SAMPLE_INVALID_ID)
		return ERR_FILE_CANT_OPEN;
	
	file=p_path;
	
	property_changed_signal.call("");
	return OK;
}

AudioSample::AudioSample(){
	
	id=AUDIO_SAMPLE_INVALID_ID;
	sfxid=AUDIO_FX_INVALID_ID;
}


AudioSample::~AudioSample() {
	
	if (id!=AUDIO_SAMPLE_INVALID_ID) {
		
		AudioSampleManager::get_singleton()->destroy( id );
	}	
}


