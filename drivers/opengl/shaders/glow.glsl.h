/* WARNING, THIS FILE WAS GENERATED, DO NOT EDIT */
#ifndef GLOW_GLSL_H
#define GLOW_GLSL_H


#include "drivers/opengl/shader_gl.h"


class GlowShaderGL : public ShaderGL {

	 virtual String get_shader_name() const { return "GlowShaderGL"; }
public:

	enum Conditionals {
		USE_TEXTURE_GLOWMAP		= 0,
		TEXTURE_GLOW_SPHERE		= 1,
		USE_TEXTURE_BASE		= 2,
	};

	enum Uniforms {
		TEXTURE_BASE		= 0,
		TEXTURE_GLOWMAP		= 1,
		GLOW		= 2,
	};

	String get_name() { return "GlowShaderGL"; };

	virtual void init() {

static const char* _conditional_strings[]={
	"#define USE_TEXTURE_GLOWMAP\n",
	"#define TEXTURE_GLOW_SPHERE\n",
	"#define USE_TEXTURE_BASE\n",
};

static const char* _uniform_strings[]={
	"texture_base",
	"texture_glowmap",
	"glow",
};

static const char* _vertex_code="\
\n\
varying vec3 normal_interp;\n\
\n\
void main() {\n\
\n\
	gl_Position = ftransform();\n\
\n\
#ifdef USE_TEXTURE_GLOWMAP\n\
#ifdef TEXTURE_GLOW_SPHERE\n\
\n\
	normal_interp = normalize( gl_NormalMatrix * gl_Normal );\n\
\n\
#else\n\
	gl_TexCoord[0] = gl_MultiTexCoord0;	\n\
#endif\n\
#endif\n\
\n\
#ifdef USE_TEXTURE_BASE\n\
	gl_TexCoord[0] = gl_MultiTexCoord0;	\n\
#endif\n\
}\n\
\n\
";

static const char* _fragment_code="\
\n\
#ifdef USE_TEXTURE_BASE\n\
uniform sampler2D texture_base;\n\
#endif\n\
\n\
#ifdef USE_TEXTURE_GLOWMAP\n\
uniform sampler2D texture_glowmap;\n\
#endif\n\
\n\
uniform float glow;\n\
\n\
varying vec3 normal_interp;\n\
\n\
void main() {\n\
\n\
	vec4 fragment = vec4(0.0);\n\
	#ifdef USE_TEXTURE_BASE\n\
	fragment.a = texture2D(texture_base, gl_TexCoord[0].st).a;\n\
	#else\n\
	fragment.a = gl_FrontMaterial.diffuse.a;\n\
	#endif\n\
\n\
	float g = glow;\n\
	\n\
	#ifdef USE_TEXTURE_GLOWMAP\n\
	#ifdef TEXTURE_GLOW_SPHERE\n\
	vec3 normal = normalize( normal_interp );\n\
	g += texture2D(texture_glowmap, vec2( normal.x * 0.5 + 0.5, -normal.y * 0.5 + 0.5 )).r;\n\
	#else\n\
	g += texture2D(texture_glowmap, gl_TexCoord[0].st).r;\n\
	#endif\n\
	#endif\n\
\n\
	fragment.a *= g;\n\
\n\
	gl_FragColor = fragment;\n\
}\n\
\n\
";

		setup(_conditional_strings,3,_uniform_strings,3,_vertex_code,_fragment_code);
	};

};

#endif

