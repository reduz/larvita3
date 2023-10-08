//
// C++ Implementation: audio_loader_wav
//
// Description: 
//
//
// Author: ,,, <red@lunatea>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "audio_loader_wav.h"
#include "os/memory.h"


AudioSampleID AudioFormatLoaderWAV::load_sample(String p_path,FileAccess *p_file) {

	FileAccess *file=p_file;

	if (file->open( p_path, FileAccess::READ )!=OK) {
		return AUDIO_SAMPLE_INVALID_ID;
	}
	

	/* CHECK RIFF */
	char riff[5];
	riff[4]=0;
	file->get_buffer((Uint8*)&riff,4); //RIFF

	if (riff[0]!='R' || riff[1]!='I' || riff[2]!='F' || riff[3]!='F') {

		file->close();
		return AUDIO_SAMPLE_INVALID_ID;
	}


	/* GET FILESIZE */
	Uint32 filesize=file->get_32();

	/* CHECK WAVE */
	
	char wave[4];
	
	file->get_buffer((Uint8*)&wave,4); //RIFF

	if (wave[0]!='W' || wave[1]!='A' || wave[2]!='V' || wave[3]!='E') {


		file->close();
		return AUDIO_SAMPLE_INVALID_ID;
	}
	
	AudioSampleManager *sm = AudioSampleManager::get_singleton(); //get sample manager ready

	bool format_found=false;
	int format_bits=0;
	int format_channels=0;
	int format_freq=0;

	AudioSampleID id=AUDIO_SAMPLE_INVALID_ID;
	
	while (!file->eof_reached()) {


		/* chunk */
		char chunkID[4];
		file->get_buffer((Uint8*)&chunkID,4); //RIFF


		
		/* chunk size */
		Uint32 chunksize=file->get_32();
		Uint32 file_pos=file->get_pos(); //save file pos, so we can skip to next chunk safely

		if (file->eof_reached()) {

			ERR_PRINT("EOF REACH");
			break;
		}
		
		if (chunkID[0]=='f' && chunkID[1]=='m' && chunkID[2]=='t' && chunkID[3]==' ' && !format_found) {
			/* IS FORMAT CHUNK */

			Uint16 compression_code=file->get_16();
			
			if (compression_code!=1) {
				ERR_PRINT("Format not supported for WAVE file (not PCM)");
				break;
			}

			format_channels=file->get_16();
			if (format_channels!=1 && format_channels !=2) {

				ERR_PRINT("Format not supported for WAVE file (not stereo or mono)");
				break;

			}

			format_freq=file->get_32(); //sampling rate

			file->get_32(); // average bits/second (unused)
			file->get_16(); // block align (unused)
			format_bits=file->get_16(); // bits per sample

			if (format_bits%8) {

				ERR_PRINT("Strange number of bits in sample (not 8,16,24,32)");
				break;
			}

			/* Dont need anything else, continue */
			format_found=true;
		}


		if (chunkID[0]=='d' && chunkID[1]=='a' && chunkID[2]=='t' && chunkID[3]=='a') {
			/* IS FORMAT CHUNK */


			if (!format_found) {
				ERR_PRINT("'data' chunk before 'format' chunk found.");
				break;

			}

			int frames=chunksize;
			frames/=format_channels;
			frames/=(format_bits>>3);

			
			id = sm->create( (format_bits==8) ? AUDIO_SAMPLE_FORMAT_PCM8 : AUDIO_SAMPLE_FORMAT_PCM16 , (format_channels==2)?true:false, frames );
			sm->set_c5_freq( id, format_freq );

			if (id==AUDIO_SAMPLE_INVALID_ID) {
	
				break;
			}

			MID mid = Memory::alloc_dynamic(chunksize,"AudioFormatLoaderWAV::data_ptr");
			Memory::lock( mid );
			void * data_ptr = Memory::get( mid );
		
			for (int i=0;i<frames;i++) {


				for (int c=0;c<format_channels;c++) {


					if (format_bits==8) {
						// 8 bit samples are UNSIGNED
						
						Uint8 s = file->get_8();
						s-=128;
						Sint8 *sp=(Sint8*)&s;
						
						Sint8 *data_ptr8=&((Sint8*)data_ptr)[i*format_channels+c];
						
						*data_ptr8=*sp;

					} else {
						//16+ bits samples are SIGNED
						// if sample is > 16 bits, just read extra bytes
						
						Uint32 data=0;
						for (int b=0;b<(format_bits>>3);b++) {
							
							data|=((Uint32)file->get_8())<<(b*8);
						}
						data<<=(32-format_bits);
						
						
						Sint32 s=data;
						
						Sint16 *data_ptr16=&((Sint16*)data_ptr)[i*format_channels+c];

						*data_ptr16=s>>16;
					}
				}
				
			}
				
				
			sm->store_data( id, data_ptr );
				
			Memory::unlock( mid );
			Memory::free_dynamic( mid );
			
			if (file->eof_reached()) {
				//premature end of file
				sm->destroy( id );
				id=AUDIO_SAMPLE_INVALID_ID;
			} 
			
			break;
		}

		file->seek( file_pos+chunksize );
	}

	
	file->close();
	return id;

}


AudioFormatLoaderWAV::AudioFormatLoaderWAV()
{
}


AudioFormatLoaderWAV::~AudioFormatLoaderWAV()
{
}


