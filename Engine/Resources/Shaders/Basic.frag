#version 330 core
layout(location = 0) out vec4 outColor;

uniform vec4 color = vec4(1);

void main()
{
    outColor = color;
}
