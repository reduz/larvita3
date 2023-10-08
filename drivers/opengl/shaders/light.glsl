[vertex]

uniform vec3 viewport;
varying vec2 screen_pos;
varying float w;
varying vec2 fraguv;
varying vec4 shadow_proj;

void main() {
	#ifdef LIGHT_MODE_DIRECTIONAL
	screen_pos = gl_Vertex.xy * viewport.xy / viewport.z; // used for screen coordinates
	fraguv = gl_MultiTexCoord0.st;
	gl_Position = gl_Vertex;
	w = 1.0;

	#else
	
	//screen_pos = gl_Vertex.xy * viewport.xy / viewport.z; // used for screen coordinates
	//screen_pos = (gl_ModelViewMatrix * gl_Vertex).xy * viewport.xy / viewport.z;
	gl_Position = ftransform();
	vec4 pos = gl_ModelViewMatrix * gl_Vertex;
	screen_pos = gl_Position.xy * viewport.xy / viewport.z;;
	w = gl_Position.w;

	fraguv = ((pos.xy * viewport.z / -pos.z) / viewport.xy) * 0.5 + 0.5;
	//fraguv = (gl_Position.xy / w) * 0.5 + 0.5;
	fraguv *= w;
	#endif

	#ifdef USE_SHADOW_MAP
	shadow_proj = gl_ModelViewMatrix * gl_Vertex;
	shadow_proj = gl_TextureMatrix[1] * shadow_proj;
	#endif
}


[fragment]

uniform vec3 viewport;
uniform sampler2D normal_texture;
uniform float zfar;
varying vec2 screen_pos;
varying vec2 fraguv;
varying float w;
varying vec4 shadow_proj;

#ifdef USE_SHADOW_MAP
uniform sampler2D shadow_map;
uniform vec3 shadow_origin;
#endif

float unpack_depth(vec2 v) {

	float ret = v.x*256.0;
	ret += v.y;
	
	return -ret / 256.0 * zfar;
}

vec3 unpack_normal(vec2 n) {

	vec3 ret;
	ret.xy = n * 2.0 - 1.0; 
	ret.z = sqrt(1.0 - dot(ret.xy, ret.xy));

	return ret;
}

vec4 process_directional(vec3 normal, vec3 frag_pos) {

	#ifdef USE_SHADOW_MAP

	float radius = gl_LightSource[0].linearAttenuation;
	vec4 bias = shadow_proj / shadow_proj.w;
	float shadow_dist = texture2D(shadow_map, bias.st).z * radius;
	if (shadow_dist < shadow_proj.z)
		return vec4(gl_LightSource[0].ambient.rgb, 0.0);

	#endif

	vec3 light_direction = -gl_LightSource[0].position.xyz;
	float NdotL = clamp( dot( normal.xyz, -light_direction ), 0.0, 1.0 );
	
	// phong shading
	vec3 light_ref = reflect( light_direction, normal.xyz );
	float eye_light = clamp( dot( light_ref, -normalize(frag_pos)), 0.0, 1.0 );
	
	vec4 ret;
	ret.rgb = gl_LightSource[0].ambient.rgb + gl_LightSource[0].diffuse.rgb * NdotL;
	ret.a = dot(gl_LightSource[0].specular.rgb, vec3(0.33)) * pow(eye_light, 30.0) * NdotL;

	return ret / 4.0;
}

vec4 process_omni(vec3 normal, vec3 frag_pos) {

	vec3 direction = frag_pos - gl_LightSource[0].position.xyz;
	float dist = length(direction);

	float radius = gl_LightSource[0].linearAttenuation;
	vec4 ret = vec4(0.0, 0.0, 0.0, 0.0);

	if (dist > radius)
		discard;

	float light_attenuation = gl_LightSource[0].constantAttenuation;
	float light_energy = gl_LightSource[0].quadraticAttenuation;

	direction = normalize(direction);
	
	// map in range from 0 to 1
	dist = clamp(1.0 - dist/radius, 0.0, 1.0);
	
	float attenuation = pow( dist, light_attenuation ) * light_energy;	
	
	float NdotL = clamp( dot( normal.xyz, -direction ), 0.0, 1.0 );	
	
	// phong shading
	vec3 light_ref = reflect( direction, normal.xyz );
	float eye_light = clamp( dot( light_ref, -normalize(frag_pos)), 0.0, 1.0 );

	ret.rgb = gl_LightSource[0].ambient.rgb * (1.0-NdotL) + gl_LightSource[0].diffuse.rgb * NdotL;
	ret.a = dot(gl_LightSource[0].specular.rgb, vec3(0.33)) * pow(eye_light, 30.0) * NdotL;
	
	ret*=attenuation;

	return ret / 4.0;
}

vec4 process_spot(vec3 normal, vec3 frag_pos) {

	vec3 spot_direction = gl_LightSource[0].spotDirection;
	vec3 direction = frag_pos - gl_LightSource[0].position.xyz;
	float dist = length(direction);

	float radius = gl_LightSource[0].linearAttenuation;

	vec4 ret = vec4(0.0, 0.0, 0.0, 0.0);

	if (dist > radius)
		discard;
		//return vec4(1.0);
		

	direction = normalize(direction);

	float cos = dot(direction, spot_direction);
	if (cos < gl_LightSource[0].spotCosCutoff)
		discard;

	float rim = (1.0 - cos) / (1.0 - gl_LightSource[0].spotCosCutoff);
	float spot_attenuation = 1.0 - pow( rim, gl_LightSource[0].spotExponent );

	float light_attenuation = gl_LightSource[0].constantAttenuation;
	float light_energy = gl_LightSource[0].quadraticAttenuation;
	
	// map in range from 0 to 1
	dist = clamp(1.0 - dist/radius, 0.0, 1.0);
	
	float attenuation = pow( dist, light_attenuation ) * light_energy * spot_attenuation;
	
	float NdotL = clamp( dot( normal.xyz, -direction ), 0.0, 1.0 );	
	
	// phong shading
	vec3 light_ref = reflect( direction, normal.xyz );
	float eye_light = clamp( dot( light_ref, -normalize(frag_pos)), 0.0, 1.0 );

	ret.rgb = gl_LightSource[0].ambient.rgb + gl_LightSource[0].diffuse.rgb * NdotL;
	ret.a = dot(gl_LightSource[0].specular.rgb, vec3(0.33)) * pow(eye_light, 30.0) * NdotL;
	
	ret*=attenuation / 4.0;

	return ret;
}


void main() {

	vec4 p = texture2D(normal_texture, fraguv / w);
	vec3 normal = unpack_normal(p.xy);
	float depth = unpack_depth(p.zw);
	vec3 pos;
	pos.z = depth;
	pos.xy = screen_pos / w * -pos.z;

#ifdef LIGHT_MODE_DIRECTIONAL
	
	gl_FragColor = process_directional(normal, pos); 
#endif

#ifdef LIGHT_MODE_OMNI

	gl_FragColor = process_omni(normal, pos);

#endif

#ifdef LIGHT_MODE_SPOT

	gl_FragColor = process_spot(normal, pos);

#endif
}

