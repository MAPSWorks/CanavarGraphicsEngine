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

uniform vec3 camera_position;
uniform Node node;
uniform DirectionalLight directional_light;

in vec3 fs_position;
in vec3 fs_normal;
out vec4 out_color;

void main()
{
    // Ambient
    float ambient = directional_light.ambient * node.ambient;

    // Diffuse
    vec3 normal = normalize(fs_normal);
    vec3 light_dir = normalize(-directional_light.direction);
    float diffuse_coef = max(dot(normal, light_dir), 0.0);
    float diffuse = directional_light.diffuse * diffuse_coef * node.diffuse;

    // Specular
    vec3 view_dir = normalize(camera_position - fs_position);
    vec3 reflect_dir = reflect(-light_dir, normal);
    float specular_coef = pow(max(dot(view_dir, reflect_dir), 0.0), node.shininess);
    float specular = directional_light.specular * specular_coef * node.specular;

    out_color = (specular + ambient + diffuse) * node.color * directional_light.color;
}
