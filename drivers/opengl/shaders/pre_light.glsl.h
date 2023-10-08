/* WARNING, THIS FILE WAS GENERATED, DO NOT EDIT */
#ifndef PRE_LIGHT_GLSL_H
#define PRE_LIGHT_GLSL_H


#include "drivers/opengl/shader_gl.h"


class PreLightShaderGL : public ShaderGL {

	 virtual String get_shader_name() const { return "PreLightShaderGL"; }
public:

	enum Conditionals {
		USE_TEXTURE_NORMALMAP		= 0,
		ALPHA_TEST		= 1,
	};

	enum Uniforms {
		TEXTURE_NORMAL		= 0,
		TEXTURE_BASE		= 1,
		ALPHA_THRESHHOLD		= 2,
		ZFAR		= 3,
	};

	String get_name() { return "PreLightShaderGL"; };

	virtual void init() {

static const char* _conditional_strings[]={
	"#define USE_TEXTURE_NORMALMAP\n",
	"#define ALPHA_TEST\n",
};

static const char* _uniform_strings[]={
	"texture_normal",
	"texture_base",
	"alpha_threshhold",
	"zfar",
};

static const char* _vertex_code="\
\n\
varying vec3 normal_interp;\n\
\n\
#ifdef USE_TEXTURE_NORMALMAP\n\
varying vec3 binormal_interp,tangent_interp;\n\
#endif\n\
\n\
varying vec4 pos;\n\
\n\
void main() {\n\
	\n\
	\n\
	normal_interp = normalize( gl_NormalMatrix * gl_Normal );\n\
	\n\
#ifdef USE_TEXTURE_NORMALMAP\n\
	\n\
	vec3 tangent=gl_MultiTexCoord6.xyz;\n\
	float binormal_facing=gl_MultiTexCoord6.w;\n\
\n\
	tangent_interp = normalize(gl_NormalMatrix *  tangent );\n\
	binormal_interp = normalize(cross( normal_interp, tangent_interp ) * binormal_facing);\n\
#endif\n\
\n\
#ifdef USE_TEXTURE_NORMALMAP\n\
	gl_TexCoord[0] = gl_MultiTexCoord0;	\n\
#endif\n\
#ifdef ALPHA_TEST\n\
	gl_TexCoord[0] = gl_MultiTexCoord0;	\n\
#endif\n\
\n\
	pos = gl_ModelViewMatrix * gl_Vertex;\n\
	gl_Position = ftransform();\n\
}\n\
\n\
\n\
";

static const char* _fragment_code="\
\n\
varying vec3 normal_interp;\n\
\n\
#ifdef USE_TEXTURE_NORMALMAP\n\
uniform sampler2D texture_normal;\n\
varying vec3 tangent_interp;\n\
varying vec3 binormal_interp;\n\
#endif\n\
\n\
#ifdef ALPHA_TEST\n\
uniform sampler2D texture_base;\n\
uniform float alpha_threshhold;\n\
#endif\n\
\n\
varying vec4 pos;\n\
\n\
uniform float zfar;\n\
\n\
vec2 pack_depth(float v) {\n\
	float ret = -v / zfar * 256.0;\n\
	return vec2(floor(ret) / 256.0, frac(ret));\n\
};\n\
\n\
vec2 pack_normal(vec3 normal) {\n\
	return normalize(normal).xy * 0.5 + 0.5;\n\
};\n\
\n\
\n\
void main() {\n\
	\n\
	\n\
	\n\
	/***************************/\n\
	/*   NORMAL+Z   */\n\
	/***************************/\n\
\n\
#ifdef ALPHA_TEST\n\
	if (texture2D(texture_base, gl_TexCoord[0].st).a <= alpha_threshhold)\n\
		discard;\n\
#endif\n\
	\n\
	vec3 normal;\n\
	\n\
#ifdef USE_TEXTURE_NORMALMAP\n\
	vec3 tn = normalize(texture2D(texture_normal, gl_TexCoord[0].st ).xyz - 0.5);	\n\
	normal=mat3( tangent_interp, binormal_interp, normal_interp ) * tn;\n\
#else\n\
	normal = normal_interp;\n\
#endif\n\
	normal = normalize( normal.xyz );\n\
\n\
	vec4 fragment;\n\
	fragment.xy = pack_normal(normal);\n\
	fragment.zw = pack_depth( pos.z );\n\
\n\
	gl_FragColor = fragment; \n\
}\n\
\n\
";

		setup(_conditional_strings,2,_uniform_strings,4,_vertex_code,_fragment_code);
	};

};

#endif

