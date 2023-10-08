//
// C++ Interface: audio_streamxm
//
// Description: 
//
//
// Author: ,,, <red@lunatea>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//



#ifndef AUDIO_STREAMXM_H
#define AUDIO_STREAMXM_H


#include "audio/audio_stream.h"
#include "audio/audio_sample_manager.h"
#include "audio/audio_mixer.h"

class FileAccess;
/**
	@author ,,, <red@lunatea>
*/
class AudioStreamXM : public AudioStream {

	IAPI_TYPE( AudioStreamXM, AudioStream );
	IAPI_INSTANCE_FUNC( AudioStreamXM );
	
	enum {
	
		XM_CONSTANT_MAX_NOTES=96,
		XM_CONSTANT_MAX_PATTERNS=255,
		XM_CONSTANT_MAX_INSTRUMENTS=255,
		XM_CONSTANT_MAX_ENVELOPE_POINTS=24,
		XM_CONSTANT_MAX_SAMPLES_PER_INSTRUMENT=16,
		XM_CONSTANT_MAX_ORDERS=256,
		XM_INVALID_SAMPLE_ID=-1
	
	};
	
	struct XM_Sample {
	
		AudioSampleID sample_id;
		Sint8 base_note;
		Sint8 finetune;
		Uint8 volume;
		Uint8 pan;
	
		
		XM_Sample() { sample_id=AUDIO_SAMPLE_INVALID_ID; }
	};

	enum {
	
		XM_ENVELOPE_POINT_COUNT_MASK=(1<<5)-1, /** First 5 bits indicade point count */
		XM_ENVELOPE_ENABLED=(1<<7), /** If Loop is enabled, Enable Sustain */
		XM_ENVELOPE_LOOP_ENABLED=(1<<6), /** Loop is enabled **/
		XM_ENVELOPE_SUSTAIN_ENABLED=(1<<5), /** If Loop is enabled, Enable Sustain */	
	};



	struct XM_Envelope {
	
		Uint16 points[XM_CONSTANT_MAX_ENVELOPE_POINTS]; /* Envelope points */
		Uint8 flags; /* Envelope Flags & Point count ( XM_EnvelopeFlags ) */
	
		Uint8 sustain_index; /* Sustain Point Index */
		Uint8 loop_begin_index; /* Loop Begin Point Index */
		Uint8 loop_end_index; /* Loop End  Point Index */
		
	};
	
	enum XM_VibratoType {
	
		XM_VIBRATO_SINE,
		XM_VIBRATO_SQUARE,
		XM_VIBRATO_SAW_UP,
		XM_VIBRATO_SAW_DOWN
	
	};

	struct XM_Instrument {

		/* Envelopes */
	
		XM_Envelope volume_envelope;
		XM_Envelope pan_envelope;
	
		/* Instrument Vibratto */
		XM_VibratoType vibrato_type;
	
		Uint8 vibrato_sweep;
		Uint8 vibrato_depth;
		Uint8 vibrato_rate;
		Uint16 fadeout;
	
		/* Note/Sample table */
	
		Uint8 note_sample[XM_CONSTANT_MAX_NOTES/2]; /* sample for each note, in nibbles */
	
		/* Sample Data  */
	
		XM_Sample * samples;
		Uint8 sample_count;

	};


	/** SONG **/
	
	enum {
		XM_SONG_FLAGS_LINEAR_PERIODS=1<<7,
		XM_SONG_FLAGS_MASK_CHANNELS_USED=(1<<5)-1 /** flags&XM_SONG_FLAGS_MASK_CHANNELS_USED + 1 to obtain**/
	
	};

	/* SONG HEADER, size in mem: 272 bytes */
	
	struct XM_Song {
	
		char name[21];
		Uint8 restart_pos;
		Uint8 order_count;
		Uint8 flags; /* flags, defined in SongFlags (including channels used) */
	
		Uint8 pattern_count;
		Uint8 instrument_count;
		Uint8 tempo;
		Uint8 speed;
	
		Uint8 ** pattern_data; /* array of pointers to patern data,using xm packing, NULL means empty pattern */
		XM_Instrument ** instrument_data;
	
		Uint8 order_list[256];
		
	};
	
	enum XM_PlayerConstants {

		XM_NOTE_OFF=97,
		XM_FIELD_EMPTY=0xFF,
		XM_COMP_NOTE_BIT=1,
		XM_COMP_INSTRUMENT_BIT=2,
		XM_COMP_VOLUME_BIT=4,
		XM_COMP_COMMAND_BIT=8,
		XM_COMP_PARAMETER_BIT=16,
		XM_MAX_CHANNELS=32	
	};

	
	struct XM_Note {
	
		Uint8 note; /* 0 .. 96 , 97 is note off */
		Uint8 instrument; /* 0 .. 127 */
		Uint8 volume; /* xm volume < 0x10 means no volume */
		Uint8 command; /* xm command, 255 means no command */
		Uint8 parameter; /* xm parameter, 0x00 to 0xFF */
	
	};
	
	static const XM_Note xm_empty_note;

	
	struct XM_EnvelopeProcess{
	
		Sint16 tick; /* current tick */
		Uint8 current_point; /* current node where ticks belong to (for speed) */
		Uint8 value;
		bool done;
	
	};
	
	struct XM_Channel {
	
		AudioVoiceID mixer_voice_id;
		
		XM_EnvelopeProcess volume_envelope;
		XM_EnvelopeProcess pan_envelope;
	
		XM_Instrument *instrument_ptr;
		XM_Sample *sample_ptr;
	
		Uint32 note_start_offset;
	
		Sint32 period; /* channel period  */
		Sint32 old_period; /* period before note */
		Sint32 tickrel_period; /* only for this tick, relative period added to output..  */
		Sint32 porta_period; /* target period for portamento  */
	
		Uint8 note; /* last note parsed */
		Uint8 instrument; /* last instrument parsed */
		Uint8 command;
		Uint8 parameter;
	
	
		bool active;
		bool note_off;
		Uint8 volume;
		Uint8 pan;
	
		Uint8 sample;
		bool portamento_active;
		bool row_has_note;
		bool restart;
	
		Uint32 restart_offset;
	
		Sint16 tickrel_volume;
		Uint16 fadeout;
		Sint16 real_note; /* note + note adjustment in sample */
		Sint8 finetune; /* finetune used */
		Uint8 volume_command;
		Uint8 note_delay;
	
		/* effect memories */
		Uint8 fx_1_memory;
		Uint8 fx_2_memory;
		Uint8 fx_3_memory;
		Uint8 fx_4_memory;
		Sint8 fx_4_vibrato_phase;
		Uint8 fx_4_vibrato_speed;
		Uint8 fx_4_vibrato_depth;
		Uint8 fx_4_vibrato_type;
		Uint8 fx_5_memory;
		Uint8 fx_6_memory;
		Sint8 fx_7_tremolo_phase;
		Uint8 fx_7_tremolo_speed;
		Uint8 fx_7_tremolo_depth;
		Uint8 fx_7_tremolo_type;
		Uint8 fx_A_memory;
		Uint8 fx_E1_memory;
		Uint8 fx_E2_memory;
		Uint8 fx_EA_memory;
		Uint8 fx_EB_memory;
		Uint8 fx_H_memory;
		Uint8 fx_P_memory;
		Uint8 fx_R_memory;
		Uint8 fx_X1_memory;
		Uint8 fx_X2_memory;
	
		XM_Channel() { mixer_voice_id=AUDIO_VOICE_INVALID_ID; }
	};
	
	struct XM_PatternDecompressor{
	
		Uint8 caches[XM_MAX_CHANNELS][5];
		Uint8 *pattern_ptr;
		Uint8 *pos_ptr;
		Sint16 row;
		Sint8 channel;
		Sint16 last_row; /* last requested row */
		Sint8 last_channel; /* last requested row */
	
	};
	
	
	struct XM_PlayerData {
	
		XM_Channel channel[XM_MAX_CHANNELS];
	
		XM_Song *song;
	
		Uint8 tick_counter;
		Uint8 current_speed;
		Uint8 current_tempo;
		Uint8 global_volume;
	
		/* Position */
	
		Uint16 current_order;
		Uint16 current_row;
		Uint16 current_pattern;
		Uint16 pattern_delay;
	
		/* flags */
	
		bool force_next_order; /* force an order change, XM commands Dxx, Bxx */
		Uint8 forced_next_order; /* order change */
		Uint16 forced_next_order_row; /* order change */
		bool active;
	
		XM_PatternDecompressor decompressor;
	
	};
	
	XM_Song *xm_song;
	XM_PlayerData *xm_player;
	
	void xm_player_reset();
	Uint16 xm_player_get_pattern_length(Uint8 p_pattern);
	void xm_player_reset_decompressor();
	XM_Note xm_player_decompress_note(Uint8 p_row, Uint8 p_channel);
	void xm_player_envelope_reset(XM_EnvelopeProcess *p_env_process );	
	Uint32 xm_player_get_linear_period(Sint16 p_note, Sint8 p_fine);
	Uint32 xm_player_get_period(Sint16 p_note, Sint8 p_fine);
	Uint32 xm_player_get_linear_frequency(Uint32 p_period);
	Uint32 xm_player_get_frequency(Uint32 p_period);
	void xm_process_notes();
	void xm_player_process_envelope( XM_Channel *ch, XM_EnvelopeProcess *p_env_process, XM_Envelope *p_envelope);
	void xm_player_do_vibrato(XM_Channel *ch);
	void xm_player_do_tone_porta(XM_Channel *ch);
	void xm_player_do_volume_slide_down(XM_Channel *ch, Uint8 val);
	void xm_player_do_volume_slide_up(XM_Channel *ch, Uint8 val);
	void xm_player_do_volume_slide(XM_Channel *ch);
	void xm_player_process_effects_and_envelopes();
	void xm_player_update_mixer();
	
	void xm_player_process_tick();
	
	void xm_player_play();
	void xm_player_stop();

	void xm_clear_song( XM_Song *p_song, Sint32 p_pattern_count, Sint32 p_instrument_count );
	Uint32 xm_recompress_pattern(Uint16 p_rows,Uint8 p_channels,void * p_dst_data);
	Error xm_loader_open_song_custom( String p_filename, XM_Song *p_song, bool p_load_music, bool p_load_instruments );

	Error xm_loader_open_song( String p_filename, XM_Song *p_song );
	Error xm_loader_open_song_music( String p_filename, XM_Song *p_song );
	Error xm_loader_open_instruments( String p_filename, XM_Song *p_song );

	
	void xm_loader_free_song( XM_Song *p_song );
	void xm_loader_free_music( XM_Song *p_song );
	void xm_loader_free_instruments( XM_Song *p_song );
	
	int repeat_counter;
	int last_order;
	
	Uint32 mixer_tick_size;
	void mixer_tick_callback(Uint32 p_usec);
	
	FileAccess *xm_fileio;
	
	Status status;
	
	Uint8 config_volume;
	
	String file;
	
	void clear_all_voices();
	
public:

	virtual void set(String p_path, const Variant& p_value); /** set a property, by path */
	virtual Variant get(String p_path) const; /** get a property, by path */	
	
	virtual void get_property_list( List<PropertyInfo> *p_list ) const; ///< Get a list with all the properies inside this object
	virtual PropertyHint get_property_hint(String p_path) const;
	
	
	virtual Status get_status() const;
	virtual void set_status(Status p_status);	

	virtual String get_name() const; //< get name of the stream

	virtual void set_volume(float p_volume);
	virtual float get_volume() const;
	
	virtual int get_length() const; ///< -1 if no length available
	virtual int get_pos() const; ///< pos in msec, -1 if no length available
	virtual Error seek(Uint32 p_msec_pos); ///< seek to msec pos (error may be raised it not available.

	virtual int get_BPM() const; ///< get BPM, -1 if no BPM available
	virtual Uint64 get_BPM_sync() const; ///< get usec tick for the last beat (check "Time::get_ticks_usec" )


	virtual int get_repeats() const;

	Error load_song(String p_file);
	Error load_song_music(String p_file);
	Error load_song_instruments(String p_file);
	void clear_song();

	AudioStreamXM();	
	~AudioStreamXM();

};

#endif
