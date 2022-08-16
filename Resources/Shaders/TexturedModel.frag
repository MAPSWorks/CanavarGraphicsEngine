#version 330 core
struct Node {
    mat4 transformation;
    float shininess;
};

struct DirectionalLight {
    vec4 color;
    vec3 direction;
    float ambient;
    float diffuse;
    float specular;
};

uniform vec3 camera_position;
uniform Node node;
uniform DirectionalLight directional_light;

uniform sampler2D texture_diffuse;
uniform sampler2D texture_specular;

in vec3 fs_position;
in vec3 fs_normal;
in vec2 fs_texture_coord;
out vec4 out_color;

void main()
{
    // DirectionalLight
    // Ambient
    vec3 ambient = directional_light.ambient * texture(texture_diffuse, fs_texture_coord).rgb;

    // Diffuse
    vec3 normal = normalize(fs_normal);
    vec3 light_dir = normalize(-directional_light.direction);
    float diffuse_coef = max(dot(normal, light_dir), 0.0);
    vec3 diffuse = directional_light.diffuse * diffuse_coef * texture(texture_diffuse, fs_texture_coord).rgb;

    // Specular
    vec3 view_dir = normalize(camera_position - fs_position);
    vec3 reflect_dir = reflect(-light_dir, normal);
    float specular_coef = pow(max(dot(view_dir, reflect_dir), 0.0), node.shininess);
    vec3 specular = directional_light.specular * specular_coef * texture(texture_specular, fs_texture_coord).rgb;

    vec3 result = ambient + diffuse + specular;
    out_color = vec4(result, 1.0);
}
