#version 330 core
layout (location = 0) in vec3 vertexPosition;
layout (location = 1) in vec3 initialPosition;
layout (location = 2) in vec3 velocity;
layout (location = 3) in float life;

uniform mat4 nodeMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

out float fsRadius;

void main()
{
    gl_Position = projectionMatrix * viewMatrix * nodeMatrix * vec4(life * velocity + initialPosition + vertexPosition, 1.0f);
    fsRadius = length(initialPosition);
}
