[vertex]

varying vec2 uv_interp;
void main() {
	
	gl_Position = gl_Vertex;
	uv_interp = gl_Vertex.xy * 0.5 + 0.5;
}

[fragment]

uniform sampler2D screen_texture;
uniform vec2 pixelsize;
uniform float bloom;
varying vec2 uv_interp;


 
void main() {
	
	// the horrible averaging blur is best for this!
	float px=pixelsize.x;
	float py=pixelsize.y;
	vec4 simple_blur=texture2D( screen_texture,  uv_interp );
	simple_blur+=texture2D( screen_texture,  vec2(uv_interp.x+px, uv_interp.y) );
	simple_blur+=texture2D( screen_texture,  vec2(uv_interp.x-px, uv_interp.y) );
	simple_blur+=texture2D( screen_texture,  vec2(uv_interp.x,uv_interp.y+py) );
	simple_blur+=texture2D( screen_texture,  vec2(uv_interp.x,uv_interp.y-py) );
	simple_blur/=5.0;
	simple_blur.rgb*= simple_blur.a + bloom;
	
		
 	gl_FragColor = vec4( simple_blur.rgb, 1.0 );
		
}
