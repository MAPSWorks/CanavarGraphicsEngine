#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 textureCoord;
layout (location = 3) in vec3 tangent;
layout (location = 4) in vec3 bitangent;
layout (location = 5) in int[4] ids;
layout (location = 6) in float[4] weights;


uniform float nodeShininess;
uniform mat4 nodeMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

out vec3 fsPosition;
out vec3 fsNormal;
out vec2 fsTextureCoord;

void main()
{
    mat3 normalMatrix = mat3(transpose(inverse(nodeMatrix)));
    fsPosition = vec3(nodeMatrix * vec4(position, 1.0));
    fsNormal = normalMatrix * normal;
    fsTextureCoord = textureCoord;

    gl_Position = projectionMatrix * viewMatrix * vec4(fsPosition, 1.0);
}
