#version 330 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 textureCoords;
layout(location = 3) in vec3 tangent;
layout(location = 4) in vec3 bitangent;
layout(location = 5) in int[4] ids;
layout(location = 6) in float[4] weights;

struct Node {
    mat4 transformation;
    mat3 normalMatrix;
    vec4 color;
    float ambient;
    float diffuse;
    float specular;
    float shininess;
};

uniform Node node;
uniform mat4 VP;

uniform bool useTextureNormal;

out vec3 fsPosition;
out vec3 fsNormal;
out vec2 fsTextureCoords;
out mat3 fsTBN;

void main()
{
    fsPosition = vec3(node.transformation * vec4(position, 1.0));
    fsNormal = node.normalMatrix * normal;
    fsTextureCoords = textureCoords;

    if (useTextureNormal)
    {
        vec3 T = normalize(vec3(node.transformation * vec4(tangent, 0.0)));
        vec3 B = normalize(vec3(node.transformation * vec4(bitangent, 0.0)));
        vec3 N = normalize(vec3(node.transformation * vec4(normal, 0.0)));
        fsTBN = node.normalMatrix * mat3(T, B, N);
    }

    gl_Position = VP * vec4(fsPosition, 1.0);
}
