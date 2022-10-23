#version 330 core

uniform int nodeID;
uniform int meshID;

out vec3 output;

void main()
{
    output = vec3(nodeID, meshID, 0);
}
