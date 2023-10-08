
#ifndef PRINT_ERROR_LIBC_H
#define PRINT_ERROR_LIBC_H

void set_print_error_libc(); ///< Calling this will make the ERR_* macros to print errors using printf

void print_error_libc(const char* p_file,int p_line,const char* p_error);




#endif
