#version 330 core
in vec3 fs_texture_coords;

uniform samplerCube skybox;

out vec4 out_color;

void main()
{    
    out_color = texture(skybox, fs_texture_coords);
}
