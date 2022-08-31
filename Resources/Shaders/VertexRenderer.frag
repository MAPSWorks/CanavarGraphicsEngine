#version 430 core

flat in int fsVertexIndex;

uniform int selectedVertex;

out vec4 outColor;

void main()
{
    if (fsVertexIndex == selectedVertex)
        outColor = vec4(0, 1, 0, 1);
    else
        outColor = vec4(1, 1, 1, 1);
}
