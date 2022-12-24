#version 330 core

out vec4 FragColor;

// in vec3 ourColor;
in vec2 texCoord;

uniform sampler2D _Texture1;
uniform sampler2D _Texture2;

void main()
{
    // FragColor = vec4(ourColor, 1.0);
    // FragColor = texture(_Texture1, texCoord);
    // FragColor = texture(_Texture2, texCoord);
    // FragColor = texture(_Texture1, texCoord) * vec4(ourColor, 1.0);

    // linearly interpolate between both textures (80% container, 20% awesomeface)
	FragColor = mix(texture(_Texture1, texCoord), texture(_Texture2, texCoord), 0.2);
}