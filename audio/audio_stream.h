//
// C++ Interface: audio_stream
//
// Description: 
//
//
// Author: ,,, <red@lunatea>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef AUDIO_STREAM_H
#define AUDIO_STREAM_H


#include "iapi.h"
#include "typedefs.h"

/**
	@author ,,, <red@lunatea>
*/
class AudioStream : public IAPI {

	IAPI_TYPE( AudioStream,IAPI );
public:	
	enum Type {
		MUSIC,
  		SFX			
	};
private:		
	Type type;
	bool loop;
public:

	enum Status {
		PLAY,
		PAUSE,
		STOP	
	};
		
	virtual Variant call(String p_method, const List<Variant>& p_params=List<Variant>());	
	virtual void get_method_list( List<MethodInfo> *p_list ) const;		
	
	// helpers
	void play();
	void pause();
	void stop();	
	
	void set_loop(bool p_loop);
	bool has_loop() const;
	
	virtual Status get_status() const=0;
	virtual void set_status(Status p_status)=0;
	

	virtual String get_name() const=0; //< get name of the stream

	virtual void set_volume(float p_volume)=0;
	virtual float get_volume() const=0;
	
	virtual int get_length() const=0; ///< -1 if no length available
	virtual int get_pos() const=0; ///< pos in msec, -1 if no length available
	virtual Error seek(Uint32 p_msec_pos)=0 ; ///< seek to msec pos (error may be raised it not available.

	virtual int get_repeats() const=0;

	virtual int get_BPM() const=0; ///< get BPM, -1 if no BPM available
	virtual Uint64 get_BPM_sync() const=0; ///< get usec tick for the last beat (check "Time::get_ticks_usec" )
	void set_audio_type(Type p_type);
	Type get_audio_type() const;

	AudioStream();
	~AudioStream();
};

#endif
