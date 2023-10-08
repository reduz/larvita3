#ifndef FILE_ACCESS_HTTP_H
#define FILE_ACCESS_HTTP_H

#include "file_access_buffered.h"

class HTTPClient;

class FileAccessHTTP : public FileAccessBuffered {

public:
	enum {
		HTTP_BUFFER_SIZE = 4096,
	};

private:
	HTTPClient* client;

	bool connect_client();

	int read_data_block(int p_offset, int p_len);
public:

	bool file_exists(String p_filename) { return false; };

	virtual Error open(String p_filename, int p_mode_flags);
	virtual void close();

	static FileAccess* create();
	static void set_as_default();

	FileAccessHTTP();
	virtual ~FileAccessHTTP();
};

#endif

