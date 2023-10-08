//
// C++ Implementation: audio_streamxm
//
// Description: 
//
//
// Author: ,,, <red@lunatea>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//


//#define XM_DEBUG

#ifdef XM_DEBUG

#include <stdio.h>

#define XM_DEBUG_PRINTF printf
#define XM_ERROR_PRINTF printf

#else

#define XM_DEBUG_PRINTF( ... ) 
#define XM_ERROR_PRINTF( ... ) 

#endif

#include "audio_stream_xm.h"
#include "os/file_access.h"
#include "audio/audio_server.h"
#include "os/memory.h"
/* macros to inrerpret the envelope points */
#define XM_ENV_OFFSET( m_point ) ((m_point)&0x1FF)
#define XM_ENV_VALUE( m_point ) ((m_point)>>9)


#define XM_AUDIO_LOCK { if (AudioServer::get_singleton()) AudioServer::get_singleton()->lock(); }
#define XM_AUDIO_UNLOCK { if (AudioServer::get_singleton()) AudioServer::get_singleton()->unlock(); }



static void xm_zero_mem(void * p_mem, Uint32 p_bytes) {

	Uint8 *mem=(Uint8*)p_mem;
	Uint32 i;
	for (i=0;i<p_bytes;i++)
		mem[i]=0;
}


void AudioStreamXM::xm_player_reset() {


	int i;
	XM_PlayerData *p=xm_player;

	for (i=0;i<XM_MAX_CHANNELS;i++) {

		xm_zero_mem( &p->channel[i], sizeof( XM_Channel) );
		p->channel[i].pan=32;
		p->channel[i].mixer_voice_id=AUDIO_VOICE_INVALID_ID;
	}

	if (p->song) {

		p->current_speed=p->song->speed;
		p->current_tempo=p->song->tempo;
	} else {

		p->current_speed=5;
		p->current_tempo=120;

	}

	p->tick_counter=p->current_speed; /* so it starts on a row */

	/* Position */

	p->current_order=0;
	p->current_row=0;
	p->pattern_delay=0;

	if (p->song) {
		p->current_pattern=p->song->order_list[0];
	}
	p->force_next_order=false;
	p->forced_next_order=0;
	p->forced_next_order_row=0;
	p->active=false;
	p->global_volume=64; /* 0x40 */
	xm_zero_mem( &p->decompressor, sizeof( XM_PatternDecompressor) );

}

Uint16 AudioStreamXM::xm_player_get_pattern_length(Uint8 p_pattern) {

	XM_PlayerData *p=xm_player;

	if (p_pattern >= p->song->pattern_count || !p->song->pattern_data[p_pattern])
		return 64;
	else
		return (Uint16)*p->song->pattern_data[p_pattern]+1;
}


void AudioStreamXM::xm_player_reset_decompressor() {

	XM_PlayerData *p=xm_player;


	if (p->decompressor.pattern_ptr) {

		int i;
		/* reset caches */
		for (i=0;i<XM_MAX_CHANNELS;i++) {

			p->decompressor.caches[i][0]=XM_FIELD_EMPTY;
			p->decompressor.caches[i][1]=XM_FIELD_EMPTY;
			p->decompressor.caches[i][2]=XM_FIELD_EMPTY;
			p->decompressor.caches[i][3]=XM_FIELD_EMPTY;
			p->decompressor.caches[i][4]=0; /* only values other than 0 are read for this as cache */
		}

		/* set stream pointer */

		p->decompressor.pos_ptr=&p->decompressor.pattern_ptr[1];
	} else {

		p->decompressor.pos_ptr=0;
	}

	/* set stream pos */

	p->decompressor.row=0;
	p->decompressor.channel=0;
	p->decompressor.last_row=0;
	p->decompressor.last_channel=0;

}

const AudioStreamXM::XM_Note AudioStreamXM::xm_empty_note={ AudioStreamXM::XM_FIELD_EMPTY, AudioStreamXM::XM_FIELD_EMPTY, 0, AudioStreamXM::XM_FIELD_EMPTY, 0 };

typedef enum {

	XM_DCMP_CMD_CHAN   =0,  /*000,   set channel */
	XM_DCMP_CMD_CACHE  =1,  /*001,   use caches */
	XM_DCMP_CMD_FIELD  =2,  /*010,   read fields */
	XM_DCMP_CMD_EOP    =3,  /*011,   end of pattern */
	XM_DCMP_CMD_CHANR  =4,  /*100,   read chan, advance row*/
	/* XM_DCMP_CMD_CACHEC =5,  101,   advance one chan, use caches,  */
 	/* XM_DCMP_CMD_FIELDC =6,  110,   advance one chan, read fields,  */
	XM_DCMP_CMD_ADVROW =7   /*111,   advance rows  */

} XM_PatternCompressionCommand;

AudioStreamXM::XM_Note AudioStreamXM::xm_player_decompress_note(Uint8 p_row, Uint8 p_channel) {

	XM_Note n=xm_empty_note;
	Uint8 * current_pattern_ptr=0;
	XM_PlayerData *p=xm_player;

	/* check current pattern validity */
	if (p->current_pattern < p->song->pattern_count && p->song->pattern_data[p->current_pattern]) {

		current_pattern_ptr=p->song->pattern_data[p->current_pattern];
	}

	/* if pattern changed, reset decompressor */
	if (current_pattern_ptr!=p->decompressor.pattern_ptr) {

		p->decompressor.pattern_ptr=current_pattern_ptr;
		xm_player_reset_decompressor();
	}

	/* check for empty pattern, return empty note if pattern is empty */

	if (!current_pattern_ptr) {

		return xm_empty_note;
	}

	/* check if the position is requested is behind the seekpos,
	   and not after. If so, reset decompressor and seek to begining */

	if (p->decompressor.last_row > p_row || (p->decompressor.last_row == p_row && p->decompressor.last_channel > p_channel)) {

		xm_player_reset_decompressor();
	}

	while( p->decompressor.row < p_row || ( p->decompressor.row == p_row && p->decompressor.channel <= p_channel ) ) {

		Uint8 cmd = *p->decompressor.pos_ptr;
		int i;

		/*printf("pdecomp cmd %i%i%i(%i) - data %i\n",(cmd>>7)&1,(cmd>>6)&1,(cmd>>5)&1,cmd>>5,cmd&0x1F);*/

		/* at end of pattern, just break */
		if ((cmd>>5)==XM_DCMP_CMD_EOP)
			break;

		switch(cmd>>5) {

			case XM_DCMP_CMD_CHANR:

				p->decompressor.row++;

			case XM_DCMP_CMD_CHAN: {

				p->decompressor.channel=cmd&0x1F; /* channel in lower 5 bits */
			} break;
			case XM_DCMP_CMD_FIELD:

				 /* read fields into the cache */
				for (i=0;i<5;i++) {

					if (cmd&(1<<i)) {

						p->decompressor.pos_ptr++;
						p->decompressor.caches[p->decompressor.channel][i]=*p->decompressor.pos_ptr;
					}
				}
				/* fallthrough because values must be read */
			case XM_DCMP_CMD_CACHE: {


				/* if not the same position, break */
				if (p->decompressor.row!=p_row || p->decompressor.channel!=p_channel)
					break;

				/* otherwise assign the caches to the note */

				if (cmd&XM_COMP_NOTE_BIT)
					n.note=p->decompressor.caches[p_channel][0];
				if (cmd&XM_COMP_INSTRUMENT_BIT)
					n.instrument=p->decompressor.caches[p_channel][1];
				if (cmd&XM_COMP_VOLUME_BIT)
					n.volume=p->decompressor.caches[p_channel][2]+0x10;
				if (cmd&XM_COMP_COMMAND_BIT)
					n.command=p->decompressor.caches[p_channel][3];
				if (cmd&XM_COMP_PARAMETER_BIT)
					n.parameter=p->decompressor.caches[p_channel][4];


			} break;
			case XM_DCMP_CMD_EOP: {

				XM_ERROR_PRINTF("Code should never reach here!");
			} break;
			case XM_DCMP_CMD_ADVROW: {

				p->decompressor.row+=(cmd&0x1F)+1;
				p->decompressor.channel=0;
			} break;
			default:
				XM_ERROR_PRINTF("INVALID COMMAND!");

		}

		/* if not at end of pattern, advance one byte */
		p->decompressor.pos_ptr++;

	}

	p->decompressor.last_row=p_row;
	p->decompressor.last_channel=p_channel;

	/*if (	n.note != xm_empty_note.note ||
		n.instrument != xm_empty_note.instrument ||
		n.volume != xm_empty_note.volume ||
		n.command != xm_empty_note.command ||
		n.parameter != xm_empty_note.parameter )
		printf("row/col: %i,%i   - %i,%i,%i,%i,%i\n",p_row,p_channel,n.note,n.instrument,n.volume,n.command,n.parameter);*/
	return n;
}

void AudioStreamXM::xm_player_envelope_reset(XM_EnvelopeProcess *p_env_process ) {

	p_env_process->tick=0;
	p_env_process->current_point=0;
	p_env_process->value=0;
	p_env_process->done=false;
}


static const Uint16 xm_amiga_period_table[12*8+1]={

	907,900,894,887,881,875,868,862,
	856,850,844,838,832,826,820,814,
	808,802,796,791,785,779,774,768,
	762,757,752,746,741,736,730,725,
	720,715,709,704,699,694,689,684,
	678,675,670,665,660,655,651,646,
	640,636,632,628,623,619,614,610,
	604,601,597,592,588,584,580,575,
	570,567,563,559,555,551,547,543,
	538,535,532,528,524,520,516,513,
	508,505,502,498,494,491,487,484,
	480,477,474,470,467,463,460,457,454 /* plus one for interpolation */
};

/* this python script can generate the table below:

import math;
for x in range(0,768):
	print 8363.0*math.pow(2.0,(6.0*12.0*16.0*4.0 - x)/ (12*16*4))

*/


 static const Uint32 xm_linear_frequency_table[768]={

	535232,534749,534266,533784,533303,532822,532341,531861,
	531381,530902,530423,529944,529466,528988,528511,528034,
	527558,527082,526607,526131,525657,525183,524709,524236,
	523763,523290,522818,522346,521875,521404,520934,520464,
	519994,519525,519057,518588,518121,517653,517186,516720,
	516253,515788,515322,514858,514393,513929,513465,513002,
	512539,512077,511615,511154,510692,510232,509771,509312,
	508852,508393,507934,507476,507018,506561,506104,505647,
	505191,504735,504280,503825,503371,502917,502463,502010,
	501557,501104,500652,500201,499749,499298,498848,498398,
	497948,497499,497050,496602,496154,495706,495259,494812,
	494366,493920,493474,493029,492585,492140,491696,491253,
	490809,490367,489924,489482,489041,488600,488159,487718,
	487278,486839,486400,485961,485522,485084,484647,484210,
	483773,483336,482900,482465,482029,481595,481160,480726,
	480292,479859,479426,478994,478562,478130,477699,477268,
	476837,476407,475977,475548,475119,474690,474262,473834,
	473407,472979,472553,472126,471701,471275,470850,470425,
	470001,469577,469153,468730,468307,467884,467462,467041,
	466619,466198,465778,465358,464938,464518,464099,463681,
	463262,462844,462427,462010,461593,461177,460760,460345,
	459930,459515,459100,458686,458272,457859,457446,457033,
	456621,456209,455797,455386,454975,454565,454155,453745,
	453336,452927,452518,452110,451702,451294,450887,450481,
	450074,449668,449262,448857,448452,448048,447644,447240,
	446836,446433,446030,445628,445226,444824,444423,444022,
	443622,443221,442821,442422,442023,441624,441226,440828,
	440430,440033,439636,439239,438843,438447,438051,437656,
	437261,436867,436473,436079,435686,435293,434900,434508,
	434116,433724,433333,432942,432551,432161,431771,431382,
	430992,430604,430215,429827,429439,429052,428665,428278,
	427892,427506,427120,426735,426350,425965,425581,425197,
	424813,424430,424047,423665,423283,422901,422519,422138,
	421757,421377,420997,420617,420237,419858,419479,419101,
	418723,418345,417968,417591,417214,416838,416462,416086,
	415711,415336,414961,414586,414212,413839,413465,413092,
	412720,412347,411975,411604,411232,410862,410491,410121,
	409751,409381,409012,408643,408274,407906,407538,407170,
	406803,406436,406069,405703,405337,404971,404606,404241,
	403876,403512,403148,402784,402421,402058,401695,401333,
	400970,400609,400247,399886,399525,399165,398805,398445,
	398086,397727,397368,397009,396651,396293,395936,395579,
	395222,394865,394509,394153,393798,393442,393087,392733,
	392378,392024,391671,391317,390964,390612,390259,389907,
	389556,389204,388853,388502,388152,387802,387452,387102,
	386753,386404,386056,385707,385359,385012,384664,384317,
	383971,383624,383278,382932,382587,382242,381897,381552,
	381208,380864,380521,380177,379834,379492,379149,378807,
	378466,378124,377783,377442,377102,376762,376422,376082,
	375743,375404,375065,374727,374389,374051,373714,373377,
	373040,372703,372367,372031,371695,371360,371025,370690,
	370356,370022,369688,369355,369021,368688,368356,368023,
	367691,367360,367028,366697,366366,366036,365706,365376,
	365046,364717,364388,364059,363731,363403,363075,362747,
	362420,362093,361766,361440,361114,360788,360463,360137,
	359813,359488,359164,358840,358516,358193,357869,357547,
	357224,356902,356580,356258,355937,355616,355295,354974,
	354654,354334,354014,353695,353376,353057,352739,352420,
	352103,351785,351468,351150,350834,350517,350201,349885,
	349569,349254,348939,348624,348310,347995,347682,347368,
	347055,346741,346429,346116,345804,345492,345180,344869,
	344558,344247,343936,343626,343316,343006,342697,342388,
	342079,341770,341462,341154,340846,340539,340231,339924,
	339618,339311,339005,338700,338394,338089,337784,337479,
	337175,336870,336566,336263,335959,335656,335354,335051,
	334749,334447,334145,333844,333542,333242,332941,332641,
	332341,332041,331741,331442,331143,330844,330546,330247,
	329950,329652,329355,329057,328761,328464,328168,327872,
	327576,327280,326985,326690,326395,326101,325807,325513,
	325219,324926,324633,324340,324047,323755,323463,323171,
	322879,322588,322297,322006,321716,321426,321136,320846,
	320557,320267,319978,319690,319401,319113,318825,318538,
	318250,317963,317676,317390,317103,316817,316532,316246,
	315961,315676,315391,315106,314822,314538,314254,313971,
	313688,313405,313122,312839,312557,312275,311994,311712,
	311431,311150,310869,310589,310309,310029,309749,309470,
	309190,308911,308633,308354,308076,307798,307521,307243,
	306966,306689,306412,306136,305860,305584,305308,305033,
	304758,304483,304208,303934,303659,303385,303112,302838,
	302565,302292,302019,301747,301475,301203,300931,300660,
	300388,300117,299847,299576,299306,299036,298766,298497,
	298227,297958,297689,297421,297153,296884,296617,296349,
	296082,295815,295548,295281,295015,294749,294483,294217,
	293952,293686,293421,293157,292892,292628,292364,292100,
	291837,291574,291311,291048,290785,290523,290261,289999,
	289737,289476,289215,288954,288693,288433,288173,287913,
	287653,287393,287134,286875,286616,286358,286099,285841,
	285583,285326,285068,284811,284554,284298,284041,283785,
	283529,283273,283017,282762,282507,282252,281998,281743,
	281489,281235,280981,280728,280475,280222,279969,279716,
	279464,279212,278960,278708,278457,278206,277955,277704,
	277453,277203,276953,276703,276453,276204,275955,275706,
	275457,275209,274960,274712,274465,274217,273970,273722,
	273476,273229,272982,272736,272490,272244,271999,271753,
	271508,271263,271018,270774,270530,270286,270042,269798,
	269555,269312,269069,268826,268583,268341,268099,267857
 };
 
Uint32 AudioStreamXM::xm_player_get_linear_period(Sint16 p_note, Sint8 p_fine) {
	
	Sint32 period = 10*12*16*4 - (Sint32)(p_note-1)*16*4 - (Sint32)p_fine/2;

	if (period<0)
		period=0;

	return (Uint32)period;
	
}
 
Uint32 AudioStreamXM::xm_player_get_period(Sint16 p_note, Sint8 p_fine) {

	XM_PlayerData *p=xm_player;

	/* Get period, specified by fasttracker xm.txt */

	if (p->song->flags&XM_SONG_FLAGS_LINEAR_PERIODS) {

		return xm_player_get_linear_period(p_note,p_fine);
	} else { /* amiga periods */

		Sint16 fine=p_fine;
		Sint16 note=p_note;
		Uint16 idx; /* index in amiga table */
		Sint16 period;

		/* fix to fit table, note always positive and fine always 0 .. 127 */
		if (fine<0) {

			fine+=128;
			--note;
		}

		if (note<0)
			note=0;

		/* find index in table */
		idx = ((note%12)<<3) + (fine >> 4);

		period = xm_amiga_period_table[idx];
		/* interpolate for further fine-ness :) */
		period+= ((fine&0xF)*(xm_amiga_period_table[idx+1]-period)) >> 4;

		/* apply octave */
		period=((period<<4)/(1<<(note/12)))<<1;
		return (Uint32)period;

	}
}

Uint32 AudioStreamXM::xm_player_get_linear_frequency(Uint32 p_period) {

	Sint32 shift=(((Sint32)p_period/768)-0);

	if (shift>0) {

		return xm_linear_frequency_table[p_period%768]>>shift;
	} else {

		return xm_linear_frequency_table[p_period%768]<<(-shift);
	}
}

Uint32 AudioStreamXM::xm_player_get_frequency(Uint32 p_period) {

	XM_PlayerData *p=xm_player;

	/* Get frequency, specified by fasttracker xm.txt */

	if (p->song->flags&XM_SONG_FLAGS_LINEAR_PERIODS) {

		return xm_player_get_linear_frequency(p_period);

	} else { /* amiga */

		return 8363*1712/p_period;

	}
}

void AudioStreamXM::xm_process_notes() {

	XM_PlayerData *p=xm_player;
	int i;
	int channels=(p->song->flags&XM_SONG_FLAGS_MASK_CHANNELS_USED)+1;

	for (i=0;i<channels;i++) {

		/* Decomrpess a Note */
		XM_Channel *ch=&p->channel[i];
		XM_Note note = xm_player_decompress_note(p->current_row, i);
		bool process_note=false;

		/* Validate instrument and note fields */

		if ((note.instrument!=XM_FIELD_EMPTY && (note.instrument>=p->song->instrument_count || p->song->instrument_data[note.instrument]==0)) || (note.note!=XM_FIELD_EMPTY && note.note>XM_NOTE_OFF))  {

			/* if any is invalid, both become invalid */
			note.instrument=XM_FIELD_EMPTY;
			note.note=XM_FIELD_EMPTY;
		}

		/* Determine wether note should be processed */

		if ( (note.note!=XM_FIELD_EMPTY || note.instrument!=XM_FIELD_EMPTY) && note.note!=XM_NOTE_OFF) {


			if (note.note==XM_FIELD_EMPTY) {
					/* if note field is empty, there is only one case where
				a note can be played.. either the channel must be inactive,
				or the instrment is different than the one previously used
				in the channel. If the conditions meet, the previos note played
				is used (as long as it exist)*/
				if ((!ch->active || ch->instrument!=note.instrument) && ch->note!=XM_FIELD_EMPTY) {

					note.note=ch->note; /* use previous channel note */
					process_note=true;
				}

			} else if (note.instrument==XM_FIELD_EMPTY) {

				if (note.note==XM_NOTE_OFF) {

					process_note=true;

				} if (ch->instrument!=XM_FIELD_EMPTY) {

					note.instrument=ch->instrument;
					process_note=true;
				}
			} else {

				process_note=true;
			}

			if (process_note) {
				/* check the sample/instrument combination for validity*/

				Uint8 sample;
				/* this was validated before.. */
				XM_Instrument *ins=p->song->instrument_data[note.instrument];
				sample=ins->note_sample[note.note>>1];
				sample=(note.note&1)?sample>>4:sample&0xF;

				if (sample >= ins->sample_count || ins->samples[sample].sample_id==AUDIO_SAMPLE_INVALID_ID)
					process_note=false; /* invalid sample */

			}

		}

		if (process_note) {

			Uint8 sample;
			bool sample_changed;

			/* set some note-start flags */
			ch->note_start_offset=0; /* unless changed by effect */
			ch->portamento_active=false; /* unless changed by effect */
			ch->row_has_note=true;
			ch->note_off=false;

			ch->note=note.note;

			/** all this was previously validated **/
			ch->instrument=note.instrument;
			ch->instrument_ptr=p->song->instrument_data[ch->instrument];

			/* extract sample nibble */
			sample=ch->instrument_ptr->note_sample[ch->note>>1];
			sample=(ch->note&1)?sample>>4:sample&0xF;

			sample_changed=ch->sample!=sample;
			ch->sample=sample;
			ch->sample_ptr=&ch->instrument_ptr->samples[sample];

			ch->real_note=(Sint16)ch->note+ch->sample_ptr->base_note;
			ch->finetune=ch->sample_ptr->finetune;

			/* envelopes */
			xm_player_envelope_reset(&ch->volume_envelope);
			xm_player_envelope_reset(&ch->pan_envelope);

			/* get period from note */
			ch->old_period=ch->period;
			ch->period=xm_player_get_period( ch->real_note , ch->sample_ptr->finetune );

			if (sample_changed || ch->old_period==0) {
				/* if sample changed, fix portamento period */
				ch->old_period=ch->period;
			}

			if (ch->period==0)
				ch->active=false;

			/* volume/pan */

			ch->volume=ch->sample_ptr->volume; /* may be overriden by volume column anyway */
			ch->pan=ch->sample_ptr->pan; /* may be overriden by volume column anyway */

			ch->restart=true;
			ch->restart_offset=0; /* unless changed by command */
			ch->active=true; /* mey got unset later */
			ch->fadeout=0xFFFF;
		} else {

			ch->row_has_note=false;
		}

		/* process note off */
		if (note.note==XM_NOTE_OFF && ch->active) {
			/* if channels is not active, ignore */

			if (ch->instrument_ptr) {

				if (ch->instrument_ptr->volume_envelope.flags&XM_ENVELOPE_ENABLED) {
						/* if the envelope is enabled, noteoff is used, for both
					the envelope and the fadeout */
					ch->note_off=true;

				} else {
					/* otherwise, just deactivate the channel */
					ch->active=false;

				}
			}
		}

		/* Volume */

		ch->volume_command=0; /* By default, reset volume command */

		if (note.volume>=0x10) {
			/* something in volume column... */

			if (note.volume>=0x10 && note.volume<=0x50) {
				/* set volume */
				ch->volume=note.volume-0x10;

			} else if (note.volume>=0xC0 && note.volume<=0xCF) {
				/* set pan */
				ch->pan = (note.volume-0xC0) << 4;
			} else {

				ch->volume_command=note.volume;
			}
		}

		/* Command / Parameter */


		ch->command=note.command;
		ch->parameter=note.parameter;

		ch->note_delay=0; /* force note delay to zero */
	} /* end processing note */

}

void AudioStreamXM::xm_player_process_envelope( XM_Channel *ch, XM_EnvelopeProcess *p_env_process, XM_Envelope *p_envelope) {

	Sint16 env_len;

	if ( !(p_envelope->flags&XM_ENVELOPE_ENABLED))
		return;
	if ( p_env_process->done ) /* Envelope is finished */
		return;

	env_len=p_envelope->flags&XM_ENVELOPE_POINT_COUNT_MASK;


	/* compute envelope first */

	if (	p_envelope->flags&XM_ENVELOPE_SUSTAIN_ENABLED &&
	        !ch->note_off ) {
		/* if sustain looping */
			if (p_env_process->current_point >= p_envelope->sustain_index ) {

				p_env_process->current_point=p_envelope->sustain_index;
				p_env_process->tick=0;
			}
	} else if ( p_envelope->flags&XM_ENVELOPE_LOOP_ENABLED ) {
		/* else if loop enabled */

		if (p_env_process->current_point >= p_envelope->loop_end_index ) {

			p_env_process->current_point=p_envelope->loop_begin_index;
			p_env_process->tick=0;
		}
	}

	if ( p_env_process->current_point >= (env_len-1) && ( p_env_process->tick > 0) ) {
		/* envelope is terminated. note tick>0 instead of ==0, as a clever
		   trick to know for certain when the envelope ended */

		p_env_process->done=true;;
		p_env_process->current_point=env_len-1;
		if (env_len==0)
			return; /* a bug, don't bother with it */
	}

	{ /* process the envelope */

		Sint16 node_len=(p_env_process->current_point >= (env_len-1)) ? 0 : (XM_ENV_OFFSET(p_envelope->points[ p_env_process->current_point+1 ]) - XM_ENV_OFFSET(p_envelope->points[ p_env_process->current_point ]));

		if (node_len==0 || ( p_env_process->tick==0)) {
			/* don't interpolate */
			p_env_process->value=XM_ENV_VALUE(p_envelope->points[ p_env_process->current_point ]);
		} else {

			Sint16 v1=XM_ENV_VALUE(p_envelope->points[ p_env_process->current_point ]);
			Sint16 v2=XM_ENV_VALUE(p_envelope->points[ p_env_process->current_point+1 ]);
			Sint16 r=v1 + p_env_process->tick * ( v2-v1 ) / node_len;
			p_env_process->value=r;
		}

		/* increment */
		if (node_len) {
			p_env_process->tick++;
			if (p_env_process->tick>=node_len) {

				p_env_process->tick=0;
				p_env_process->current_point++;
			}
		}

	}


}

/** EFFECT COMMAND LISTINGS **/

enum XM_FX_List {
	/* list must be contiguous, for most compilers to
	   create a jump table in the switch() */
	XM_FX_0_ARPEGGIO,
	XM_FX_1_PORTA_UP,
	XM_FX_2_PORTA_DOWN,
	XM_FX_3_TONE_PORTA,
	XM_FX_4_VIBRATO,
	XM_FX_5_TONE_PORTA_AND_VOL_SLIDE,
	XM_FX_6_VIBRATO_AND_VOL_SLIDE,
	XM_FX_7_TREMOLO,
	XM_FX_8_SET_PANNING,
	XM_FX_9_SAMPLE_OFFSET,
	XM_FX_A_VOLUME_SLIDE,
	XM_FX_B_POSITION_JUMP,
	XM_FX_C_SET_VOLUME,
	XM_FX_D_PATTERN_BREAK,
	XM_FX_E_SPECIAL,
	XM_FX_F_SET_SPEED_AND_TEMPO,
	XM_FX_G_SET_GLOBAL_VOLUME,
	XM_FX_H_GLOBAL_VOLUME_SLIDE,
	XM_FX_I_UNUSED,
	XM_FX_J_UNUSED,
	XM_FX_K_KEY_OFF,
	XM_FX_L_SET_ENVELOPE_POS,
	XM_FX_M_UNUSED,
	XM_FX_N_UNUSED,
	XM_FX_O_UNUSED,
	XM_FX_P_PAN_SLIDE,
	XM_FX_R_RETRIG,
	XM_FX_S_UNUSED,
	XM_FX_T_TREMOR,
	XM_FX_U_UNUSED,
	XM_FX_V_UNUSED,
	XM_FX_W_UNUSED,
	XM_FX_X_EXTRA_FINE_PORTA,
	XM_FX_Y_UNUSED,
	XM_FX_Z_FILTER /* for mixers that can do filtering? */
};


static const Uint8 xm_fx_4_and_7_table[32]={ /* vibrato sine table */
	0, 24, 49, 74, 97,120,141,161,180,197,212,224,235,244,250,253,
 255,253,250,244,235,224,212,197,180,161,141,120, 97, 74, 49, 24
};

enum XM_FX_E_List {
	XM_FX_E0_AMIGA_FILTER,
	XM_FX_E1_FINE_PORTA_UP,
	XM_FX_E2_FINE_PORTA_DOWN,
	XM_FX_E3_SET_GLISS_CONTROL,
	XM_FX_E4_SET_VIBRATO_CONTROL,
	XM_FX_E5_SET_FINETUNE,
	XM_FX_E6_SET_LOOP_BEGIN,
	XM_FX_E7_SET_TREMOLO_CONTROL,
	XM_FX_E8_UNUSED,
	XM_FX_E9_RETRIG_NOTE,
	XM_FX_EA_FINE_VOL_SLIDE_UP,
	XM_FX_EB_FINE_VOL_SLIDE_DOWN,
	XM_FX_EC_NOTE_CUT,
	XM_FX_ED_NOTE_DELAY,
	XM_FX_EE_PATTERN_DELAY

};

enum XM_VX_List {
	XM_VX_6_VOL_SLIDE_DOWN = 6,
	XM_VX_7_VOL_SLIDE_UP,
	XM_VX_8_FINE_VOL_SLIDE_DOWN,
	XM_VX_9_FINE_VOL_SLIDE_UP,
	XM_VX_A_SET_VIBRATO_SPEED,
	XM_VX_B_SET_VIBRATO_DEPTH,
	XM_VX_C_SET_PANNING, /* this is unused; panning is processed elsewhere */
	XM_VX_D_PAN_SLIDE_LEFT,
	XM_VX_E_PAN_SLIDE_RIGHT,
	XM_VX_F_TONE_PORTA
};

void AudioStreamXM::xm_player_do_vibrato(XM_Channel *ch) {

	XM_PlayerData *p=xm_player;

	/* 0 .. 32 index, both pos and neg */
	Uint8 idx=ABS(ch->fx_4_vibrato_phase>>2)&0x1F;
	Sint16 modifier=0;
	switch(ch->fx_4_vibrato_type) {

		case 0: {
			/* Sine Wave */
			modifier=xm_fx_4_and_7_table[idx];
		} break;
		case 1:{
			/* Square Wave */
			modifier=0xFF;
		} break;
		case 2:{
			/* Saw Wave */
			modifier=(Sint16)idx<<3;
			if (ch->fx_4_vibrato_phase<0)
				modifier=0xFF-modifier;
		} break;
		case 3:{
			/* can't figure out this */
		} break;
	}

	modifier*=ch->fx_4_vibrato_depth;
	modifier>>=5;

	ch->tickrel_period+=(ch->fx_4_vibrato_phase<0)?-modifier:modifier;

	if (p->tick_counter>0)
		ch->fx_4_vibrato_phase+=ch->fx_4_vibrato_speed;


}
void AudioStreamXM::xm_player_do_tone_porta(XM_Channel *ch) {

	XM_PlayerData *p=xm_player;

	if (ch->porta_period!=0 && p->tick_counter) {


		/* porta period must be valid */

		Sint32 dist=(Sint32)ch->period-(Sint32)ch->porta_period;

		if (dist==0)
			return; /* nothing to do, we're at same period */

		if (ABS(dist)<(ch->fx_3_memory<<2)) {
			/* make it reach */
			ch->period=ch->porta_period;
		} else if (dist<0) {
			/* make it raise */
			ch->period+=ch->fx_3_memory<<2;
		} else if (dist>0) {

			ch->period-=ch->fx_3_memory<<2;
		}
	}
}

void AudioStreamXM::xm_player_do_volume_slide_down(XM_Channel *ch, Uint8 val) {
	Sint8 new_vol = (Sint8)ch->volume - (Sint8)val;
	if(new_vol<0)
		new_vol=0;
	ch->volume=new_vol;
}

void AudioStreamXM::xm_player_do_volume_slide_up(XM_Channel *ch, Uint8 val) {
	Sint8 new_vol = (Sint8)ch->volume + (Sint8)val;
	if (new_vol>64)
		new_vol=64;
	ch->volume=new_vol;
}


void AudioStreamXM::xm_player_do_volume_slide(XM_Channel *ch) {

	XM_PlayerData *p=xm_player;

	Uint8 param=(ch->parameter>0)?ch->parameter:ch->fx_A_memory;
	Uint8 param_up=param>>4;
	Uint8 param_down=param&0xF;

	if (p->tick_counter==0)
		return;

	if (ch->parameter)
		ch->fx_A_memory=ch->parameter;


	if (param_up) {
		xm_player_do_volume_slide_up(ch, param_down);
	} else if (param_down) { /* up has priority over down */
		xm_player_do_volume_slide_down(ch, param_down);
	}

}


void AudioStreamXM::xm_player_process_effects_and_envelopes()
{
	XM_PlayerData *p=xm_player;
	int i;
	int channels=(p->song->flags&XM_SONG_FLAGS_MASK_CHANNELS_USED)+1;

	for (i=0;i<channels;i++)
	{
		XM_Channel *ch=&p->channel[i];

		/* reset pre-effect variables */
		ch->tickrel_period=0;
		ch->tickrel_volume=0;


		/* PROCESS VOLUME COLUMN FOR EFFECT COMMANDS */
		Uint8 vcmd   = ch->volume_command  >> 4;
		Uint8 vparam = ch->volume_command  & 0xF;
		switch(vcmd) {
			case XM_VX_6_VOL_SLIDE_DOWN: {
				xm_player_do_volume_slide_down(ch, vparam);
			} break;

			case XM_VX_7_VOL_SLIDE_UP: {
				xm_player_do_volume_slide_up(ch, vparam);
			} break;

			case XM_VX_8_FINE_VOL_SLIDE_DOWN: {
				if (p->tick_counter!=0)
					break;
				xm_player_do_volume_slide_down(ch, vparam);
			} break;

			case XM_VX_9_FINE_VOL_SLIDE_UP: {
				if (p->tick_counter!=0)
					break;
				xm_player_do_volume_slide_up(ch, vparam);
			} break;

			case XM_VX_A_SET_VIBRATO_SPEED: {
				if (p->tick_counter==0) {
					if (vparam)
						ch->fx_4_vibrato_speed=vparam<<2;
				}
				xm_player_do_vibrato(ch);
			} break;

			case XM_VX_B_SET_VIBRATO_DEPTH: {
				if (p->tick_counter==0) {
					if (vparam)
						ch->fx_4_vibrato_depth=vparam;
				}
				xm_player_do_vibrato(ch);
			} break;

			case XM_VX_C_SET_PANNING: {
			} break;

			case XM_VX_D_PAN_SLIDE_LEFT: {
				Sint16 new_pan = (Uint16)ch->pan - vparam;
				if (new_pan<0)
					new_pan=0;
				ch->pan=(Uint8)new_pan;
			} break;

			case XM_VX_E_PAN_SLIDE_RIGHT: {
				Sint16 new_pan = (Uint16)ch->pan + vparam;
				if (new_pan>255)
					new_pan=255;
				ch->pan=(Uint8)new_pan;
			} break;

			case XM_VX_F_TONE_PORTA: {
				if(vparam)
					ch->fx_3_memory=vparam<<4;

				if (p->tick_counter==0 && ch->row_has_note) {
					ch->porta_period=ch->period;
					ch->period=ch->old_period;
					ch->restart=false;
				}

				xm_player_do_tone_porta(ch);
			} break;
		}



		/* PROCESS  EFFECT COMMANDS */
		switch(ch->command) {

			case XM_FX_0_ARPEGGIO: {

				Uint32 base_period = xm_player_get_period( ch->real_note , ch->finetune );
				Uint8 ofs;

				switch(p->tick_counter%3) {
					case 0: ofs=0; break;
					case 1: ofs=ch->parameter>>4; break;
					case 2: ofs=ch->parameter&0xF; break;
				}

				ch->tickrel_period += xm_player_get_period( ch->real_note+ofs , ch->finetune )-base_period;

			} break;
			case XM_FX_1_PORTA_UP: {

				Uint8 param;
				if (p->tick_counter==0)
					break;
				param=(ch->parameter>0)?ch->parameter:ch->fx_1_memory;
				if (ch->parameter)
					ch->fx_1_memory=param;
				ch->period-=param<<2;

			} break;
			case XM_FX_2_PORTA_DOWN: {

				Uint8 param;
				if (p->tick_counter==0)
					break;

				param=(ch->parameter>0)?ch->parameter:ch->fx_2_memory;
				if (ch->parameter)
					ch->fx_2_memory=ch->parameter;

				ch->period+=param<<2;

			} break;
			case XM_FX_3_TONE_PORTA: {

				Uint8 param=(ch->parameter>0)?ch->parameter:ch->fx_3_memory;
				if (ch->parameter)
					ch->fx_3_memory=param;

				if (p->tick_counter==0 && ch->row_has_note) {

					ch->porta_period=ch->period;
					ch->period=ch->old_period;
					ch->restart=false;
				}

				xm_player_do_tone_porta(ch);

			} break;
			case XM_FX_4_VIBRATO: {
				/* reset phase on new note */
				if (p->tick_counter==0) {

					if (ch->row_has_note)
						ch->fx_4_vibrato_phase=0;
					if (ch->parameter&0xF)
						ch->fx_4_vibrato_depth=ch->parameter&0xF;
					if (ch->parameter&0xF0)
						ch->fx_4_vibrato_speed=(ch->parameter&0xF0)>>2;
				}
				xm_player_do_vibrato(ch);
			} break;

			case XM_FX_5_TONE_PORTA_AND_VOL_SLIDE: {
				xm_player_do_volume_slide(ch);
				xm_player_do_tone_porta(ch);
			} break;

			case XM_FX_6_VIBRATO_AND_VOL_SLIDE: {

				xm_player_do_volume_slide(ch);
				xm_player_do_vibrato(ch);

			} break;
			case XM_FX_7_TREMOLO: {

				XM_PlayerData *p=xm_player;

				/* 0 .. 32 index, both pos and neg */
				Uint8 idx=ABS(ch->fx_7_tremolo_phase>>2)&0x1F;
				Sint16 modifier=0;
				switch(ch->fx_7_tremolo_type) {

					case 0: {
						/* Sine Wave */
						modifier=xm_fx_4_and_7_table[idx];
					} break;
					case 1:{
						/* Square Wave */
						modifier=0xFF;
					} break;
					case 2:{
						/* Saw Wave */
						modifier=(Sint16)idx<<3;
						if (ch->fx_7_tremolo_phase<0)
							modifier=0xFF-modifier;
					} break;
					case 3:{
						/* can't figure out this */
					} break;
				}

				modifier*=ch->fx_7_tremolo_depth;
				modifier>>=7;

				ch->tickrel_volume+=(ch->fx_7_tremolo_phase<0)?-modifier:modifier;

				if (p->tick_counter>0)
					ch->fx_7_tremolo_phase+=ch->fx_7_tremolo_speed;

			} break;
			case XM_FX_8_SET_PANNING: {

				if (p->tick_counter==0) {

					ch->pan=ch->parameter;
				}
			} break;
			case XM_FX_9_SAMPLE_OFFSET: {

				ch->restart_offset=(Uint32)ch->parameter<<8;
			} break;
			case XM_FX_A_VOLUME_SLIDE: {

				xm_player_do_volume_slide(ch);
			} break;
			case XM_FX_B_POSITION_JUMP: {

				if (p->tick_counter!=0 || ch->parameter >= p->song->order_count)
					break; /* pointless */
				if (p->force_next_order) {
					/* already forced? just change order */
					p->forced_next_order=ch->parameter;
				} else {
					p->force_next_order=true;
					p->forced_next_order=ch->parameter;
				}

			} break;
			case XM_FX_C_SET_VOLUME: {

				if (p->tick_counter!=0 || ch->parameter >64)
					break;

				ch->volume=ch->parameter;
			} break;
			case XM_FX_D_PATTERN_BREAK: {

				if (p->tick_counter!=0)
					break; /* pointless */
				if (p->force_next_order) {
					/* already forced? just change order */
					p->forced_next_order_row=ch->parameter;

				} else {
					p->force_next_order=true;
					p->forced_next_order_row=ch->parameter;
					p->forced_next_order=p->current_order+1;
				}

			} break;
			case XM_FX_E_SPECIAL: {

				Uint8 ecmd=ch->parameter>>4;
				Uint8 eparam=ch->parameter&0xF;
				switch(ecmd) {
					case XM_FX_E0_AMIGA_FILTER:{

					}break;

					case XM_FX_E1_FINE_PORTA_UP: {

						Uint8 param;
						if (p->tick_counter!=0)
							break;
						param=(eparam>0)?eparam:ch->fx_E1_memory;
						if (eparam)
							ch->fx_E2_memory=param;
						ch->period-=param<<2;


					} break;
					case XM_FX_E2_FINE_PORTA_DOWN: {

						Uint8 param;
						if (p->tick_counter!=0)
							break;
						param=(eparam>0)?eparam:ch->fx_E2_memory;
						if (eparam)
							ch->fx_E2_memory=param;
						ch->period-=param<<2;

					} break;
					case XM_FX_E3_SET_GLISS_CONTROL: {

						/* IGNORED, DEPRECATED */
					} break;
					case XM_FX_E4_SET_VIBRATO_CONTROL: {
						if (p->tick_counter!=0)
							break;

						if (eparam<4)
							ch->fx_4_vibrato_type=eparam;

					} break;
					case XM_FX_E5_SET_FINETUNE: {

						if (eparam<4)
							ch->finetune=((Sint8)eparam-8)<<4;
					} break;
					case XM_FX_E6_SET_LOOP_BEGIN: {

						/* IGNORED, difficult to
						   support in hardware*/
					} break;
					case XM_FX_E7_SET_TREMOLO_CONTROL: {

						if (p->tick_counter!=0)
							break;

						if (eparam<4)
							ch->fx_7_tremolo_type=eparam;

					} break;
					case XM_FX_E8_UNUSED: {


					} break;
					case XM_FX_E9_RETRIG_NOTE: {

						/* this needs more testing... it gets a lot of validations already */

						if (eparam && (p->tick_counter%eparam)==0 && ch->old_period!=0 && ch->sample_ptr && ch->instrument_ptr) {
							ch->restart=true;
							xm_player_envelope_reset(&ch->volume_envelope);
							xm_player_envelope_reset(&ch->pan_envelope);
							ch->active=true;

						}
					} break;
					case XM_FX_EA_FINE_VOL_SLIDE_UP: {
						Uint8 param;
						if (p->tick_counter!=0)
							break;
						param=(eparam>0)?eparam:ch->fx_EA_memory;
						if (eparam)
							ch->fx_EA_memory=param;

						xm_player_do_volume_slide_up(ch, param);
					} break;
					case XM_FX_EB_FINE_VOL_SLIDE_DOWN: {
						Uint8 param;
						if (p->tick_counter!=0)
							break;
						param=(eparam>0)?eparam:ch->fx_EB_memory;
						if (eparam)
							ch->fx_EB_memory=param;

						xm_player_do_volume_slide_down(ch, param);
					} break;
					case XM_FX_EC_NOTE_CUT: {

						if (p->tick_counter==eparam)
							ch->active=false;

					} break;

					case XM_FX_ED_NOTE_DELAY: {
						if (p->tick_counter !=0 || ch->note_delay >= p->current_speed) {
							break;
						}
						ch->note_delay=eparam;
					} break;

					case XM_FX_EE_PATTERN_DELAY: {
						if (p->tick_counter!=0)
							break;
						p->pattern_delay=eparam;
					} break;
				}

			} break;
			case XM_FX_F_SET_SPEED_AND_TEMPO: {

				if (p->tick_counter!=0)
					break; /* pointless */

				if (ch->parameter==0)
					break;
				else if (ch->parameter<0x1F)
					p->current_speed=ch->parameter;
				else
					p->current_tempo=ch->parameter;

			} break;
			case XM_FX_G_SET_GLOBAL_VOLUME: {

				if (p->tick_counter!=0 || ch->parameter>64)
					break; /* pointless */
				p->global_volume=ch->parameter;
			} break;
			case XM_FX_H_GLOBAL_VOLUME_SLIDE: {

				XM_PlayerData *p=xm_player;
				Uint8 param,param_up,param_down;


				if (p->tick_counter==0)
					return;

				param=(ch->parameter>0)?ch->parameter:ch->fx_H_memory;
				param_up=param>>4;
				param_down=param&0xF;
				if (ch->parameter)
					ch->fx_H_memory=ch->parameter;


				if (param_up) {

					Sint8 new_vol = (Sint8)p->global_volume + (Sint8)param_up;
					if (new_vol>64)
						new_vol=64;
					p->global_volume=new_vol;

				} else if (param_down) { /* up has priority over down */

					Sint8 new_vol = (Sint8)p->global_volume - (Sint8)param_down;
					if (new_vol<0)
						new_vol=0;
					p->global_volume=new_vol;
				}

			} break;
			case XM_FX_I_UNUSED: {


			} break;
			case XM_FX_J_UNUSED: {


			} break;
			case XM_FX_K_KEY_OFF: {
				ch->note_off=true;
			} break;
			case XM_FX_L_SET_ENVELOPE_POS: {

				/* this is weird, should i support it? Impulse Tracker doesn't.. */
			} break;
			case XM_FX_M_UNUSED: {


			} break;
			case XM_FX_N_UNUSED: {


			} break;
			case XM_FX_O_UNUSED: {


			} break;
			case XM_FX_P_PAN_SLIDE: {

				XM_PlayerData *p=xm_player;
				Uint8 param,param_up,param_down;

				if (p->tick_counter==0)
					return;

				param=(ch->parameter>0)?ch->parameter:ch->fx_P_memory;
				param_up=param>>4;
				param_down=param&0xF;
				if (ch->parameter)
					ch->fx_P_memory=ch->parameter;


				if (param_up) {

					Sint16 new_pan = (Sint16)ch->pan + (Sint16)param_up;
					if (new_pan>255)
						new_pan=255;
					ch->pan=new_pan;

				} else if (param_down) { /* up has priority over down */

					Sint16 new_pan = (Sint16)ch->pan - (Sint16)param_down;
					if (new_pan<0)
						new_pan=0;
					ch->pan=new_pan;
				}

			} break;
			case XM_FX_R_RETRIG: {


			} break;
			case XM_FX_S_UNUSED: {


			} break;
			case XM_FX_T_TREMOR: {


			} break;
			case XM_FX_U_UNUSED: {


			} break;
			case XM_FX_V_UNUSED: {


			} break;
			case XM_FX_W_UNUSED: {


			} break;
			case XM_FX_X_EXTRA_FINE_PORTA: {


			} break;
			case XM_FX_Y_UNUSED: {


			} break;
			case XM_FX_Z_FILTER : {


			} break;
		}

		/* avoid zero or negative period  */

		if (ch->period<=0)
			ch->active=false;

		/* process note delay */

		if (ch->note_delay) {
			continue;
		}

		if (ch->active) {
			xm_player_process_envelope( ch, &ch->volume_envelope, &ch->instrument_ptr->volume_envelope);
			xm_player_process_envelope( ch, &ch->pan_envelope, &ch->instrument_ptr->pan_envelope);
		}


	}
}

void AudioStreamXM::xm_player_update_mixer() {


	AudioMixer *am = AudioServer::get_singleton()->get_mixer();
	
	XM_PlayerData *p=xm_player;
	int i;
	int channels=(p->song->flags&XM_SONG_FLAGS_MASK_CHANNELS_USED)+1;

	for (i=0;i<channels;i++) {

		XM_Channel *ch=&p->channel[i];

		if (!ch->active) {

			if (am->is_voice_active(ch->mixer_voice_id)) {
				am->voice_stop(ch->mixer_voice_id);
				ch->mixer_voice_id=AUDIO_VOICE_INVALID_ID;
				
			}
			continue;
		}

		/* reset pre-effect variables */

		if (ch->note_delay) { /* if requested delay, don't do a thing */
			ch->note_delay--;
			continue;
		}

		/* start voice if needed */
		if (ch->restart) {

			if (ch->mixer_voice_id!=AUDIO_VOICE_INVALID_ID)
				am->voice_stop(ch->mixer_voice_id);
			
			ch->mixer_voice_id=am->voice_alloc(ch->sample_ptr->sample_id, ch->restart_offset);
			ch->restart=false;
/*			printf("TOMIXER starting channel %i, with sample %i, offset %i\n",i,ch->sample,ch->restart_offset); */
		}

		/* check channel activity */

		if (ch->active && !am->is_voice_active(ch->mixer_voice_id)) {

			ch->active=false;
/*			printf("TOMIXER TERMINATED channel %i FROM mixer\n",i); */
			continue;

		}

		if (!ch->active && am->is_voice_active(ch->mixer_voice_id)) {

			am->voice_stop(ch->mixer_voice_id);
			ch->mixer_voice_id=AUDIO_VOICE_INVALID_ID;
/*			printf("TOMIXER TERMINATED channel %i TO mixer\n",i); */
			continue;
		}



		{ /* VOLUME PROCESS */
			Sint16 final_volume;
			final_volume=((ch->volume+ch->tickrel_volume) * 255) >> 6;
			

			if (ch->instrument_ptr->volume_envelope.flags&XM_ENVELOPE_ENABLED) {

				final_volume = (final_volume * ch->volume_envelope.value) >> 6;

				if (ch->note_off) {

					Uint16 fade_down=ch->instrument_ptr->fadeout;

					if (fade_down>0xFFF || fade_down>=ch->fadeout)
						ch->fadeout=0;
					else
						ch->fadeout-=fade_down;

					if (ch->fadeout==0) {

						ch->active=false;
					}

				}
			}

			final_volume =  (final_volume * p->global_volume *config_volume) >> 14;


			if (final_volume>255)
				final_volume=255;

			if (final_volume<0)
				final_volume=0;
			
			
			
			am->voice_set_volume( ch->mixer_voice_id, final_volume );
		}

		{ /* PAN PROCESS */

			Sint16 final_pan=ch->pan;

			if (ch->instrument_ptr->pan_envelope.flags&XM_ENVELOPE_ENABLED) {
				final_pan=final_pan+((Sint16)ch->pan_envelope.value-32)*(128-ABS(final_pan-128))/32;
			}

			if (final_pan<0)
				final_pan=0;
			if (final_pan>255)
				final_pan=255;

			am->voice_set_pan( ch->mixer_voice_id, final_pan );
		}

		am->voice_set_speed( ch->mixer_voice_id, xm_player_get_frequency( ch->period + ch->tickrel_period ) );

	}

	mixer_tick_size=(2500000/p->current_tempo);
	//am->set_process_callback_interval( 2500000/p->current_tempo ); /* re convert tempo to usecs */

}

void AudioStreamXM::mixer_tick_callback(Uint32 p_usec) {
	
	if (status!=PLAY)
		return;

	config_volume=Uint8( get_audio_type()==MUSIC?(AudioServer::get_singleton()->get_music_volume()*255.0):(AudioServer::get_singleton()->get_sfx_volume()*255.0));
	
	while (p_usec) {

		if (p_usec>=mixer_tick_size) {
		
		
			p_usec-=mixer_tick_size;
			xm_player_process_tick();
			mixer_tick_size=(2500000/xm_player->current_tempo);
		} else {
		
			mixer_tick_size-=p_usec;
			p_usec=0;
			
		}
	}

}
void AudioStreamXM::xm_player_process_tick() {


	XM_PlayerData *p=xm_player;


	if (!p) {
		XM_ERROR_PRINTF("Player Unconfigured (missing player)!");
		return;
	}
	if (!p->song) {

		return; /* if song set is null, don't do a thing */
	}
	if (!p->active) {

		return;
	}

	/* Check Ticks */

	if (p->tick_counter >= (p->current_speed+p->pattern_delay)) {

		/* Tick Reaches Zero, process row */

		p->tick_counter=0;
		p->pattern_delay=0;

		/* change order, as requested by some command */
		if (p->force_next_order) {

			if (p->forced_next_order<p->song->order_count) {
				p->current_order=p->forced_next_order;
				p->current_row=p->forced_next_order_row;
				p->current_pattern=p->song->order_list[p->current_order];
			}
			p->force_next_order=false;
			p->forced_next_order_row=0;

		}

		/** process a row of nnotes **/
		xm_process_notes();

		/* increment row and check pattern/order changes */
		p->current_row++;

		if (p->current_row>=xm_player_get_pattern_length( p->current_pattern )) {

			p->current_row=0;
			p->current_order++;
			if (p->current_order>=p->song->order_count)
				p->current_order=p->song->restart_pos;


			p->current_pattern=p->song->order_list[p->current_order];

		}



	}

	/** PROCESS EFFECTS AND ENVELOPES**/

	xm_player_process_effects_and_envelopes();

	/** UPDATE MIXER **/

	xm_player_update_mixer();

	/** DECREMENT TICK */
	p->tick_counter++;

	/*if (p->current_order!=last_order) {
	
		printf("current: %i, last %i, repeats %i\n",p->current_order,last_order,repeat_counter);
	}*/
	if (p->current_order<last_order) {
	
		repeat_counter++;
	}
	last_order=p->current_order;


/*	XM_DEBUG_PRINTF("playing row %i, order %i\n",p->current_row,p->current_order); */
}




void AudioStreamXM::xm_player_play() {

	if (!xm_player->song) {

		XM_ERROR_PRINTF("NO SONG SET IN PLAYER");
		return;
	}

	xm_player_stop(); /* stop if playing */

	XM_AUDIO_LOCK
	xm_player_reset();
	xm_player->active=true;
	XM_AUDIO_UNLOCK


}
void AudioStreamXM::xm_player_stop() {


	XM_AUDIO_LOCK

	/* @TODO STOP ALL MIXER VOICES */

	xm_player_reset();
	xm_player->active=false;

	XM_AUDIO_UNLOCK


}


/******** LOADER ***********/


void AudioStreamXM::xm_clear_song( XM_Song *p_song, Sint32 p_pattern_count, Sint32 p_instrument_count ) {

	int i;

	/**
	  * Erasing must be done in the opposite way as creating.
	  * This way, allocating memory and deallocating memory from a song can work like a stack,
	  * to simplify the memory allocator in some implementations.
	  */

	XM_AUDIO_LOCK

	/* Instruments first */

	for (i=(p_instrument_count-1);i>=0;i--) {

		if (i>=p_song->instrument_count) {

			XM_ERROR_PRINTF("Invalid clear instrument amount specified.");
			continue;
		}

		if ( p_song->instrument_data[i] ) {

			XM_Instrument *ins = p_song->instrument_data[i];
			int j;

			for (j=(ins->sample_count-1);j>=0;j--) {

				if (ins->samples[j].sample_id!=AUDIO_SAMPLE_INVALID_ID) {

					AudioSampleManager::get_singleton()->destroy( ins->samples[j].sample_id );
				}
			}

			if (ins->samples) {
				memdelete_arr( ins->samples );
			}
			
			memdelete( p_song->instrument_data[i] );
		}
	}

	if ( p_song->instrument_data && p_instrument_count>=0) {

		memdelete_arr( p_song->instrument_data );
		p_song->instrument_data=0;
	}



	/* Patterns Last */
	for (i=(p_pattern_count-1);i>=0;i--) {

		if (i>=p_song->pattern_count) {

			XM_ERROR_PRINTF("Invalid clear pattern amount specified.");
			continue;
		}

		if ( p_song->pattern_data[i] ) {
			memdelete_arr( p_song->pattern_data[i] );
		}
	}

	if ( p_song->pattern_data && p_pattern_count) {

		memdelete_arr( p_song->pattern_data );
		p_song->pattern_data=0;
	}

	if (p_pattern_count && p_instrument_count) {
		
		xm_zero_mem( p_song, sizeof(XM_Song) );
		p_song->tempo=125;
		p_song->speed=6;  		
	}
	
	XM_AUDIO_UNLOCK


}

/**
 * Recompress Pattern from XM-Compression to Custom (smaller/faster to read) compression
 * Read pattern.txt included with this file for the format.
 * @param p_rows Rows for source pattern
 * @param channels Channels for source pattern
 * @param p_dst_data recompress target pointer, if null, will just compute size
 * @return size of recompressed buffer
 */



Uint32 AudioStreamXM::xm_recompress_pattern(Uint16 p_rows,Uint8 p_channels,void * p_dst_data) {

#define XM_COMP_ADD_BYTE(m_b) do {\
	if (dst_data)\
		dst_data[data_size]=m_b;\
	++data_size;\
	} while(0)\

	FileAccess *f=xm_fileio;

	Uint8 *dst_data=(Uint8*)p_dst_data;
	Uint32 data_size=0;
	int i,j,k;
	Uint8 caches[32][5];
	Sint8 last_channel=-1;
	Uint16 last_row=0;



	for (i=0;i<32;i++) {

		caches[i][0]=XM_FIELD_EMPTY;
		caches[i][1]=XM_FIELD_EMPTY;
		caches[i][2]=XM_FIELD_EMPTY;
		caches[i][3]=XM_FIELD_EMPTY;
		caches[i][4]=0; /* only values other than 0 are read for this as cache */
	}
	for(j=0;j<p_rows;j++)
		for(k=0;k<p_channels;k++) {

		Uint8 note=XM_FIELD_EMPTY;
		Uint8 instrument=0; /* Empty XM Instrument */
		Uint8 volume=0; /* Empty XM Volume */
		Uint8 command=XM_FIELD_EMPTY;
		Uint8 parameter=0;
		Uint8 arb=0; /* advance one row bit */

		Uint8 aux_byte=0;

		Uint8 cache_bits=0;
		Uint8 new_field_bits=0;

		aux_byte=f->get_8();
		if (!(aux_byte&0x80)) {

			note=aux_byte;
			aux_byte=0xFE; /* if bit 7 not set, read all of them except the note */
		}

		if (aux_byte&1) note=f->get_8();
		if (aux_byte&2) instrument=f->get_8();
		if (aux_byte&4) volume=f->get_8();
		if (aux_byte&8) command=f->get_8();
		if (aux_byte&16) parameter=f->get_8();



		if (note>97)
			note=XM_FIELD_EMPTY;

		if (instrument==0)
			instrument=XM_FIELD_EMPTY;
		else
			instrument--;

		if (volume<0x10)
			volume=XM_FIELD_EMPTY;
		else volume-=0x10;

		if (command==0 && parameter==0) {
			/* this equals to nothing */
			command=XM_FIELD_EMPTY;
		}


		/** COMPRESS!!! **/

		/* Check differences with cache and place them into bits */
		cache_bits|=(note!=XM_FIELD_EMPTY && note==caches[k][0])?XM_COMP_NOTE_BIT:0;
		cache_bits|=(instrument!=XM_FIELD_EMPTY && instrument==caches[k][1])?XM_COMP_INSTRUMENT_BIT:0;
		cache_bits|=(volume!=XM_FIELD_EMPTY && volume==caches[k][2])?XM_COMP_VOLUME_BIT:0;
		cache_bits|=(command!=XM_FIELD_EMPTY && command==caches[k][3])?XM_COMP_COMMAND_BIT:0;
		cache_bits|=(parameter!=0 && parameter==caches[k][4])?XM_COMP_PARAMETER_BIT:0;

		/* Check new field values and place them into bits and cache*/

		if (note!=XM_FIELD_EMPTY && !(cache_bits&XM_COMP_NOTE_BIT)) {

			new_field_bits|=XM_COMP_NOTE_BIT;
			caches[k][0]=note;

		}


		if (instrument!=XM_FIELD_EMPTY && !(cache_bits&XM_COMP_INSTRUMENT_BIT)) {

			new_field_bits|=XM_COMP_INSTRUMENT_BIT;
			caches[k][1]=instrument;

		}

		if (volume!=XM_FIELD_EMPTY && !(cache_bits&XM_COMP_VOLUME_BIT)) {

			new_field_bits|=XM_COMP_VOLUME_BIT;
			caches[k][2]=volume;

		}

		if (command!=XM_FIELD_EMPTY && !(cache_bits&XM_COMP_COMMAND_BIT)) {

			new_field_bits|=XM_COMP_COMMAND_BIT;
			caches[k][3]=command;

		}

		if (parameter!=0 && !(cache_bits&XM_COMP_PARAMETER_BIT)) {

			new_field_bits|=XM_COMP_PARAMETER_BIT;
			caches[k][4]=parameter;

		}



		if (!new_field_bits && !cache_bits) {
			continue; /* nothing to store, empty field */
		}



		/* Seek to Row */

		if (j>0 && last_row==(j-1) && last_channel!=k) {
			arb=0x80;
			last_row=j;

		} else while (last_row<j) {



			Uint16 diff=j-last_row;

			if (diff>0x20) {
				/* The maximum value of advance_rows command is 32 (0xFF)
				   so, if the rows that are needed to advance are greater than that,
				   advance 32, then issue more advance_rows commands */
				XM_COMP_ADD_BYTE( 0xFF ); /* Advance 32 rows */
				last_row+=0x20;
			} else {


				XM_COMP_ADD_BYTE( 0xE0+(diff-1) ); /* Advance needed rows */
				last_row+=diff;
			}

			last_channel=0; /* advancing rows sets the last channel to zero */
		}
		/* Seek to Channel */



		if (last_channel!=k) {

			XM_COMP_ADD_BYTE( arb|k );
		}

		if (cache_bits) {

			XM_COMP_ADD_BYTE( cache_bits|(1<<5) );
		}

		if (new_field_bits) {

			XM_COMP_ADD_BYTE( new_field_bits|(2<<5) );
			for (i=0;i<5;i++) {

				if (new_field_bits&(1<<i)) {

					XM_COMP_ADD_BYTE( caches[k][i] );
				}
			}
		}

	}

	XM_COMP_ADD_BYTE( 0x60 ); /* end of pattern */

#undef XM_COMP_ADD_BYTE

	return data_size;
}




Error AudioStreamXM::xm_loader_open_song_custom( String p_filename, XM_Song *p_song, bool p_load_music, bool p_load_instruments )
{
	int i,j,k;
	FileAccess *f=xm_fileio;

	XM_DEBUG_PRINTF("\n*** LOADING XM: %s ***\n\n",p_filename.ascii().get_data());

	if (!p_load_music && !p_load_instruments) {
		return OK; /* nothing to do... */
	}

	if (f->open(p_filename, FileAccess::READ)!=OK) {

		return ERR_FILE_CANT_OPEN;
	};

	xm_clear_song( p_song, p_load_music?p_song->pattern_count:-1, p_load_instruments?p_song->instrument_count:-1 );

    	/**************************************
	LOAD Identifier
	***************************************/
	{


		Uint8 idtext[18];
		f->get_buffer(idtext,17);
		idtext[17]=0;

		/** TODO validate identifier **/


	}

	/**************************************
	LOAD Header
	***************************************/
	{

		Uint8 hex1a;
		Uint16 version;
		Uint32 headersize;
		f->get_buffer((Uint8*)p_song->name,20);

		p_song->name[20]=0;

		XM_DEBUG_PRINTF("Song Name: %s\n",p_song->name);

		hex1a=f->get_8();
		if (hex1a!=0x1A) { /* XM "magic" byte.. */

			f->close();
			return ERR_FILE_UNRECOGNIZED;
		}

		for (i=0;i<20;i++) /* skip trackername */
			f->get_8();


		version=f->get_16();

		headersize=f->get_32();

		if (p_load_music) {
			Uint8 chans;

			p_song->order_count=f->get_16();

			p_song->restart_pos=f->get_16();

			if (p_song->restart_pos>=p_song->order_count)
				p_song->restart_pos=0;

			chans=f->get_16();
			if (chans>32) {

				XM_ERROR_PRINTF("Invalid Number of Channels: %i > 32",chans);
				return ERR_FILE_CORRUPT;
			}
			p_song->flags=(chans-1)&0x1F; /* use 5 bits, +1 */

			p_song->pattern_count=f->get_16();

			if (p_load_instruments)
				p_song->instrument_count=f->get_16();
			else
				f->get_16(); /* ignore */

			if ( f->get_16() ) /* flags. only linear periods */
				p_song->flags|=XM_SONG_FLAGS_LINEAR_PERIODS;


			p_song->speed=f->get_16();
			p_song->tempo=f->get_16();

			f->get_buffer(p_song->order_list,256);

#ifdef XM_DEBUG
			XM_DEBUG_PRINTF("Song Header:\n");
			XM_DEBUG_PRINTF("\tChannels: %i\n",chans);
			XM_DEBUG_PRINTF("\tOrders: %i\n",p_song->order_count);
			XM_DEBUG_PRINTF("\tPatterns: %i\n",p_song->pattern_count);
			XM_DEBUG_PRINTF("\tInstruments: %i\n",p_song->instrument_count);
			XM_DEBUG_PRINTF("\tRestart At: %i\n",p_song->restart_pos);
			XM_DEBUG_PRINTF("\tTempo: %i\n",p_song->tempo);
			XM_DEBUG_PRINTF("\tSpeed: %i\n",p_song->speed);
			XM_DEBUG_PRINTF("\tOrders: ");

			for (i=0;i<p_song->order_count;i++) {
				if (i>0)
					XM_DEBUG_PRINTF(", ");
				XM_DEBUG_PRINTF("%i",p_song->order_list[i]);

			}

			XM_DEBUG_PRINTF("\n");

#endif
		} else {

			f->get_16(); /* skip order count */
			f->get_16(); /* skip restart pos */
			f->get_16(); /* skip flags */
			f->get_16(); /* skip pattern count */
			p_song->instrument_count=f->get_16();
			/* skip to end of header */
			XM_DEBUG_PRINTF("Skipping Header.. \n");
			XM_DEBUG_PRINTF("\tInstruments: %i \n",p_song->instrument_count);
		}

		while (f->get_pos() < (headersize+60))
			f->get_8(); /* finish reading header */

	}

        /**************************************
	LOAD Patterns
	***************************************/

	if (p_load_music && p_song->pattern_count) {

		p_song->pattern_data = memnew_arr( Uint8*, p_song->pattern_count );

		if (!p_song->pattern_data) { /* Handle OUT OF MEMORY */
			/* xm_clear_song( p_song, -1, -1 ); pointless */
			f->close();
			return ERR_OUT_OF_MEMORY;
		}

		xm_zero_mem(p_song->pattern_data,sizeof(Uint8*)*p_song->pattern_count);

	}

	XM_DEBUG_PRINTF("\n\n");

	for (i=0;i<p_song->pattern_count;i++) {

		Uint32 _ofs=f->get_pos(); /* current file position */
		Uint32 header_len=f->get_32(); /* pattern header len */
		Uint8 packing=f->get_8(); /* if packing = 1, this pattern is pre-packed */
		Uint16 rows=f->get_16(); /* rows */
		Uint16 packed_data_size=f->get_16(); /* pattern header len */


		XM_DEBUG_PRINTF("Pattern: %i\n",i);
		XM_DEBUG_PRINTF("\tHeader Len: %i\n",header_len);
		XM_DEBUG_PRINTF("\tRows: %i\n",rows);
		XM_DEBUG_PRINTF("\tPacked Size: %i\n",packed_data_size);

		while (f->get_pos() < (_ofs+header_len))
			f->get_8(); /* finish reading header */

		if (p_load_music) {

			if (packed_data_size==0) {
				p_song->pattern_data[i]=0;

			} else if (packing==1) { /* pre packed pattern */

				Uint8 *pdata = memnew_arr( Uint8, packed_data_size);

				if (!pdata) { /* Handle OUT OF MEMORY */

					xm_clear_song( p_song, i, -1 );
					f->close();
					return ERR_OUT_OF_MEMORY;
				}

				f->get_buffer(pdata,packed_data_size);
			} else { /* pack on the fly while reading */

				Uint32 _pack_begin_pos=f->get_pos();

				Uint32 repacked_size=xm_recompress_pattern( rows, (p_song->flags&0x1f)+1 , 0 ); /* just calculate size */
				Uint8 *pdata=0;

				XM_DEBUG_PRINTF("\tRePacked Size: %i\n",repacked_size);

				f->seek(_pack_begin_pos);

				pdata = memnew_arr( Uint8, 1+repacked_size );


				if (!pdata) { /* Handle OUT OF MEMORY */

					xm_clear_song( p_song, i, -1 );
					f->close();
					return ERR_OUT_OF_MEMORY;
				}

				pdata[0]=rows-1; /* first byte is rows */

				/* on the fly recompress */
				xm_recompress_pattern( rows, (p_song->flags&0x1f)+1 , &pdata[1] );

				p_song->pattern_data[i]=pdata;
			}

		} /* Just skip to end othersize */

		while (f->get_pos() < (_ofs+header_len+packed_data_size))
			f->get_8(); /* finish reading header */
	}

       /**************************************
	LOAD INSTRUMENTS!
       ***************************************/

	if (p_load_instruments && p_song->instrument_count) {

		p_song->instrument_data = memnew_arr(XM_Instrument*,p_song->instrument_count);


		if (!p_song->instrument_data) { /* Handle OUT OF MEMORY */

			xm_clear_song( p_song, p_load_music?p_song->pattern_count:-1, 0 );
			f->close();
			return ERR_OUT_OF_MEMORY;
		}

		xm_zero_mem(p_song->instrument_data,sizeof(XM_Instrument*)*p_song->instrument_count);

	} else {

		/* Don't load instruments */
		f->close();
		return OK;
	}



	for (i=0;i<p_song->instrument_count;i++) {

		Uint32 	_ofs=f->get_pos();
		Uint32 header_len=f->get_32(); /* instrument size */
		Uint16 samples;
		XM_Instrument *_ins=0;
		Uint32 sample_header_size;
		for (j=0;j<22;j++)
			f->get_8(); /* skip name */
		f->get_8(); /* ignore type */
		samples=f->get_16();

		XM_DEBUG_PRINTF("Instrument: %i\n",i);
		XM_DEBUG_PRINTF("\tHeader Len: %i\n",header_len);

		if (samples == 0 ) { /* empty instrument */
			p_song->instrument_data[i]=0; /* no instrument by default */
			XM_DEBUG_PRINTF("\tSkipped!\n");

			while((f->get_pos()-_ofs)<header_len)
				f->get_8(); /* go to end of header */
			continue;
			/** @TODO goto header len **/
		} else if (samples>XM_CONSTANT_MAX_SAMPLES_PER_INSTRUMENT) {

			XM_ERROR_PRINTF("\tHas invalid sample count: %i\n",samples);

			xm_clear_song( p_song, p_load_music?p_song->pattern_count:-1, i );
			f->close();
			return ERR_FILE_CORRUPT;

		} else {

			p_song->instrument_data[i]=memnew( XM_Instrument );


			if (!p_song->instrument_data[i]) { /* Out of Memory */

				xm_clear_song( p_song, p_load_music?p_song->pattern_count:-1, i );
				f->close();
				return ERR_OUT_OF_MEMORY;
			}

			xm_zero_mem(p_song->instrument_data[i],sizeof(XM_Instrument));

		}

		_ins = p_song->instrument_data[i];
		_ins->sample_count=samples;

		/* reset the samples */
		_ins->samples=0;

		sample_header_size=f->get_32(); /* "sample header size" is redundant, so i ignore it */
		XM_DEBUG_PRINTF("\tSample Header Size: %i\n",sample_header_size);

		for (j=0;j<48;j++) {
			/* convert to nibbles */
			Uint8 nb=f->get_8()&0xF;
			nb|=f->get_8()<<4;
			_ins->note_sample[j]=nb;
		}

		for (j=0;j<12;j++) {

			Uint16 ofs=f->get_16();
			Uint16 val=f->get_16();
			_ins->volume_envelope.points[j]=(val<<9)|ofs; /* encode into 16 bits */
		}
		for (j=0;j<12;j++) {

			Uint16 ofs=f->get_16();
			Uint16 val=f->get_16();
			_ins->pan_envelope.points[j]=(val<<9)|ofs; /* encode into 16 bits */
		}

		_ins->volume_envelope.flags=f->get_8();
		_ins->pan_envelope.flags=f->get_8();

		_ins->volume_envelope.sustain_index=f->get_8();
		_ins->volume_envelope.loop_begin_index=f->get_8();
		_ins->volume_envelope.loop_end_index=f->get_8();

		_ins->pan_envelope.sustain_index=f->get_8();
		_ins->pan_envelope.loop_begin_index=f->get_8();
		_ins->pan_envelope.loop_end_index=f->get_8();


		{ /* Volume Envelope Flags */
			Uint8 env_flags=f->get_8();
			if ( env_flags&1 )
				_ins->volume_envelope.flags|=XM_ENVELOPE_ENABLED;
			if ( env_flags&2 )
				_ins->volume_envelope.flags|=XM_ENVELOPE_SUSTAIN_ENABLED;
			if ( env_flags&4 )
				_ins->volume_envelope.flags|=XM_ENVELOPE_LOOP_ENABLED;
		}
		{ /* Pan Envelope Flags */
			Uint8 env_flags=f->get_8();
			if ( env_flags&1 )
				_ins->pan_envelope.flags|=XM_ENVELOPE_ENABLED;
			if ( env_flags&2 )
				_ins->pan_envelope.flags|=XM_ENVELOPE_SUSTAIN_ENABLED;
			if ( env_flags&4 )
				_ins->pan_envelope.flags|=XM_ENVELOPE_LOOP_ENABLED;
		}

		_ins->vibrato_type = (XM_VibratoType)f->get_8();
		_ins->vibrato_sweep = f->get_8();
		_ins->vibrato_depth = f->get_8();
		_ins->vibrato_rate = f->get_8();
		_ins->fadeout = f->get_16();

		f->get_16(); /* reserved */

#ifdef XM_DEBUG

		XM_DEBUG_PRINTF("\tVolume Envelope:\n");
		XM_DEBUG_PRINTF("\t\tPoints: %i\n", _ins->volume_envelope.flags&XM_ENVELOPE_POINT_COUNT_MASK);
		XM_DEBUG_PRINTF("\t\tEnabled: %s\n", (_ins->volume_envelope.flags&XM_ENVELOPE_ENABLED)?"Yes":"No");
		XM_DEBUG_PRINTF("\t\tSustain: %s\n", (_ins->volume_envelope.flags&XM_ENVELOPE_SUSTAIN_ENABLED)?"Yes":"No");
		XM_DEBUG_PRINTF("\t\tLoop Enabled: %s\n", (_ins->volume_envelope.flags&XM_ENVELOPE_LOOP_ENABLED)?"Yes":"No");

		XM_DEBUG_PRINTF("\tPan Envelope:\n");
		XM_DEBUG_PRINTF("\t\tPoints: %i\n", _ins->pan_envelope.flags&XM_ENVELOPE_POINT_COUNT_MASK);
		XM_DEBUG_PRINTF("\t\tEnabled: %s\n", (_ins->pan_envelope.flags&XM_ENVELOPE_ENABLED)?"Yes":"No");
		XM_DEBUG_PRINTF("\t\tSustain: %s\n", (_ins->pan_envelope.flags&XM_ENVELOPE_SUSTAIN_ENABLED)?"Yes":"No");
		XM_DEBUG_PRINTF("\t\tLoop Enabled: %s\n", (_ins->pan_envelope.flags&XM_ENVELOPE_LOOP_ENABLED)?"Yes":"No");

#endif

		while((f->get_pos()-_ofs)<header_len)
			f->get_8(); /* Skip rest of header */

		/**** SAMPLES *****/

		/* allocate array */
		_ins->samples = memnew_arr( XM_Sample, samples );

		if (!_ins->samples) { /* Out of Memory */

			xm_clear_song( p_song, p_load_music?p_song->pattern_count:-1, i );
			f->close();
			return ERR_OUT_OF_MEMORY;
		}

		/* allocate samples */

		xm_zero_mem(_ins->samples,sizeof(XM_Sample)*samples);
		for (int m=0;m<samples;m++)
			_ins->samples[m].sample_id=AUDIO_SAMPLE_INVALID_ID;		

		{
 			//AudioSampleID sample_ids[XM_CONSTANT_MAX_SAMPLES_PER_INSTRUMENT];
			//bool recompress_sample[16]; /* samples to recompress */

			XM_DEBUG_PRINTF("\tSample_Names:\n");
			/** First pass, Sample Headers **/
			for (j=0;j<samples;j++) {

				XM_Sample *_smp=&_ins->samples[j];

				Uint32 length = f->get_32(); /* in bytes */
				Uint32 loop_start = f->get_32();
				Uint32 loop_end = f->get_32();
				AudioSampleFormat format;
				AudioSampleLoopType loop;
				Uint8 flags;
				_smp->volume = f->get_8();
				_smp->finetune = (Sint8)f->get_8();
				flags = f->get_8();
				_smp->pan = f->get_8();
				_smp->base_note = (Sint8)f->get_8();
				f->get_8(); /* reserved */
				Uint32 frame_length=length;

#ifdef XM_DEBUG
				XM_DEBUG_PRINTF("\t\t%i- ",j);
				for (k=0;k<22;k++) {
					char n=f->get_8();
					XM_DEBUG_PRINTF("%c",n);
				}
				XM_DEBUG_PRINTF("\n");
#else
				for (k=0;k<22;k++) {
					f->get_8();
				}
#endif

				XM_DEBUG_PRINTF("\tSample %i:, length %i\n",j,length);

				if (length>0) {
					/* SAMPLE DATA */

					
					switch ((flags>>3) & 0x2) {
						/* bit 3 of XM sample flags specify extended (propertary) format */
						case 0: 
							format=AUDIO_SAMPLE_FORMAT_PCM8;	
						break;
						case 2: 			
							format=AUDIO_SAMPLE_FORMAT_PCM16;
							frame_length/=2; /* cut in half,since length is bytes */
							loop_start/=2; /* cut in half,since length is bytes */
							loop_end/=2; /* cut in half,since length is bytes */		
						break;

					}
					switch (flags & 0x3) {

						case 3:
						case 0: {
							loop=AUDIO_SAMPLE_LOOP_NONE;
						} break;
						case 1: {								
							loop=AUDIO_SAMPLE_LOOP_FORWARD;
						} break;
						case 2: { 
							loop=AUDIO_SAMPLE_LOOP_PING_PONG;
						} break;
					}

					loop_end+=loop_start;
					
					AudioSampleManager * sm = AudioSampleManager::get_singleton();
					
					_smp->sample_id=sm->create(format,false,frame_length);

					if (_smp->sample_id==AUDIO_SAMPLE_INVALID_ID) { /* Out of Memory */

						xm_clear_song( p_song, p_load_music?p_song->pattern_count:-1, i );
						f->close();
						return ERR_OUT_OF_MEMORY;
					}
					
					sm->set_loop_type( _smp->sample_id, loop );
					sm->set_loop_begin( _smp->sample_id, loop_start );
					sm->set_loop_end( _smp->sample_id, loop_end );
					Uint32 sample_period = xm_player_get_linear_period( 49+_smp->base_note, _smp->finetune );
					Uint32 sample_freq = xm_player_get_linear_frequency(sample_period);
					sm->set_c5_freq( _smp->sample_id, sample_freq );
					
				} else {

					XM_DEBUG_PRINTF("\t\tSkipped!\n");
					_smp->sample_id=AUDIO_SAMPLE_INVALID_ID;
				}

			}

			/** Second pass, Sample Data **/
			for (j=0;j<samples;j++) {

				XM_Sample *_smp=&_ins->samples[j];

				if (_smp->sample_id==AUDIO_SAMPLE_INVALID_ID)
					continue;

				AudioSampleManager * sm = AudioSampleManager::get_singleton();
							
				switch (sm->get_format( _smp->sample_id )) {

					case AUDIO_SAMPLE_FORMAT_PCM16: {
						
						int bytes=sm->get_size(_smp->sample_id )*2;
						MID mid = Memory::alloc_dynamic( bytes );
						Memory::lock( mid );
						Sint16*data =(Sint16*)Memory::get(mid); 
						
						Sint16 old=0;
						for (k=0;k<sm->get_size(_smp->sample_id );k++) {
							Sint16 d=(Sint16)f->get_16();
							data[k]=d+old;
							old=data[k];
						}
						
						sm->store_data(_smp->sample_id, data );
						Memory::unlock( mid );
						Memory::free_dynamic(mid);
					} break;
					case AUDIO_SAMPLE_FORMAT_PCM8: {
						int bytes=sm->get_size(_smp->sample_id );
						MID mid = Memory::alloc_dynamic( bytes );
						Memory::lock( mid );
						Sint8*data =(Sint8*)Memory::get(mid); 
						Sint8 old=0;
						for (k=0;k<sm->get_size(_smp->sample_id );k++) {
							Sint8 d=(Sint8)f->get_8();
							data[k]=d+old;
							old=data[k];
						}
						sm->store_data(_smp->sample_id, data );
						Memory::unlock( mid );
						Memory::free_dynamic(mid);

					} break;
				}

#ifdef XM_DEBUG

				XM_DEBUG_PRINTF("\t\tLength: %i\n",sm->get_size(_smp->sample_id ));
				switch (sm->get_loop_type(_smp->sample_id )) {

					case AUDIO_SAMPLE_LOOP_NONE:  XM_DEBUG_PRINTF("\t\tLoop: Disabled\n"); break;
					case AUDIO_SAMPLE_LOOP_FORWARD:   XM_DEBUG_PRINTF("\t\tLoop: Forward\n"); break;
					case AUDIO_SAMPLE_LOOP_PING_PONG: XM_DEBUG_PRINTF("\t\tLoop: PingPong\n"); break;
				}

				XM_DEBUG_PRINTF("\t\tLoop Begin: %i\n",sm->get_loop_begin(_smp->sample_id ));
				XM_DEBUG_PRINTF("\t\tLoop End: %i\n",sm->get_loop_end(_smp->sample_id ));
				XM_DEBUG_PRINTF("\t\tVolume: %i\n",_smp->volume);
				XM_DEBUG_PRINTF("\t\tPan: %i\n",_smp->pan);
				XM_DEBUG_PRINTF("\t\tBase Note: %i\n",_smp->base_note);
				XM_DEBUG_PRINTF("\t\tFineTune: %i\n",_smp->finetune);
				XM_DEBUG_PRINTF("\t\tC5Freq: %i\n",sm->get_c5_freq(_smp->sample_id));
				switch (sm->get_format(_smp->sample_id )) {
					/* bit 3 of XM sample flags specify extended (propertary) format */
					case AUDIO_SAMPLE_FORMAT_PCM8: XM_DEBUG_PRINTF("\t\tFormat: PCM8\n"); break;
					case AUDIO_SAMPLE_FORMAT_PCM16: XM_DEBUG_PRINTF("\t\tFormat: PCM16\n"); break;
				}

#endif
			}



		}


	}

	f->close();
	return OK;

}
Error AudioStreamXM::xm_loader_open_song( String p_filename, XM_Song *p_song ) {

	return xm_loader_open_song_custom( p_filename, p_song, true, true );
}

Error AudioStreamXM::xm_loader_open_song_music( String p_filename, XM_Song *p_song ) {

	return xm_loader_open_song_custom( p_filename, p_song, true, false );
}
Error AudioStreamXM::xm_loader_open_instruments( String p_filename, XM_Song *p_song )  {

	return xm_loader_open_song_custom( p_filename, p_song, false, true );
}


void AudioStreamXM::xm_loader_free_song( XM_Song *p_song ) {

	xm_clear_song(p_song, p_song->pattern_count, p_song->instrument_count );
}
void AudioStreamXM::xm_loader_free_music( XM_Song *p_song ) {

	xm_clear_song( p_song, p_song->pattern_count, -1 );

}
void AudioStreamXM::xm_loader_free_instruments( XM_Song *p_song ) {

	xm_clear_song( p_song, -1, p_song->instrument_count );

}


AudioStream::Status AudioStreamXM::get_status() const {

	return status;
}

void AudioStreamXM::clear_all_voices() {

	XM_AUDIO_LOCK;
	for (int i=0;i<XM_MAX_CHANNELS;i++) {
	
		if (xm_player->channel[i].active) {
		
			AudioServer::get_singleton()->get_mixer()->voice_stop( xm_player->channel[i].mixer_voice_id );
			xm_player->channel[i].mixer_voice_id=AUDIO_VOICE_INVALID_ID;
			xm_player->channel[i].active=false;
			
		}
	}
	XM_AUDIO_UNLOCK;
}

void AudioStreamXM::set_status(Status p_status) {

	if (status==p_status)
		return;
		
	switch( p_status ) {
		case PLAY: {
		
			if (status==STOP) {
				xm_player_reset();			
				repeat_counter=0;
				last_order=0;
				if (xm_song->order_count==0)
					return;
				
				xm_player->active=true;
				
			}
			
		} break;
		case PAUSE: {
		
			if (status==STOP)
				return;
			clear_all_voices();
		} break;
		case STOP: {
		
			clear_all_voices();
			//if (status==STOP)
			//	return;
		
		} break;
	}
	
	status=p_status;
}

String AudioStreamXM::get_name() const {

	return xm_song->name;
}

void AudioStreamXM::set_volume(float p_volume) {


}
float AudioStreamXM::get_volume() const {

	return 1.0;
}

int AudioStreamXM::get_length() const {

	return -1;
}
int AudioStreamXM::get_pos() const {

	return 0;
}
Error AudioStreamXM::seek(Uint32 p_msec_pos) {

	return OK;
}

int AudioStreamXM::get_BPM() const {

	return -1;
}
Uint64 AudioStreamXM::get_BPM_sync() const {

	return 0;
}

Error AudioStreamXM::load_song(String p_file) {
	
	Error err = xm_loader_open_song( p_file, xm_song );
	if (err==OK) {
	
		file=p_file;
	}
	
	return err;
}
Error AudioStreamXM::load_song_music(String p_file) {
	
	return xm_loader_open_song_music( p_file, xm_song );
		
}
Error AudioStreamXM::load_song_instruments(String p_file) {
	
	return xm_loader_open_instruments( p_file, xm_song );
	
}
void AudioStreamXM::clear_song() {
	
	xm_clear_song( xm_song, xm_song->pattern_count, xm_song->instrument_count );	
}


void AudioStreamXM::set(String p_path, const Variant& p_value) {
	
	if (p_path=="file") {
	
		if (p_value.get_string()=="") {
			clear_song();
			file="";
		} else {
			load_song(p_value.get_string());		
		}
	}
		
}
Variant AudioStreamXM::get(String p_path) const {
	
	if (p_path=="file")
		return file;
	
	return Variant();
}

void AudioStreamXM::get_property_list( List<PropertyInfo> *p_list ) const {
	
	p_list->push_back( PropertyInfo( Variant::STRING, "file" ) );
	
}



IAPI::PropertyHint AudioStreamXM::get_property_hint(String p_path) const {

	if (p_path=="file") {
	
		PropertyHint hint;
		hint.type=PropertyHint::HINT_FILE;
		hint.hint=".xm";
		hint.description="Set to load an XM-format song.";
		return hint;
	}
	
	return PropertyHint();
}

int AudioStreamXM::get_repeats() const {

	return repeat_counter;
}

AudioStreamXM::AudioStreamXM() {

	// original code just wants it available all the time. No problem with this!
	
	
	mixer_tick_size=0;
	xm_fileio = FileAccess::create();
	
	xm_song=memnew( XM_Song );
	xm_zero_mem( xm_song, sizeof(XM_Song) );
	xm_song->tempo=125;
	xm_song->speed=6;  

	
	xm_player=memnew( XM_PlayerData );
	xm_zero_mem( xm_player, sizeof(XM_PlayerData) );
	xm_player->song=xm_song;
	xm_player_reset();
	 
	xm_player->active=true;
	
	status=STOP;
	repeat_counter=0;
	last_order=0;
	
	AudioMixer *am = AudioServer::get_singleton()->get_mixer();
	
	am->mixer_tick_signal.connect( this, &AudioStreamXM::mixer_tick_callback);
	config_volume=255;	
	mixer_tick_size=(2500000/125);
}


AudioStreamXM::~AudioStreamXM() {

	clear_all_voices();	
	clear_song();
	memdelete(xm_song);
	memdelete( xm_player );
	memdelete(xm_fileio);
}
 
