#version 330 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 instancePosition;
layout(location = 2) in vec3 instanceNormal;

uniform mat4 MVP;
uniform mat4 vertexModelTransformation;

flat out int fsVertexIndex;

void main()
{
    vec3 newPosition = vec3(vertexModelTransformation * vec4(position, 1));
    gl_Position = MVP * vec4(0.0f * instanceNormal + instancePosition + newPosition, 1);
    fsVertexIndex = gl_InstanceID;
}
