#version 330 core

uniform int nodeID;
uniform int meshID;

flat in int fsVertexID; // Do not interpolate

layout (location = 0) out ivec4 IDs;

void main()
{
     IDs = ivec4(nodeID, meshID, fsVertexID, 1);
}
