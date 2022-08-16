#version 330 core
layout (triangles) in;
layout (line_strip, max_vertices = 6) out;

in vec3 gs_normal[];

uniform mat4 projection_matrix;

void generate_line(int index)
{
    gl_Position = projection_matrix * gl_in[index].gl_Position;
    EmitVertex();
    gl_Position = projection_matrix * (gl_in[index].gl_Position + 0.25 * vec4(gs_normal[index], 0.0));
    EmitVertex();
    EndPrimitive();
}

void main()
{
    generate_line(0);
    generate_line(1);
    generate_line(2);
}
