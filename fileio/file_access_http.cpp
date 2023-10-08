#include "file_access_http.h"

#include <string.h>

#include "error_macros.h"
#include "os/http_client.h"

#include "global_vars.h"

void FileAccessHTTP::set_as_default() {
	
	FileAccess::create_func = FileAccessHTTP::create;
};

FileAccess* FileAccessHTTP::create() {
	
	return memnew(FileAccessHTTP);
};

Error FileAccessHTTP::open(String p_filename, int p_mode_flags) {
	
	close();

	if (!connect_client()) {
		return set_error(ERR_CANT_OPEN);
	};
		
	client->get(p_filename, 0, 1);
	if (client->get_status() != HTTPClient::STATUS_DATA_READY) {
		
		return set_error(ERR_CANT_OPEN);
	};
	
	file.size = client->get_total_file_size();
	file.offset = 0;
	file.open = true;
	file.name = p_filename;
	
	return set_error(OK);
};

bool FileAccessHTTP::connect_client() {

	if (client) {
		if (client->get_status() == HTTPClient::STATUS_CONNECTED) {
			
			return true;
		};
		
		memdelete(client);
	};
	
	client = memnew(HTTPClient);

	String host = GlobalVars::get_singleton()->get("http_resource_host").get_string();
	if (host == "") host = "localhost";
	
	int port = GlobalVars::get_singleton()->get("http_resource_port").get_int();
	if (port == 0) port = 80;
	
	client->connect(host, port);
	
	return client->get_status() == HTTPClient::STATUS_CONNECTED;
};

int FileAccessHTTP::read_data_block(int p_offset, int p_len) {

	if (!connect_client()) return -1;
	
	if (client->get(file.name, p_offset, p_offset + (p_len - 1)) != 0)
		return -1;
	
	cache.buffer.resize(p_len);
	
	int to_read = p_len;
	int buf_ofs = 0;
	
	ERR_FAIL_COND_V(to_read != client->data_left(), -1); // assert that to_read == data_left
	
	while (to_read) {
		
		cache.buffer.write_lock();
		int read = client->get_data(&cache.buffer.write()[buf_ofs], to_read);
		cache.buffer.write_unlock();
		
		if (read == -1) {
			return -1;
		};
		
		to_read -= read;
		buf_ofs += read;
	};
	
	cache.offset = p_offset;
	
	return p_len;
};

void FileAccessHTTP::close() {
	
	if (client) {
		memdelete(client);
		client = NULL;
	};
	
	file.offset = 0;
	file.size = 0;
	file.open = false;
	file.name = "";
	
	cache.buffer.resize(0);
	cache.offset = 0;
};



FileAccessHTTP::FileAccessHTTP() {

	client = NULL;	
	set_endian_conversion(false);
	set_cache_size(HTTP_BUFFER_SIZE);
};

FileAccessHTTP::~FileAccessHTTP(){
	
	if (client) {
		
		memdelete(client);
	};
}
