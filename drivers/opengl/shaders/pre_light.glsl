[vertex]

varying vec3 normal_interp;

#ifdef USE_TEXTURE_NORMALMAP
varying vec3 binormal_interp,tangent_interp;
#endif

varying vec4 pos;

void main() {
	
	
	normal_interp = normalize( gl_NormalMatrix * gl_Normal );
	
#ifdef USE_TEXTURE_NORMALMAP
	// those are sent via the multitexcoord arrays 
	vec3 tangent=gl_MultiTexCoord6.xyz;
	float binormal_facing=gl_MultiTexCoord6.w;

	tangent_interp = normalize(gl_NormalMatrix *  tangent );
	binormal_interp = normalize(cross( normal_interp, tangent_interp ) * binormal_facing);
#endif

#ifdef USE_TEXTURE_NORMALMAP
	gl_TexCoord[0] = gl_MultiTexCoord0;	
#endif
#ifdef ALPHA_TEST
	gl_TexCoord[0] = gl_MultiTexCoord0;	
#endif

	pos = gl_ModelViewMatrix * gl_Vertex;
	gl_Position = ftransform();
}


[fragment]

varying vec3 normal_interp;

#ifdef USE_TEXTURE_NORMALMAP
uniform sampler2D texture_normal;
varying vec3 tangent_interp;
varying vec3 binormal_interp;
#endif

#ifdef ALPHA_TEST
uniform sampler2D texture_base;
uniform float alpha_threshhold;
#endif

varying vec4 pos;

uniform float zfar;

vec2 pack_depth(float v) {
	float ret = -v / zfar * 256.0;
	return vec2(floor(ret) / 256.0, frac(ret));
};

vec2 pack_normal(vec3 normal) {
	return normalize(normal).xy * 0.5 + 0.5;
};


void main() {
	
	// stuff to fill in
	
	/***************************/
	/*   NORMAL+Z   */
	/***************************/

#ifdef ALPHA_TEST
	if (texture2D(texture_base, gl_TexCoord[0].st).a <= alpha_threshhold)
		discard;
#endif
	
	vec3 normal;
	
#ifdef USE_TEXTURE_NORMALMAP
	vec3 tn = normalize(texture2D(texture_normal, gl_TexCoord[0].st ).xyz - 0.5);	
	normal=mat3( tangent_interp, binormal_interp, normal_interp ) * tn;
#else
	normal = normal_interp;
#endif
	normal = normalize( normal.xyz );

	vec4 fragment;
	fragment.xy = pack_normal(normal);
	fragment.zw = pack_depth( pos.z );

	gl_FragColor = fragment; 
}

