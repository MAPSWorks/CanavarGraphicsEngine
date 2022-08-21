#version 330 core

in float fsRadius;

uniform float radius;

out vec4 outColor;


void main()
{
    if(fsRadius > 0.96 * radius)
        outColor = vec4(1,0,0,0.05);
    else  if (fsRadius < 0.1 * radius)
        outColor = vec4(1,1,1, 1 - fsRadius / radius);
    else
       outColor = mix(vec4(1,0.5,0,0.3), vec4(1,1,0,0.5), exp(-2 * fsRadius));
}
