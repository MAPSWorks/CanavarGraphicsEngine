#version 330 core
layout (triangles) in;
layout (line_strip, max_vertices = 6) out;

in vec3 gsPosition[];

uniform mat4 projectionMatrix;

void main()
{
    gl_Position = projectionMatrix * vec4(gsPosition[0], 1.0);
    EmitVertex();
    gl_Position = projectionMatrix * vec4(gsPosition[1], 1.0);
    EmitVertex();
    gl_Position = projectionMatrix * vec4(gsPosition[2], 1.0);
    EmitVertex();
    gl_Position = projectionMatrix * vec4(gsPosition[0], 1.0);
    EmitVertex();
    EndPrimitive();
}
