#version 330 core
layout(location = 0) in vec3 cubeVertexPosition;
layout(location = 1) in vec3 vertexPosition;
layout(location = 2) in vec3 vertexNormal;

uniform mat4 MVP;
uniform mat4 VMT; // Vertex Model Transformation

flat out int fsVertexIndex;

void main()
{
    gl_Position = MVP * vec4(vertexPosition + vec3(VMT * vec4(cubeVertexPosition, 1)), 1);
    fsVertexIndex = gl_InstanceID;
}
