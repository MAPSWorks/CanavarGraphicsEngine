#version 330 core
struct Node {
    mat4 transformation;
    float shininess;
};

struct Light {
    vec4 color;
    vec3 position;
    float ambient;
    float diffuse;
    float specular;
};

uniform vec3 camera_position;
uniform Node node;
uniform Light light;

uniform sampler2D texture_diffuse;
uniform sampler2D texture_specular;

in vec3 fs_position;
in vec3 fs_normal;
in vec2 fs_texture_coord;
out vec4 out_color;

void main()
{
    // Ambient
    vec3 ambient = light.ambient * texture(texture_diffuse, fs_texture_coord).rgb;

    // Diffuse
    vec3 norm = normalize(fs_normal);
    vec3 lightDir = normalize(light.position - fs_position);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * texture(texture_diffuse, fs_texture_coord).rgb;

    // Specular
    vec3 viewDir = normalize(camera_position - fs_position);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), node.shininess);
    vec3 specular = light.specular * spec * texture(texture_specular, fs_texture_coord).rgb;

    vec3 result = ambient + diffuse + specular;
    out_color = vec4(result, 1.0);
}
