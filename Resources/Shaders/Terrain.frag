#version 330 core
in vec3 fs_world_position;
in vec2 fs_tex_coord;
in vec3 fs_normal;
in float fs_distance_from_position;
in float height;

struct DirectionalLight {
    vec4 color;
    vec3 direction;
    float ambient;
    float diffuse;
    float specular;
};

uniform DirectionalLight directional_light;

uniform vec3 camera_position;
uniform vec3 fog_color;
uniform float fog_fall_off;
uniform vec3 seed;
uniform vec2 offset;
uniform int octaves;
uniform bool draw_fog;
uniform float amplitude;
uniform float freq;
uniform bool normals;
uniform float grass_coverage;
uniform float water_height;
uniform vec3 rock_color;

uniform sampler2D sand, grass1, grass0, rock, snow, rock_normal;

out vec4 out_color;

const mat2 m = mat2(0.8, -0.6, 0.6, 0.8);

float noise(int ind, int x, int y)
{
    const int primes[39] = int[39](995615039,
                                   600173719,
                                   701464987,
                                   831731269,
                                   162318869,
                                   136250887,
                                   174329291,
                                   946737083,
                                   245679977,
                                   362489573,
                                   795918041,
                                   350777237,
                                   457025711,
                                   880830799,
                                   909678923,
                                   787070341,
                                   177340217,
                                   593320781,
                                   405493717,
                                   291031019,
                                   391950901,
                                   458904767,
                                   676625681,
                                   424452397,
                                   531736441,
                                   939683957,
                                   810651871,
                                   997169939,
                                   842027887,
                                   423882827,
                                   192405871,
                                   129403928,
                                   102948294,
                                   102948203,
                                   912030912,
                                   910204948,
                                   958730910,
                                   759204855,
                                   859302983);

    int n = x + y * 57;
    n = (n << 13) ^ n;
    int a = primes[ind * 3], b = primes[ind * 3 + 1], c = primes[ind * 3 + 2];
    int t = (n * (n * n * a + b) + c) & 0x7fffffff;
    return float(1.0 - t / 1073741824.0);
}

// Dummy random
float random2d(vec2 st)
{
    return fract(sin(dot(st.xy, vec2(12.9898, 78.233) + seed.xy)) * 43758.5453123);
}

// Cosine interpolation
float interpolate(float a, float b, float x)
{
    float ft = x * 3.14159265f;
    float f = (1 - cos(ft)) * 0.5;
    return a * (1 - f) + b * f;
}

float interpolated_noise(vec2 xy)
{
    float x = xy.x, y = xy.y;
    int integer_X = int(floor(x));
    float fractional_X = fract(x);
    int integer_Y = int(floor(y));
    float fractional_Y = fract(y);
    vec2 randomInput = vec2(integer_X, integer_Y);
    float a = random2d(randomInput);
    float b = random2d(randomInput + vec2(1.0, 0.0));
    float c = random2d(randomInput + vec2(0.0, 1.0));
    float d = random2d(randomInput + vec2(1.0, 1.0));

    vec2 w = vec2(fractional_X, fractional_Y);
    w = w * w * w * (10.0 + w * (-15.0 + 6.0 * w));

    float k0 = a, k1 = b - a, k2 = c - a, k3 = d - c - b + a;

    return k0 + k1 * w.x + k2 * w.y + k3 * w.x * w.y;
}

float smoothstepd(float x)
{
    return 6.0 * x * (1.0 - x);
}

vec2 smoothstepd(float a, float b, float x)
{
    if (x < a)
        return vec2(0.0, 0.0);
    if (x > b)
        return vec2(1.0, 0.0);
    float ir = 1.0 / (b - a);
    x = (x - a) * ir;
    return vec2(x * x * (3.0 - 2.0 * x), 6.0 * x * (1.0 - x) * ir);
}

vec2 interpolated_noise_d(vec2 xy)
{
    float x = xy.x, y = xy.y;
    int integer_X = int(floor(x));
    float fractional_X = fract(x);
    int integer_Y = int(floor(y));
    float fractional_Y = fract(y);

    vec2 randomInput = vec2(integer_X, integer_Y);
    float a = random2d(randomInput + vec2(0.0, 0.0));
    float b = random2d(randomInput + vec2(1.0, 0.0));
    float c = random2d(randomInput + vec2(0.0, 1.0));
    float d = random2d(randomInput + vec2(1.0, 1.0));

    float k0 = a, k1 = b - a, k2 = c - a, k3 = d - c - b + a;

    float dndx = (k1 + k3 * fractional_Y) * smoothstepd(fractional_X);
    float dndy = (k2 + k3 * fractional_X) * smoothstepd(fractional_Y);

    return vec2(dndx, dndy);
}

uniform float power;

float perlin(float x, float y)
{
    vec2 st = vec2(x, y);
    float persistence = 0.5;
    float total = 0;
    float frequency = 0.005 * freq;
    float ampl = amplitude;
    for (int i = 0; i < octaves; ++i)
    {
        frequency *= 2.0;
        ampl *= persistence;
        vec2 v = frequency * m * st;
        total += interpolated_noise(v) * ampl;
    }
    return pow(total, power);
}

vec3 compute_normals(vec3 world_pos, out mat3 tbn)
{
    float st = 1.0;
    float dhdu = (perlin((world_pos.x + st), world_pos.z) - perlin((world_pos.x - st), world_pos.z)) / (2.0 * st);
    float dhdv = (perlin(world_pos.x, (world_pos.z + st)) - perlin(world_pos.x, (world_pos.z - st))) / (2.0 * st);

    vec3 X = vec3(1.0, dhdu, 1.0);
    vec3 Z = vec3(0.0, dhdv, 1.0);

    vec3 n = normalize(cross(Z, X));
    tbn = mat3(normalize(X), normalize(Z), n);

    return n;
}

vec3 compute_normals(vec2 gradient)
{
    vec3 X = vec3(1.0, gradient.r, 0.0);
    vec3 Z = vec3(0.0, gradient.g, 1.0);

    vec3 n = normalize(cross(Z, X));

    return n;
}

vec4 ambient()
{
    vec4 ambient_rgba = directional_light.ambient * directional_light.color;
    return ambient_rgba;
}

vec4 diffuse(vec3 normal)
{
    vec3 directional_light_dir = normalize(-directional_light.direction);
    float diffuse_factor = max(0.0, dot(directional_light_dir, normal));
    vec4 diffuse = diffuse_factor * directional_light.color * directional_light.diffuse;
    return diffuse;
}

vec4 specular(vec3 normal)
{
    vec3 directional_light_dir = normalize(-directional_light.direction);
    float specularFactor = 0.01f;
    vec3 view_dir = normalize(camera_position - fs_world_position);
    vec3 reflect_dir = reflect(-directional_light_dir, normal);
    float spec = pow(max(dot(view_dir, reflect_dir), 0.0), 32.0);
    vec4 specular = spec * directional_light.color * directional_light.specular;
    return specular;
}

float perlin(float x, float y, int oct)
{
    vec2 distances = vec2(500.0, 2000.0);
    int distance_factor = int(clamp((distances.y - fs_distance_from_position) * 3.0 / (distances.y - distances.x), 0.0, 3.0));
    distance_factor = 3 - distance_factor;

    float persistence = 0.5;
    float total = 0, frequency = 0.05 * freq, ampl = 1.0;
    for (int i = 0; i < octaves; ++i)
    {
        frequency *= 2;
        ampl *= persistence;

        total += interpolated_noise(vec2(x, y) * frequency) * ampl;
    }
    return total;
}

vec4 get_texture(inout vec3 normal, const mat3 TBN)
{
    float trans = 20.;

    vec4 sand_t = vec4(244, 231, 127, 255) / 255;
    vec4 rock_t = vec4(rock_color, 1.0);
    vec4 grass_t = vec4(92, 196, 66, 255) / 255;

    sand_t = texture(sand, fs_tex_coord * 10.0);
    sand_t.rg *= 1.3;
    rock_t = texture(rock, fs_tex_coord * vec2(1.0, 1.256).yx);
    rock_t.rgb *= vec3(2.5, 2.0, 2.0);
    grass_t = texture(grass0, fs_tex_coord * 12.0);
    vec4 grass_t1 = texture(grass1, fs_tex_coord * 12.0);
    float perlin_blending_coeff = clamp(perlin(fs_world_position.x, fs_world_position.z, 2) * 2.0 - 0.2, 0.0, 1.0);
    grass_t = mix(grass_t * 1.3, grass_t1 * 0.75, perlin_blending_coeff);
    grass_t.rgb *= 0.5;


    float grassCoverage = grass_coverage;

    vec4 heightColor;
    float cosV = abs(dot(normal, vec3(0.0, 1.0, 0.0)));
    float tenPercentGrass = grassCoverage - grassCoverage * 0.1;
    float blendingCoeff = pow((cosV - tenPercentGrass) / (grassCoverage * 0.1), 1.0);

    if (height <= water_height + trans)
    {
        heightColor = sand_t;
    } else if (height <= water_height + 2 * trans)
    {
        heightColor = mix(sand_t, grass_t, pow((height - water_height - trans) / trans, 1.0));
    } else if (cosV > grassCoverage)
    {
        heightColor = grass_t;
        mix(normal, vec3(0.0, 1.0, 0.0), 0.25);
    } else if (cosV > tenPercentGrass)
    {
        heightColor = mix(rock_t, grass_t, blendingCoeff);
        normal = mix(TBN * (texture(rock_normal, fs_tex_coord * vec2(2.0, 2.5).yx).rgb * 2.0 - 1.0), normal, blendingCoeff);
    } else
    {
        heightColor = rock_t;
        normal = TBN * (texture(rock_normal, fs_tex_coord * vec2(2.0, 2.5).yx).rgb * 2.0 - 1.0);
    }

    return heightColor;
}

const float c = 18.;
const float b = 3.e-6;

float applyFog(in vec3 rgb,       // original color of the pixel
               in float dist,     // camera to point distance
               in vec3 cameraPos, // camera position
               in vec3 rayDir)    // camera to point vector
{
    float fogAmount = c * exp(-cameraPos.y * fog_fall_off) * (1.0 - exp(-dist * rayDir.y * fog_fall_off)) / rayDir.y;
    vec3 fogColor = vec3(0.5, 0.6, 0.7);
    return clamp(fogAmount, 0.0, 1.0);
}

void main()
{
    // calculate fog color
    vec2 u_FogDist = vec2(2500.0, 10000.0);

    bool normals_fog = true;
    float fogFactor = applyFog(vec3(0.0), distance(camera_position, fs_world_position), camera_position, normalize(fs_world_position - camera_position));
    float eps = 0.1;
    if (fogFactor >= 0.0 && fogFactor > 1. - eps)
    {
        //normals_fog = false;
    }

    vec3 n;
    mat3 TBN;
    if (normals && normals_fog)
    {
        //n = computeNormals(fbmd_9(WorldPos.xz).gb);
        n = compute_normals(fs_world_position, TBN);
        //smoothing
        /**float st = 0.1;
                vec3 n1 = computeNormals(WorldPos + vec3(-st, 0, st));
                vec3 n3 = computeNormals(WorldPos + vec3(0, 0, st));
                vec3 n2 = computeNormals(WorldPos + vec3(st, 0, st));
                vec3 n4 = computeNormals(WorldPos + vec3(-st, 0, 0));
                vec3 n5 = computeNormals(WorldPos + vec3(st, 0, 0));
                vec3 n6 = computeNormals(WorldPos + vec3(0, 0, -st));
                vec3 n7 = computeNormals(WorldPos + vec3(-st, 0, -st));
                vec3 n8 = computeNormals(WorldPos + vec3(st, 0, -st));
                **/
        //n = n + n1 + n2 + n3 + n4 + n5 + n6 + n7 + n8;
        n = normalize(n);
    } else
    {
        n = vec3(0, 1, 0);
    }

    vec4 heightColor = get_texture(n, TBN);
    //heightColor = vec4(perlin(WorldPos.x, WorldPos.z, 4));

    vec4 ambient = ambient();
    vec4 diffuse = diffuse(n);
    vec4 specular = specular(n);

    // putting all together
    vec4 color = heightColor * ambient + specular * 0 + diffuse;
    if (draw_fog)
    {
        out_color = mix(color, vec4(mix(fog_color * 1.1, fog_color * 0.85, clamp(fs_world_position.y / (1500. * 16.) * amplitude, 0.0, 1.0)), 1.0f), fogFactor);
        out_color.a = fs_world_position.y / water_height;
    } else
    {
        out_color = color;
        out_color.a = fs_world_position.y / water_height;
    }

    out_color = vec4(1,1,1,1);
};
