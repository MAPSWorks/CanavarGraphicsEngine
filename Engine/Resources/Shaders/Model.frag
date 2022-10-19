#version 330 core

uniform vec3 cameraPos;
uniform vec3 sunDir;
uniform vec4 color;
uniform float ambient;
uniform float diffuse;
uniform float specular;
uniform float shininess;

in vec4 fsPosition;
in vec3 fsNormal;

out vec4 outColor;

void main()
{
    vec3 viewDir = normalize(cameraPos - fsPosition.xyz);

    // Diffuse
    float diffuseFactor = max(dot(fsNormal, sunDir), 0.0) * diffuse;

    // Specular
    vec3 reflectDir = reflect(-sunDir, fsNormal);
    vec3 halfwayDir = normalize(sunDir + viewDir);
    float specularFactor = pow(max(dot(fsNormal, halfwayDir), 0.0), shininess) * specular;

    outColor = (ambient + diffuseFactor + specularFactor) * color;

}
