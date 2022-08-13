#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 textureCoord;
layout (location = 3) in vec3 tangent;
layout (location = 4) in vec3 bitangent;
layout (location = 5) in int[4] ids;
layout (location = 6) in float[4] weights;

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
    fs_position = vec3(node.transformation * vec4(position, 1.0));
    fs_normal = mat3(transpose(inverse(node.transformation))) * normal;

    gl_Position = projection_matrix * view_matrix * vec4(fs_position, 1.0);
}
