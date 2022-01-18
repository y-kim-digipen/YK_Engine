#version 450 core

layout (binding = 0) uniform sampler2D tex_object0;

in vec2 fragUV;

out vec3 fragColor;

void main()
{
    vec3 tColor = texture( tex_object0, fragUV ).rgb;
    fragColor = tColor;
}