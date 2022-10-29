#version 330 core

uniform int nodeID;
uniform int meshID;

flat in int fsVertexID; // Do not interpolate

layout (location = 0) out uvec4 IDs;

void main()
{
     IDs = uvec4(nodeID, meshID, fsVertexID, 1);
}
