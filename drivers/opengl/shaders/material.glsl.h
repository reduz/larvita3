/* WARNING, THIS FILE WAS GENERATED, DO NOT EDIT */
#ifndef MATERIAL_GLSL_H
#define MATERIAL_GLSL_H


#include "drivers/opengl/shader_gl.h"


class MaterialShaderGL : public ShaderGL {

	 virtual String get_shader_name() const { return "MaterialShaderGL"; }
public:

	enum Conditionals {
		USE_TEXTURE_NORMALMAP		= 0,
		SPOT_SHADOW		= 1,
		USE_FOG		= 2,
		USE_TEXTURE_BASE		= 3,
		FOG_TYPE_LINEAR		= 4,
		FOG_TYPE_EXP		= 5,
		FOG_TYPE_EXP2		= 6,
		USE_TEXTURE_DETAIL		= 7,
		USE_TEXTURE_SPECULAR		= 8,
		USE_TEXTURE_EMISSION		= 9,
		USE_TEXTURE_REFLECTION		= 10,
		USE_TEXTURE_REFMASK		= 11,
		USE_TEXTURE_GLOWMAP		= 12,
		USE_TEXTURE_LIGHT_PASS		= 13,
		USE_GLOW		= 14,
		ALPHA_TEST		= 15,
		TEXTURE_EMISSION_SPHERE		= 16,
		NO_LIGHTS		= 17,
		LIGHT_0_DIRECTIONAL		= 18,
		LIGHT_0_OMNI		= 19,
		LIGHT_0_SPOT		= 20,
		LIGHT_1_DIRECTIONAL		= 21,
		LIGHT_1_OMNI		= 22,
		LIGHT_1_SPOT		= 23,
		LIGHT_2_DIRECTIONAL		= 24,
		LIGHT_2_OMNI		= 25,
		LIGHT_2_SPOT		= 26,
		LIGHT_3_DIRECTIONAL		= 27,
		LIGHT_3_OMNI		= 28,
		LIGHT_3_SPOT		= 29,
		LIGHT_4_DIRECTIONAL		= 30,
		LIGHT_4_OMNI		= 31,
		LIGHT_4_SPOT		= 32,
		LIGHT_5_DIRECTIONAL		= 33,
		LIGHT_5_OMNI		= 34,
		LIGHT_5_SPOT		= 35,
		LIGHT_6_DIRECTIONAL		= 36,
		LIGHT_6_OMNI		= 37,
		LIGHT_6_SPOT		= 38,
		LIGHT_7_DIRECTIONAL		= 39,
		LIGHT_7_OMNI		= 40,
		LIGHT_7_SPOT		= 41,
		TEXTURE_GLOW_SPHERE		= 42,
	};

	enum Uniforms {
		VIEWPORT		= 0,
		TEXTURE_BASE		= 1,
		TEXTURE_DETAIL		= 2,
		TEXTURE_NORMAL		= 3,
		TEXTURE_SPECULAR		= 4,
		TEXTURE_EMISSION		= 5,
		TEXTURE_REFLECTION		= 6,
		TEXTURE_REFMASK		= 7,
		TEXTURE_GLOWMAP		= 8,
		TEXTURE_LIGHT_PASS		= 9,
		TEXTURE_SHADOW		= 10,
		GLOW		= 11,
		ALPHA_THRESHHOLD		= 12,
	};

	String get_name() { return "MaterialShaderGL"; };

	virtual void init() {

static const char* _conditional_strings[]={
	"#define USE_TEXTURE_NORMALMAP\n",
	"#define SPOT_SHADOW\n",
	"#define USE_FOG\n",
	"#define USE_TEXTURE_BASE\n",
	"#define FOG_TYPE_LINEAR\n",
	"#define FOG_TYPE_EXP\n",
	"#define FOG_TYPE_EXP2\n",
	"#define USE_TEXTURE_DETAIL\n",
	"#define USE_TEXTURE_SPECULAR\n",
	"#define USE_TEXTURE_EMISSION\n",
	"#define USE_TEXTURE_REFLECTION\n",
	"#define USE_TEXTURE_REFMASK\n",
	"#define USE_TEXTURE_GLOWMAP\n",
	"#define USE_TEXTURE_LIGHT_PASS\n",
	"#define USE_GLOW\n",
	"#define ALPHA_TEST\n",
	"#define TEXTURE_EMISSION_SPHERE\n",
	"#define NO_LIGHTS\n",
	"#define LIGHT_0_DIRECTIONAL\n",
	"#define LIGHT_0_OMNI\n",
	"#define LIGHT_0_SPOT\n",
	"#define LIGHT_1_DIRECTIONAL\n",
	"#define LIGHT_1_OMNI\n",
	"#define LIGHT_1_SPOT\n",
	"#define LIGHT_2_DIRECTIONAL\n",
	"#define LIGHT_2_OMNI\n",
	"#define LIGHT_2_SPOT\n",
	"#define LIGHT_3_DIRECTIONAL\n",
	"#define LIGHT_3_OMNI\n",
	"#define LIGHT_3_SPOT\n",
	"#define LIGHT_4_DIRECTIONAL\n",
	"#define LIGHT_4_OMNI\n",
	"#define LIGHT_4_SPOT\n",
	"#define LIGHT_5_DIRECTIONAL\n",
	"#define LIGHT_5_OMNI\n",
	"#define LIGHT_5_SPOT\n",
	"#define LIGHT_6_DIRECTIONAL\n",
	"#define LIGHT_6_OMNI\n",
	"#define LIGHT_6_SPOT\n",
	"#define LIGHT_7_DIRECTIONAL\n",
	"#define LIGHT_7_OMNI\n",
	"#define LIGHT_7_SPOT\n",
	"#define TEXTURE_GLOW_SPHERE\n",
};

static const char* _uniform_strings[]={
	"viewport",
	"texture_base",
	"texture_detail",
	"texture_normal",
	"texture_specular",
	"texture_emission",
	"texture_reflection",
	"texture_refmask",
	"texture_glowmap",
	"texture_light_pass",
	"texture_shadow",
	"glow",
	"alpha_threshhold",
};

static const char* _vertex_code="\
\n\
varying vec3 normal_interp;\n\
varying vec4 color;\n\
\n\
#ifdef USE_TEXTURE_NORMALMAP\n\
varying vec3 binormal_interp;\n\
varying vec3 tangent_interp;\n\
#endif\n\
\n\
#ifdef SPOT_SHADOW\n\
varying vec4 shadow_proj;\n\
#endif\n\
\n\
uniform vec3 viewport;\n\
varying vec4 pos;\n\
varying vec2 fraguv;\n\
varying float w;\n\
\n\
#ifdef USE_FOG\n\
varying float fog_factor;\n\
#endif\n\
\n\
void main() {\n\
	\n\
	\n\
	normal_interp = normalize( gl_NormalMatrix * gl_Normal );\n\
	color = gl_Color;\n\
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
#ifdef USE_TEXTURE_BASE	\n\
	gl_TexCoord[0] = gl_MultiTexCoord0;	\n\
#endif\n\
\n\
#ifdef SPOT_SHADOW\n\
	shadow_proj = gl_ModelViewMatrix * gl_Vertex;\n\
	shadow_proj = gl_TextureMatrix[4] * shadow_proj;\n\
#endif\n\
\n\
	pos = gl_ModelViewMatrix * gl_Vertex;\n\
	vec4 spos = ftransform();\n\
	gl_Position = spos;\n\
	fraguv = ((pos.xy * viewport.z / -pos.z) / viewport.xy) * 0.5 + 0.5;\n\
	fraguv *= spos.w;\n\
	w = spos.w;\n\
	\n\
	\n\
	\n\
#ifdef USE_FOG\n\
	gl_FogFragCoord = length(pos) * normalize(pos.z);\n\
	#ifdef FOG_TYPE_LINEAR\n\
	fog_factor = (gl_Fog.end - gl_FogFragCoord) / (gl_Fog.end - gl_Fog.start);\n\
	#endif\n\
	const float LOG2 = 1.442695;\n\
	#ifdef FOG_TYPE_EXP\n\
	fog_factor = exp2(-gl_Fog.density * gl_FogFragCoord * LOG2);\n\
	#endif\n\
	#ifdef FOG_TYPE_EXP2\n\
	fog_factor = exp2(-gl_Fog.density * gl_Fog.density * gl_FogFragCoord * gl_FogFragCoord * LOG2);\n\
	#endif\n\
	fog_factor = clamp(fog_factor, 0.0, 1.0);\n\
#endif\n\
}\n\
\n\
\n\
";

static const char* _fragment_code="\
\n\
varying vec3 normal_interp;\n\
\n\
#ifdef USE_TEXTURE_BASE\n\
uniform sampler2D texture_base;\n\
#endif\n\
#ifdef USE_TEXTURE_DETAIL\n\
uniform sampler2D texture_detail;\n\
#endif\n\
#ifdef USE_TEXTURE_NORMALMAP\n\
uniform sampler2D texture_normal;\n\
varying vec3 binormal_interp;\n\
varying vec3 tangent_interp;\n\
#endif\n\
#ifdef USE_TEXTURE_SPECULAR\n\
uniform sampler2D texture_specular;\n\
#endif\n\
#ifdef USE_TEXTURE_EMISSION\n\
uniform sampler2D texture_emission;\n\
#endif\n\
#ifdef USE_TEXTURE_REFLECTION\n\
uniform sampler2D texture_reflection;\n\
#endif\n\
#ifdef USE_TEXTURE_REFMASK\n\
uniform sampler2D texture_refmask;\n\
#endif\n\
#ifdef USE_TEXTURE_GLOWMAP\n\
uniform sampler2D texture_glowmap;\n\
#endif\n\
#ifdef USE_TEXTURE_LIGHT_PASS\n\
uniform sampler2D texture_light_pass;\n\
#endif\n\
\n\
#ifdef SPOT_SHADOW\n\
uniform sampler2D texture_shadow;\n\
varying vec4 shadow_proj;\n\
#endif\n\
\n\
#ifdef USE_GLOW\n\
uniform float glow;\n\
#endif\n\
\n\
#ifdef ALPHA_TEST\n\
uniform float alpha_threshhold;\n\
#endif\n\
\n\
#ifdef USE_FOG\n\
varying float fog_factor;\n\
#endif\n\
\n\
varying vec4 color;\n\
varying vec4 pos;\n\
varying vec2 fraguv;\n\
varying float w;\n\
\n\
\n\
\n\
vec3 process_directional(int p_idx, vec3 normal, vec3 frag_pos, vec3 ambient, vec3 diffuse, vec3 specular, float specular_intensity) {\n\
\n\
	vec3 light_direction = -gl_LightSource[p_idx].position.xyz;\n\
	float NdotL = clamp( dot( normal.xyz, -light_direction ), 0.0, 1.0 );\n\
	\n\
	\n\
	vec3 light_ref = reflect( light_direction, normal.xyz );\n\
	float eye_light = clamp( dot( light_ref, -normalize(frag_pos)), 0.0, 1.0 );\n\
	\n\
	vec3 ret = vec3(0.0, 0.0, 0.0);\n\
\n\
	ret += ambient * gl_LightSource[p_idx].ambient.rgb * diffuse.rgb;\n\
	ret += NdotL * ( diffuse * gl_LightSource[p_idx].diffuse.rgb + \n\
	                   specular * gl_LightSource[p_idx].specular.rgb * pow( eye_light, specular_intensity ) );\n\
	return ret;\n\
}\n\
\n\
vec3 process_omni(int p_idx, vec3 normal, vec3 frag_pos, vec3 ambient, vec4 diffuse, vec3 specular, float specular_intensity) {\n\
\n\
	vec3 direction = frag_pos - gl_LightSource[p_idx].position.xyz;\n\
	float dist = length(direction);\n\
\n\
	float radius = gl_LightSource[p_idx].linearAttenuation;\n\
\n\
	vec3 ret = vec3(0.0, 0.0, 0.0);\n\
	\n\
	if (dist > radius)\n\
		return ret;\n\
\n\
	float light_attenuation = gl_LightSource[p_idx].constantAttenuation;\n\
	float light_energy = gl_LightSource[p_idx].quadraticAttenuation;\n\
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
	ret += ambient * gl_LightSource[p_idx].ambient.rgb * diffuse.rgb;\n\
	ret += NdotL * ( diffuse.rgb * gl_LightSource[p_idx].diffuse.rgb + \n\
	                   specular * gl_LightSource[p_idx].specular.rgb * pow( eye_light, specular_intensity ) );				\n\
	ret*=attenuation;\n\
\n\
	return ret;\n\
}\n\
\n\
vec3 process_spot(int p_idx, vec3 normal, vec3 frag_pos, vec3 ambient, vec4 diffuse, vec3 specular, float specular_intensity) {\n\
\n\
	vec3 spot_direction = gl_LightSource[p_idx].spotDirection;\n\
	vec3 direction = frag_pos - gl_LightSource[p_idx].position.xyz;\n\
	float dist = length(direction);\n\
\n\
	float radius = gl_LightSource[p_idx].linearAttenuation;\n\
\n\
	vec3 ret = vec3(0.0, 0.0, 0.0);\n\
\n\
	if (dist > radius)\n\
		return ret;\n\
\n\
	#ifdef SPOT_SHADOW\n\
\n\
	vec4 bias = shadow_proj / shadow_proj.w;\n\
	float shadow_dist = texture2D(texture_shadow, bias.st).z * radius;\n\
	if (shadow_dist < shadow_proj.z)\n\
		return vec3(0.0);\n\
\n\
	#endif\n\
		\n\
	direction = normalize(direction);\n\
\n\
	float cos = dot(direction, spot_direction);\n\
	if (cos < gl_LightSource[p_idx].spotCosCutoff)\n\
		return ret;\n\
\n\
	float rim = (1.0 - cos) / (1.0 - gl_LightSource[p_idx].spotCosCutoff);\n\
	float spot_attenuation = 1.0 - pow( rim, gl_LightSource[p_idx].spotExponent );\n\
\n\
	float light_attenuation = gl_LightSource[p_idx].constantAttenuation;\n\
	float light_energy = gl_LightSource[p_idx].quadraticAttenuation;\n\
	\n\
	\n\
	dist = clamp(1.0 - dist/radius, 0.0, 1.0);\n\
	\n\
	float attenuation = pow( dist, light_attenuation ) * light_energy * spot_attenuation;	\n\
	\n\
	float NdotL = clamp( dot( normal.xyz, -direction ), 0.0, 1.0 );	\n\
	\n\
	\n\
	vec3 light_ref = reflect( direction, normal.xyz );\n\
	float eye_light = clamp( dot( light_ref, -normalize(frag_pos)), 0.0, 1.0 );\n\
	\n\
	ret += ambient * gl_LightSource[p_idx].ambient.rgb * diffuse.rgb;\n\
	ret += NdotL * ( diffuse.rgb * gl_LightSource[p_idx].diffuse.rgb + \n\
	                   specular * gl_LightSource[p_idx].specular.rgb * pow( eye_light, specular_intensity ) );				\n\
	ret*=attenuation;\n\
\n\
	return ret;\n\
}\n\
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
	vec4 diffuse;\n\
#ifdef USE_TEXTURE_BASE\n\
	\n\
	diffuse = texture2D(texture_base, gl_TexCoord[0].st);\n\
	#ifdef ALPHA_TEST\n\
	if (diffuse.a <= alpha_threshhold)\n\
		discard;\n\
	#endif\n\
#else\n\
	\n\
	diffuse = gl_FrontMaterial.diffuse;\n\
#endif\n\
	\n\
	vec3 normal;\n\
\n\
#ifdef USE_TEXTURE_NORMALMAP\n\
	vec3 tn = texture2D(texture_normal, gl_TexCoord[0].st ).xyz - 0.5;	\n\
	normal=mat3( tangent_interp, binormal_interp, normal_interp ) * tn;\n\
#else\n\
	normal = normal_interp;\n\
#endif\n\
	normal = normalize( normal.xyz );\n\
\n\
	vec2 sphere_uv = vec2( normal.x * 0.5 + 0.5, -normal.y * 0.5 + 0.5 );\n\
\n\
#ifdef USE_TEXTURE_SPECULAR\n\
	vec3 specular = texture2D(texture_specular, gl_TexCoord[0].st).rgb;\n\
#else\n\
	vec3 specular = gl_FrontMaterial.specular.rgb;\n\
#endif\n\
\n\
#ifdef USE_TEXTURE_EMISSION\n\
#ifdef TEXTURE_EMISSION_SPHERE\n\
	vec3 ambient = texture2D(texture_emission, sphere_uv).rgb;\n\
#else\n\
	vec3 ambient = texture2D(texture_emission, gl_TexCoord[0].st).rgb;\n\
#endif\n\
#else\n\
	vec3 ambient = gl_FrontMaterial.ambient.rgb;\n\
#endif\n\
\n\
	float specular_intensity = gl_FrontMaterial.shininess;\n\
	vec4 fragment = vec4(0.0, 0.0, 0.0, 0.0);\n\
\n\
#ifdef USE_TEXTURE_REFLECTION\n\
	vec4 ref = texture2D(texture_reflection, sphere_uv);\n\
	float interp = 0.5;\n\
#ifdef USE_TEXTURE_REFMASK\n\
	interp = texture2D(texture_refmask, gl_TexCoord[0].st).r;\n\
#endif\n\
	diffuse = (diffuse * interp) + (ref * (1.0 - interp));\n\
#endif\n\
\n\
#ifdef NO_LIGHTS\n\
\n\
#ifdef USE_TEXTURE_LIGHT_PASS\n\
\n\
	vec4 light = texture2D(texture_light_pass, fraguv / w) * 4.0;\n\
	fragment.rgb = /* ambient + */ light.rgb * diffuse.rgb + (specular * light.a);\n\
	fragment.a = diffuse.a;\n\
#else\n\
\n\
	fragment = diffuse;\n\
#endif\n\
\n\
#else\n\
\n\
	vec3 rpos = vec3(pos.x, pos.y, pos.z);\n\
\n\
#ifdef LIGHT_0_DIRECTIONAL\n\
	fragment.rgb += process_directional(0, normal, rpos, ambient, diffuse.rgb, specular, specular_intensity);\n\
#endif\n\
#ifdef LIGHT_0_OMNI\n\
	fragment.rgb += process_omni(0, normal, pos.xyz, ambient, diffuse, specular, specular_intensity);\n\
#endif\n\
#ifdef LIGHT_0_SPOT\n\
	fragment.rgb += process_spot(0, normal, pos.xyz, ambient, diffuse, specular, specular_intensity);\n\
#endif\n\
\n\
#ifdef LIGHT_1_DIRECTIONAL\n\
	fragment.rgb += process_directional(1, normal, rpos, ambient, diffuse.rgb, specular, specular_intensity);\n\
#endif\n\
#ifdef LIGHT_1_OMNI\n\
	fragment.rgb += process_omni(1, normal, pos.xyz, ambient, diffuse, specular, specular_intensity);\n\
#endif\n\
#ifdef LIGHT_1_SPOT\n\
	fragment.rgb += process_spot(1, normal, pos.xyz, ambient, diffuse, specular, specular_intensity);\n\
#endif\n\
\n\
#ifdef LIGHT_2_DIRECTIONAL\n\
	fragment.rgb += process_directional(2, normal, rpos, ambient, diffuse.rgb, specular, specular_intensity);\n\
#endif\n\
#ifdef LIGHT_2_OMNI\n\
	fragment.rgb += process_omni(2, normal, pos.xyz, ambient, diffuse, specular, specular_intensity);\n\
#endif\n\
#ifdef LIGHT_2_SPOT\n\
	fragment.rgb += process_spot(2, normal, pos.xyz, ambient, diffuse, specular, specular_intensity);\n\
#endif\n\
\n\
#ifdef LIGHT_3_DIRECTIONAL\n\
	fragment.rgb += process_directional(3, normal, rpos, ambient, diffuse.rgb, specular, specular_intensity);\n\
#endif\n\
#ifdef LIGHT_3_OMNI\n\
	fragment.rgb += process_omni(3, normal, pos.xyz, ambient, diffuse, specular, specular_intensity);\n\
#endif\n\
#ifdef LIGHT_3_SPOT\n\
	fragment.rgb += process_spot(3, normal, pos.xyz, ambient, diffuse, specular, specular_intensity);\n\
#endif\n\
\n\
#ifdef LIGHT_4_DIRECTIONAL\n\
	fragment.rgb += process_directional(4, normal, rpos, ambient, diffuse.rgb, specular, specular_intensity);\n\
#endif\n\
#ifdef LIGHT_4_OMNI\n\
	fragment.rgb += process_omni(4, normal, pos.xyz, ambient, diffuse, specular, specular_intensity);\n\
#endif\n\
#ifdef LIGHT_4_SPOT\n\
	fragment.rgb += process_spot(4, normal, pos.xyz, ambient, diffuse, specular, specular_intensity);\n\
#endif\n\
\n\
#ifdef LIGHT_5_DIRECTIONAL\n\
	fragment.rgb += process_directional(5, normal, rpos, ambient, diffuse.rgb, specular, specular_intensity);\n\
#endif\n\
#ifdef LIGHT_5_OMNI\n\
	fragment.rgb += process_omni(5, normal, pos.xyz, ambient, diffuse, specular, specular_intensity);\n\
#endif\n\
#ifdef LIGHT_5_SPOT\n\
	fragment.rgb += process_spot(5, normal, pos.xyz, ambient, diffuse, specular, specular_intensity);\n\
#endif\n\
\n\
#ifdef LIGHT_6_DIRECTIONAL\n\
	fragment.rgb += process_directional(6, normal, rpos, ambient, diffuse.rgb, specular, specular_intensity);\n\
#endif\n\
#ifdef LIGHT_6_OMNI\n\
	fragment.rgb += process_omni(6, normal, pos.xyz, ambient, diffuse, specular, specular_intensity);\n\
#endif\n\
#ifdef LIGHT_6_SPOT\n\
	fragment.rgb += process_spot(6, normal, pos.xyz, ambient, diffuse, specular, specular_intensity);\n\
#endif\n\
\n\
#ifdef LIGHT_7_DIRECTIONAL\n\
	fragment.rgb += process_directional(7, normal, rpos, ambient, diffuse.rgb, specular, specular_intensity);\n\
#endif\n\
#ifdef LIGHT_7_OMNI\n\
	fragment.rgb += process_omni(7, normal, pos.xyz, ambient, diffuse, specular, specular_intensity);\n\
#endif\n\
#ifdef LIGHT_7_SPOT\n\
	fragment.rgb += process_spot(7, normal, pos.xyz, ambient, diffuse, specular, specular_intensity);\n\
#endif\n\
\n\
	fragment.a = diffuse.a;\n\
#endif \n\
\n\
	fragment = fragment * color;\n\
\n\
#ifdef USE_FOG\n\
	fragment.xyz = mix(gl_Fog.color.xyz, fragment.xyz, fog_factor);\n\
#endif\n\
\n\
#ifdef USE_GLOW\n\
	#ifdef USE_TEXTURE_GLOWMAP\n\
		#ifdef TEXTURE_GLOW_SPHERE\n\
			fragment.a = glow + texture2D(texture_glowmap, sphere_uv).a;\n\
		#else\n\
			fragment.a = glow + texture2D(texture_glowmap, gl_TexCoord[0].st).a;\n\
		#endif\n\
	#else\n\
		fragment.a = glow;\n\
	#endif\n\
#endif\n\
\n\
	gl_FragColor = fragment; \n\
}\n\
\n\
";

		setup(_conditional_strings,43,_uniform_strings,13,_vertex_code,_fragment_code);
	};

};

#endif

