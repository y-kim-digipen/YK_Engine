#version 450 core

layout (binding=0) uniform sampler2D tex_object0;//Original Image;
layout (binding=1) uniform sampler2D tex_object1;//reflection;

in vec2 fragUV;

out vec3 finalColor;
void main() {
    finalColor = texture(tex_object0, fragUV).rgb + texture(tex_object1, fragUV).rgb;
}

