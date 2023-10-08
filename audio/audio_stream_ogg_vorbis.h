//
// C++ Interface: audio_stream_ogg_vorbis
//
// Description: 
//
//
// Author: ,,, <red@lunatea>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef AUDIO_STREAM_OGG_VORBIS_H
#define AUDIO_STREAM_OGG_VORBIS_H

#include "audio/audio_stream.h"
#include "audio/audio_sample_manager.h"
#include "audio/audio_mixer.h"

#include "fileio/oggvorbis/ivorbisfile.h"
#include "os/file_access.h"

/**
	@author ,,, <red@lunatea>
*/

class AudioStreamOGGVorbis : public AudioStream {

	IAPI_TYPE( AudioStreamOGGVorbis, AudioStream );
	IAPI_INSTANCE_FUNC( AudioStreamOGGVorbis );
	
	
	FileAccess *f;
	
	ov_callbacks _ov_callbacks;
	
	static size_t _ov_read_func(void *p_dst,size_t p_data, size_t p_count, void *_f);
	static int _ov_seek_func(void *_f,ogg_int64_t offs, int whence);
	static int _ov_close_func(void *_f);
	static long _ov_tell_func(void *_f);


	/* 16k frames of rb */
	enum {
		RB_BITS=14,
		RB_LEN=(1<<RB_BITS),
		RB_MASK=RB_LEN-1,
  		READ_BUFF_LEN=16384,
		RB_MIN_MIX=2048,
		MIX_FRAC_BITS=13,
    		MIX_FRAC_LEN=(1<<MIX_FRAC_BITS),
		MIX_FRAC_MASK=MIX_FRAC_LEN-1,
				
	};

	Sint16 read_buf[READ_BUFF_LEN];
	Sint16 rb[RB_LEN*2];
	volatile int rb_read_pos;
	volatile int rb_write_pos;
	
	Sint32 offset;
	
	String file;

	bool stream_loaded;
	Status status;
	OggVorbis_File vf;
	int stream_channels;
	int stream_srate;
	int current_section;
	
	int repeats;
	Uint8 config_volume;
	void mix_callback(Sint32 *p_dest, int p_frames);

public:

	virtual void set(String p_path, const Variant& p_value); /** set a property, by path */
	virtual Variant get(String p_path) const; /** get a property, by path */	
	virtual IAPI::PropertyHint get_property_hint(String p_path) const;
	
	
	virtual void get_property_list( List<PropertyInfo> *p_list ) const; ///< Get a list with all the properies inside this object
	
	virtual Status get_status() const;
	virtual void set_status(Status p_status);	

	virtual String get_name() const;

	virtual void set_volume(float p_volume);
	virtual float get_volume() const;
	
	virtual int get_length() const; 
	virtual int get_pos() const; 
	virtual Error seek(Uint32 p_msec_pos); 

	virtual int get_BPM() const;
	virtual Uint64 get_BPM_sync() const; 

	virtual int get_repeats() const;

	Error load_stream(String p_file);
	void clear_stream();

	void poll(float p_time); ///< if nothing pollable found, you can always poll by hand

	AudioStreamOGGVorbis();
	~AudioStreamOGGVorbis();

};

#endif
