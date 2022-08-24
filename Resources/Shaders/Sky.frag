#version 430 core
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

in vec2 fsTextureCoords;

uniform vec3 skyColorTop;
uniform vec3 skyColorBottom;
uniform vec3 lightDirection;
uniform vec3 lightColor;
uniform int width;
uniform int height;
uniform mat4 inverseProjectionMatrix;
uniform mat4 inverseViewMatrix;

out vec4 outColor;

vec3 raySphereIntersectionSkyMap(vec3 rayDirection, float r)
{
    float t;
    vec3 sphereCenter = vec3(0.0);
    float rr = r * r;

    vec3 L = -sphereCenter;
    float a = dot(rayDirection, rayDirection);
    float b = 2.0 * dot(rayDirection, L);
    float c = dot(L, L) - rr;

    float discr = b * b - 4.0 * a * c;
    t = max(0.0, (-b + sqrt(discr)) / 2);

    return rayDirection * t;
}

vec3 computeClipSpaceCoord(ivec2 fragCoord)
{
    vec2 NDS = 2.0 * vec2(fragCoord.xy) / vec2(width, height) - 1.0;
    return vec3(NDS, 1.0);
}

vec2 computeScreenPos(vec2 ndc)
{
    return (ndc * 0.5 + 0.5);
}

vec4 colorCubeMap(vec3 endPos, const vec3 d)
{
    vec3 color = mix(skyColorBottom, skyColorTop, clamp(1 - exp(8.5 - 17 * clamp(normalize(d).y * 0.5 + 0.5, 0.0, 1.0)), 0.0, 1.0));
    float sun = clamp(dot(normalize(-lightDirection), d), 0.0, 1.0);
    vec3 sunColor = lightColor * pow(sun, 4250);

    return vec4(color + vec3(1, 1, 0.25) * sunColor, 1.0);
}

void main()
{
    ivec2 fragCoord = ivec2(gl_FragCoord.xy);

    vec4 rayClip = vec4(computeClipSpaceCoord(fragCoord), 1.0);
    vec4 rayView = inverseProjectionMatrix * rayClip;
    rayView = vec4(rayView.xy, -1.0, 0.0);

    vec3 worldDir = vec3(inverseViewMatrix * rayView);
    worldDir = normalize(worldDir);

    vec3 cubeMapEndPos = raySphereIntersectionSkyMap(worldDir, 1);

    vec4 bg = colorCubeMap(cubeMapEndPos, worldDir);

    outColor = vec4(bg.rgb, 1.0);
}
