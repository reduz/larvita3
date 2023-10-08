

//#ifdef LIBC_ENABLED

#include <stdio.h>
#include  "print_error_libc.h"
#include "error_macros.h"
#include "print_string.h"
#include "rstring.h"

void print_string_libc(String p_string) {

	printf("%ls\n",p_string.c_str());
}

void set_print_error_libc() {

	print_error_func=print_error_libc;
	_print_func=print_string_libc;
}

void print_error_libc(const char* p_file,int p_line,const char* p_error) {

	printf("***ERROR*** %s:%i - %s\n",p_file,p_line,p_error);
}

//#endif

