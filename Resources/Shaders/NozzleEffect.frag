#version 330 core

in float fsRadius;
in float fsLength;

uniform float radius;

out vec4 outColor;

void main()
{
    if(fsLength > 6.0f)
        outColor = vec4(0,0,0,0);
    else if(fsRadius > 0.94 * radius)
        outColor = vec4(1,0,0,0.05);
    else  if (fsRadius < 0.1 * radius)
        outColor = vec4(1,1,1, 1 - fsRadius / radius);
    else
       outColor = mix(vec4(1,0.5,0,0.3), vec4(1,1,0,0.5), exp(-2 * fsRadius));
}
