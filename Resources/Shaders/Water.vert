#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 textureCoord;
layout(location = 3) in vec2 offsetPosition;

uniform mat4 nodeMatrix;
uniform mat4 VP;

out vec3 fsNormal;
out vec4 fsClipSpaceCoords;
out vec2 fsTextureCoords;
out vec4 fsPosition;

void main()
{
    fsPosition = vec4(position, 1.0) + vec4(offsetPosition.x, 0, offsetPosition.y, 0);
    fsClipSpaceCoords = VP * nodeMatrix * fsPosition;

    fsTextureCoords = textureCoord;
    fsNormal = normal;

    gl_Position = fsClipSpaceCoords;
}
