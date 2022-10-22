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

uniform bool useTextureAmbient;
uniform bool useTextureDiffuse;
uniform bool useTextureSpecular;
uniform bool useTextureNormal;

uniform sampler2D textureAmbient;
uniform sampler2D textureDiffuse;
uniform sampler2D textureSpecular;
uniform sampler2D textureNormal;

in vec4 fsPosition;
in vec3 fsNormal;
in vec2 fsTextureCoords;
in mat3 fsTBN;

out vec4 outColor;

vec3 getNormal()
{
    if (useTextureNormal)
    {
        vec3 normal = texture(textureNormal, fsTextureCoords).rgb;
        normal = 2.0 * normal - 1.0;
        normal = normalize(fsTBN * normal);
        return normal;
    } else
        return fsNormal;
}

float getHazeFactor()
{
    float distance = length(cameraPos - fsPosition.xyz);
    float factor = exp(-pow(distance * 0.00005f * haze.density, haze.gradient));
    return clamp(factor, 0.0f, 1.0f);
}

void main()
{
    vec3 normal = getNormal();
    vec4 ambientColor = vec4(0);
    vec4 diffuseColor = vec4(0);
    vec4 specularColor = vec4(0);

    vec3 viewDir = normalize(cameraPos - fsPosition.xyz);

    // Ambient
    if (useTextureAmbient)
    {
        ambientColor = texture(textureAmbient, fsTextureCoords) * sun.ambient * model.ambient;
    }

    // Diffuse
    if (useTextureDiffuse)
    {
        float diffuseFactor = max(dot(normal, sun.direction), 0.0) * sun.diffuse * model.diffuse;
        diffuseColor = texture(textureDiffuse, fsTextureCoords) * diffuseFactor;
    }

    if (useTextureSpecular)
    {
        // Specular
        vec3 reflectDir = reflect(-sun.direction, normal);
        vec3 halfwayDir = normalize(sun.direction + viewDir);
        float specularFactor = pow(max(dot(normal, halfwayDir), 0.0), model.shininess) * sun.specular * model.specular;
        specularColor = texture(textureSpecular, fsTextureCoords) * specularFactor;
    }

    vec4 color = (ambientColor + diffuseColor + specularColor) * sun.color;

    if (haze.enabled)
    {
        float hazeFactor = getHazeFactor();
        outColor = mix(vec4(haze.color, 1) * clamp(sun.direction.y, 0.0f, 1.0f), color, hazeFactor);
    } else
    {
        outColor = color;
    }
}
