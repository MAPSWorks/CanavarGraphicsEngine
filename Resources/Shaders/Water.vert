#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 textureCoords;

uniform mat4 nodeMatrix;
uniform mat4 VP;

out vec3 fsNormal;
out vec4 fsClipSpaceCoords;
out vec2 fsTextureCoords;
out vec4 fsPosition;

void main()
{
    fsTextureCoords = textureCoords;
    fsNormal = normal;
    fsPosition = nodeMatrix * vec4(position, 1.0);
    fsClipSpaceCoords = VP * fsPosition;
    gl_Position = fsClipSpaceCoords;
}
