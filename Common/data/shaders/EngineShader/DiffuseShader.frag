#version 420 core

uniform vec3 EmissiveColor;
out vec3 fragColor;


void main()
{
    fragColor = EmissiveColor;
}