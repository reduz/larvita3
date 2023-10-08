//
// C++ Interface: rstring
//
// Description:
//
//
// Author: red <red@killy>, (C) 2005
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef RSTRING_H
#define RSTRING_H

#include "typedefs.h"

/**
	@author red <red@killy>
*/



class CharString {

	struct Shared {

		int refcount;
		char *data;
	};

	Shared *shared;
	void free_shared();
	void take_shared(Shared *p_shared);

friend class String;
	CharString(char *p_data);
public:
	int length() const;
	const CharString& operator=(const CharString &p_str);
	const char *get_data();
	operator const char*() {return get_data();};
	operator char*() {return (char*)get_data();};
	CharString();
	CharString(const CharString &p_src);
	~CharString();
};

class String {
public:
	typedef wchar_t CharType;
private:

	struct Shared {
		int len;
		int refcount;
		CharType data[1]; /** data will always at least one (zero)*/
	};

	Shared *shared;

	/**
	 * Creates the shared data, to a fixed length in case it's needed, and a zero always att he end
	 * By default creates empty
	 */
	//void create_shared(int p_length=0);

	/**
	 * Free shared data. If still being used by other strings, it decreases the refcount, otherwise it deletes the data
	 */

	void free_shared();

	/**
	 * Copy the data from another string. It shares the data with the other string
	 */
	void copy_from(String& p_string);

	/**
	 * Copy the data from something else, the data is not shared as it is not possible
	 */

	void copy_from(const char *p_cstr);
	void copy_from(const CharType *p_cstr,int p_clip_to_len=-1);
	void copy_from(const CharType& p_char);

	/**
	 * Resize the data, copy on write should often be used before
	 */

	void resize_shared(int p_newsize);


	 /**
 	 * Copy on write:
	 * If the current string is going to be EDITED (not replaced)
	 * this class creates a local copy of the shared string data (in case it is used by many strings, otherwise it's not touched)
 */
	void copy_on_write(); //copy on write

public:

	/* Regular Operators */
	String operator=(String p_str);
	/*bool operator=(CharType p_chr); causes more trouble than benefit */
	bool operator==(String p_str) const;
	bool operator!=(String p_str) const;
	String operator+(const String &) const;
	String operator+(CharType p_char) const;

	String& operator+=(const String &);
	String& operator+=(CharType p_str);
	String& operator+=(const char * p_str);
	String& operator+=(const CharType * p_str);

	/* Compatibility Operators */

	void operator=(const char *p_str);
	void operator=(const CharType *p_str);
	bool operator==(const char *p_str) const;
	bool operator==(const CharType *p_str) const;
	bool operator!=(const char *p_str) const;
	bool operator!=(const CharType *p_str) const;
	bool operator<(const CharType *p_str) const;
	bool operator<(const char *p_str) const;
	bool operator<(String p_str) const;

	signed char nocasecmp_to(String p_str) const; ////strcmp like, <0 for less 0 for equal > 0 for greater

	/* [] op */
	const CharType& operator[](int p_idx) const; //constref
	CharType& operator[](int p_idx); //assignment

	const CharType * c_str() const;
	/* standard size stuff */

	int length() const;
	int size() const;
	bool empty() const;

	/* complex helpers */
	String substr(int p_from,int p_chars) const;
	int find(String p_str,int p_from=0) const; ///< return <0 if failed
	int find_last(String p_str) const; ///< return <0 if failed
	int findn(String p_str,int p_from=0) const; ///< return <0 if failed, case insensitive
	void replace(String p_key,String p_with);
	void insert(int p_at_pos,String p_string);
	static String num(double p_num,int p_digits=-1);
	static String hex(int p_num, bool p_add_0x = true);
	static String chr(CharType p_char);
	double to_double() const;
	int to_int() const;

	int get_slice_count(String p_splitter) const;
	String get_slice(String p_splitter,int p_slice) const;

	String to_upper() const;

	String left(int p_pos) const;
	String right(int p_pos) const;
	String strip_edges();

	void erase(int p_pos, int p_chars);

	CharString ascii(bool p_allow_extended=false) const;
	CharString utf8() const;
	bool parse_utf8(const char* p_utf8); //return true on error
	static String utf8(const char* p_utf8);

	Uint32 hash() const; /* hash the string */
	
	/**
	 * The constructors must not depend on other overloads
	 */
	String();
	String(const String &p_string);
/*	String(CharType p_char);*/
	String(const char *p_str);
	String(const CharType *p_str,int p_clip_to_len=-1);
	~String();

};


bool operator==(const char*p_chr, const String& p_str);

String operator+(const char*p_chr, const String& p_str);
String operator+(String::CharType p_chr, const String& p_str);




 /* end of namespace */


#endif
