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

uniform vec3 camera_position;
uniform Node node;
uniform DirectionalLight directional_light;
uniform PointLight point_lights[8];
uniform SpotLight spot_lights[8];
uniform int number_of_point_lights;
uniform int number_of_spot_lights;

uniform sampler2D texture_diffuse;
uniform sampler2D texture_specular;

in vec3 fs_position;
in vec3 fs_normal;
in vec2 fs_texture_coord;

out vec4 out_color;

void main()
{
    // Common Variables
    vec3 normal = normalize(fs_normal);
    vec3 directional_light_dir = normalize(-directional_light.direction);
    vec3 texture_diffuse_rgb = texture(texture_diffuse, fs_texture_coord).rgb;
    vec3 texture_specular_rgb = texture(texture_specular, fs_texture_coord).rgb;
    vec3 view_dir = normalize(camera_position - fs_position);

    vec3 result = vec3(0,0,0);

    // Directional Light
    {
        // Ambient
        vec3 ambient = directional_light.ambient * texture_diffuse_rgb;

        // Diffuse
        float diffuse_coef = max(dot(normal, directional_light_dir), 0.0);
        vec3 diffuse = directional_light.diffuse * diffuse_coef * texture_diffuse_rgb;

        // Specular
        vec3 reflect_dir = reflect(-directional_light_dir, normal);
        float specular_coef = pow(max(dot(view_dir, reflect_dir), 0.0), node.shininess);
        vec3 specular = directional_light.specular * specular_coef * texture_specular_rgb;

        result = ambient + diffuse + specular;
    }

    // Point Lights
    {
        for(int i = 0; i < number_of_point_lights; i++) {
            // Ambient
            vec3 ambient  = point_lights[i].ambient  * texture_diffuse_rgb;

            // Diffuse
            vec3 light_dir = normalize(point_lights[i].position - fs_position);
            float diffuse_coef = max(dot(normal, light_dir), 0.0);
            vec3 diffuse  = point_lights[i].diffuse  * diffuse_coef * texture_diffuse_rgb;

            // Specular
            vec3 reflect_dir = reflect(-light_dir, normal);
            float specular_coef = pow(max(dot(view_dir, reflect_dir), 0.0), node.shininess);
            vec3 specular = point_lights[i].specular * specular_coef * texture_specular_rgb;

            // Attenuation
            float distance = length(point_lights[i].position - fs_position);
            float attenuation = 1.0f / (point_lights[i].constant +
                                        point_lights[i].linear * distance +
                                        point_lights[i].quadratic * (distance * distance));

            ambient  *= attenuation;
            diffuse  *= attenuation;
            specular *= attenuation;
            result += (ambient + diffuse + specular);
        }
    }


    // Spot Lights
    {
        for(int i = 0; i < number_of_spot_lights; i++) {
            // Ambient
            vec3 ambient = spot_lights[i].ambient * texture_diffuse_rgb;

            // Diffuse
            vec3 light_dir = normalize(spot_lights[i].position - fs_position);
            float diffuse_coef = max(dot(normal, light_dir), 0.0);
            vec3 diffuse = spot_lights[i].diffuse * diffuse_coef * texture_diffuse_rgb;

            // Specular
            vec3 reflect_dir = reflect(-light_dir, normal);
            float specular_coef = pow(max(dot(view_dir, reflect_dir), 0.0), node.shininess);
            vec3 specular = spot_lights[i].specular * specular_coef * texture_specular_rgb;

            // Spotlight (soft edges)
            float theta = dot(light_dir, normalize(-spot_lights[i].direction));
            float epsilon = (spot_lights[i].cut_off_angle - spot_lights[i].outer_cut_off_angle);
            float intensity = clamp((theta - spot_lights[i].outer_cut_off_angle) / epsilon, 0.0, 1.0);
            diffuse  *= intensity;
            specular *= intensity;

            // Attenuation
            float distance = length(spot_lights[i].position - fs_position);
            float attenuation = 1.0f / (spot_lights[i].constant +
                                        spot_lights[i].linear * distance +
                                        spot_lights[i].quadratic * (distance * distance));
            ambient  *= attenuation;
            diffuse  *= attenuation;
            specular *= attenuation;

            result += (ambient + diffuse + specular);
        }
    }

    out_color = vec4(result, 1.0);
}
