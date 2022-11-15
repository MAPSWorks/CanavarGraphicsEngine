#version 330 core
layout(location = 0) in vec3 point;

uniform mat4 VP;

void main()
{
    gl_Position = VP * vec4(point, 1);
}
