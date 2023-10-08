#ifdef OPENGL_ENABLED

#include "gl_extensions.h"

#include "error_macros.h"

Error init_gl_extensions() {
	#ifdef GLEW_ENABLED
	GLenum err = glewInit();
	ERR_FAIL_COND_V(err!=GLEW_OK, FAILED);
	#endif

	return OK;
};

#endif
