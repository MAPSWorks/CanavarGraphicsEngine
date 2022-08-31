#version 330 core

uniform int nodeIndex;
uniform int meshIndex;

out uvec4 outColor;

void main()
{
    outColor = uvec4(nodeIndex, meshIndex, gl_PrimitiveID, 0);
}
