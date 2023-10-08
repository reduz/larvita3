#ifndef ERROR_MACROS_H
#define ERROR_MACROS_H


/**
 * Error macros. Unlike exceptions and asserts, these macros try to mantain consistency and stability
 * inside the code. It is recommended to always return processable data, so in case of an error, the
 * engine can stay working well. 
 * In most cases, bugs and/or invalid data are not fatal and should never allow a perfectly running application
 * to fail or crash.
 */

/** 
 * Pointer to the error macro priting function. Reassign to any function to have errors printed
 */

extern void (*print_error_func)(const char* p_file,int p_line,const char *p_error);

/** Function used by the error macros */
void print_error(const char* p_file,int p_line,const char *p_error);


#define _STR(m_x) #m_x
#define _FNL __FILE__":"

/** An index has failed if m_index<0 or m_index >=m_size, the function exists */

#define ERR_FAIL_INDEX(m_index,m_size) \
	 do {if ((m_index)<0 || (m_index)>=(m_size)) { \
		print_error(__FILE__,__LINE__,"Index "_STR(m_index)" out of size ("_STR(m_size)").");	\
		return;	\
	}} while(0);	\

/** An index has failed if m_index<0 or m_index >=m_size, the function exists.
  * This function returns an error value, if returning Error, please select the most
  * appropriate error condition from error_macros.h
  */

#define ERR_FAIL_INDEX_V(m_index,m_size,m_retval) \
	 do {if ((m_index)<0 || (m_index)>=(m_size)) { \
		print_error(__FILE__,__LINE__,"Index "_STR(m_index)" out of size ("_STR(m_size)").");	\
		return m_retval;	\
	}} while (0);

/** An error condition happened (m_cond tested true) (WARNING this is the opposite as assert(). 
 * the function will exit.
 */

#define ERR_FAIL_COND(m_cond) \
	{ if ( m_cond ) {	\
		print_error(__FILE__,__LINE__,"Condition ' "_STR(m_cond)" ' failed.");	\
		return;	 \
	} }	\

/** An error condition happened (m_cond tested true) (WARNING this is the opposite as assert(). 
 * the function will exit.
 * This function returns an error value, if returning Error, please select the most
 * appropriate error condition from error_macros.h
 */

#define ERR_FAIL_COND_V(m_cond,m_retval) \
	{ if ( m_cond ) {	\
		print_error(__FILE__,__LINE__,"Condition ' "_STR(m_cond)" ' failed. returned: "_STR(m_retval));	\
		return m_retval;	 \
	} }	\

/** An error condition happened (m_cond tested true) (WARNING this is the opposite as assert(). 
 * the loop will skip to the next iteration.
 */

#define ERR_CONTINUE(m_cond) \
	{ if ( m_cond ) {	\
		print_error(__FILE__,__LINE__,"Condition ' "_STR(m_cond)" ' failed. Continuing..:");	\
		continue;\
	} }	\

/** Print an error string and return
 */

#define ERR_FAIL() \
{ \
		print_error(__FILE__,__LINE__,"Method/Function Failed.");	\
		return;\
} \

/** Print an error string and return with value
 */

#define ERR_FAIL_V(m_value) \
{ \
		print_error(__FILE__,__LINE__,"Method/Function Failed, returning: "__STR(m_value));	\
		return m_value;\
} \

/** Print an error string.
 */

#define ERR_PRINT(m_string) \
	{ \
		print_error(__FILE__,__LINE__,m_string);	\
	} \

/** Print a warning string.
 */

#define WARN_PRINT(m_string) \
	{ \
		print_error(__FILE__,__LINE__,m_string);	\
	} \



#endif
