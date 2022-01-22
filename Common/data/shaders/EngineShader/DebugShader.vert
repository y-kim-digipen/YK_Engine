#version 450 core

in layout(location = 2) vec2 vUV;

out vec2 fragUV;

void main()
{
    fragUV = vUV;
}