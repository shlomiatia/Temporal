#version 330

varying vec2 v_textureCoordinate;
varying vec4 v_color;
uniform sampler2D u_texture;
uniform int u_type;

void main(void) {
	if(u_type == -1) {
		gl_FragColor = v_color;
	} else if(u_type == 0) {
		gl_FragColor =  texture2D(u_texture, v_textureCoordinate) * v_color;
	}
}