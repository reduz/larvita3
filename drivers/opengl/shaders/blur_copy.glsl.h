/* WARNING, THIS FILE WAS GENERATED, DO NOT EDIT */
#ifndef BLUR_COPY_GLSL_H
#define BLUR_COPY_GLSL_H


#include "drivers/opengl/shader_gl.h"


class BlurCopyShaderGL : public ShaderGL {

	 virtual String get_shader_name() const { return "BlurCopyShaderGL"; }
public:

	enum Uniforms {
		SCREEN_TEXTURE		= 0,
		PIXELSIZE		= 1,
		BLOOM		= 2,
	};

	String get_name() { return "BlurCopyShaderGL"; };

	virtual void init() {

static const char **_conditional_strings=NULL;
static const char* _uniform_strings[]={
	"screen_texture",
	"pixelsize",
	"bloom",
};

static const char* _vertex_code="\
\n\
varying vec2 uv_interp;\n\
void main() {\n\
	\n\
	gl_Position = gl_Vertex;\n\
	uv_interp = gl_Vertex.xy * 0.5 + 0.5;\n\
}\n\
\n\
";

static const char* _fragment_code="\
\n\
uniform sampler2D screen_texture;\n\
uniform vec2 pixelsize;\n\
uniform float bloom;\n\
varying vec2 uv_interp;\n\
\n\
\n\
 \n\
void main() {\n\
	\n\
	\n\
	float px=pixelsize.x;\n\
	float py=pixelsize.y;\n\
	vec4 simple_blur=texture2D( screen_texture,  uv_interp );\n\
	simple_blur+=texture2D( screen_texture,  vec2(uv_interp.x+px, uv_interp.y) );\n\
	simple_blur+=texture2D( screen_texture,  vec2(uv_interp.x-px, uv_interp.y) );\n\
	simple_blur+=texture2D( screen_texture,  vec2(uv_interp.x,uv_interp.y+py) );\n\
	simple_blur+=texture2D( screen_texture,  vec2(uv_interp.x,uv_interp.y-py) );\n\
	simple_blur/=5.0;\n\
	simple_blur.rgb*= simple_blur.a + bloom;\n\
	\n\
		\n\
 	gl_FragColor = vec4( simple_blur.rgb, 1.0 );\n\
		\n\
}\n\
";

		setup(_conditional_strings,0,_uniform_strings,3,_vertex_code,_fragment_code);
	};

};

#endif

