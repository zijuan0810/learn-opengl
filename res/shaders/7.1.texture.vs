#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

uniform mat4 _Model;
uniform mat4 _View;
uniform mat4 _Proj;

// out vec3 ourColor;
out vec2 texCoord;

void main()
{
    gl_Position = _Proj * _View * _Model * vec4(aPos, 1.0);
    // ourColor = aColor;
    texCoord = aTexCoord;
}