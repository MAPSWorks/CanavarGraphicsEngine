#version 430 core
in vec2 fsTextureCoords;

uniform sampler2D depthTexture;
uniform sampler2DMS colorTexture;
uniform mat4 inverseVP;
uniform mat4 previousVP;
uniform int width;
uniform int height;
uniform int samples;

out vec4 outColor;

vec4 getColor(ivec2 coords)
{
    vec4 color = vec4(0);

    for (int i = 0; i < 4; i++)
    {
        color += texelFetch(colorTexture, coords, i);
    }

    return color / 4.0f;
}

vec4 getSample(vec2 center, vec2 coords)
{
    if (coords.x >= 1 || coords.x <= 0 || coords.y >= 1 || coords.y <= 0)
        return getColor(ivec2(width * center.x, height * center.y));

    return getColor(ivec2(width * coords.x, height * coords.y));
}

vec2 getDeltaPosition()
{
    // Get the depth buffer value at this pixel.
    float zOverW = texture(depthTexture, fsTextureCoords).w;

    // H is the viewport position at this pixel in the range -1 to 1.
    vec4 H = vec4(fsTextureCoords.x * 2 - 1, (1 - fsTextureCoords.y) * 2 - 1, zOverW, 1);

    // Current viewport position
    vec4 currentPos = H;

    // Transform by the view-projection inverse.
    vec4 D = inverseVP * H;

    // Divide by w to get the world position.
    vec4 worldPos = D / D.w;

    // Use the world position, and transform by the previous view-projection matrix.
    vec4 previousPos = previousVP * worldPos;

    // Convert to nonhomogeneous points [-1,1] by dividing by w.
    previousPos /= previousPos.w;

    return (previousPos - currentPos).xy;
}

void main()
{
    vec2 coords = fsTextureCoords;
    vec2 deltaPosition = getDeltaPosition();
    vec2 step = deltaPosition / samples;

    vec4 color = vec4(0);

    for (int i = 0; i < samples; i++)
    {
        color += getSample(fsTextureCoords, coords + i * step);
    }

    outColor = color / samples;
}
