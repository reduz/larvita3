[vertex]

void main() {

	gl_TexCoord[0] = gl_MultiTexCoord0;	
	gl_Position = gl_Vertex;
}

[fragment]

uniform sampler2D screen_texture;

#ifdef BLUR_ENABLED
uniform sampler2D blur_texture;
#endif

#ifdef SSAO_ENABLED
uniform sampler2D normal_pos_texture;
#endif

uniform float zfar;
uniform vec2 pixelsize;
uniform vec2 screen_to_viewport;


vec3 unpack_normal(vec2 n) {

	vec3 ret;
	ret.xy = n * 2.0 - 1.0; 
	ret.z = -sqrt(1.0 - dot(ret.xy, ret.xy));

	return ret;
}

float unpack_depth(vec2 v) {

	float ret = v.x*256.0;
	ret += v.y;
	
	return ret / 256.0 * zfar;
}


#ifdef SSAO_ENABLED
vec3 get_pos( vec2 uv ) {

	vec3 pos;
	pos.z=unpack_depth( texture2D( normal_pos_texture, uv).zw );
	pos.xy = (uv * 2.0 - 1.0) * screen_to_viewport * pos.z;
	return pos;
}


float compute_ao( vec2 uv, vec3 normal, vec3 pos ) {

	
	vec3 uv_pos=get_pos(uv);
	vec3 rel = pos - uv_pos;
	float ao = dot(normalize(rel), normal) * 0.5 + 0.5;;
	return ao;
}

float compute_ssao() {

	vec3 pos = get_pos( gl_TexCoord[0].xy );
	vec3 normal = unpack_normal( gl_TexCoord[0].xy );
	
	float pw = pixelsize.x;
	float ph = pixelsize.y;

	float ao = 0.0;
	float aoscale=1.0;
	
	for (int i=0;i<3;i++) {

		ao+=compute_ao( vec2(gl_TexCoord[0].x+pw,gl_TexCoord[0].y+ph), normal, pos )/aoscale;
		ao+=compute_ao( vec2(gl_TexCoord[0].x+pw,gl_TexCoord[0].y-ph), normal, pos )/aoscale;
		ao+=compute_ao( vec2(gl_TexCoord[0].x-pw,gl_TexCoord[0].y+ph), normal, pos )/aoscale;
		ao+=compute_ao( vec2(gl_TexCoord[0].x-pw,gl_TexCoord[0].y-ph), normal, pos )/aoscale;

/*
		d=get_depth( vec2(gl_TexCoord[0].x+pw,gl_TexCoord[0].y+ph));
		ao+=compare_depths(depth,d)/aoscale;

		d=get_depth( vec2(gl_TexCoord[0].x-pw,gl_TexCoord[0].y+ph));
		ao+=compare_depths(depth,d)/aoscale;
	
		d=get_depth( vec2(gl_TexCoord[0].x+pw,gl_TexCoord[0].y-ph));
		ao+=compare_depths(depth,d)/aoscale;

		d=get_depth( vec2(gl_TexCoord[0].x-pw,gl_TexCoord[0].y-ph));
		ao+=compare_depths(depth,d)/aoscale;
*/	
		pw*=2.0;
		ph*=2.0;
		//aoscale*=1.2;
	}

	ao/=16.0;
	ao=pow(ao,0.70);
	return ao;
}
#endif


void main() {

	vec4 fragment = texture2D( screen_texture, gl_TexCoord[0].st );
	
	#ifdef BLUR_ENABLED
	vec4 blur = texture2D( blur_texture, gl_TexCoord[0].st );
	fragment.rgb+=pow(blur.rgb, 1.4)*4.0;
	#endif

	#ifdef SSAO_ENABLED
	//fragment.rgb=vec4( fragment.rgb * compute_ssao(), 1.0 );
	#endif
	
	//fragment.xyz = unpack_normal(fragment.xy);

	//fragment.r = 1.0;
	gl_FragColor = fragment;
}

