#version 330 core
struct Node {
    mat4 transformation;
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

uniform vec3 cameraPosition;
uniform Node node;
uniform DirectionalLight directionalLight;
uniform PointLight pointLights[8];
uniform SpotLight spotLights[8];
uniform int numberOfPointLights;
uniform int numberOfSpotLights;

in vec3 fsPosition;
in vec3 fsNormal;

out vec4 outColor;

void main()
{
    // Common Variables
    vec3 normal = normalize(fsNormal);
    vec3 directionalLightDir = normalize(-directionalLight.direction);
    vec3 viewDir = normalize(cameraPosition - fsPosition);

    vec4 result = vec4(0,0,0,0);

    // Directional Light
    {
        // Ambient
        float ambient = directionalLight.ambient * node.ambient;

        // Diffuse
        float diffuseFactor = max(dot(normal, directionalLightDir), 0.0);
        float diffuse = diffuseFactor * directionalLight.diffuse * node.diffuse;

        // Specular
        vec3 reflectDir = reflect(-directionalLightDir, normal);
        float specularFactor = pow(max(dot(viewDir, reflectDir), 0.0), node.shininess);
        float specular = specularFactor * directionalLight.specular * node.specular;

        result = (specular + ambient + diffuse) * node.color * directionalLight.color;
    }

    // Point Lights
    {
        for(int i = 0; i < numberOfPointLights; i++) {
            // Ambient
            float ambient  = pointLights[i].ambient  * node.ambient;

            // Diffuse
            vec3 lightDir = normalize(pointLights[i].position - fsPosition);
            float diffuse_coef = max(dot(normal, lightDir), 0.0);
            float diffuse  = pointLights[i].diffuse  * diffuse_coef * node.diffuse;

            // Specular
            vec3 reflectDir = reflect(-lightDir, normal);
            float specularFactor = pow(max(dot(viewDir, reflectDir), 0.0), node.shininess);
            float specular = specularFactor * pointLights[i].specular * node.specular;

            // Attenuation
            float distance = length(pointLights[i].position - fsPosition);
            float attenuation = 1.0f / (pointLights[i].constant +
                                        pointLights[i].linear * distance +
                                        pointLights[i].quadratic * (distance * distance));

            ambient  *= attenuation;
            diffuse  *= attenuation;
            specular *= attenuation;
            result += (ambient + diffuse + specular) * node.color * pointLights[i].color;
        }
    }


    // Spot Lights
    {
        for(int i = 0; i < numberOfSpotLights; i++) {
            // Ambient
            float ambient = spotLights[i].ambient * node.ambient;

            // Diffuse
            vec3 lightDir = normalize(spotLights[i].position - fsPosition);
            float diffuseFactor = max(dot(normal, lightDir), 0.0);
            float diffuse = diffuseFactor * spotLights[i].diffuse * node.diffuse;

            // Specular
            vec3 reflectDir = reflect(-lightDir, normal);
            float specularFactor = pow(max(dot(viewDir, reflectDir), 0.0), node.shininess);
            float specular = specularFactor * spotLights[i].specular * node.specular;

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

            result += (ambient + diffuse + specular) * node.color * spotLights[i].color;
        }
    }

    outColor = result;
}
