/* WARNING, THIS FILE WAS GENERATED, DO NOT EDIT */
#ifndef BLUR_GLSL_H
#define BLUR_GLSL_H


#include "drivers/opengl/shader_gl.h"


class BlurShaderGL : public ShaderGL {

	 virtual String get_shader_name() const { return "BlurShaderGL"; }
public:

	enum Conditionals {
		VERTICAL_PASS		= 0,
	};

	enum Uniforms {
		SCREEN_TEXTURE		= 0,
		PIXELSIZE		= 1,
	};

	String get_name() { return "BlurShaderGL"; };

	virtual void init() {

static const char* _conditional_strings[]={
	"#define VERTICAL_PASS\n",
};

static const char* _uniform_strings[]={
	"screen_texture",
	"pixelsize",
};

static const char* _vertex_code="\
\n\
varying vec2 uv_interp;\n\
\n\
void main() {\n\
	\n\
\n\
	gl_Position = gl_Vertex;\n\
	uv_interp = gl_Vertex.xy *0.5 + 0.5;\n\
}\n\
\n\
";

static const char* _fragment_code="\
\n\
\n\
uniform sampler2D screen_texture;\n\
varying vec2 uv_interp;\n\
uniform vec2 pixelsize;\n\
\n\
 \n\
void main() {\n\
	\n\
	\n\
#ifdef VERTICAL_PASS\n\
\n\
	vec3 blur=texture2D( screen_texture,  vec2(uv_interp.x,uv_interp.y-pixelsize.y*2.0) ).rgb;\n\
	blur+=texture2D( screen_texture,  vec2(uv_interp.x,uv_interp.y-pixelsize.y) ).rgb;\n\
	blur+=texture2D( screen_texture,  uv_interp ).rgb;\n\
	blur+=texture2D( screen_texture,  vec2(uv_interp.x,uv_interp.y+pixelsize.y) ).rgb;\n\
	blur+=texture2D( screen_texture,  vec2(uv_interp.x,uv_interp.y+pixelsize.y*2.0) ).rgb;\n\
#else\n\
\n\
	vec3 blur=texture2D( screen_texture,  vec2(uv_interp.x-pixelsize.x*2.0,uv_interp.y) ).rgb;\n\
	blur+=texture2D( screen_texture,  vec2(uv_interp.x-pixelsize.x,uv_interp.y) ).rgb;\n\
	blur+=texture2D( screen_texture,  uv_interp ).rgb;\n\
	blur+=texture2D( screen_texture,  vec2(uv_interp.x+pixelsize.x,uv_interp.y) ).rgb;\n\
	blur+=texture2D( screen_texture,  vec2(uv_interp.x+pixelsize.x*2.0,uv_interp.y) ).rgb;\n\
\n\
#endif\n\
\n\
	blur/=5.0;\n\
\n\
\n\
	gl_FragColor = vec4( blur, 1.0 );\n\
		\n\
}\n\
";

		setup(_conditional_strings,1,_uniform_strings,2,_vertex_code,_fragment_code);
	};

};

#endif

