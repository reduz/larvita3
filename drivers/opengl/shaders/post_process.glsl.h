/* WARNING, THIS FILE WAS GENERATED, DO NOT EDIT */
#ifndef POST_PROCESS_GLSL_H
#define POST_PROCESS_GLSL_H


#include "drivers/opengl/shader_gl.h"


class PostProcessShaderGL : public ShaderGL {

	 virtual String get_shader_name() const { return "PostProcessShaderGL"; }
public:

	enum Conditionals {
		BLUR_ENABLED		= 0,
		SSAO_ENABLED		= 1,
	};

	enum Uniforms {
		SCREEN_TEXTURE		= 0,
		BLUR_TEXTURE		= 1,
		NORMAL_POS_TEXTURE		= 2,
		ZFAR		= 3,
		PIXELSIZE		= 4,
		SCREEN_TO_VIEWPORT		= 5,
	};

	String get_name() { return "PostProcessShaderGL"; };

	virtual void init() {

static const char* _conditional_strings[]={
	"#define BLUR_ENABLED\n",
	"#define SSAO_ENABLED\n",
};

static const char* _uniform_strings[]={
	"screen_texture",
	"blur_texture",
	"normal_pos_texture",
	"zfar",
	"pixelsize",
	"screen_to_viewport",
};

static const char* _vertex_code="\
\n\
void main() {\n\
\n\
	gl_TexCoord[0] = gl_MultiTexCoord0;	\n\
	gl_Position = gl_Vertex;\n\
}\n\
\n\
";

static const char* _fragment_code="\
\n\
uniform sampler2D screen_texture;\n\
\n\
#ifdef BLUR_ENABLED\n\
uniform sampler2D blur_texture;\n\
#endif\n\
\n\
#ifdef SSAO_ENABLED\n\
uniform sampler2D normal_pos_texture;\n\
#endif\n\
\n\
uniform float zfar;\n\
uniform vec2 pixelsize;\n\
uniform vec2 screen_to_viewport;\n\
\n\
\n\
vec3 unpack_normal(vec2 n) {\n\
\n\
	vec3 ret;\n\
	ret.xy = n * 2.0 - 1.0; \n\
	ret.z = -sqrt(1.0 - dot(ret.xy, ret.xy));\n\
\n\
	return ret;\n\
}\n\
\n\
float unpack_depth(vec2 v) {\n\
\n\
	float ret = v.x*256.0;\n\
	ret += v.y;\n\
	\n\
	return ret / 256.0 * zfar;\n\
}\n\
\n\
\n\
#ifdef SSAO_ENABLED\n\
vec3 get_pos( vec2 uv ) {\n\
\n\
	vec3 pos;\n\
	pos.z=unpack_depth( texture2D( normal_pos_texture, uv).zw );\n\
	pos.xy = (uv * 2.0 - 1.0) * screen_to_viewport * pos.z;\n\
	return pos;\n\
}\n\
\n\
\n\
float compute_ao( vec2 uv, vec3 normal, vec3 pos ) {\n\
\n\
	\n\
	vec3 uv_pos=get_pos(uv);\n\
	vec3 rel = pos - uv_pos;\n\
	float ao = dot(normalize(rel), normal) * 0.5 + 0.5;;\n\
	return ao;\n\
}\n\
\n\
float compute_ssao() {\n\
\n\
	vec3 pos = get_pos( gl_TexCoord[0].xy );\n\
	vec3 normal = unpack_normal( gl_TexCoord[0].xy );\n\
	\n\
	float pw = pixelsize.x;\n\
	float ph = pixelsize.y;\n\
\n\
	float ao = 0.0;\n\
	float aoscale=1.0;\n\
	\n\
	for (int i=0;i<3;i++) {\n\
\n\
		ao+=compute_ao( vec2(gl_TexCoord[0].x+pw,gl_TexCoord[0].y+ph), normal, pos )/aoscale;\n\
		ao+=compute_ao( vec2(gl_TexCoord[0].x+pw,gl_TexCoord[0].y-ph), normal, pos )/aoscale;\n\
		ao+=compute_ao( vec2(gl_TexCoord[0].x-pw,gl_TexCoord[0].y+ph), normal, pos )/aoscale;\n\
		ao+=compute_ao( vec2(gl_TexCoord[0].x-pw,gl_TexCoord[0].y-ph), normal, pos )/aoscale;\n\
\n\
/*\n\
		d=get_depth( vec2(gl_TexCoord[0].x+pw,gl_TexCoord[0].y+ph));\n\
		ao+=compare_depths(depth,d)/aoscale;\n\
\n\
		d=get_depth( vec2(gl_TexCoord[0].x-pw,gl_TexCoord[0].y+ph));\n\
		ao+=compare_depths(depth,d)/aoscale;\n\
	\n\
		d=get_depth( vec2(gl_TexCoord[0].x+pw,gl_TexCoord[0].y-ph));\n\
		ao+=compare_depths(depth,d)/aoscale;\n\
\n\
		d=get_depth( vec2(gl_TexCoord[0].x-pw,gl_TexCoord[0].y-ph));\n\
		ao+=compare_depths(depth,d)/aoscale;\n\
*/	\n\
		pw*=2.0;\n\
		ph*=2.0;\n\
		\n\
	}\n\
\n\
	ao/=16.0;\n\
	ao=pow(ao,0.70);\n\
	return ao;\n\
}\n\
#endif\n\
\n\
\n\
void main() {\n\
\n\
	vec4 fragment = texture2D( screen_texture, gl_TexCoord[0].st );\n\
	\n\
	#ifdef BLUR_ENABLED\n\
	vec4 blur = texture2D( blur_texture, gl_TexCoord[0].st );\n\
	fragment.rgb+=pow(blur.rgb, 1.4)*4.0;\n\
	#endif\n\
\n\
	#ifdef SSAO_ENABLED\n\
	\n\
	#endif\n\
	\n\
	\n\
\n\
	\n\
	gl_FragColor = fragment;\n\
}\n\
\n\
";

		setup(_conditional_strings,2,_uniform_strings,6,_vertex_code,_fragment_code);
	};

};

#endif

