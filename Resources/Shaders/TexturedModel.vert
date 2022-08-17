#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texture_coord;
layout (location = 3) in vec3 tangent;
layout (location = 4) in vec3 bitangent;
layout (location = 5) in int[4] ids;
layout (location = 6) in float[4] weights;


uniform float node_shininess;
uniform mat4 node_transformation;
uniform mat4 view_matrix;
uniform mat4 projection_matrix;

out vec3 fs_position;
out vec3 fs_normal;
out vec2 fs_texture_coord;

void main()
{
    mat3 normal_matrix = mat3(transpose(inverse(node_transformation)));
    fs_position = vec3(node_transformation * vec4(position, 1.0));
    fs_normal = normal_matrix * normal;
    fs_texture_coord = texture_coord;

    gl_Position = projection_matrix * view_matrix * vec4(fs_position, 1.0);
}
