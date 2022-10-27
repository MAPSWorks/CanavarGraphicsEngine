#version 330 core

in vec3 fsPosition;
in float fsLife;
in float fsDeadAfter;

layout (location = 0) out vec4 fragColor;
layout (location = 1) out vec4 brightColor;

void main()
{
    fragColor = vec4(1);
    brightColor = vec4(1);
}
