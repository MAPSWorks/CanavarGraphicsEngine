#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;

out vec3 fsPosition;
out vec3 fsNormal;

struct Node {
    mat4 transformation;
    vec4 color;
    float ambient;
    float diffuse;
    float specular;
    float shininess;
};

uniform Node node;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

void main()
{
    mat3 normalMatrix = mat3(transpose(inverse(node.transformation)));
    fsPosition = vec3(node.transformation * vec4(position, 1.0));
    fsNormal =  normalMatrix * normal;

    gl_Position = projectionMatrix * viewMatrix * vec4(fsPosition, 1.0);
}
