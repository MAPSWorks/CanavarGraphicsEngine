#version 330 core
layout (location = 0) in vec3 position;

out vec3 fs_texture_coords;
uniform mat4 projection_matrix;
uniform mat4 view_matrix;

void main()
{
    fs_texture_coords = position;
    vec4 pos = projection_matrix * view_matrix * vec4(position, 1.0);
    gl_Position = pos.xyww; // w / w = 1 -> for faking depth buffer
}
