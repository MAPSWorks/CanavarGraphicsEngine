#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;

uniform mat4 node_matrix;
uniform mat4 view_matrix;
uniform mat4 projection_matrix;

out vec3 gs_normal;

void main()
{
    gl_Position = view_matrix * node_matrix * vec4(position, 1.0);
    gs_normal = mat3(transpose(inverse(view_matrix * node_matrix))) * normal;
}
