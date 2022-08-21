#version 430 core
in vec2 fsTextureCoord;

uniform sampler2DMS firstPassTexture;

out vec4 outColor;

vec4 applyBlur1(ivec2 windowCoords, int sampleNum) {

    vec4 c0 = 1 * texelFetch(firstPassTexture, windowCoords + 1 * ivec2(1,1), sampleNum);
    vec4 c1 = 1 * texelFetch(firstPassTexture, windowCoords + 1 * ivec2(-1,-1), sampleNum);
    vec4 c2 = 1 * texelFetch(firstPassTexture, windowCoords + 1 * ivec2(-1,1), sampleNum);
    vec4 c3 = 1 * texelFetch(firstPassTexture, windowCoords + 1 * ivec2(1,-1), sampleNum);

    vec4 n0 = 2 * texelFetch(firstPassTexture, windowCoords + 1 * ivec2(0,1), sampleNum);
    vec4 n1 = 2 * texelFetch(firstPassTexture, windowCoords + 1 * ivec2(0,-1), sampleNum);
    vec4 n2 = 2 * texelFetch(firstPassTexture, windowCoords + 1 * ivec2(-1,0), sampleNum);
    vec4 n3 = 2 * texelFetch(firstPassTexture, windowCoords + 1 * ivec2(1,0), sampleNum);

    vec4 d0 = 4 * texelFetch(firstPassTexture, windowCoords + 2 * ivec2(1,1), sampleNum);
    vec4 d1 = 4 * texelFetch(firstPassTexture, windowCoords + 2 * ivec2(-1,-1), sampleNum);
    vec4 d2 = 4 * texelFetch(firstPassTexture, windowCoords + 2 * ivec2(-1,1), sampleNum);
    vec4 d3 = 4 * texelFetch(firstPassTexture, windowCoords + 2 * ivec2(1,-1), sampleNum);

    vec4 m0 = 2 * texelFetch(firstPassTexture, windowCoords + 2 * ivec2(0,1), sampleNum);
    vec4 m1 = 2 * texelFetch(firstPassTexture, windowCoords + 2 * ivec2(0,-1), sampleNum);
    vec4 m2 = 2 * texelFetch(firstPassTexture, windowCoords + 2 * ivec2(-1,0), sampleNum);
    vec4 m3 = 2 * texelFetch(firstPassTexture, windowCoords + 2 * ivec2(1,0), sampleNum);


    return (c0 + c1 + c2 + c3 + n0 + n1 + n2 + n3 + d0 + d1 + d2 + d3 + m0 + m1 + m2 + m3) / 36.0f;
}

vec4 applyBlur2(ivec2 windowCoords, int sampleNum) {

    vec4 c0 = 1 * texelFetch(firstPassTexture, windowCoords + 1 * ivec2(1,1), sampleNum);
    vec4 c1 = 1 * texelFetch(firstPassTexture, windowCoords + 1 * ivec2(-1,-1), sampleNum);
    vec4 c2 = 1 * texelFetch(firstPassTexture, windowCoords + 1 * ivec2(-1,1), sampleNum);
    vec4 c3 = 1 * texelFetch(firstPassTexture, windowCoords + 1 * ivec2(1,-1), sampleNum);

    vec4 n0 = 2 * texelFetch(firstPassTexture, windowCoords + 1 * ivec2(0,1), sampleNum);
    vec4 n1 = 2 * texelFetch(firstPassTexture, windowCoords + 1 * ivec2(0,-1), sampleNum);
    vec4 n2 = 2 * texelFetch(firstPassTexture, windowCoords + 1 * ivec2(-1,0), sampleNum);
    vec4 n3 = 2 * texelFetch(firstPassTexture, windowCoords + 1 * ivec2(1,0), sampleNum);

    vec4 p = 4 * texelFetch(firstPassTexture, windowCoords, sampleNum);

    return (c0 + c1 + c2 + c3 + n0 + n1 + n2 + n3 + p) / 16.0f;
}


void main()
{
    ivec2 fragCoord = ivec2(int(gl_FragCoord.x), int(gl_FragCoord.y));

    vec4 sample1 = applyBlur1(fragCoord, 0);
    vec4 sample2 = applyBlur1(fragCoord, 1);
    vec4 sample3 = applyBlur1(fragCoord, 2);
    vec4 sample4 = applyBlur1(fragCoord, 3);

    outColor = vec4(sample1 + sample2 + sample3 + sample4) / 4.0f;
}
