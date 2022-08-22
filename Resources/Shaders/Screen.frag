#version 430 core
in vec2 fsTextureCoords;

uniform sampler2DMS screenTexture;
uniform int width;
uniform int height;

out vec4 outColor;

void main()
{
    // texelFetch requires a vec of ints for indexing (since we're indexing pixel locations).
    // fsTextureCoord is in range [0, 1], we need to map it to [0, windowWidth].

    ivec2 windowCoords = ivec2(width, height);
    windowCoords.x = int(windowCoords.x * fsTextureCoords.x);
    windowCoords.y = int(windowCoords.y * fsTextureCoords.y);

    vec4 sample1 = texelFetch(screenTexture, windowCoords, 0);
    vec4 sample2 = texelFetch(screenTexture, windowCoords, 1);
    vec4 sample3 = texelFetch(screenTexture, windowCoords, 2);
    vec4 sample4 = texelFetch(screenTexture, windowCoords, 3);

    outColor = vec4(sample1 + sample2 + sample3 + sample4) / 4.0f;
}
