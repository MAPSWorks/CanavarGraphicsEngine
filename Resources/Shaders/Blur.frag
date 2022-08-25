#version 430 core
in vec2 fsTextureCoords;

uniform sampler2DMS screenTexture;
uniform bool horizontal;

out vec4 outColor;

uniform float weight[5] = float[](0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

vec4 getColor(ivec2 coords)
{
    vec4 color = vec4(0);

    for (int i = 0; i < 4; i++)
    {
        color += texelFetch(screenTexture, coords, i);
    }

    return color / 4.0f;
}

void main()
{
    ivec2 coords = ivec2(gl_FragCoord.xy);
    vec4 totalColor = getColor(coords) * weight[0];
    if (horizontal)
    {
        for (int i = 1; i < 5; i++)
        {
            totalColor += getColor(coords + ivec2(i, 0)) * weight[i];
            totalColor += getColor(coords - ivec2(i, 0)) * weight[i];
        }

        outColor = totalColor;
    } else
    {
        for (int i = 1; i < 5; i++)
        {
            totalColor += getColor(coords + ivec2(0, i)) * weight[i];
            totalColor += getColor(coords - ivec2(0, i)) * weight[i];
        }

        outColor = totalColor;
    }

}
