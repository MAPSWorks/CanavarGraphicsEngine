#version 330 core
layout (triangles) in;
layout (line_strip, max_vertices = 6) out;

in vec3 gs_position[];

uniform mat4 projection_matrix;

void main()
{
    gl_Position = projection_matrix * vec4(gs_position[0], 1.0);
    EmitVertex();
    gl_Position = projection_matrix * vec4(gs_position[1], 1.0);
    EmitVertex();
    gl_Position = projection_matrix * vec4(gs_position[2], 1.0);
    EmitVertex();
    gl_Position = projection_matrix * vec4(gs_position[0], 1.0);
    EmitVertex();
    EndPrimitive();
}
