#ifndef ERROR_LIST_H
#define ERROR_LIST_H

/** Error List. Please never compare an error against FAILED
 * Either do result != OK , or !result. This way, Error fail
 * values can be more detailed in the future.
 *
 * This is a generic error list, mainly for organizing a language of returning errors.
 */

enum Error {
	OK,
	FAILED, ///< Generic fail error
	ERR_UNAVAILABLE, ///< What is requested is unsupported/unavailable
	ERR_UNCONFIGURED, ///< The object being used hasnt been properly set up yet
	ERR_UNAUTHORIZED, ///< Missing credentials for requested resource
	ERR_PARAMETER_RANGE_ERROR, ///< Parameter given out of range
	ERR_OUT_OF_MEMORY, ///< Out of memory
	ERR_FILE_NOT_FOUND,
	ERR_FILE_BAD_DRIVE,
	ERR_FILE_BAD_PATH,
	ERR_FILE_NO_PERMISSION,
	ERR_FILE_ALREADY_IN_USE,
	ERR_FILE_CANT_OPEN,
	ERR_FILE_CANT_WRITE,
	ERR_FILE_CANT_READ,
	ERR_FILE_UNRECOGNIZED,
	ERR_FILE_CORRUPT,
	ERR_FILE_EOF,
	ERR_CANT_OPEN, ///< Can't open a resource/socket/file
 	ERROR_QUERY_FAILED,
	ERR_ALREADY_IN_USE,
 	ERR_LOCKED, ///< resource is locked
 	ERR_TIMEOUT,
 	ERR_CANT_AQUIRE_RESOURCE,
	ERR_INVALID_DATA, ///< Data passed is invalid
	ERR_INVALID_PARAMETER, ///< Parameter passed is invalid
	ERR_ALREADY_EXISTS, ///< When adding, item already exists
	ERR_DOES_NOT_EXIST, ///< When retrieving/erasing, it item does not exist
	ERR_DATABASE_CANT_READ, ///< database is full
	ERR_DATABASE_CANT_WRITE, ///< database is full
 	ERR_COMPILATION_FAILED,
  	ERR_LINK_FAILED,
	ERR_BUG, ///< a bug in the software certainly happened, due to a double check failing or unexpected behavior.
	ERR_OMFG_THIS_IS_VERY_VERY_BAD, ///< shit happens, has never been used, though
	ERR_WTF = ERR_OMFG_THIS_IS_VERY_VERY_BAD ///< short version of the above
};



#endif

