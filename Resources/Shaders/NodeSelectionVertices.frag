#version 330 core
flat in int fsVertexIndex;

out uvec3 outColor;

void main()
{
    outColor = uvec3(fsVertexIndex, 0, 1); // Vertex index
}
