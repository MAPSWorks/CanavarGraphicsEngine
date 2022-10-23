#version 330 core

flat in int fsVertexIndex;

out vec3 outVertexIndex;

void main()
{
    outVertexIndex = vec3(fsVertexIndex, 0, 1); // Vertex index
}
