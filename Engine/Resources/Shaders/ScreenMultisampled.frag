#version 430 core
in vec2 fsTextureCoords;

uniform sampler2DMS screenTexture;
uniform int samples;

out vec4 outColor;

void main()
{
    vec4 color = vec4(0);

    for(int i = 0; i < samples; i++)
        color += texelFetch(screenTexture, ivec2(gl_FragCoord.xy), i);

    outColor = color / float(samples);
}
