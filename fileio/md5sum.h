#ifndef MD5_CHECKSUM_H
#define MD5_CHECKSUM_H

#include "types/string/rstring.h"
#include "types/dvector.h"

class MD5Checksum {
	
public:
	
	struct Digest {

		static const Digest ZERO;
		
		unsigned char digest[16];

		bool operator==(const Digest& p_r);		
		void clear();
		bool is_zero();
		Digest& operator=(const Digest& p_r);
		operator const void*() { return is_zero()?NULL:this; };
		
		String as_string();
		void from_string(String p_str);

		Digest(const String& p_str) { from_string(p_str); };
		Digest(const Digest& p_r) { operator=(p_r); };
		Digest() { clear(); };
	};
	
	static Digest compute(const DVector<Uint8>& p_vector);
	static Digest compute(const Uint8* p_buffer, int p_size);
	static Digest compute(String p_fname);
};


#endif
