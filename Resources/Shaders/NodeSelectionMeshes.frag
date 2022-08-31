#version 330 core
uniform int nodeIndex;
uniform int meshIndex;

out uvec3 outColor;

void main()
{
    outColor = uvec3(nodeIndex, meshIndex, 0);
}
