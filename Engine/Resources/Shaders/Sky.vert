#version 330 core
const vec3 VERTICES[4] = vec3[4](vec3(-1.0f, -1.0f, 1.0f),
                                 vec3(1.0f, -1.0f, 1.0f),
                                 vec3(-1.0f, 1.0f, 1.0f),
                                 vec3(1.0f, 1.0f, 1.0f));


uniform mat4 IVP; // Inverse view-projection matrix

out vec3 fsDirection;

void main()
{
    vec4 clipPos = vec4(VERTICES[gl_VertexID].xy, -1.0f, 1.0f);
    vec4 viewPos  = IVP * clipPos;
    fsDirection = normalize(viewPos.xyz);
    gl_Position = vec4(VERTICES[gl_VertexID].xy, -1.0f, 1.0f);
}
