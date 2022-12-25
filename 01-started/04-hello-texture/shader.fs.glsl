#version 330 core

in vec3 ourColor;
in vec2 TexCoord;

uniform float glfw_time;
uniform sampler2D ourTexture;

vec4 blur(vec2 uv, sampler2D tex0) 
{
    float disp = 0.;
    float intensity = .2;
    const int passes = 6;
    vec4 c1 = vec4(0.0);
    disp = intensity*(0.5-distance(0.5, .1));
  
    for (int xi=0; xi<passes; xi++) {
        float x = float(xi) / float(passes) - 0.5;
        for (int yi=0; yi<passes; yi++) {
            float y = float(yi) / float(passes) - 0.5;
            vec2 v = vec2(x, y);
            float d = disp;
            c1 += texture2D(tex0, uv + d*v);
        }
    }
    c1 /= float(passes*passes);
    return c1;
}

void main()
{
	//FragColor = vec4(ourColor, 1.0f);
	vec2 offset = vec2(0, 0);
	offset.x = sin(glfw_time);
	offset.y = cos(glfw_time);
//	vec4 texColor = texture(ourTexture, TexCoord + offset);
	vec4 texColor = blur(TexCoord, ourTexture);
	vec4 baseColor = vec4(ourColor, 1.0f);
	gl_FragColor = baseColor * texColor;
}