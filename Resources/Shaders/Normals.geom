#version 330 core
layout(triangles) in;
layout(line_strip, max_vertices = 6) out;

in vec3 gsNormal[];

uniform mat4 projectionMatrix;

void generateLine(int index)
{
    gl_Position = projectionMatrix * gl_in[index].gl_Position;
    EmitVertex();
    gl_Position = projectionMatrix * (gl_in[index].gl_Position + 0.25 * vec4(gsNormal[index], 0.0));
    EmitVertex();
    EndPrimitive();
}

void main()
{
    generateLine(0);
    generateLine(1);
    generateLine(2);
}
