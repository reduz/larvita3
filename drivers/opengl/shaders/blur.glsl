[vertex]

varying vec2 uv_interp;

void main() {
	

	gl_Position = gl_Vertex;
	uv_interp = gl_Vertex.xy *0.5 + 0.5;
}

[fragment]


uniform sampler2D screen_texture;
varying vec2 uv_interp;
uniform vec2 pixelsize;

 
void main() {
	
	
#ifdef VERTICAL_PASS
// simple 5x5 gaussian blur kernel
	vec3 blur=texture2D( screen_texture,  vec2(uv_interp.x,uv_interp.y-pixelsize.y*2.0) ).rgb;
	blur+=texture2D( screen_texture,  vec2(uv_interp.x,uv_interp.y-pixelsize.y) ).rgb;
	blur+=texture2D( screen_texture,  uv_interp ).rgb;
	blur+=texture2D( screen_texture,  vec2(uv_interp.x,uv_interp.y+pixelsize.y) ).rgb;
	blur+=texture2D( screen_texture,  vec2(uv_interp.x,uv_interp.y+pixelsize.y*2.0) ).rgb;
#else

	vec3 blur=texture2D( screen_texture,  vec2(uv_interp.x-pixelsize.x*2.0,uv_interp.y) ).rgb;
	blur+=texture2D( screen_texture,  vec2(uv_interp.x-pixelsize.x,uv_interp.y) ).rgb;
	blur+=texture2D( screen_texture,  uv_interp ).rgb;
	blur+=texture2D( screen_texture,  vec2(uv_interp.x+pixelsize.x,uv_interp.y) ).rgb;
	blur+=texture2D( screen_texture,  vec2(uv_interp.x+pixelsize.x*2.0,uv_interp.y) ).rgb;

#endif

	blur/=5.0;


	gl_FragColor = vec4( blur, 1.0 );
		
}