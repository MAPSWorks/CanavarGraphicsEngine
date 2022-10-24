#version 330 core
layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 initialPosition;
layout(location = 2) in vec3 velocityDirection;
layout(location = 3) in float life;

uniform mat4 MVP;
uniform mat4 scaling;
uniform float velocity;

out float fsRadius;
out float fsDistance;

void main()
{
    vec3 position = vec3(scaling * vec4(vertexPosition, 1.0f));
    position = life * velocity * velocityDirection + initialPosition + position;
    gl_Position = MVP * vec4(position, 1.0f);
    fsRadius = length(initialPosition);
    fsDistance = position.z;
}
