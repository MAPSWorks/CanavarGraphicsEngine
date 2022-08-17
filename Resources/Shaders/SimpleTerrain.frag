#version 330 core

struct TileProperties {
    int cellCount;
    float offset;
    float amplitude;
    int octaves;
    float roughness;
    float ambient;
    float diffuse;
    float shininess;
    float specular;
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
    float cut_off_angle;
    float outer_cut_off_angle;
};

uniform TileProperties properties;
uniform vec3 camera_position;
uniform DirectionalLight directional_light;

//uniform PointLight point_lights[8];
//uniform SpotLight spot_lights[8];
//uniform int number_of_point_lights;
//uniform int number_of_spot_lights;

//uniform sampler2D texture_diffuse;
//uniform sampler2D texture_specular;

in vec3 fs_position;
in vec3 fs_normal;
in vec2 fs_texture_coord;

out vec4 out_color;

void main()
{
    // Common Variables
    vec3 normal = normalize(fs_normal);
    vec3 directional_light_dir = normalize(-directional_light.direction);
    vec3 view_dir = normalize(camera_position - fs_position);
    vec4 color = vec4(63.0f / 256, 155.0f / 256, 11.0f / 256, 1.0f);

    vec4 result = vec4(0,0,0,0);

    // Directional Light
    {
        // Ambient
        float ambient = directional_light.ambient * properties.ambient;

        // Diffuse
        float diffuse_coef = max(dot(normal, directional_light_dir), 0.0);
        float diffuse = directional_light.diffuse * diffuse_coef * properties.diffuse;

        // Specular
        vec3 reflect_dir = reflect(-directional_light_dir, normal);
        float specular_coef = pow(max(dot(view_dir, reflect_dir), 0.0), properties.shininess);
        float specular = directional_light.specular * specular_coef * properties.specular;

        result = (specular + ambient + diffuse) * color * directional_light.color;
    }

    out_color = result;
}
