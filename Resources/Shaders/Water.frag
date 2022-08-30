#version 330 core

// Thanks to ThinMatrix for his water tutorial! https://www.youtube.com/watch?v=HusvGeEDU_U&list=PLRIWtICgwaX23jiqVByUs0bqhnalNTNZh

//MIT License

//Copyright (c) 2018 Federico Vaccaro

//Permission is hereby granted, free of charge, to any person obtaining a copy
//of this software and associated documentation files (the "Software"), to deal
//in the Software without restriction, including without limitation the rights
//to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//copies of the Software, and to permit persons to whom the Software is
//furnished to do so, subject to the following conditions:

//The above copyright notice and this permission notice shall be included in all
//copies or substantial portions of the Software.

//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//SOFTWARE.

struct Haze {
    bool enabled;
    vec3 color;
    float density;
    float gradient;
};

in vec3 fsNormal;
in vec4 fsClipSpaceCoords;
in vec2 fsTextureCoords;
in vec4 fsPosition;

uniform float moveFactor;
uniform vec3 cameraPosition;
uniform vec3 lightColor;
uniform vec3 lightDirection;

uniform sampler2D reflectionTex;
uniform sampler2D refractionTex;

uniform Haze haze;

out vec4 outColor;

const float distFactor = 0.05;

float Random3D(in vec3 st)
{
    return fract(sin(dot(st.xyz, vec3(12.9898, 78.233, 141.1525))) * 43758.5453123);
}

float Interpolate(float a, float b, float x)
{ // cosine interpolation
    float ft = x * 3.1415927f;
    float f = (1. - cos(ft)) * 0.5;
    return a * (1. - f) + b * f;
}

float InterpolatedNoise(int ind, float x, float y, float z)
{
    int integer_X = int(floor(x));
    float fractional_X = fract(x);
    int integer_Y = int(floor(y));
    float fractional_Y = fract(y);
    int integer_Z = int(floor(z));
    float fractional_Z = fract(z);

    vec3 randomInput = vec3(integer_X, integer_Y, integer_Z);
    float v1 = Random3D(randomInput + vec3(0.0, 0.0, 0.0));
    float v2 = Random3D(randomInput + vec3(1.0, 0.0, 0.0));
    float v3 = Random3D(randomInput + vec3(0.0, 1.0, 0.0));
    float v4 = Random3D(randomInput + vec3(1.0, 1.0, 0.0));

    float v5 = Random3D(randomInput + vec3(0.0, 0.0, 1.0));
    float v6 = Random3D(randomInput + vec3(1.0, 0.0, 1.0));
    float v7 = Random3D(randomInput + vec3(0.0, 1.0, 1.0));
    float v8 = Random3D(randomInput + vec3(1.0, 1.0, 1.0));

    float i1 = Interpolate(v1, v2, fractional_X);
    float i2 = Interpolate(v3, v4, fractional_X);

    float i3 = Interpolate(v5, v6, fractional_X);
    float i4 = Interpolate(v7, v8, fractional_X);

    float y1 = Interpolate(i1, i2, fractional_Y);
    float y2 = Interpolate(i3, i4, fractional_Y);

    return Interpolate(y1, y2, fractional_Z);
}

float perlin(float x, float y, float z)
{
    int numOctaves = 3;
    float persistence = 0.5;
    float total = 0., frequency = 0.025, amplitude = 6.0;
    for (int i = 0; i < numOctaves; ++i)
    {
        frequency *= 2.;
        amplitude *= persistence;

        x += moveFactor * 13.0;
        y += moveFactor * 7.0;

        total += InterpolatedNoise(0, x * frequency, y * frequency, z * frequency) * amplitude;
        //total += InterpolatedNoise(0, y * frequency, x * frequency, (z+0.5) * frequency) * amplitude;
    }
    return pow(total, 1.0);
}

vec3 computeNormals(vec3 WorldPos)
{
    float st = 0.35;
    float dhdu = (perlin((WorldPos.x + st), WorldPos.z, moveFactor * 10.0) - perlin((WorldPos.x - st), WorldPos.z, moveFactor * 10.0)) / (2.0 * st);
    float dhdv = (perlin(WorldPos.x, (WorldPos.z + st), moveFactor * 10.0) - perlin(WorldPos.x, (WorldPos.z - st), moveFactor * 10.0)) / (2.0 * st);

    vec3 X = vec3(1.0, dhdu, 1.0);
    vec3 Z = vec3(0.0, dhdv, 1.0);

    vec3 n = normalize(cross(Z, X));
    vec3 norm = mix(n, fsNormal, 0.5);
    norm = normalize(norm);
    return norm;
}

vec3 random3(vec3 p)
{
    return fract(sin(vec3(dot(p, vec3(127.1, 311.7, 194.1729)), dot(p, vec3(269.5, 183.3, 72.0192)), dot(p, vec3(183.3, 72.0192, 311.7)))) * 43758.5453);
}

float worley(vec3 st)
{
    float color = 0.0;

    // Scale
    st *= 0.5;

    // Tile the space
    vec3 i_st = floor(st);
    vec3 f_st = fract(st);

    float m_dist = 1.; // minimun distance

    for (int y = -1; y <= 1; y++)
    {
        for (int x = -1; x <= 1; x++)
        {
            for (int z = -1; z <= 1; z++)
            {
                // Neighbor place in the grid
                vec3 neighbor = vec3(float(x), float(y), float(z));

                // Random position from current + neighbor place in the grid
                vec3 point = random3(i_st + neighbor);

                // Vector between the pixel and the point
                vec3 diff = neighbor + point - f_st;

                // Distance to the point
                float dist = pow(length(diff), 1.0);

                // Keep the closer distance
                m_dist = min(m_dist, dist);
            }
        }
    }

    // Draw the min distance (distance field)
    color += m_dist;

    return color;
}

float getHazeFactor()
{
    if (haze.enabled)
    {
        float distance = length(cameraPosition - fsPosition.xyz);
        float factor = exp(-pow(distance * 0.00005f * haze.density, haze.gradient));
        return clamp(factor, 0.0f, 1.0f);
    } else
        return 0.0f;
}

void main()
{
    float grain = 50.0;
    vec2 ndc = (fsClipSpaceCoords.xy / fsClipSpaceCoords.w) / 2.0 + 0.5;
    float st = 0.1;
    float dhdu = (perlin((fsPosition.x + st), fsPosition.z, moveFactor * 10.0) - perlin((fsPosition.x - st), fsPosition.z, moveFactor * 10.0)) / (2.0 * st);
    float dhdv = (perlin(fsPosition.x, (fsPosition.z + st), moveFactor * 10.0) - perlin(fsPosition.x, (fsPosition.z - st), moveFactor * 10.0)) / (2.0 * st);

    float floorY = texture(refractionTex, ndc).a;
    float waterDepth = 1.0 - floorY;
    float waterDepthClamped = clamp(waterDepth * 5.0, 0.0, 1.0);

    vec2 totalDistortion = vec2(dhdu, dhdv) * distFactor * waterDepth;

    vec2 reflectionTexCoords = vec2(ndc.x, -ndc.y);
    reflectionTexCoords += totalDistortion;
    reflectionTexCoords.x = clamp(reflectionTexCoords.x, 0.001, 0.999);
    reflectionTexCoords.y = clamp(reflectionTexCoords.y, -0.999, -0.001);
    vec4 reflectionColor = texture(reflectionTex, reflectionTexCoords);

    vec2 refractionTexCoords = ndc;

    refractionTexCoords += totalDistortion;
    refractionTexCoords = clamp(refractionTexCoords, 0.001, 0.999);
    vec4 refractionColor = texture(refractionTex, refractionTexCoords);

    vec3 toCameraVector = fsPosition.xyz - cameraPosition;
    float fresnelFactor = max(dot(normalize(-toCameraVector), vec3(0.0, 1.0, 0.0)), 0.0);
    fresnelFactor = pow(fresnelFactor, 3.0);
    vec4 refr_reflCol = mix(reflectionColor, refractionColor, fresnelFactor);

    // calculate diffuse illumination
    vec3 norm = computeNormals(fsPosition.xyz);
    norm = mix(norm, vec3(0.0, 1.0, 0.0), 0.25);

    float diffuseFactor = max(0.0, dot(lightDirection, norm.rgb));
    float diffuseConst = 0.5;
    vec3 diffuse = diffuseFactor * diffuseConst * lightColor;

    // calculate specular illumination
    float specularFactor = 1.5f;
    vec3 viewDir = normalize(cameraPosition - fsPosition.xyz);
    vec3 reflectDir = reflect(-lightDirection, normalize(mix(norm, normalize(viewDir * 0.8 + vec3(0.0, 1.0, 0.0)), 0.2)));
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 512.);
    vec3 specular = spec * lightColor * specularFactor;

    float hazeFactor = getHazeFactor();
    vec4 color = vec4(0.2, 0.71, 0.85, 1.0);
    outColor = mix(mix(refr_reflCol, color * 0.8, 0.1) * 0.8 + vec4(diffuse + specular, 1.0), vec4(haze.color, 1.0f), hazeFactor);
    float foam = perlin(fsPosition.x * 4.0, fsPosition.z * 4.0, moveFactor * 10.0) * 0.25;
    foam = mix(foam * pow((1.0 - waterDepth), 8.0), foam * 0.01, 0.0);
    outColor.rgb *= 0.95;
    outColor.a = waterDepthClamped;
}
