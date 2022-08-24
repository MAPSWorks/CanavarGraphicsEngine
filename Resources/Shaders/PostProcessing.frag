#version 330 core
in vec2 fsTextureCoords;

uniform sampler2D cloudTex;

out vec4 outColor;

vec3 TonemapACES(vec3 x)
{
    const float A = 2.51f;
    const float B = 0.03f;
    const float C = 2.43f;
    const float D = 0.59f;
    const float E = 0.14f;
    return (x * (A * x + B)) / (x * (C * x + D) + E);
}


void main()
{
    outColor = texture(cloudTex, fsTextureCoords);
}
