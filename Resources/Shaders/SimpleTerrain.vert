#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texture_coord;

struct TileProperties {
    int cellCount;
    float offset;
    float amplitude;
    int octaves;
    float roughness;
    float ambient;
    float diffuse;
    float shininess;
    float specular;
};

uniform TileProperties properties;
uniform mat4 node_matrix;
uniform mat4 view_matrix;
uniform mat4 projection_matrix;

out vec3 fs_position;
out vec3 fs_normal;
out vec2 fs_texture_coord;

/* Function to linearly interpolate between a0 and a1
 * Weight w should be in the range [0.0, 1.0]
 */
float interpolate(float a0, float a1, float w) {
    /* // You may want clamping by inserting:
     * if (0.0 > w) return a0;
     * if (1.0 < w) return a1;
     */
    return (a1 - a0) * w + a0;
    /* // Use this cubic interpolation [[Smoothstep]] instead, for a smooth appearance:
     * return (a1 - a0) * (3.0 - w * 2.0) * w * w + a0;
     *
     * // Use [[Smootherstep]] for an even smoother result with a second derivative equal to zero on boundaries:
     * return (a1 - a0) * ((w * (w * 6.0 - 15.0) + 10.0) * w * w * w) + a0;
     */
}

vec2 randomGradient(int ix, int iy) {
    // No precomputed gradients mean this works for any number of grid coordinates
    int w = 8 * 4;
    int s = w / 2; // rotation width
    int a = ix;
    int b = iy;
    a *= 328415743; b ^= a << s | a >> w-s;
    b *= 191152077; a ^= b << s | b >> w-s;
    a *= 204841925;
    float random = a * (3.1415965f / ~(~0u >> 1)); // in [0, 2*Pi]

    return vec2(cos(random), sin(random));
}

// Computes the dot product of the distance and gradient vectors.
float dotGridGradient(int ix, int iy, float x, float y) {
    // Get gradient from integer coordinates
    vec2 gradient = randomGradient(ix, iy);

    // Compute the distance vector
    float dx = x - float(ix);
    float dy = y - float(iy);

    // Compute the dot-product
    return (dx*gradient.x + dy*gradient.y);
}

// Compute Perlin noise at coordinates x, y
float perlin(float x, float y) {
    // Determine grid cell coordinates
    int x0 = int(floor(x));
    int x1 = x0 + 1;
    int y0 = int(floor(y));
    int y1 = y0 + 1;

    // Determine interpolation weights
    // Could also use higher order polynomial/s-curve here
    float sx = x - float(x0);
    float sy = y - float(y0);

    // Interpolate between grid point gradients
    float n0, n1, ix0, ix1, value;

    n0 = dotGridGradient(x0, y0, x, y);
    n1 = dotGridGradient(x1, y0, x, y);
    ix0 = interpolate(n0, n1, sx);

    n0 = dotGridGradient(x0, y1, x, y);
    n1 = dotGridGradient(x1, y1, x, y);
    ix1 = interpolate(n0, n1, sx);

    value = interpolate(ix0, ix1, sy);
    return value; // Will return in range -1 to 1. To make it in range 0 to 1, multiply by 0.5 and add 0.5
}

float generateHeight(float x, float z)
{
    return properties.amplitude * perlin(x * properties.roughness, z * properties.roughness);
}

void main()
{
    float height_left = generateHeight(position.x - properties.offset, position.z);
    float height_right = generateHeight(position.x + properties.offset, position.z);
    float height_back = generateHeight(position.x, position.z - properties.offset);
    float height_front = generateHeight(position.x, position.z + properties.offset);

    vec3 new_normal = normalize(vec3(height_left - height_right, 2.0f, height_back - height_front));
    vec3 new_position = vec3(position.x, generateHeight(position.x, position.z), position.z);

    fs_position = vec3(node_matrix * vec4(new_position, 1.0));
    fs_normal = mat3(node_matrix) * new_normal;
    fs_texture_coord = texture_coord;

    gl_Position = projection_matrix * view_matrix * vec4(fs_position, 1.0);
}
