#version 330 core

in vec3 ourColor;
in vec2 TexCoord;

uniform float glfw_time;
uniform sampler2D texture1;
uniform sampler2D texture2;


void main()
{
	vec4 texColor1 = texture2D(texture1, TexCoord);
	vec4 texColor2 = texture2D(texture2, TexCoord);
	vec4 texColor = mix(texColor1, texColor2, 0.2);
	texColor.a = texColor1.a;
	vec4 baseColor = vec4(ourColor, 1.0f);
//	gl_FragColor = baseColor * texColor;

	if(texColor2.a < 0.01)
        discard;
	gl_FragColor = texColor2;
}