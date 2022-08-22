#version 430 core
in vec2 fsTextureCoords;

uniform sampler2D depthTexture;
uniform sampler2DMS colorTexture;
uniform mat4 inverseVP;
uniform mat4 previousVP;
uniform int width;
uniform int height;

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

vec4 getSample(ivec2 coords, int x, int y)
{
    if(coords.x + x >= width || coords.x + x <= 0 || coords.y + y >= height || coords.y + y <= 0)
        return getColor(coords);

    return getColor(coords + ivec2(x, y));
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
    int x = int(fsTextureCoords.x * width);
    int y = int(fsTextureCoords.y * height);
    ivec2 coords = ivec2(x, y);

    vec2 deltaPos = getDeltaPosition();
    int dx = int(width * deltaPos.x);
    int dy = int(height * deltaPos.y);

    int p = dx / abs(dx);
    int q = dy / abs(dy);
    int n = clamp(abs(dx), 1, 8);
    int m = clamp(abs(dy), 1, 8);

    vec4 totalColor = vec4(0);
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < m; j++)
        {
            totalColor += getSample(coords, p * i, q * j);
        }
    }

    outColor = totalColor / (n * m);
}
