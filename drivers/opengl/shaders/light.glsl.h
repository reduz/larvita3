/* WARNING, THIS FILE WAS GENERATED, DO NOT EDIT */
#ifndef LIGHT_GLSL_H
#define LIGHT_GLSL_H


#include "drivers/opengl/shader_gl.h"


class LightShaderGL : public ShaderGL {

	 virtual String get_shader_name() const { return "LightShaderGL"; }
public:

	enum Conditionals {
		LIGHT_MODE_DIRECTIONAL		= 0,
		USE_SHADOW_MAP		= 1,
		LIGHT_MODE_OMNI		= 2,
		LIGHT_MODE_SPOT		= 3,
	};

	enum Uniforms {
		VIEWPORT		= 0,
		NORMAL_TEXTURE		= 1,
		ZFAR		= 2,
		SHADOW_MAP		= 3,
		SHADOW_ORIGIN		= 4,
	};

	String get_name() { return "LightShaderGL"; };

	virtual void init() {

static const char* _conditional_strings[]={
	"#define LIGHT_MODE_DIRECTIONAL\n",
	"#define USE_SHADOW_MAP\n",
	"#define LIGHT_MODE_OMNI\n",
	"#define LIGHT_MODE_SPOT\n",
};

static const char* _uniform_strings[]={
	"viewport",
	"normal_texture",
	"zfar",
	"shadow_map",
	"shadow_origin",
};

static const char* _vertex_code="\
\n\
uniform vec3 viewport;\n\
varying vec2 screen_pos;\n\
varying float w;\n\
varying vec2 fraguv;\n\
varying vec4 shadow_proj;\n\
\n\
void main() {\n\
	#ifdef LIGHT_MODE_DIRECTIONAL\n\
	screen_pos = gl_Vertex.xy * viewport.xy / viewport.z; \n\
	fraguv = gl_MultiTexCoord0.st;\n\
	gl_Position = gl_Vertex;\n\
	w = 1.0;\n\
\n\
	#else\n\
	\n\
	\n\
	\n\
	gl_Position = ftransform();\n\
	vec4 pos = gl_ModelViewMatrix * gl_Vertex;\n\
	screen_pos = gl_Position.xy * viewport.xy / viewport.z;;\n\
	w = gl_Position.w;\n\
\n\
	fraguv = ((pos.xy * viewport.z / -pos.z) / viewport.xy) * 0.5 + 0.5;\n\
	\n\
	fraguv *= w;\n\
	#endif\n\
\n\
	#ifdef USE_SHADOW_MAP\n\
	shadow_proj = gl_ModelViewMatrix * gl_Vertex;\n\
	shadow_proj = gl_TextureMatrix[1] * shadow_proj;\n\
	#endif\n\
}\n\
\n\
\n\
";

static const char* _fragment_code="\
\n\
uniform vec3 viewport;\n\
uniform sampler2D normal_texture;\n\
uniform float zfar;\n\
varying vec2 screen_pos;\n\
varying vec2 fraguv;\n\
varying float w;\n\
varying vec4 shadow_proj;\n\
\n\
#ifdef USE_SHADOW_MAP\n\
uniform sampler2D shadow_map;\n\
uniform vec3 shadow_origin;\n\
#endif\n\
\n\
float unpack_depth(vec2 v) {\n\
\n\
	float ret = v.x*256.0;\n\
	ret += v.y;\n\
	\n\
	return -ret / 256.0 * zfar;\n\
}\n\
\n\
vec3 unpack_normal(vec2 n) {\n\
\n\
	vec3 ret;\n\
	ret.xy = n * 2.0 - 1.0; \n\
	ret.z = sqrt(1.0 - dot(ret.xy, ret.xy));\n\
\n\
	return ret;\n\
}\n\
\n\
vec4 process_directional(vec3 normal, vec3 frag_pos) {\n\
\n\
	#ifdef USE_SHADOW_MAP\n\
\n\
	float radius = gl_LightSource[0].linearAttenuation;\n\
	vec4 bias = shadow_proj / shadow_proj.w;\n\
	float shadow_dist = texture2D(shadow_map, bias.st).z * radius;\n\
	if (shadow_dist < shadow_proj.z)\n\
		return vec4(gl_LightSource[0].ambient.rgb, 0.0);\n\
\n\
	#endif\n\
\n\
	vec3 light_direction = -gl_LightSource[0].position.xyz;\n\
	float NdotL = clamp( dot( normal.xyz, -light_direction ), 0.0, 1.0 );\n\
	\n\
	\n\
	vec3 light_ref = reflect( light_direction, normal.xyz );\n\
	float eye_light = clamp( dot( light_ref, -normalize(frag_pos)), 0.0, 1.0 );\n\
	\n\
	vec4 ret;\n\
	ret.rgb = gl_LightSource[0].ambient.rgb + gl_LightSource[0].diffuse.rgb * NdotL;\n\
	ret.a = dot(gl_LightSource[0].specular.rgb, vec3(0.33)) * pow(eye_light, 30.0) * NdotL;\n\
\n\
	return ret / 4.0;\n\
}\n\
\n\
vec4 process_omni(vec3 normal, vec3 frag_pos) {\n\
\n\
	vec3 direction = frag_pos - gl_LightSource[0].position.xyz;\n\
	float dist = length(direction);\n\
\n\
	float radius = gl_LightSource[0].linearAttenuation;\n\
	vec4 ret = vec4(0.0, 0.0, 0.0, 0.0);\n\
\n\
	if (dist > radius)\n\
		discard;\n\
\n\
	float light_attenuation = gl_LightSource[0].constantAttenuation;\n\
	float light_energy = gl_LightSource[0].quadraticAttenuation;\n\
\n\
	direction = normalize(direction);\n\
	\n\
	\n\
	dist = clamp(1.0 - dist/radius, 0.0, 1.0);\n\
	\n\
	float attenuation = pow( dist, light_attenuation ) * light_energy;	\n\
	\n\
	float NdotL = clamp( dot( normal.xyz, -direction ), 0.0, 1.0 );	\n\
	\n\
	\n\
	vec3 light_ref = reflect( direction, normal.xyz );\n\
	float eye_light = clamp( dot( light_ref, -normalize(frag_pos)), 0.0, 1.0 );\n\
\n\
	ret.rgb = gl_LightSource[0].ambient.rgb * (1.0-NdotL) + gl_LightSource[0].diffuse.rgb * NdotL;\n\
	ret.a = dot(gl_LightSource[0].specular.rgb, vec3(0.33)) * pow(eye_light, 30.0) * NdotL;\n\
	\n\
	ret*=attenuation;\n\
\n\
	return ret / 4.0;\n\
}\n\
\n\
vec4 process_spot(vec3 normal, vec3 frag_pos) {\n\
\n\
	vec3 spot_direction = gl_LightSource[0].spotDirection;\n\
	vec3 direction = frag_pos - gl_LightSource[0].position.xyz;\n\
	float dist = length(direction);\n\
\n\
	float radius = gl_LightSource[0].linearAttenuation;\n\
\n\
	vec4 ret = vec4(0.0, 0.0, 0.0, 0.0);\n\
\n\
	if (dist > radius)\n\
		discard;\n\
		\n\
		\n\
\n\
	direction = normalize(direction);\n\
\n\
	float cos = dot(direction, spot_direction);\n\
	if (cos < gl_LightSource[0].spotCosCutoff)\n\
		discard;\n\
\n\
	float rim = (1.0 - cos) / (1.0 - gl_LightSource[0].spotCosCutoff);\n\
	float spot_attenuation = 1.0 - pow( rim, gl_LightSource[0].spotExponent );\n\
\n\
	float light_attenuation = gl_LightSource[0].constantAttenuation;\n\
	float light_energy = gl_LightSource[0].quadraticAttenuation;\n\
	\n\
	\n\
	dist = clamp(1.0 - dist/radius, 0.0, 1.0);\n\
	\n\
	float attenuation = pow( dist, light_attenuation ) * light_energy * spot_attenuation;\n\
	\n\
	float NdotL = clamp( dot( normal.xyz, -direction ), 0.0, 1.0 );	\n\
	\n\
	\n\
	vec3 light_ref = reflect( direction, normal.xyz );\n\
	float eye_light = clamp( dot( light_ref, -normalize(frag_pos)), 0.0, 1.0 );\n\
\n\
	ret.rgb = gl_LightSource[0].ambient.rgb + gl_LightSource[0].diffuse.rgb * NdotL;\n\
	ret.a = dot(gl_LightSource[0].specular.rgb, vec3(0.33)) * pow(eye_light, 30.0) * NdotL;\n\
	\n\
	ret*=attenuation / 4.0;\n\
\n\
	return ret;\n\
}\n\
\n\
\n\
void main() {\n\
\n\
	vec4 p = texture2D(normal_texture, fraguv / w);\n\
	vec3 normal = unpack_normal(p.xy);\n\
	float depth = unpack_depth(p.zw);\n\
	vec3 pos;\n\
	pos.z = depth;\n\
	pos.xy = screen_pos / w * -pos.z;\n\
\n\
#ifdef LIGHT_MODE_DIRECTIONAL\n\
	\n\
	gl_FragColor = process_directional(normal, pos); \n\
#endif\n\
\n\
#ifdef LIGHT_MODE_OMNI\n\
\n\
	gl_FragColor = process_omni(normal, pos);\n\
\n\
#endif\n\
\n\
#ifdef LIGHT_MODE_SPOT\n\
\n\
	gl_FragColor = process_spot(normal, pos);\n\
\n\
#endif\n\
}\n\
\n\
";

		setup(_conditional_strings,4,_uniform_strings,5,_vertex_code,_fragment_code);
	};

};

#endif

