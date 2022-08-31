#version 330 core

struct Node {
    mat4 transformation;
    mat3 normalMatrix;
    vec4 color;
    float ambient;
    float diffuse;
    float specular;
    float shininess;
};

struct DirectionalLight {
    vec4 color;
    vec3 direction;
    float ambient;
    float diffuse;
    float specular;
};

struct PointLight {
    vec4 color;
    vec3 position;
    float ambient;
    float diffuse;
    float specular;
    float constant;
    float linear;
    float quadratic;
};

struct SpotLight {
    vec4 color;
    vec3 position;
    vec3 direction;
    float ambient;
    float diffuse;
    float specular;
    float constant;
    float linear;
    float quadratic;
    float cutOffAngle;
    float outerCutOffAngle;
};

struct Haze {
    bool enabled;
    vec3 color;
    float density;
    float gradient;
};

uniform Node node;
uniform vec3 cameraPosition;
uniform DirectionalLight directionalLight;
uniform PointLight pointLights[8];
uniform SpotLight spotLights[8];
uniform int numberOfPointLights;
uniform int numberOfSpotLights;
uniform Haze haze;

uniform sampler2D textureAmbient;
uniform sampler2D textureDiffuse;
uniform sampler2D textureSpecular;
uniform sampler2D textureNormal;

uniform bool useTexture;
uniform bool useTextureAmbient;
uniform bool useTextureDiffuse;
uniform bool useTextureSpecular;
uniform bool useTextureNormal;

uniform bool useBlinnShading;

uniform bool modelSelected = false;
uniform bool meshSelected = false;

in vec3 fsPosition;
in vec3 fsNormal;
in vec2 fsTextureCoords;
in mat3 fsTBN;

out vec4 outColor;

vec4 getAmbientColor()
{
    if (useTexture)
    {
        if (useTextureAmbient)
            return texture(textureAmbient, fsTextureCoords);
        else
            return vec4(0, 0, 0, 0);
    } else
    {
        return node.ambient * node.color;
    }
}

vec4 getDiffuseColor()
{
    if (useTexture)
    {
        if (useTextureDiffuse)
            return node.diffuse * texture(textureDiffuse, fsTextureCoords);
        else
            return vec4(0, 0, 0, 0);
    } else
    {
        return node.diffuse * node.color;
    }
}

vec4 getSpecularColor()
{
    if (useTexture)
    {
        if (useTextureSpecular)
            return node.specular * texture(textureSpecular, fsTextureCoords);
        else
            return vec4(0, 0, 0, 0);
    } else
    {
        return node.specular * node.color;
    }
}

vec3 getNormal()
{
    if (useTexture)
    {
        if (useTextureNormal)
        {
            vec3 normal = texture(textureNormal, fsTextureCoords).rgb;
            normal = 2.0 * normal - 1.0;
            normal = normalize(fsTBN * normal);
            return normal;
        } else
            return normalize(fsNormal);
    } else
    {
        return normalize(fsNormal);
    }
}

float getHazeFactor()
{
    float distance = length(cameraPosition - fsPosition);
    float factor = exp(-pow(distance * 0.00005f * haze.density, haze.gradient));
    return clamp(factor, 0.0f, 1.0f);
}

void main()
{
    // Common Variables
    vec3 normal = getNormal();
    vec3 directionalLightDir = normalize(-directionalLight.direction);
    vec4 ambientColor = getAmbientColor();
    vec4 diffuseColor = getDiffuseColor();
    vec4 specularColor = getSpecularColor();
    vec3 viewDir = normalize(cameraPosition - fsPosition);

    vec4 result = vec4(0, 0, 0, 0);

    // Directional Light
    {
        // Ambient
        vec4 ambient = directionalLight.ambient * ambientColor;

        // Diffuse
        float diffuseFactor = max(dot(normal, directionalLightDir), 0.0);
        vec4 diffuse = diffuseFactor * directionalLight.diffuse * diffuseColor;

        // Specular
        vec3 reflectDir = reflect(-directionalLightDir, normal);
        vec4 specular = vec4(0, 0, 0, 0);

        if (useBlinnShading)
        {
            vec3 halfwayDir = normalize(directionalLightDir + viewDir);
            float specularFactor = pow(max(dot(normal, halfwayDir), 0.0), node.shininess);
            specular = specularFactor * directionalLight.specular * specularColor;
        } else
        {
            float specularFactor = pow(max(dot(viewDir, reflectDir), 0.0), node.shininess);
            specular = specularFactor * directionalLight.specular * specularColor;
        }

        float specularFactor = pow(max(dot(viewDir, reflectDir), 0.0), node.shininess);

        result = (ambient + diffuse + specular) * directionalLight.color;
    }

    // Point Lights
    {
        for (int i = 0; i < numberOfPointLights; i++)
        {
            // Ambient
            vec4 ambient = pointLights[i].ambient * ambientColor;

            // Diffuse
            vec3 lightDir = normalize(pointLights[i].position - fsPosition);
            float diffuseFactor = max(dot(normal, lightDir), 0.0);
            vec4 diffuse = pointLights[i].diffuse * diffuseFactor * diffuseColor;

            // Specular
            vec3 reflectDir = reflect(-lightDir, normal);
            vec4 specular = vec4(0, 0, 0, 0);

            if (useBlinnShading)
            {
                vec3 halfwayDir = normalize(lightDir + viewDir);
                float specularFactor = pow(max(dot(normal, halfwayDir), 0.0), node.shininess);
                specular = specularFactor * pointLights[i].specular * specularColor;
            } else
            {
                float specularFactor = pow(max(dot(viewDir, reflectDir), 0.0), node.shininess);
                specular = specularFactor * pointLights[i].specular * specularColor;
            }

            // Attenuation
            float distance = length(pointLights[i].position - fsPosition);
            float attenuation = 1.0f / (pointLights[i].constant + pointLights[i].linear * distance + pointLights[i].quadratic * (distance * distance));

            ambient *= attenuation;
            diffuse *= attenuation;
            specular *= attenuation;
            result += (ambient + diffuse + specular) * pointLights[i].color;
        }
    }

    // Spot Lights
    {
        for (int i = 0; i < numberOfSpotLights; i++)
        {
            // Ambient
            vec4 ambient = spotLights[i].ambient * ambientColor;

            // Diffuse
            vec3 lightDir = normalize(spotLights[i].position - fsPosition);
            float diffuseFactor = max(dot(normal, lightDir), 0.0);
            vec4 diffuse = diffuseFactor * spotLights[i].diffuse * diffuseColor;

            // Specular
            vec3 reflectDir = reflect(-lightDir, normal);
            vec4 specular = vec4(0, 0, 0, 0);

            if (useBlinnShading)
            {
                vec3 halfwayDir = normalize(lightDir + viewDir);
                float specularFactor = pow(max(dot(normal, halfwayDir), 0.0), node.shininess);
                specular = specularFactor * spotLights[i].specular * specularColor;
            } else
            {
                float specularFactor = pow(max(dot(viewDir, reflectDir), 0.0), node.shininess);
                specular = specularFactor * spotLights[i].specular * specularColor;
            }

            // Spotlight (soft edges)
            float theta = dot(lightDir, normalize(-spotLights[i].direction));
            float epsilon = (spotLights[i].cutOffAngle - spotLights[i].outerCutOffAngle);
            float intensity = clamp((theta - spotLights[i].outerCutOffAngle) / epsilon, 0.0, 1.0);
            diffuse *= intensity;
            specular *= intensity;

            // Attenuation
            float distance = length(spotLights[i].position - fsPosition);
            float attenuation = 1.0f / (spotLights[i].constant + spotLights[i].linear * distance + spotLights[i].quadratic * (distance * distance));
            ambient *= attenuation;
            diffuse *= attenuation;
            specular *= attenuation;

            result += (ambient + diffuse + specular) * spotLights[i].color;
        }
    }

    if (haze.enabled)
    {
        float hazeFactor = getHazeFactor();
        if (modelSelected)
        {
            vec4 color = mix(vec4(haze.color, 1), result, hazeFactor);

            if (meshSelected)
                outColor = mix(color, vec4(1, 1, 0, 1), 0.125);
            else
                outColor = mix(color, vec4(1, 0, 0, 1), 0.125);

        } else
        {
            outColor = mix(vec4(haze.color, 1), result, hazeFactor);
        }

    } else
    {
        if (modelSelected)
        {
            if (meshSelected)
                outColor = mix(result, vec4(1, 1, 0, 1), 0.125);
            else
                outColor = mix(result, vec4(1, 0, 0, 1), 0.125);

        } else
        {
            outColor = result;
        }
    }
}
