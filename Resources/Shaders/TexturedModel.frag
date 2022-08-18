#version 330 core

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

uniform vec3 cameraPosition;
uniform float nodeShininess;
uniform DirectionalLight directionalLight;
uniform PointLight pointLights[8];
uniform SpotLight spotLights[8];
uniform int numberOfPointLights;
uniform int numberOfSpotLights;

uniform sampler2D textureDiffuse;
uniform sampler2D textureSpecular;

in vec3 fsPosition;
in vec3 fsNormal;
in vec2 fsTextureCoord;

out vec4 outColor;

void main()
{
    // Common Variables
    vec3 normal = normalize(fsNormal);
    vec3 directionalLightDir = normalize(-directionalLight.direction);
    vec4 textureDiffuseRGBA = texture(textureDiffuse, fsTextureCoord).rgba;
    vec4 textureSpecularRGBA = texture(textureSpecular, fsTextureCoord).rgba;
    vec3 viewDir = normalize(cameraPosition - fsPosition);

    vec4 result = vec4(0,0,0,0);

    // Directional Light
    {
        // Ambient
        vec4 ambient = directionalLight.ambient * textureDiffuseRGBA;

        // Diffuse
        float diffuseFactor = max(dot(normal, directionalLightDir), 0.0);
        vec4 diffuse = diffuseFactor * directionalLight.diffuse * textureDiffuseRGBA;

        // Specular
        vec3 reflectDir = reflect(-directionalLightDir, normal);
        float specularFactor = pow(max(dot(viewDir, reflectDir), 0.0), nodeShininess);
        vec4 specular = specularFactor * directionalLight.specular * textureSpecularRGBA;

        result = (ambient + diffuse + specular) * directionalLight.color;
    }

    // Point Lights
    {
        for(int i = 0; i < numberOfPointLights; i++) {
            // Ambient
            vec4 ambient  = pointLights[i].ambient  * textureDiffuseRGBA;

            // Diffuse
            vec3 lightDir = normalize(pointLights[i].position - fsPosition);
            float diffuseFactor = max(dot(normal, lightDir), 0.0);
            vec4 diffuse  = pointLights[i].diffuse  * diffuseFactor * textureDiffuseRGBA;

            // Specular
            vec3 reflectDir = reflect(-lightDir, normal);
            float specularFactor = pow(max(dot(viewDir, reflectDir), 0.0), nodeShininess);
            vec4 specular = specularFactor * pointLights[i].specular * textureSpecularRGBA;

            // Attenuation
            float distance = length(pointLights[i].position - fsPosition);
            float attenuation = 1.0f / (pointLights[i].constant +
                                        pointLights[i].linear * distance +
                                        pointLights[i].quadratic * (distance * distance));

            ambient  *= attenuation;
            diffuse  *= attenuation;
            specular *= attenuation;
            result += (ambient + diffuse + specular) * pointLights[i].color;
        }
    }


    // Spot Lights
    {
        for(int i = 0; i < numberOfSpotLights; i++) {
            // Ambient
            vec4 ambient = spotLights[i].ambient * textureDiffuseRGBA;

            // Diffuse
            vec3 lightDir = normalize(spotLights[i].position - fsPosition);
            float diffuseFactor = max(dot(normal, lightDir), 0.0);
            vec4 diffuse = diffuseFactor * spotLights[i].diffuse * textureDiffuseRGBA;

            // Specular
            vec3 reflectDir = reflect(-lightDir, normal);
            float specularFactor = pow(max(dot(viewDir, reflectDir), 0.0), nodeShininess);
            vec4 specular = specularFactor * spotLights[i].specular * textureSpecularRGBA;

            // Spotlight (soft edges)
            float theta = dot(lightDir, normalize(-spotLights[i].direction));
            float epsilon = (spotLights[i].cutOffAngle - spotLights[i].outerCutOffAngle);
            float intensity = clamp((theta - spotLights[i].outerCutOffAngle) / epsilon, 0.0, 1.0);
            diffuse  *= intensity;
            specular *= intensity;

            // Attenuation
            float distance = length(spotLights[i].position - fsPosition);
            float attenuation = 1.0f / (spotLights[i].constant +
                                        spotLights[i].linear * distance +
                                        spotLights[i].quadratic * (distance * distance));
            ambient  *= attenuation;
            diffuse  *= attenuation;
            specular *= attenuation;

            result += (ambient + diffuse + specular) * spotLights[i].color;
        }
    }

    outColor = result;
}
