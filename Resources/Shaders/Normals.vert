#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;

uniform mat4 nodeMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

out vec3 gsNormal;

void main()
{
    gl_Position = viewMatrix * nodeMatrix * vec4(position, 1.0);
    mat3 normalMatrix = mat3(transpose(inverse(viewMatrix * nodeMatrix)));
    gsNormal = normalMatrix * normal;
}
