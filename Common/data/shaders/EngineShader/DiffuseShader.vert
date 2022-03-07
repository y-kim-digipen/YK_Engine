#version 450 core

uniform mat4 vertexTransform;
uniform mat4 vertexNormalTransform;

in layout(location = 0) vec3 vPosition;

void main()
{
    gl_Position = vertexTransform * vec4( vPosition, 1.0f );
}