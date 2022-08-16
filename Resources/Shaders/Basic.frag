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

in vec3 fs_position;
in vec3 fs_normal;

out vec4 out_color;

void main()
{
    // Common Variables
    vec3 normal = normalize(fs_normal);
    vec3 directional_light_dir = normalize(-directional_light.direction);
    vec3 view_dir = normalize(camera_position - fs_position);

    vec4 result = vec4(0,0,0,0);

    // Directional Light
    {
        // Ambient
        float ambient = directional_light.ambient * node.ambient;

        // Diffuse
        float diffuse_coef = max(dot(normal, directional_light_dir), 0.0);
        float diffuse = directional_light.diffuse * diffuse_coef * node.diffuse;

        // Specular
        vec3 reflect_dir = reflect(-directional_light_dir, normal);
        float specular_coef = pow(max(dot(view_dir, reflect_dir), 0.0), node.shininess);
        float specular = directional_light.specular * specular_coef * node.specular;

        result = (specular + ambient + diffuse) * node.color * directional_light.color;
    }

    // Point Lights
    {
        for(int i = 0; i < number_of_point_lights; i++) {
            // Ambient
            float ambient  = point_lights[i].ambient  * node.ambient;

            // Diffuse
            vec3 light_dir = normalize(point_lights[i].position - fs_position);
            float diffuse_coef = max(dot(normal, light_dir), 0.0);
            float diffuse  = point_lights[i].diffuse  * diffuse_coef * node.diffuse;

            // Specular
            vec3 reflect_dir = reflect(-light_dir, normal);
            float specular_coef = pow(max(dot(view_dir, reflect_dir), 0.0), node.shininess);
            float specular = point_lights[i].specular * specular_coef * node.specular;

            // Attenuation
            float distance = length(point_lights[i].position - fs_position);
            float attenuation = 1.0f / (point_lights[i].constant +
                                        point_lights[i].linear * distance +
                                        point_lights[i].quadratic * (distance * distance));

            ambient  *= attenuation;
            diffuse  *= attenuation;
            specular *= attenuation;
            result += (ambient + diffuse + specular) * node.color * point_lights[i].color;
        }
    }


    // Spot Lights
    {
        for(int i = 0; i < number_of_spot_lights; i++) {
            // Ambient
            float ambient = spot_lights[i].ambient * node.ambient;

            // Diffuse
            vec3 light_dir = normalize(spot_lights[i].position - fs_position);
            float diffuse_coef = max(dot(normal, light_dir), 0.0);
            float diffuse = spot_lights[i].diffuse * diffuse_coef * node.diffuse;

            // Specular
            vec3 reflect_dir = reflect(-light_dir, normal);
            float specular_coef = pow(max(dot(view_dir, reflect_dir), 0.0), node.shininess);
            float specular = spot_lights[i].specular * specular_coef * node.specular;

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

            result += (ambient + diffuse + specular) * node.color * spot_lights[i].color;
        }
    }

    out_color = result;
}
