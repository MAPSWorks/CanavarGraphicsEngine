#version 330 core
layout(location = 0) in vec3 position;

out vec3 fsTextureCoords;
uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;

void main()
{
    fsTextureCoords = position;
    vec4 pos = projectionMatrix * viewMatrix * vec4(position, 1.0);
    gl_Position = pos.xyww; // w / w = 1 -> for faking depth buffer
}
