#version 430 core
in vec2 fsTextureCoords;

uniform sampler2D depthTexture;
uniform sampler2DMS screenTexture;

uniform mat4 inverseViewProjectionMatrix;
uniform mat4 previousViewProjectionMatrix;
uniform int width;
uniform int height;
uniform float strength;
uniform int samples;

out vec4 outColor;

vec4 getColor(ivec2 coords) {
    vec4 color = vec4(0);

    for(int i = 0; i < 4; i++) {
         color += texelFetch(screenTexture, coords, i);
    }

    return color / 4.0f;
}

void main()
{
    // Get the depth buffer value at this pixel.
    float zOverW = texture(depthTexture, fsTextureCoords).w;

    // H is the viewport position at this pixel in the range -1 to 1.
    vec4 H = vec4(fsTextureCoords.x * 2 - 1, (1 - fsTextureCoords.y) * 2 - 1, zOverW, 1);

    // Transform by the view-projection inverse.
    vec4 D = inverseViewProjectionMatrix * H;

    // Divide by w to get the world position.
    //vec4 worldPos = D / D.w;
    vec4 worldPos = D;

    // Current viewport position
    vec4 currentPos = H;

    // Use the world position, and transform by the previous view-projection matrix.
    vec4 previousPos = previousViewProjectionMatrix * worldPos;

    // Convert to nonhomogeneous points [-1,1] by dividing by w.
    //previousPos /= previousPos.w;

    // Use this frame's position and last frame's to compute the pixel velocity.
    int x = int(width * (previousPos - currentPos).x * strength);
    int y = int(height * (previousPos - currentPos).y * strength);
    ivec2 velocity = ivec2(x,y);

    ivec2 coords = ivec2(0, 0);
    coords.x = int(width * fsTextureCoords.x);
    coords.y = int(height * fsTextureCoords.y);

    // Get the initial color at this pixel.
    vec4 initialColor =  getColor(coords);

    coords = coords + velocity;
    vec4 color = vec4(0);
    for(int i = 1; i < samples; ++i, coords += velocity) {
        // Sample the color buffer along the velocity vector.
        if(coords.x > width || coords.y > height || coords.x < 0 || coords.y < 0){
            outColor = initialColor;
            return;
        }
        // Add the current color to our color sum.
        color += getColor(coords);
    } // Average all of the samples to get the final blur color.

    outColor = (initialColor + color) / samples;
}
