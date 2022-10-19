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

uniform Sun sun;
uniform Model model;
uniform vec3 cameraPos;

in vec4 fsPosition;
in vec3 fsNormal;

out vec4 outColor;

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

    outColor = (ambient + diffuse + specular) * model.color * sun.color;
}
