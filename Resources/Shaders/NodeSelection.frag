#version 330 core

uniform int nodeIndex;

out uvec3 outColor;

void main()
{
    outColor = uvec3(nodeIndex, gl_PrimitiveID, 0);
}
