#version 330 core

in vec2 TexCoord;

uniform vec3 u_color;


void main()
{
	vec4 baseColor = vec4(u_color, 1.0f);
	gl_FragColor = baseColor;
}