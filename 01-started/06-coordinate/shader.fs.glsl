#version 330 core

in vec2 TexCoord;

uniform vec3 u_color;
uniform float glfw_time;
uniform sampler2D texture1;
uniform sampler2D texture2;


void main()
{
	vec4 texColor1 = texture2D(texture1, TexCoord);
	vec4 texColor2 = texture2D(texture2, TexCoord);
	vec4 texColor = mix(texColor1, texColor2, 0.2);
	texColor.a = texColor1.a;
	vec4 baseColor = vec4(u_color, 1.0f);
	gl_FragColor = baseColor * texColor;
}