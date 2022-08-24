#version 430 core
in vec2 fsTextureCoords;

uniform sampler2D skyTexture;
uniform int width;
uniform int height;

out vec4 outColor;

void main()
{
    float offset_x = 1.0f / float(width);
    float offset_y = 1.0f / float(height);

    vec4 c0 = 1 * texture(skyTexture, fsTextureCoords + vec2(offset_x, offset_y));
    vec4 c1 = 1 * texture(skyTexture, fsTextureCoords + vec2(-offset_x, -offset_y));
    vec4 c2 = 1 * texture(skyTexture, fsTextureCoords + vec2(-offset_x, offset_y));
    vec4 c3 = 1 * texture(skyTexture, fsTextureCoords + vec2(offset_x, -offset_y));

    vec4 n0 = 2 * texture(skyTexture, fsTextureCoords + vec2(0, offset_y));
    vec4 n1 = 2 * texture(skyTexture, fsTextureCoords + vec2(0, -offset_y));
    vec4 n2 = 2 * texture(skyTexture, fsTextureCoords + vec2(-offset_x, 0));
    vec4 n3 = 2 * texture(skyTexture, fsTextureCoords + vec2(offset_x, 0));

    vec4 p = 4 * texture(skyTexture, fsTextureCoords);

    outColor = (c0 + c1 + c2 + c3 + n0 + n1 + n2 + n3 + p) / 16.0f;
}
