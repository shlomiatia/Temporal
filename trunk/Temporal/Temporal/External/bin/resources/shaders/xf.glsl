#version 330

varying vec2 v_textureCoordinate;
varying vec4 v_color;
uniform sampler2D u_texture;
uniform int u_type;

uniform float u_time;

void wave()
{
	float wavesPerSecond = 0.75;
	float wavesCount = 4;
	float waveSizeDivider = 1000; // Screen / X
	float offset = u_time * 2*3.14159 * wavesPerSecond;
	vec2 textureCoordinate = v_textureCoordinate;
	textureCoordinate.x += sin(textureCoordinate.y * wavesCount*2*3.14159 + offset) / waveSizeDivider;
	gl_FragColor = texture2D(u_texture, textureCoordinate) * v_color;
}

float gaussian(float x, float deviation)
{
	return (1.0 / sqrt(2.0 * 3.141592 * deviation)) * exp(-((x * x) / (2.0 * deviation)));	
}

void blur(int orientation)
{
	float halfBlur = 4.0;
	float strength = 0.6+(1.0+sin(u_time/2.0*3.14159))*0.2;//0.8
	float deviation = halfBlur * 0.35;
	deviation *= deviation;
	vec2 texelSize = vec2(1.0/128.0,1.0/72.0); 
	vec4 colour = vec4(0.0);
		
	for (int i = 0; i < 9; ++i)
	{
		float offset = float(i) - halfBlur;
		float gaussianResult = gaussian(offset * strength, deviation);
		vec2 texelOffset = vec2(0.0);
		if(orientation == 0)	{
			texelOffset = vec2(offset * texelSize.x, 0.0);
		} else {
			texelOffset = vec2(0.0, offset * texelSize.y);
		}
		vec4 texColour = texture2D(u_texture, v_textureCoordinate + texelOffset) * gaussianResult;
		colour += texColour;
	}
	gl_FragColor = clamp(colour, 0.0, 1.0);
	gl_FragColor.w = 1.0;
}

void blurH()
{
	blur(0);
}

void blurV()
{
	blur(1);
}
 
void main(void) {
	if(u_type == 0) {
		wave();
	} else if(u_type == 1) {
		blurH();
	} else if(u_type == 2) {
		blurV();
	}
}