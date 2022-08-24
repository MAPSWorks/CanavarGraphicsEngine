#version 430 core
in vec2 fsTextureCoords;

uniform sampler2DMS onlySkyTexture;
uniform sampler2D skyAndCloudsTexture;

out vec4 outColor;

vec4 getColor(sampler2DMS texture, ivec2 coords) {

    vec4 sample1 = texelFetch(texture, coords, 0);
    vec4 sample2 = texelFetch(texture, coords, 1);
    vec4 sample3 = texelFetch(texture, coords, 2);
    vec4 sample4 = texelFetch(texture, coords, 3);

   return vec4(sample1 + sample2 + sample3 + sample4) / 4.0f;
}

void main()
{
    outColor = texture(skyAndCloudsTexture, fsTextureCoords);
}
