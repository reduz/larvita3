#ifdef GLEW_ENABLED
#include "GL/glew.h"
#else
#include "drivers/opengl/GLee.h"
#endif

#include "error_list.h"

Error init_gl_extensions();

