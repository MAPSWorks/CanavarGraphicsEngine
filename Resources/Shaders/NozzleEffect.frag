#version 430 core
in vec2 fsTextureCoord;

uniform sampler2DMS firstPassTexture;
uniform float strength;
uniform int blurFactor;
uniform vec4 color;

out vec4 outColor;

vec4 applyBlur(ivec2 windowCoords, int sampleNum) {
    vec4 c0 = 3 *texelFetch(firstPassTexture, windowCoords + blurFactor * ivec2(1,1), sampleNum);
    vec4 c1 = 3 * texelFetch(firstPassTexture, windowCoords + blurFactor * ivec2(-1,-1), sampleNum);
    vec4 c2 = 3 * texelFetch(firstPassTexture, windowCoords + blurFactor * ivec2(-1,1), sampleNum);
    vec4 c3 = 3 * texelFetch(firstPassTexture, windowCoords + blurFactor * ivec2(1,-1), sampleNum);

    vec4 n0 = 3 * texelFetch(firstPassTexture, windowCoords + blurFactor * ivec2(0,1), sampleNum);
    vec4 n1 = 3 * texelFetch(firstPassTexture, windowCoords + blurFactor * ivec2(0,-1), sampleNum);
    vec4 n2 = 3 * texelFetch(firstPassTexture, windowCoords + blurFactor * ivec2(-1,0), sampleNum);
    vec4 n3 = 3 * texelFetch(firstPassTexture, windowCoords + blurFactor * ivec2(1,0), sampleNum);

    vec4 p = 0 * texelFetch(firstPassTexture, windowCoords, sampleNum);

    return (c0 + c1 + c2 + c3 + n0 + n1 + n2 + n3 + p) / 24.0f;
}

void main()
{
    ivec2 fragCoord = ivec2(int(gl_FragCoord.x), int(gl_FragCoord.y));

    vec4 sample1 = applyBlur(fragCoord, 0);
    vec4 sample2 = applyBlur(fragCoord, 1);
    vec4 sample3 = applyBlur(fragCoord, 2);
    vec4 sample4 = applyBlur(fragCoord, 3);

    outColor = color * vec4(sample1 + sample2 + sample3 + sample4) / 4.0f;
}
