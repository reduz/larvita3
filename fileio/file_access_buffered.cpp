#include "file_access_buffered.h"

#include <string.h>

#include "error_macros.h"

#include "global_vars.h"

Error FileAccessBuffered::set_error(Error p_error) {
	
	return (last_error = p_error);
};

void FileAccessBuffered::set_cache_size(int p_size) {
	
	cache_size = p_size;
};

int FileAccessBuffered::get_cache_size() {
	
	return cache_size;
};

bool FileAccessBuffered::cache_data_left(int p_len) {

	if (p_len > cache_size) {
		ERR_PRINT("Asking for chunk bigger than cache");
		return false;
	};
	
	if (file.size < file.offset + p_len) {
		return false;
	};

	int buff_data_left = (cache.offset + cache.buffer.size()) - file.offset;
	if (file.offset >= cache.offset && buff_data_left >= p_len) {
		return true;
	};

	int file_data_left = file.size - file.offset;
	if (file_data_left > cache_size) {
		file_data_left = cache_size;
	};
	
	int read_ofs = file.offset;
	if (file_data_left < cache_size) {
		
		read_ofs = file.size - cache_size;
		file_data_left = cache_size;
	};
	if (read_ofs < 0) {
		read_ofs = 0;
		file_data_left = file.size;
	};
	
	return read_data_block(read_ofs, file_data_left) >= p_len;
};

void FileAccessBuffered::seek(Uint32 p_position) {
	
	file.offset = p_position;
};

void FileAccessBuffered::seek_end(Sint32 p_position) {
	
	file.offset = file.size + p_position;
};

Uint32 FileAccessBuffered::get_pos() {
	
	return file.offset;
};

Uint32 FileAccessBuffered::get_len() {
	
	return file.size;
};

bool FileAccessBuffered::eof_reached() {
	
	return file.offset == file.size;
};

Uint8 FileAccessBuffered::get_8() {

	ERR_FAIL_COND_V(!file.open,0);

	if (!cache_data_left(1)) {
		ERR_PRINT("Error reading byte");
		return 0;
	};

	Uint8 byte = cache.buffer[file.offset - cache.offset];
	++file.offset;
	
	return byte;
};

void FileAccessBuffered::get_buffer(Uint8 *p_dest,int p_elements) {

	ERR_FAIL_COND(!file.open);

	if (p_elements + file.offset > file.size) {
		ERR_PRINT("Not enough data in file");
		return;
	};
	
	int to_read = p_elements;
	int max_read_size = cache_size;
	int arr_offset = 0;
	while (to_read > 0) {

		int read = to_read > max_read_size?max_read_size:to_read;
		
		if (!cache_data_left(read)) {
			
			ERR_PRINT("Error reading");
			return;
		};

		cache.buffer.write_lock();
		memcpy(p_dest+arr_offset, &cache.buffer.read()[file.offset - cache.offset], read);
		cache.buffer.write_unlock();
		
		file.offset += read;
		arr_offset += read;
		
		to_read -= read;
	};
};

void FileAccessBuffered::set_endian_conversion(bool p_swap) {

	perform_endian_swap = p_swap;
};

bool FileAccessBuffered::is_open() {
	
	return file.open;
};

Error FileAccessBuffered::get_error() {
	
	return last_error;
};

void FileAccessBuffered::store_8(Uint8 p_dest) {

	ERR_PRINT("File access is read-only");
};

FileAccessBuffered::FileAccessBuffered() {

	perform_endian_swap=false;
	cache_size = DEFAULT_CACHE_SIZE;
};

FileAccessBuffered::~FileAccessBuffered(){
	
}
