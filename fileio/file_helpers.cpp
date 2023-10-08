#include "file_helpers.h"

#include "os/file_access.h"

String FileHelpers::read_file_to_string(String p_filename) {

	String ret;
	
	FileAccess* fa = FileAccess::create();
	ERR_FAIL_COND_V(!fa, ret);
	
	if (fa->open(p_filename, FileAccess::READ) != OK) {
		
		memdelete(fa);
		ERR_FAIL_V("");
	};
	
	for (unsigned int i=0; i<fa->get_len(); i++) {
		
		ret += String::chr(fa->get_8());
	};

	fa->close();
	memdelete(fa);
	
	return ret;	
};

Error FileHelpers::write_string_to_file(String p_filename, const String p_contents) {
	
	FileAccess* fa = FileAccess::create();
	ERR_FAIL_COND_V(!fa, FAILED);
	
	if (fa->open(p_filename, FileAccess::WRITE) != OK) {
		
		memdelete(fa);
		ERR_FAIL_V(FAILED);
	};

	CharString cs = p_contents.utf8();
	fa->store_buffer((Uint8*)cs.get_data(), cs.length());

	Error err = fa->get_error();
	
	fa->close();
	memdelete(fa);
	
	return err;
};
	
Error FileHelpers::read_file_to_buffer(String p_filename, DVector<Uint8>& p_buffer) {

	FileAccess* fa = FileAccess::create();
	ERR_FAIL_COND_V(!fa, FAILED);
	
	if (fa->open(p_filename, FileAccess::READ) != OK) {
		
		memdelete(fa);
		ERR_FAIL_V(FAILED);
	};

	p_buffer.resize(fa->get_len());
	p_buffer.write_lock();
	fa->get_buffer(p_buffer.write(), fa->get_len());
	p_buffer.write_unlock();
	
	Error err = fa->get_error();
	
	fa->close();
	memdelete(fa);
	
	return err;
};

Error FileHelpers::write_buffer_to_file(String p_filename, const DVector<Uint8>& p_buffer) {
	
	FileAccess* fa = FileAccess::create();
	ERR_FAIL_COND_V(!fa, FAILED);
	
	if (fa->open(p_filename, FileAccess::WRITE) != OK) {
		
		memdelete(fa);
		ERR_FAIL_V(FAILED);
	};

	p_buffer.read_lock();
	fa->store_buffer(p_buffer.read(), p_buffer.size());
	p_buffer.read_unlock();
	
	Error err = fa->get_error();
	
	fa->close();
	memdelete(fa);
	
	return err;
};
