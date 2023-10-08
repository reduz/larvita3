//
// C++ Interface: audio_sample_manager_software
//
// Description: 
//
//
// Author: ,,, <red@lunatea>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef AUDIO_SAMPLE_MANAGER_SOFTWARE_H
#define AUDIO_SAMPLE_MANAGER_SOFTWARE_H


#include "audio/audio_server.h"
#include "pool_allocator.h"
#include "audio/audio_sample_manager.h"

/**
	@author ,,, <red@lunatea>
*/
class AudioSampleManagerSoftware : AudioSampleManager {

	enum {
		
		MAX_SAMPLES=256,
		SAMPLE_MEMORY=1*1024*1024, // 1mb, a lot
		GUARD_OFFSET=4
	};


	PoolAllocator pool;
	struct SampleData {
	
		PoolAllocator::ID pool_id;
		bool active;
		bool stereo;
		AudioSampleFormat format;
		AudioSampleLoopType loop_type;
		int loop_begin,loop_end;
		Sint32 c5freq;
		
		SampleData() { active=false; pool_id=-1; stereo=false; loop_begin=0; loop_end=0; c5freq=1; loop_type=AUDIO_SAMPLE_LOOP_NONE; format=AUDIO_SAMPLE_FORMAT_PCM8; }
		
	} sample_data[ MAX_SAMPLES ];


	static AudioSampleManagerSoftware* singleton;
public:

	static AudioSampleManagerSoftware* get_singleton();

	virtual AudioSampleID create(AudioSampleFormat p_format,bool p_stereo,Sint32 p_len);
	virtual void store_data(AudioSampleID p_id,const void *p_sample);
	virtual void retrieve_data(AudioSampleID p_id,void *p_dest);	
	virtual void destroy(AudioSampleID p_id);
	virtual bool check(AudioSampleID p_id) const; // return false if invalid

	virtual void set_c5_freq(AudioSampleID p_id,Sint32 p_freq);
	virtual void set_loop_begin(AudioSampleID p_id,Sint32 p_begin);
	virtual void set_loop_end(AudioSampleID p_id,Sint32 p_end);
	virtual void set_loop_type(AudioSampleID p_id,AudioSampleLoopType p_type);

	virtual Sint32 get_loop_begin(AudioSampleID p_id) const;
	virtual Sint32 get_loop_end(AudioSampleID p_id) const;
	virtual AudioSampleLoopType get_loop_type(AudioSampleID p_id) const;
	virtual Sint32 get_c5_freq(AudioSampleID p_id) const;
	virtual AudioSampleFormat get_format(AudioSampleID p_id) const;
	virtual Sint32 get_size(AudioSampleID p_id) const;
	const void* get_data_ptr(AudioSampleID p_id)  const; ///< Special function only present in main memory allocated samples

	virtual bool is_stereo(AudioSampleID p_id) const;

	AudioSampleManagerSoftware(int p_sample_memory);
	
	~AudioSampleManagerSoftware();

};

#endif
