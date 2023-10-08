[vertex]

varying vec3 normal_interp;

void main() {

	gl_Position = ftransform();

#ifdef USE_TEXTURE_GLOWMAP
#ifdef TEXTURE_GLOW_SPHERE

	normal_interp = normalize( gl_NormalMatrix * gl_Normal );

#else
	gl_TexCoord[0] = gl_MultiTexCoord0;	
#endif
#endif

#ifdef USE_TEXTURE_BASE
	gl_TexCoord[0] = gl_MultiTexCoord0;	
#endif
}

[fragment]

#ifdef USE_TEXTURE_BASE
uniform sampler2D texture_base;
#endif

#ifdef USE_TEXTURE_GLOWMAP
uniform sampler2D texture_glowmap;
#endif

uniform float glow;

varying vec3 normal_interp;

void main() {

	vec4 fragment = vec4(0.0);
	#ifdef USE_TEXTURE_BASE
	fragment.a = texture2D(texture_base, gl_TexCoord[0].st).a;
	#else
	fragment.a = gl_FrontMaterial.diffuse.a;
	#endif

	float g = glow;
	
	#ifdef USE_TEXTURE_GLOWMAP
	#ifdef TEXTURE_GLOW_SPHERE
	vec3 normal = normalize( normal_interp );
	g += texture2D(texture_glowmap, vec2( normal.x * 0.5 + 0.5, -normal.y * 0.5 + 0.5 )).r;
	#else
	g += texture2D(texture_glowmap, gl_TexCoord[0].st).r;
	#endif
	#endif

	fragment.a *= g;

	gl_FragColor = fragment;
}

