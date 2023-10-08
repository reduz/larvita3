#ifndef FILE_HELPERS_H
#define FILE_HELPERS_H

#include "rstring.h"
#include "dvector.h"

class FileHelpers {

public:

	static String read_file_to_string(String p_filename);
	static Error write_string_to_file(String p_filename, const String p_contents);
	
	static Error read_file_to_buffer(String p_filename, DVector<Uint8>& p_buffer);
	static Error write_buffer_to_file(String p_filename, const DVector<Uint8>& p_buffer);
	
};

#endif
