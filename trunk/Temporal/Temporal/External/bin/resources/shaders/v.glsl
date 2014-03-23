#version 330

attribute vec2 a_coordinate;
attribute vec2 a_textureCoordinate;
attribute vec4 a_color;
varying vec2 v_textureCoordinate;
varying vec4 v_color;
uniform mat4 u_projection;

void main(void) {
	gl_Position = u_projection * vec4(a_coordinate, 0.0, 1.0);
	v_textureCoordinate = a_textureCoordinate;
	v_color = a_color;
}