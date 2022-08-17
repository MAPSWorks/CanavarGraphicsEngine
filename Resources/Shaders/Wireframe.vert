#version 330 core
layout (location = 0) in vec3 position;

uniform mat4 node_matrix;
uniform mat4 view_matrix;

out vec3 gs_position;

void main()
{
    gs_position = vec3(view_matrix * node_matrix * vec4(position, 1.0));
}
