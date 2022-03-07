#version 420 core

uniform mat4 vertexTransform;
uniform mat4 vertexNormalTransform;

in layout(location = 0) vec3 vPosition;
in layout(location = 1) vec3 vertexNormal;
//in layout(location = 2) vec3 diffuseColor;

uniform vec3 diffuseColor = vec3( 1.f, 0.3f, 0.3f );
//int i = vec3(1.f);
out VS_OUT
{
    vec3 fragDiffuse;
    vec3 fragNormal;
} vs_out;

void main()
{
    vs_out.fragDiffuse = diffuseColor;
    vs_out.fragNormal = mat3(vertexNormalTransform) * vertexNormal;

	gl_Position = vertexTransform * vec4( vPosition, 1.0f );
}
