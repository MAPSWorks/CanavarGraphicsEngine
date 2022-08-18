#version 430 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texture_coord;
layout (location = 3) in vec2 offset_position;

uniform mat4 node_matrix;

out vec3 cs_position;
out vec3 cs_normal;
out vec2 cs_texture_coord;

void main()
{
    cs_position = vec3(node_matrix * vec4(position, 1.0));
    cs_position.xz += offset_position;
    cs_normal = normal;
    cs_texture_coord = texture_coord;
}
