#version 430 core
in vec2 fsTextureCoord;

uniform sampler2DMS screenTexture;

out vec4 outColor;

vec4 applyBlur1(ivec2 windowCoords, int sampleNum)
{
    vec4 c0 = 1 * texelFetch(screenTexture, windowCoords + 1 * ivec2(1, 1), sampleNum);
    vec4 c1 = 1 * texelFetch(screenTexture, windowCoords + 1 * ivec2(-1, -1), sampleNum);
    vec4 c2 = 1 * texelFetch(screenTexture, windowCoords + 1 * ivec2(-1, 1), sampleNum);
    vec4 c3 = 1 * texelFetch(screenTexture, windowCoords + 1 * ivec2(1, -1), sampleNum);

    vec4 n0 = 2 * texelFetch(screenTexture, windowCoords + 1 * ivec2(0, 1), sampleNum);
    vec4 n1 = 2 * texelFetch(screenTexture, windowCoords + 1 * ivec2(0, -1), sampleNum);
    vec4 n2 = 2 * texelFetch(screenTexture, windowCoords + 1 * ivec2(-1, 0), sampleNum);
    vec4 n3 = 2 * texelFetch(screenTexture, windowCoords + 1 * ivec2(1, 0), sampleNum);

    vec4 d0 = 4 * texelFetch(screenTexture, windowCoords + 2 * ivec2(1, 1), sampleNum);
    vec4 d1 = 4 * texelFetch(screenTexture, windowCoords + 2 * ivec2(-1, -1), sampleNum);
    vec4 d2 = 4 * texelFetch(screenTexture, windowCoords + 2 * ivec2(-1, 1), sampleNum);
    vec4 d3 = 4 * texelFetch(screenTexture, windowCoords + 2 * ivec2(1, -1), sampleNum);

    vec4 m0 = 2 * texelFetch(screenTexture, windowCoords + 2 * ivec2(0, 1), sampleNum);
    vec4 m1 = 2 * texelFetch(screenTexture, windowCoords + 2 * ivec2(0, -1), sampleNum);
    vec4 m2 = 2 * texelFetch(screenTexture, windowCoords + 2 * ivec2(-1, 0), sampleNum);
    vec4 m3 = 2 * texelFetch(screenTexture, windowCoords + 2 * ivec2(1, 0), sampleNum);

    return (c0 + c1 + c2 + c3 + n0 + n1 + n2 + n3 + d0 + d1 + d2 + d3 + m0 + m1 + m2 + m3) / 36.0f;
}

vec4 applyBlur2(ivec2 windowCoords, int sampleNum)
{
    vec4 c0 = 1 * texelFetch(screenTexture, windowCoords + 1 * ivec2(1, 1), sampleNum);
    vec4 c1 = 1 * texelFetch(screenTexture, windowCoords + 1 * ivec2(-1, -1), sampleNum);
    vec4 c2 = 1 * texelFetch(screenTexture, windowCoords + 1 * ivec2(-1, 1), sampleNum);
    vec4 c3 = 1 * texelFetch(screenTexture, windowCoords + 1 * ivec2(1, -1), sampleNum);

    vec4 n0 = 2 * texelFetch(screenTexture, windowCoords + 1 * ivec2(0, 1), sampleNum);
    vec4 n1 = 2 * texelFetch(screenTexture, windowCoords + 1 * ivec2(0, -1), sampleNum);
    vec4 n2 = 2 * texelFetch(screenTexture, windowCoords + 1 * ivec2(-1, 0), sampleNum);
    vec4 n3 = 2 * texelFetch(screenTexture, windowCoords + 1 * ivec2(1, 0), sampleNum);

    vec4 p = 4 * texelFetch(screenTexture, windowCoords, sampleNum);

    return (c0 + c1 + c2 + c3 + n0 + n1 + n2 + n3 + p) / 16.0f;
}

const float weight[5] = float[](0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

vec4 applyHorizontalLinearBlur(ivec2 windowCoords, int sampleNum)
{
    vec4 result = weight[0] * texelFetch(screenTexture, windowCoords, sampleNum);

    for (int i = 1; i < 5; i++)
    {
        result += weight[i] * texelFetch(screenTexture, windowCoords + i * ivec2(1, 0), sampleNum);
        result += weight[i] * texelFetch(screenTexture, windowCoords - i * ivec2(1, 0), sampleNum);
    }

    return result;
}

vec4 applyVerticalLinearBlur(ivec2 windowCoords, int sampleNum)
{
    vec4 result = weight[0] * texelFetch(screenTexture, windowCoords, sampleNum);

    for (int i = 1; i < 5; i++)
    {
        result += weight[i] * texelFetch(screenTexture, windowCoords + i * ivec2(0, 1), sampleNum);
        result += weight[i] * texelFetch(screenTexture, windowCoords - i * ivec2(0, 1), sampleNum);
    }

    return result;
}

void main()
{
    ivec2 coords = ivec2(gl_FragCoord.xy);
    vec4 sample1 = applyBlur2(coords, 0);
    vec4 sample2 = applyBlur2(coords, 1);
    vec4 sample3 = applyBlur2(coords, 2);
    vec4 sample4 = applyBlur2(coords, 3);
    outColor = vec4(sample1 + sample2 + sample3 + sample4) / 4.0f;

    //    if(applyHorizontalBlur)
    //    {
    //        vec4 sample1 = applyHorizontalLinearBlur(fragCoord, 0);
    //        vec4 sample2 = applyHorizontalLinearBlur(fragCoord, 1);
    //        vec4 sample3 = applyHorizontalLinearBlur(fragCoord, 2);
    //        vec4 sample4 = applyHorizontalLinearBlur(fragCoord, 3);
    //        outColor = vec4(sample1 + sample2 + sample3 + sample4) / 4.0f;
    //    }
    //    else
    //    {
    //        vec4 sample1 = applyVerticalLinearBlur(fragCoord, 0);
    //        vec4 sample2 = applyVerticalLinearBlur(fragCoord, 1);
    //        vec4 sample3 = applyVerticalLinearBlur(fragCoord, 2);
    //        vec4 sample4 = applyVerticalLinearBlur(fragCoord, 3);
    //        outColor = vec4(sample1 + sample2 + sample3 + sample4) / 4.0f;
    //    }
}
