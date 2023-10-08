//
// C++ Implementation: audio_stream_ogg_vorbis
//
// Description: 
//
//
// Author: ,,, <red@lunatea>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "audio_stream_ogg_vorbis.h"
#include "audio/audio_server.h"
#include "scene/main/scene_main_loop.h"

size_t AudioStreamOGGVorbis::_ov_read_func(void *p_dst,size_t p_data, size_t p_count, void *_f) {

	//printf("read to %p, %i bytes, %i nmemb, %p\n",p_dst,p_data,p_count,_f);
	FileAccess *fa=(FileAccess*)_f;
	size_t read_total = p_data*p_count;
	
	if (fa->eof_reached())
		return 0;
	
	Uint8 *dst=(Uint8*)p_dst;
	
	for (size_t i=0;i<read_total;i++) {
	
		dst[i]=fa->get_8();
		if (fa->eof_reached())
			return i;
	}
	return read_total;
}
int AudioStreamOGGVorbis::_ov_seek_func(void *_f,ogg_int64_t offs, int whence) {

	//printf("seek to %p, offs %i, whence %i\n",_f,(int)offs,whence);
	
#ifdef SEEK_SET
	//printf("seek set defined\n");
	FileAccess *fa=(FileAccess*)_f;
	
	if (whence==SEEK_SET) {
	
		fa->seek(offs);			
	} else if (whence==SEEK_CUR) {
	
		fa->seek(fa->get_pos()+offs);			
	} else if (whence==SEEK_END) {

		fa->seek_end(offs);			
	} else {
		
		ERR_PRINT("BUG, wtf was whence set to?\n");
	}
	int ret=fa->eof_reached()?-1:0;
	//printf("returning %i\n",ret);
	return ret;

#else
	return -1; // no seeking
#endif

}
int AudioStreamOGGVorbis::_ov_close_func(void *_f) {

//	printf("close %p\n",_f);
	
	FileAccess *fa=(FileAccess*)_f;
	if (fa->is_open())
		fa->close();
	return 0;
}
long AudioStreamOGGVorbis::_ov_tell_func(void *_f) {

	//printf("close %p\n",_f);
	
	FileAccess *fa=(FileAccess*)_f;
	return fa->get_pos();	
}


void AudioStreamOGGVorbis::mix_callback(Sint32 *p_dest, int p_frames) {

	if (status!=PLAY)
		return;

	int write_pos_cache=rb_write_pos;
	
	Sint32 increment=(stream_srate*MIX_FRAC_LEN)/AudioServer::get_singleton()->get_stream_mix_rate();
	
	
	int rb_todo;
	
	if (write_pos_cache==rb_read_pos) {
		return;
	} else if (rb_read_pos<write_pos_cache) {
		
		rb_todo=write_pos_cache-rb_read_pos-1;
	} else {
		
		rb_todo=(RB_LEN-rb_read_pos)+write_pos_cache-1;
	}
	int todo = MIN( ((Sint64)rb_todo*increment)>>MIX_FRAC_BITS, p_frames );
			
	 
	for (int i=0;i<todo;i++) {
		
		offset = (offset + increment)&(((1<<(RB_BITS+MIX_FRAC_BITS))-1));
		Uint32 pos = offset >> MIX_FRAC_BITS;
		Uint32 frac = offset & MIX_FRAC_MASK;
		
		ERR_FAIL_COND(pos>=RB_LEN);
		Uint32 pos_next = (pos+1)&RB_MASK;
		//printf("rb pos %i\n",pos);		
		
		Sint32 l = rb[(pos<<1)+0];
		Sint32 r = rb[(pos<<1)+1];
		Sint32 l_next = rb[(pos_next<<1)+0];
		Sint32 r_next = rb[(pos_next<<1)+1];
		
		l=l+((l_next-l)*(Sint32)frac >> MIX_FRAC_BITS);
		r=r+((r_next-r)*(Sint32)frac >> MIX_FRAC_BITS);
		
		p_dest[(i<<1)+0]+=l*config_volume;
		p_dest[(i<<1)+1]+=r*config_volume;
		
	}
	
	
	rb_read_pos=offset>>MIX_FRAC_BITS;
	
/*
	
	for (int i=0;i<p_frames;i++) {
		
		if (((rb_read_pos+1)&RB_MASK)==write_pos_cache) {
			
			//p_dest[(i<<1)+0]=0;
			//p_dest[(i<<1)+1]=0;
		} else {
			
			p_dest[(i<<1)+0]+=((Sint32)rb[(rb_read_pos<<1)+0])*config_volume;
			p_dest[(i<<1)+1]+=((Sint32)rb[(rb_read_pos<<1)+1])*config_volume;
			rb_read_pos=(rb_read_pos+1)&RB_MASK;
		}
	}	
*/	
}

void AudioStreamOGGVorbis::poll(float p_time) {

	if (status!=PLAY)
		return;
		
	int todo;
	int read_pos_cache=rb_read_pos;
	
	if (read_pos_cache==rb_write_pos) {
		todo=RB_LEN-1;
	} else if (read_pos_cache>rb_write_pos) {
		
		todo=read_pos_cache-rb_write_pos-1;
	} else {
		
		todo=(RB_LEN-rb_write_pos)+read_pos_cache-1;
	}
 
	config_volume=Uint8( get_audio_type()==MUSIC?(AudioServer::get_singleton()->get_music_volume()*255.0):(AudioServer::get_singleton()->get_sfx_volume()*255.0));
		
	if (todo<RB_MIN_MIX)
		return; // 
	
	while (todo) {

		int to_mix=MIN(todo,(READ_BUFF_LEN/stream_channels));
		//printf("to mix %i - mix me %i bytes\n",to_mix,to_mix*stream_channels*sizeof(Sint16));
		
		long ret=ov_read(&vf,read_buf,to_mix*stream_channels*sizeof(Sint16),&current_section);
		if (ret<0) {
			
			ERR_PRINT("Error reading OGG Vorbis File");	
			break;
		} else if (ret==0) { // end of song, reload?
				
			ov_clear(&vf);	
			
			if (f->is_open())
				f->close();
				
			if (!has_loop()) {
			
				AudioServer::get_singleton()->lock();
				f->close();
	
				AudioServer::get_singleton()->cancel_output_stream( Method2<Sint32*,int>( this, &AudioStreamOGGVorbis::mix_callback ) );			
				status=STOP;			
				stream_loaded=false;
	
				AudioServer::get_singleton()->unlock();
				
				return;
			}
			
			f->open(file,FileAccess::READ);
			
			int errv = ov_open_callbacks(f,&vf,NULL,0,_ov_callbacks);
			if (errv!=0) {
				AudioServer::get_singleton()->lock();
				f->close();
	
				AudioServer::get_singleton()->cancel_output_stream( Method2<Sint32*,int>( this, &AudioStreamOGGVorbis::mix_callback ) );			
				status=STOP;			
				stream_loaded=false;
	
				AudioServer::get_singleton()->unlock();
				
				return; // :(
			}
						
			repeats++;
			continue;
			
		}
		
		ret/=stream_channels;
		ret/=sizeof(Sint16);
		//printf("mixed %i\n",ret);
		
		switch(stream_channels) {
					
			case 1: {
				
				for (int i=0;i<ret;i++) {
					
					rb[(rb_write_pos<<1)+0]=read_buf[i];
					rb[(rb_write_pos<<1)+1]=read_buf[i];
					rb_write_pos=(rb_write_pos+1)&RB_MASK;
				}
			} break;
			default: { // deal with the first 2 channels of everything
				
				for (int i=0;i<ret;i++) {
					
					rb[(rb_write_pos<<1)+0]=read_buf[i*stream_channels];
					rb[(rb_write_pos<<1)+1]=read_buf[i*stream_channels+1];
					rb_write_pos=(rb_write_pos+1)&RB_MASK;
					ERR_FAIL_COND(rb_write_pos==read_pos_cache);

				}
			} break;
		}
		
		todo-=ret;
	}	

}


void AudioStreamOGGVorbis::set(String p_path, const Variant& p_value) {
	
	if (p_path=="file") {
	
		if (p_value.get_string()=="") {
			clear_stream();
			file="";
		} else {
			file="";
			load_stream(p_value.get_string());		
		}
	}
		
}
Variant AudioStreamOGGVorbis::get(String p_path) const {
	
	if (p_path=="file")
		return file;
	
	return Variant();
}

void AudioStreamOGGVorbis::get_property_list( List<PropertyInfo> *p_list ) const {
	
	p_list->push_back( PropertyInfo( Variant::STRING, "file" ) );
	
}



IAPI::PropertyHint AudioStreamOGGVorbis::get_property_hint(String p_path) const {

	if (p_path=="file") {
	
		PropertyHint hint;
		hint.type=PropertyHint::HINT_FILE;
		hint.hint=".ogg";
		hint.description="Set to load an OGG-Vorbis stream.";
		return hint;
	}
	
	return PropertyHint();
}

AudioStream::Status AudioStreamOGGVorbis::get_status() const  {

	return status;
}
void AudioStreamOGGVorbis::set_status(Status p_status)  {

	if (status==p_status)
		return;
		
	switch (p_status) {
	
		case PLAY: {
		
			if (status==STOP) {
			
			
				if (load_stream(file)==OK) {
				
				
					AudioServer::get_singleton()->lock();

					AudioServer::get_singleton()->request_output_stream( Method2<Sint32*,int>( this, &AudioStreamOGGVorbis::mix_callback ) );
					
					AudioServer::get_singleton()->unlock();				
					
					if (SceneMainLoop::get_singleton())
						SceneMainLoop::get_singleton()->frame_process_signal.connect( this, &AudioStreamOGGVorbis::poll);
					
				} else
					return; 
					
			}
		
		} break;
		case STOP: {
		
			clear_stream();		
		} break;
		default: {};
	}
	
	status=p_status;
		
}

String AudioStreamOGGVorbis::get_name() const  {

	return "";
}

void AudioStreamOGGVorbis::set_volume(float p_volume)  {


}
float AudioStreamOGGVorbis::get_volume() const  {

	return 1.0;
}

int AudioStreamOGGVorbis::get_length() const  {

	return 0;
}
int AudioStreamOGGVorbis::get_pos() const  {

	return -1;
}
Error AudioStreamOGGVorbis::seek(Uint32 p_msec_pos)  {

	return OK;
}

int AudioStreamOGGVorbis::get_BPM() const  {

	return -1;
}
Uint64 AudioStreamOGGVorbis::get_BPM_sync() const  {

	return 0;
}

int AudioStreamOGGVorbis::get_repeats() const  {

	return repeats;
}




Error AudioStreamOGGVorbis::load_stream(String p_file)  {

	if (stream_loaded)
		clear_stream();

	AudioServer::get_singleton()->lock();

	status=STOP;
	file="";
	
	Error err = f->open(p_file,FileAccess::READ);

	if (err) {
		AudioServer::get_singleton()->unlock();
		
		ERR_FAIL_COND_V( err, err );
	}
	
	int errv = ov_open_callbacks(f,&vf,NULL,0,_ov_callbacks);

	rb_read_pos=0;
	rb_write_pos=0;

	const vorbis_info *vinfo=ov_info(&vf,-1);
	stream_channels=vinfo->channels;
	stream_srate=vinfo->rate;
	offset=0;
			
	AudioServer::get_singleton()->unlock();

	switch(errv) {
	
		case OV_EREAD: { // - A read from media returned an error.
			f->close();
			ERR_FAIL_V( ERR_FILE_CANT_READ );
		} break;
		case OV_EVERSION:  // - Vorbis version mismatch.
		case OV_ENOTVORBIS: { // - Bitstream is not Vorbis data.
			f->close();
			ERR_FAIL_V( ERR_FILE_UNRECOGNIZED );
		} break;		
		case OV_EBADHEADER: { // - Invalid Vorbis bitstream header.
			f->close();
			ERR_FAIL_V( ERR_FILE_CORRUPT );
		} break;
		case OV_EFAULT: { // - Internal logic fault; indicates a bug or heap/stack corruption.	
		
			f->close();
			ERR_FAIL_V( ERR_BUG );
		} break;
	}
	
	file=p_file;		
	repeats=0;
	stream_loaded=true;
	
	return OK;
}
void AudioStreamOGGVorbis::clear_stream() {

	if (!stream_loaded)
		return;
	
	AudioServer::get_singleton()->lock();

	status=STOP;
	ov_clear(&vf);
	f->close();
	stream_loaded=false;

	AudioServer::get_singleton()->cancel_output_stream( Method2<Sint32*,int>( this, &AudioStreamOGGVorbis::mix_callback ) );

	AudioServer::get_singleton()->unlock();
	
	if (SceneMainLoop::get_singleton())
		SceneMainLoop::get_singleton()->frame_process_signal.disconnect( this, &AudioStreamOGGVorbis::poll);
}

AudioStreamOGGVorbis::AudioStreamOGGVorbis() {

	status=STOP;
	_ov_callbacks.read_func=_ov_read_func;
	_ov_callbacks.seek_func=_ov_seek_func;
	_ov_callbacks.close_func=_ov_close_func;
	_ov_callbacks.tell_func=_ov_tell_func;
	f = FileAccess::create();
	stream_loaded=false;
	repeats=0;
	stream_channels=0;
	stream_srate=0;
	current_section=0;	
	config_volume=255;
	
	for (int i=0;i<(RB_LEN*2);i++) {
		
		rb[i]=0;	
	}
}


AudioStreamOGGVorbis::~AudioStreamOGGVorbis() {

	clear_stream();
	memdelete(f);
}


