#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texture_coord;

uniform mat4 node_matrix;
uniform mat4 view_matrix;
uniform mat4 projection_matrix;

out vec3 fs_position;
out vec3 fs_normal;
out vec2 fs_texture_coord;

void main()
{
    fs_position = vec3(node_matrix * vec4(position, 1.0));
    fs_normal = mat3(transpose(inverse(node_matrix))) * normal;
    fs_texture_coord = texture_coord;

    gl_Position = projection_matrix * view_matrix * vec4(fs_position, 1.0);
}
