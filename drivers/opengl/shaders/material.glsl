[vertex]

varying vec3 normal_interp;
varying vec4 color;

#ifdef USE_TEXTURE_NORMALMAP
varying vec3 binormal_interp;
varying vec3 tangent_interp;
#endif

#ifdef SPOT_SHADOW
varying vec4 shadow_proj;
#endif

uniform vec3 viewport;
varying vec4 pos;
varying vec2 fraguv;
varying float w;

#ifdef USE_FOG
varying float fog_factor;
#endif

void main() {
	
	
	normal_interp = normalize( gl_NormalMatrix * gl_Normal );
	color = gl_Color;
	
#ifdef USE_TEXTURE_NORMALMAP
	// those are sent via the multitexcoord arrays 
	vec3 tangent=gl_MultiTexCoord6.xyz;
	float binormal_facing=gl_MultiTexCoord6.w;

	tangent_interp = normalize(gl_NormalMatrix *  tangent );
	binormal_interp = normalize(cross( normal_interp, tangent_interp ) * binormal_facing);
#endif

#ifdef USE_TEXTURE_BASE	
	gl_TexCoord[0] = gl_MultiTexCoord0;	
#endif

#ifdef SPOT_SHADOW
	shadow_proj = gl_ModelViewMatrix * gl_Vertex;
	shadow_proj = gl_TextureMatrix[4] * shadow_proj;
#endif

	pos = gl_ModelViewMatrix * gl_Vertex;
	vec4 spos = ftransform();
	gl_Position = spos;
	fraguv = ((pos.xy * viewport.z / -pos.z) / viewport.xy) * 0.5 + 0.5;
	fraguv *= spos.w;
	w = spos.w;
	//fraguv = (spos.xy / spos.w) * 0.5 + 0.5;
	//screen_pos = gl_Vertex.xy * viewport.xy / viewport.z; // used for screen coordinates
	
#ifdef USE_FOG
	gl_FogFragCoord = length(pos) * normalize(pos.z);
	#ifdef FOG_TYPE_LINEAR
	fog_factor = (gl_Fog.end - gl_FogFragCoord) / (gl_Fog.end - gl_Fog.start);
	#endif
	const float LOG2 = 1.442695;
	#ifdef FOG_TYPE_EXP
	fog_factor = exp2(-gl_Fog.density * gl_FogFragCoord * LOG2);
	#endif
	#ifdef FOG_TYPE_EXP2
	fog_factor = exp2(-gl_Fog.density * gl_Fog.density * gl_FogFragCoord * gl_FogFragCoord * LOG2);
	#endif
	fog_factor = clamp(fog_factor, 0.0, 1.0);
#endif
}


[fragment]

varying vec3 normal_interp;

#ifdef USE_TEXTURE_BASE
uniform sampler2D texture_base;
#endif
#ifdef USE_TEXTURE_DETAIL
uniform sampler2D texture_detail;
#endif
#ifdef USE_TEXTURE_NORMALMAP
uniform sampler2D texture_normal;
varying vec3 binormal_interp;
varying vec3 tangent_interp;
#endif
#ifdef USE_TEXTURE_SPECULAR
uniform sampler2D texture_specular;
#endif
#ifdef USE_TEXTURE_EMISSION
uniform sampler2D texture_emission;
#endif
#ifdef USE_TEXTURE_REFLECTION
uniform sampler2D texture_reflection;
#endif
#ifdef USE_TEXTURE_REFMASK
uniform sampler2D texture_refmask;
#endif
#ifdef USE_TEXTURE_GLOWMAP
uniform sampler2D texture_glowmap;
#endif
#ifdef USE_TEXTURE_LIGHT_PASS
uniform sampler2D texture_light_pass;
#endif

#ifdef SPOT_SHADOW
uniform sampler2D texture_shadow;
varying vec4 shadow_proj;
#endif

#ifdef USE_GLOW
uniform float glow;
#endif

#ifdef ALPHA_TEST
uniform float alpha_threshhold;
#endif

#ifdef USE_FOG
varying float fog_factor;
#endif

varying vec4 color;
varying vec4 pos;
varying vec2 fraguv;
varying float w;

//[02:19] <reduz> (1/R * (R-x) )^A * E should be more usefriendly than GL's traditional 1/(c+lx+qx)

vec3 process_directional(int p_idx, vec3 normal, vec3 frag_pos, vec3 ambient, vec3 diffuse, vec3 specular, float specular_intensity) {

	vec3 light_direction = -gl_LightSource[p_idx].position.xyz;
	float NdotL = clamp( dot( normal.xyz, -light_direction ), 0.0, 1.0 );
	
	// phong shading
	vec3 light_ref = reflect( light_direction, normal.xyz );
	float eye_light = clamp( dot( light_ref, -normalize(frag_pos)), 0.0, 1.0 );
	
	vec3 ret = vec3(0.0, 0.0, 0.0);

	ret += ambient * gl_LightSource[p_idx].ambient.rgb * diffuse.rgb;
	ret += NdotL * ( diffuse * gl_LightSource[p_idx].diffuse.rgb + 
	                   specular * gl_LightSource[p_idx].specular.rgb * pow( eye_light, specular_intensity ) );
	return ret;
}

vec3 process_omni(int p_idx, vec3 normal, vec3 frag_pos, vec3 ambient, vec4 diffuse, vec3 specular, float specular_intensity) {

	vec3 direction = frag_pos - gl_LightSource[p_idx].position.xyz;
	float dist = length(direction);

	float radius = gl_LightSource[p_idx].linearAttenuation;

	vec3 ret = vec3(0.0, 0.0, 0.0);
	
	if (dist > radius)
		return ret;

	float light_attenuation = gl_LightSource[p_idx].constantAttenuation;
	float light_energy = gl_LightSource[p_idx].quadraticAttenuation;

	direction = normalize(direction);
	
	// map in range from 0 to 1
	dist = clamp(1.0 - dist/radius, 0.0, 1.0);
	
	float attenuation = pow( dist, light_attenuation ) * light_energy;	
	
	float NdotL = clamp( dot( normal.xyz, -direction ), 0.0, 1.0 );	
	
	// phong shading
	vec3 light_ref = reflect( direction, normal.xyz );
	float eye_light = clamp( dot( light_ref, -normalize(frag_pos)), 0.0, 1.0 );
	
	ret += ambient * gl_LightSource[p_idx].ambient.rgb * diffuse.rgb;
	ret += NdotL * ( diffuse.rgb * gl_LightSource[p_idx].diffuse.rgb + 
	                   specular * gl_LightSource[p_idx].specular.rgb * pow( eye_light, specular_intensity ) );				
	ret*=attenuation;

	return ret;
}

vec3 process_spot(int p_idx, vec3 normal, vec3 frag_pos, vec3 ambient, vec4 diffuse, vec3 specular, float specular_intensity) {

	vec3 spot_direction = gl_LightSource[p_idx].spotDirection;
	vec3 direction = frag_pos - gl_LightSource[p_idx].position.xyz;
	float dist = length(direction);

	float radius = gl_LightSource[p_idx].linearAttenuation;

	vec3 ret = vec3(0.0, 0.0, 0.0);

	if (dist > radius)
		return ret;

	#ifdef SPOT_SHADOW

	vec4 bias = shadow_proj / shadow_proj.w;
	float shadow_dist = texture2D(texture_shadow, bias.st).z * radius;
	if (shadow_dist < shadow_proj.z)
		return vec3(0.0);

	#endif
		
	direction = normalize(direction);

	float cos = dot(direction, spot_direction);
	if (cos < gl_LightSource[p_idx].spotCosCutoff)
		return ret;

	float rim = (1.0 - cos) / (1.0 - gl_LightSource[p_idx].spotCosCutoff);
	float spot_attenuation = 1.0 - pow( rim, gl_LightSource[p_idx].spotExponent );

	float light_attenuation = gl_LightSource[p_idx].constantAttenuation;
	float light_energy = gl_LightSource[p_idx].quadraticAttenuation;
	
	// map in range from 0 to 1
	dist = clamp(1.0 - dist/radius, 0.0, 1.0);
	
	float attenuation = pow( dist, light_attenuation ) * light_energy * spot_attenuation;	
	
	float NdotL = clamp( dot( normal.xyz, -direction ), 0.0, 1.0 );	
	
	// phong shading
	vec3 light_ref = reflect( direction, normal.xyz );
	float eye_light = clamp( dot( light_ref, -normalize(frag_pos)), 0.0, 1.0 );
	
	ret += ambient * gl_LightSource[p_idx].ambient.rgb * diffuse.rgb;
	ret += NdotL * ( diffuse.rgb * gl_LightSource[p_idx].diffuse.rgb + 
	                   specular * gl_LightSource[p_idx].specular.rgb * pow( eye_light, specular_intensity ) );				
	ret*=attenuation;

	return ret;
}


void main() {
	
	// stuff to fill in
	
	/***************************/
	/*   NORMAL+Z   */
	/***************************/

	vec4 diffuse;
#ifdef USE_TEXTURE_BASE
	// if texture, use texture for diffuse
	diffuse = texture2D(texture_base, gl_TexCoord[0].st);
	#ifdef ALPHA_TEST
	if (diffuse.a <= alpha_threshhold)
		discard;
	#endif
#else
	// otherwise use default diffuse
	diffuse = gl_FrontMaterial.diffuse;
#endif
	
	vec3 normal;

#ifdef USE_TEXTURE_NORMALMAP
	vec3 tn = texture2D(texture_normal, gl_TexCoord[0].st ).xyz - 0.5;	
	normal=mat3( tangent_interp, binormal_interp, normal_interp ) * tn;
#else
	normal = normal_interp;
#endif
	normal = normalize( normal.xyz );

	vec2 sphere_uv = vec2( normal.x * 0.5 + 0.5, -normal.y * 0.5 + 0.5 );

#ifdef USE_TEXTURE_SPECULAR
	vec3 specular = texture2D(texture_specular, gl_TexCoord[0].st).rgb;
#else
	vec3 specular = gl_FrontMaterial.specular.rgb;
#endif

#ifdef USE_TEXTURE_EMISSION
#ifdef TEXTURE_EMISSION_SPHERE
	vec3 ambient = texture2D(texture_emission, sphere_uv).rgb;
#else
	vec3 ambient = texture2D(texture_emission, gl_TexCoord[0].st).rgb;
#endif
#else
	vec3 ambient = gl_FrontMaterial.ambient.rgb;
#endif

	float specular_intensity = gl_FrontMaterial.shininess;
	vec4 fragment = vec4(0.0, 0.0, 0.0, 0.0);

#ifdef USE_TEXTURE_REFLECTION
	vec4 ref = texture2D(texture_reflection, sphere_uv);
	float interp = 0.5;
#ifdef USE_TEXTURE_REFMASK
	interp = texture2D(texture_refmask, gl_TexCoord[0].st).r;
#endif
	diffuse = (diffuse * interp) + (ref * (1.0 - interp));
#endif

#ifdef NO_LIGHTS

#ifdef USE_TEXTURE_LIGHT_PASS

	vec4 light = texture2D(texture_light_pass, fraguv / w) * 4.0;
	fragment.rgb = /* ambient + */ light.rgb * diffuse.rgb + (specular * light.a);
	fragment.a = diffuse.a;
#else

	fragment = diffuse;
#endif

#else

	vec3 rpos = vec3(pos.x, pos.y, pos.z);

#ifdef LIGHT_0_DIRECTIONAL
	fragment.rgb += process_directional(0, normal, rpos, ambient, diffuse.rgb, specular, specular_intensity);
#endif
#ifdef LIGHT_0_OMNI
	fragment.rgb += process_omni(0, normal, pos.xyz, ambient, diffuse, specular, specular_intensity);
#endif
#ifdef LIGHT_0_SPOT
	fragment.rgb += process_spot(0, normal, pos.xyz, ambient, diffuse, specular, specular_intensity);
#endif

#ifdef LIGHT_1_DIRECTIONAL
	fragment.rgb += process_directional(1, normal, rpos, ambient, diffuse.rgb, specular, specular_intensity);
#endif
#ifdef LIGHT_1_OMNI
	fragment.rgb += process_omni(1, normal, pos.xyz, ambient, diffuse, specular, specular_intensity);
#endif
#ifdef LIGHT_1_SPOT
	fragment.rgb += process_spot(1, normal, pos.xyz, ambient, diffuse, specular, specular_intensity);
#endif

#ifdef LIGHT_2_DIRECTIONAL
	fragment.rgb += process_directional(2, normal, rpos, ambient, diffuse.rgb, specular, specular_intensity);
#endif
#ifdef LIGHT_2_OMNI
	fragment.rgb += process_omni(2, normal, pos.xyz, ambient, diffuse, specular, specular_intensity);
#endif
#ifdef LIGHT_2_SPOT
	fragment.rgb += process_spot(2, normal, pos.xyz, ambient, diffuse, specular, specular_intensity);
#endif

#ifdef LIGHT_3_DIRECTIONAL
	fragment.rgb += process_directional(3, normal, rpos, ambient, diffuse.rgb, specular, specular_intensity);
#endif
#ifdef LIGHT_3_OMNI
	fragment.rgb += process_omni(3, normal, pos.xyz, ambient, diffuse, specular, specular_intensity);
#endif
#ifdef LIGHT_3_SPOT
	fragment.rgb += process_spot(3, normal, pos.xyz, ambient, diffuse, specular, specular_intensity);
#endif

#ifdef LIGHT_4_DIRECTIONAL
	fragment.rgb += process_directional(4, normal, rpos, ambient, diffuse.rgb, specular, specular_intensity);
#endif
#ifdef LIGHT_4_OMNI
	fragment.rgb += process_omni(4, normal, pos.xyz, ambient, diffuse, specular, specular_intensity);
#endif
#ifdef LIGHT_4_SPOT
	fragment.rgb += process_spot(4, normal, pos.xyz, ambient, diffuse, specular, specular_intensity);
#endif

#ifdef LIGHT_5_DIRECTIONAL
	fragment.rgb += process_directional(5, normal, rpos, ambient, diffuse.rgb, specular, specular_intensity);
#endif
#ifdef LIGHT_5_OMNI
	fragment.rgb += process_omni(5, normal, pos.xyz, ambient, diffuse, specular, specular_intensity);
#endif
#ifdef LIGHT_5_SPOT
	fragment.rgb += process_spot(5, normal, pos.xyz, ambient, diffuse, specular, specular_intensity);
#endif

#ifdef LIGHT_6_DIRECTIONAL
	fragment.rgb += process_directional(6, normal, rpos, ambient, diffuse.rgb, specular, specular_intensity);
#endif
#ifdef LIGHT_6_OMNI
	fragment.rgb += process_omni(6, normal, pos.xyz, ambient, diffuse, specular, specular_intensity);
#endif
#ifdef LIGHT_6_SPOT
	fragment.rgb += process_spot(6, normal, pos.xyz, ambient, diffuse, specular, specular_intensity);
#endif

#ifdef LIGHT_7_DIRECTIONAL
	fragment.rgb += process_directional(7, normal, rpos, ambient, diffuse.rgb, specular, specular_intensity);
#endif
#ifdef LIGHT_7_OMNI
	fragment.rgb += process_omni(7, normal, pos.xyz, ambient, diffuse, specular, specular_intensity);
#endif
#ifdef LIGHT_7_SPOT
	fragment.rgb += process_spot(7, normal, pos.xyz, ambient, diffuse, specular, specular_intensity);
#endif

	fragment.a = diffuse.a;
#endif // NO_LIGHTS

	fragment = fragment * color;

#ifdef USE_FOG
	fragment.xyz = mix(gl_Fog.color.xyz, fragment.xyz, fog_factor);
#endif

#ifdef USE_GLOW
	#ifdef USE_TEXTURE_GLOWMAP
		#ifdef TEXTURE_GLOW_SPHERE
			fragment.a = glow + texture2D(texture_glowmap, sphere_uv).a;
		#else
			fragment.a = glow + texture2D(texture_glowmap, gl_TexCoord[0].st).a;
		#endif
	#else
		fragment.a = glow;
	#endif
#endif

	gl_FragColor = fragment; 
}

