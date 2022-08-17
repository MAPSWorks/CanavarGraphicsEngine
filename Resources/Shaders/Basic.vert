#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;

out vec3 fs_position;
out vec3 fs_normal;

struct Node {
    mat4 transformation;
    vec4 color;
    float ambient;
    float diffuse;
    float specular;
    float shininess;
};

uniform Node node;
uniform mat4 view_matrix;
uniform mat4 projection_matrix;

void main()
{
    mat3 normal_matrix = mat3(transpose(inverse(node.transformation)));
    fs_position = vec3(node.transformation * vec4(position, 1.0));
    fs_normal =  normal_matrix * normal;

    gl_Position = projection_matrix * view_matrix * vec4(fs_position, 1.0);
}
