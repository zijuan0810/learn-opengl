#version 330 core

in vec2 TexCoord;

uniform vec3 u_color;
uniform sampler2D texture_diffuse1;


void main()
{
	vec4 texColor = texture2D(texture_diffuse1, TexCoord);
	vec4 baseColor = vec4(u_color, 1.0f);
	gl_FragColor = baseColor * texColor;
}