#version 330 core

in vec3 fsDirection;

out vec4 outColor;

void main()
{
    outColor = vec4(fsDirection, 1.0F);
}
