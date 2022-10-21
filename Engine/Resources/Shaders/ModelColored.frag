#version 330 core

struct Sun
{
    vec3 direction;
    vec4 color;
    float ambient;
    float diffuse;
    float specular;
};

struct Model
{
    vec4 color;
    float ambient;
    float diffuse;
    float specular;
    float shininess;
};

struct Haze
{
    bool enabled;
    vec3 color;
    float density;
    float gradient;
};

uniform Haze haze;
uniform Sun sun;
uniform Model model;
uniform vec3 cameraPos;

in vec4 fsPosition;
in vec3 fsNormal;

out vec4 outColor;

float getHazeFactor()
{
    float distance = length(cameraPos - fsPosition.xyz);
    float factor = exp(-pow(distance * 0.00005f * haze.density, haze.gradient));
    return clamp(factor, 0.0f, 1.0f);
}

void main()
{
    vec3 viewDir = normalize(cameraPos - fsPosition.xyz);

    // Ambient
    float ambient = model.ambient * sun.ambient;

    // Diffuse
    float diffuse = max(dot(fsNormal, sun.direction), 0.0) * sun.diffuse * model.diffuse;

    // Specular
    vec3 reflectDir = reflect(-sun.direction, fsNormal);
    vec3 halfwayDir = normalize(sun.direction + viewDir);
    float specular = pow(max(dot(fsNormal, halfwayDir), 0.0), model.shininess) * model.specular * sun.specular;

    vec4 color = (ambient + diffuse + specular) * model.color * sun.color;

    if (haze.enabled)
    {
        float hazeFactor = getHazeFactor();
        outColor = mix(vec4(haze.color, 1) * clamp(-sun.direction.y, 0.0f, 1.0f), color, hazeFactor);
    } else
    {
        outColor = color;
    }
}
