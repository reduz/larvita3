#ifdef OS_OTHER

#include "os/file_access.h"

#include "os/memory.h"

#include <time.h>
#include <unistd.h>

extern "C" {

#include "sqliteInt.h"

#define MAX_PATHNAME 512


typedef struct lvFile lvFile;
struct lvFile {
  sqlite3_io_methods const *pMethod;  /* Always the first entry */
  
  FileAccess* fa;
};

static int lvClose(sqlite3_file *id){

	lvFile *pFile = (lvFile *)id;
	if( !pFile ) return SQLITE_OK;

	pFile->fa->close();
	memdelete(pFile->fa);

	memset(pFile, 0, sizeof(lvFile));
	
	return SQLITE_OK;
};

static int lvRead(
  sqlite3_file *id, 
  void *pBuf, 
  int amt,
  sqlite3_int64 offset
){
	assert( id );
	lvFile* f = (lvFile*)id;
	f->fa->seek(offset);
	f->fa->get_buffer((Uint8*)pBuf, amt);
	
	return SQLITE_OK;
}

static int lvWrite(
  sqlite3_file *id, 
  const void *pBuf, 
  int amt,
  sqlite3_int64 offset 
){
	assert( id );
	assert( amt>0 );
	lvFile* f = (lvFile*)id;
	
	f->fa->seek(offset);
	f->fa->store_buffer((Uint8*)pBuf, amt);
	
	return SQLITE_OK;
};

static int lvTruncate(sqlite3_file *id, i64 nByte) {
	ERR_FAIL_COND_V(1, SQLITE_IOERR_TRUNCATE);
};

static int lvFileSize(sqlite3_file *id, i64 *pSize) {
	
	assert( id );
	lvFile* f = (lvFile*)id;
	*pSize = f->fa->get_len();
	
	return SQLITE_OK;
};

static int lvSync(sqlite3_file *id, int flags) {

	ERR_FAIL_COND_V(1, SQLITE_IOERR_FSYNC);
};

static int lvCheckReservedLock(sqlite3_file *id) {
  return 0;
}

static int lvLock(sqlite3_file *id, int locktype) {
  return SQLITE_OK;
}

static int lvUnlock(sqlite3_file *id, int locktype) {
  return SQLITE_OK;
}

static int lvFileControl(sqlite3_file *id, int op, void *pArg){

	*(int*)pArg = NO_LOCK;
	return SQLITE_OK;
};

static int lvSectorSize(sqlite3_file *id){
	return SQLITE_DEFAULT_SECTOR_SIZE;
}

static int lvDeviceCharacteristics(sqlite3_file *id){
  return 0;
}


static const sqlite3_io_methods sqlite3Larvita2IoMethod = {
  1,                        /* iVersion */
  lvClose,
  lvRead,
  lvWrite,
  lvTruncate,
  lvSync,
  lvFileSize,
  lvLock,
  lvUnlock,
  lvCheckReservedLock,
  lvFileControl,
  lvSectorSize,
  lvDeviceCharacteristics
};

static int lvOpen(
  sqlite3_vfs *pVfs, 
  const char *zPath, 
  sqlite3_file *pFile,
  int flags,
  int *pOutFlags
){

	int isExclusive  = (flags & SQLITE_OPEN_EXCLUSIVE);
	int isDelete     = (flags & SQLITE_OPEN_DELETEONCLOSE);
	int isCreate     = (flags & SQLITE_OPEN_CREATE);
	int isReadonly   = (flags & SQLITE_OPEN_READONLY);
	int isReadWrite  = (flags & SQLITE_OPEN_READWRITE);

	ERR_FAIL_COND_V(isDelete, SQLITE_PERM);
	
	/* If creating a master or main-file journal, this function will open
	** a file-descriptor on the directory too. The first time lvSync()
	** is called the directory file descriptor will be fsync()ed and close()d.
	*/
	//int isOpenDirectory = (isCreate && (eType==SQLITE_OPEN_MASTER_JOURNAL || eType==SQLITE_OPEN_MAIN_JOURNAL));
	
	assert((isReadonly==0 || isReadWrite==0) && (isReadWrite || isReadonly));
	assert(isCreate==0 || isReadWrite);
	assert(isExclusive==0 || isCreate);
	assert(isDelete==0 || isCreate);

	/*
	assert( eType!=SQLITE_OPEN_MAIN_DB || !isDelete );
	assert( eType!=SQLITE_OPEN_MAIN_JOURNAL || !isDelete );
	assert( eType!=SQLITE_OPEN_MASTER_JOURNAL || !isDelete );
	*/
	
	/* Assert that the upper layer has set one of the "file-type" flags. */
	/*
	assert( eType==SQLITE_OPEN_MAIN_DB      || eType==SQLITE_OPEN_TEMP_DB 
	   || eType==SQLITE_OPEN_MAIN_JOURNAL || eType==SQLITE_OPEN_TEMP_JOURNAL 
	   || eType==SQLITE_OPEN_SUBJOURNAL   || eType==SQLITE_OPEN_MASTER_JOURNAL 
	   || eType==SQLITE_OPEN_TRANSIENT_DB
	);
	*/

	int oflags = 0;
	
	if( isReadonly )  oflags = FileAccess::READ;
	if( isReadWrite ) oflags = FileAccess::READ_WRITE;
	if( isCreate )    oflags = FileAccess::READ_WRITE;
	if( isExclusive ) oflags = FileAccess::READ_WRITE;

	memset(pFile, 0, sizeof(lvFile));

	FileAccess* fa;
	if (0) {
		fa = FileAccess::create();
		Error err = fa->open(zPath, oflags);
		if (err != OK) {
			if (oflags & FileAccess::WRITE) {
			
				/* Failed to open the file for read/write access. Try read-only. */
				flags &= ~(SQLITE_OPEN_READWRITE|SQLITE_OPEN_CREATE);
				flags |= SQLITE_OPEN_READONLY;

				return lvOpen(pVfs, zPath, pFile, flags, pOutFlags);
			};
	
			memdelete(fa);
			ERR_FAIL_COND_V( err != OK, SQLITE_CANTOPEN );
		};
	} else {

		if (oflags & FileAccess::WRITE) {
		
			/* Failed to open the file for read/write access. Try read-only. */
			flags &= ~(SQLITE_OPEN_READWRITE|SQLITE_OPEN_CREATE);
			flags |= SQLITE_OPEN_READONLY;

			return lvOpen(pVfs, zPath, pFile, flags, pOutFlags);
		};
	
		fa = FileAccess::create();
		Error err = fa->open(zPath, oflags);
		if (err != OK) {
			memdelete(fa);
			ERR_FAIL_COND_V( err != OK, SQLITE_CANTOPEN );
		};		
	};
	
	if( pOutFlags ){
		*pOutFlags = flags;
	};

	lvFile* f = (lvFile*)pFile;
	f->fa = fa;
	f->pMethod = &sqlite3Larvita2IoMethod;
	
	return SQLITE_OK;
};

static int lvDelete(sqlite3_vfs *pVfs, const char *zPath, int dirSync) {

	//unlink(zPath);
	return SQLITE_OK;
	//ERR_FAIL_COND_V(1, SQLITE_PERM);
};

static int lvAccess(sqlite3_vfs *pVfs, const char *zPath, int flags){
	switch( flags ){
	case SQLITE_ACCESS_EXISTS: {
		FileAccess* fa = FileAccess::create();
		int ret = fa->file_exists(zPath)?1:0;
		memdelete(fa);
		return ret;
	};
	case SQLITE_ACCESS_READWRITE:
		return 0;
	case SQLITE_ACCESS_READ: {
		FileAccess* fa = FileAccess::create();
		int ret = fa->file_exists(zPath)?1:0;
		memdelete(fa);
		return ret;
	};
	default:
		assert(!"Invalid flags argument");
	}
	
	return 0;
}

static int lvGetTempname(sqlite3_vfs *pVfs, int nBuf, char *zBuf) {

	ERR_FAIL_COND_V(1, SQLITE_PERM);
};

static int lvFullPathname(
  sqlite3_vfs *pVfs,            /* Pointer to vfs object */
  const char *zPath,            /* Possibly relative input path */
  int nOut,                     /* Size of output buffer in bytes */
  char *zOut                    /* Output buffer */
){

	assert( pVfs->mxPathname==MAX_PATHNAME );
	zOut[nOut-1] = '\0';
	sqlite3_snprintf(nOut, zOut, "%s", zPath);
	
	return SQLITE_OK;
};

static void *lvDlOpen(sqlite3_vfs *pVfs, const char *zFilename){
  ERR_FAIL_COND_V(1, NULL);
}
static void lvDlError(sqlite3_vfs *pVfs, int nBuf, char *zBufOut) { 

};

static void *lvDlSym(sqlite3_vfs *pVfs, void *pHandle, const char *zSymbol){
	
	ERR_FAIL_COND_V(1, NULL);
};

static void lvDlClose(sqlite3_vfs *pVfs, void *pHandle) {

};

static int lvRandomness(sqlite3_vfs *pVfs, int nBuf, char *zBuf){
	
	return SQLITE_OK;
};

static int lvSleep(sqlite3_vfs *pVfs, int microseconds) {
	
	//int seconds = (microseconds+999999)/1000000;
	//sleep(seconds);
	//return seconds*1000000;
	
	return 1;
};

static int lvCurrentTime(sqlite3_vfs *pVfs, double *prNow){

	time_t t;
	time(&t);
	*prNow = t/86400.0 + 2440587.5;

	return 0;
}


sqlite3_vfs *sqlite3OsDefaultVfs(void){
  static sqlite3_vfs lvVfs = {
    1,                  /* iVersion */
    sizeof(lvFile),   /* szOsFile */
    MAX_PATHNAME,       /* mxPathname */
    0,                  /* pNext */
    "larvita2",             /* zName */
    0,                  /* pAppData */
  
    lvOpen,           /* xOpen */
    lvDelete,         /* xDelete */
    lvAccess,         /* xAccess */
    lvGetTempname,    /* xGetTempName */
    lvFullPathname,   /* xFullPathname */
    lvDlOpen,         /* xDlOpen */
    lvDlError,        /* xDlError */
    lvDlSym,          /* xDlSym */
    lvDlClose,        /* xDlClose */
    lvRandomness,     /* xRandomness */
    lvSleep,          /* xSleep */
    lvCurrentTime     /* xCurrentTime */
  };
  
  return &lvVfs;
}

};

#endif
