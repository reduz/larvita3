//
// C++ Implementation: audio_sample_manager_software
//
// Description: 
//
//
// Author: ,,, <red@lunatea>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "audio_sample_manager_software.h"


AudioSampleManagerSoftware* AudioSampleManagerSoftware::singleton=NULL;

AudioSampleManagerSoftware* AudioSampleManagerSoftware::get_singleton() {

	return singleton;
}

AudioSampleID AudioSampleManagerSoftware::create(AudioSampleFormat p_format,bool p_stereo,Sint32 p_len) {

	ERR_FAIL_COND_V(p_len<=0,AUDIO_SAMPLE_INVALID_ID);
	
	int bytes=p_len;
	if(p_stereo)
		bytes*=2;
	if (p_format==AUDIO_SAMPLE_FORMAT_PCM16)
		bytes*=2;
			
	int smpid=-1;
	for (int i=0;i<MAX_SAMPLES;i++) {
		
		if (sample_data[i].active)
			continue;
		smpid=i;
		break;
	}
	
	ERR_FAIL_COND_V(smpid==-1,AUDIO_SAMPLE_INVALID_ID); // out of slots
		    
	PoolAllocator::ID id = pool.alloc(bytes+GUARD_OFFSET*2);
	if (id<0)
		return AUDIO_SAMPLE_INVALID_ID;
	
	
	sample_data[smpid].pool_id=id;
	sample_data[smpid].stereo=p_stereo;
	sample_data[smpid].format=p_format;
	sample_data[smpid].loop_begin=0;
	sample_data[smpid].loop_begin=0;
	sample_data[smpid].loop_type=AUDIO_SAMPLE_LOOP_NONE;
	sample_data[smpid].active=true;
	
	Uint8 *mem = (Uint8*)pool.get(id);
	for(int i=0;i<bytes;i++)
		mem[i]=0;
	return smpid;
}

void AudioSampleManagerSoftware::store_data(AudioSampleID p_id,const void *p_sample) {

	ERR_FAIL_INDEX(p_id,MAX_SAMPLES);
	ERR_FAIL_COND( !sample_data[p_id].active );
	
	AudioServer::get_singleton()->lock();
	
	PoolAllocator::ID poolid=sample_data[p_id].pool_id;
	int len = pool.get_size(poolid)-GUARD_OFFSET*2;
	Uint8*dst=&(((Uint8*)pool.get(poolid))[GUARD_OFFSET]);
	const Uint8 *src=(Uint8*)p_sample;
	
	for (int i=0;i<len;i++) {
	
		dst[i]=src[i];
	}
	// zero post/pre data	
	for (int i=0;i<GUARD_OFFSET;i++) {
	
		dst[-GUARD_OFFSET+i]=0;
		dst[len+i]=0;
	}
	
	AudioServer::get_singleton()->unlock();

}

void AudioSampleManagerSoftware::retrieve_data(AudioSampleID p_id,void *p_dest) {

	ERR_FAIL_INDEX(p_id,MAX_SAMPLES);
	ERR_FAIL_COND( !sample_data[p_id].active );
	
	AudioServer::get_singleton()->lock();
	
	PoolAllocator::ID poolid=sample_data[p_id].pool_id;
	int len = pool.get_size(poolid)-GUARD_OFFSET*2;
	
	Uint8*dst=(Uint8*)p_dest;
	const Uint8 *src=&(((const Uint8*)pool.get(poolid))[GUARD_OFFSET]);
	for (int i=0;i<len;i++) {
	
		dst[i]=src[i];
	}
	
	AudioServer::get_singleton()->unlock();

}
void AudioSampleManagerSoftware::destroy(AudioSampleID p_id) {

	ERR_FAIL_INDEX(p_id,MAX_SAMPLES);
	ERR_FAIL_COND( !sample_data[p_id].active );
	AudioServer::get_singleton()->lock();
	
	PoolAllocator::ID poolid=sample_data[p_id].pool_id;
		
	pool.free(poolid);
	sample_data[p_id].active=false;
	
	AudioServer::get_singleton()->unlock();


}

const void* AudioSampleManagerSoftware::get_data_ptr(AudioSampleID p_id)  const {

	ERR_FAIL_INDEX_V(p_id,MAX_SAMPLES,NULL);
	ERR_FAIL_COND_V( !sample_data[p_id].active, NULL );
		
	return &(((const Uint8*)pool.get(sample_data[p_id].pool_id))[GUARD_OFFSET]);
}


bool AudioSampleManagerSoftware::check(AudioSampleID p_id) const {

	if (p_id<0 || p_id>=MAX_SAMPLES)
		return false;
	return sample_data[p_id].active;

}

void AudioSampleManagerSoftware::set_c5_freq(AudioSampleID p_id,Sint32 p_freq) {

	ERR_FAIL_INDEX(p_id,MAX_SAMPLES);
	SampleData &sd=sample_data[p_id];
	
	ERR_FAIL_COND( !sd.active );
	ERR_FAIL_COND(p_freq<=0);
	ERR_FAIL_COND(p_freq>=0xFFFFFF);
	
	AudioServer::get_singleton()->lock();
	
	sd.c5freq=p_freq;
	AudioServer::get_singleton()->unlock();
	
	
}
void AudioSampleManagerSoftware::set_loop_begin(AudioSampleID p_id,Sint32 p_begin) {

	ERR_FAIL_INDEX(p_id,MAX_SAMPLES);
	SampleData &sd=sample_data[p_id];
	ERR_FAIL_COND( !sd.active );
	
	if (p_begin<0)
		p_begin=0;
	if (p_begin>=get_size(p_id))
		p_begin=get_size(p_id)-1;
		
	AudioServer::get_singleton()->lock();
		
	sd.loop_begin=p_begin;
	AudioServer::get_singleton()->unlock();
	
	if (sd.loop_begin>=sd.loop_end) {
	
		set_loop_end(p_id,sd.loop_begin+1);
	}

}
void AudioSampleManagerSoftware::set_loop_end(AudioSampleID p_id,Sint32 p_end) {

	ERR_FAIL_INDEX(p_id,MAX_SAMPLES);
	SampleData &sd=sample_data[p_id];
	ERR_FAIL_COND( !sd.active );
	
	if (p_end<1)
		p_end=1;
	if (p_end>get_size(p_id))
		p_end=get_size(p_id);
		
	AudioServer::get_singleton()->lock();
		
	sd.loop_end=p_end;
	AudioServer::get_singleton()->unlock();
	
	if (sd.loop_end<=sd.loop_begin) {
	
		set_loop_begin(p_id,sd.loop_end-1);
	}

}
void AudioSampleManagerSoftware::set_loop_type(AudioSampleID p_id,AudioSampleLoopType p_type) {

	ERR_FAIL_INDEX(p_id,MAX_SAMPLES);
	SampleData &sd=sample_data[p_id];
	ERR_FAIL_COND( !sd.active );

	AudioServer::get_singleton()->lock();
	sd.loop_type=p_type;
	AudioServer::get_singleton()->unlock();
}

Sint32 AudioSampleManagerSoftware::get_loop_begin(AudioSampleID p_id) const {
	
	ERR_FAIL_INDEX_V(p_id,MAX_SAMPLES,-1);
	const SampleData &sd=sample_data[p_id];
	ERR_FAIL_COND_V( !sd.active, -1 );
	
	return sd.loop_begin;


}
Sint32 AudioSampleManagerSoftware::get_loop_end(AudioSampleID p_id) const {

	ERR_FAIL_INDEX_V(p_id,MAX_SAMPLES,-1);
	const SampleData &sd=sample_data[p_id];
	ERR_FAIL_COND_V( !sd.active, -1 );
	
	return sd.loop_end;

}
AudioSampleLoopType AudioSampleManagerSoftware::get_loop_type(AudioSampleID p_id) const {

	ERR_FAIL_INDEX_V(p_id,MAX_SAMPLES,AUDIO_SAMPLE_LOOP_NONE);
	const SampleData &sd=sample_data[p_id];
	ERR_FAIL_COND_V( !sd.active, AUDIO_SAMPLE_LOOP_NONE );
	
	return sd.loop_type;

}
Sint32 AudioSampleManagerSoftware::get_c5_freq(AudioSampleID p_id) const {

	ERR_FAIL_INDEX_V(p_id,MAX_SAMPLES,-1);
	const SampleData &sd=sample_data[p_id];
	ERR_FAIL_COND_V( !sd.active, -1 );
	
	return sd.c5freq;

}
Sint32 AudioSampleManagerSoftware::get_size(AudioSampleID p_id) const {

	ERR_FAIL_INDEX_V(p_id,MAX_SAMPLES,-1);
	ERR_FAIL_COND_V( !sample_data[p_id].active,-1 );
	
	//AudioServer::get_singleton()->lock(); -- audio thread uses this, so no locking is better
	
	int len = pool.get_size(sample_data[p_id].pool_id)-GUARD_OFFSET*2;
	switch(sample_data[p_id].format) {
	
		case AUDIO_SAMPLE_FORMAT_PCM8: len/=1; break;
		case AUDIO_SAMPLE_FORMAT_PCM16: len/=2; break;
	}
	
	if (sample_data[p_id].stereo)
		len/=2;
	
	//AudioServer::get_singleton()->unlock();
	
	return len;	

}

AudioSampleFormat AudioSampleManagerSoftware::get_format(AudioSampleID p_id) const {

	ERR_FAIL_INDEX_V(p_id,MAX_SAMPLES,AUDIO_SAMPLE_FORMAT_PCM8);
	const SampleData &sd=sample_data[p_id];
	ERR_FAIL_COND_V( !sd.active, AUDIO_SAMPLE_FORMAT_PCM8 );
	
	return sd.format;

}
bool AudioSampleManagerSoftware::is_stereo(AudioSampleID p_id) const {

	ERR_FAIL_INDEX_V(p_id,MAX_SAMPLES,false);
	const SampleData &sd=sample_data[p_id];
	ERR_FAIL_COND_V( !sd.active, false );
	
	return sd.stereo;

}

AudioSampleManagerSoftware::AudioSampleManagerSoftware(int p_sample_memory) : pool(p_sample_memory,false,MAX_SAMPLES) {

	AudioSampleManagerSoftware::singleton=this;

}


AudioSampleManagerSoftware::~AudioSampleManagerSoftware() {

	for(int i=0;i<MAX_SAMPLES;i++) {
	
		if (sample_data[i].active) {
		
			// todo, error
			pool.free(sample_data[i].pool_id);
		}
	}
}


