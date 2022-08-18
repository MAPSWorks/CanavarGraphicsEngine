#version 330 core
layout (location = 0) in vec3 position;

uniform mat4 nodeMatrix;
uniform mat4 viewMatrix;

out vec3 gsPosition;

void main()
{
    gsPosition = vec3(viewMatrix * nodeMatrix * vec4(position, 1.0));
}
