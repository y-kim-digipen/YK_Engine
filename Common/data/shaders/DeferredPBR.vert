#version 450 core

//uniform mat4 modelToWorldTransform;     //todo change its' name to viewTransform
//uniform mat4 perspectiveMatrix;         //todo change its' name to projectionTransform

uniform mat4  vertexTransform;

// input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 vPosition;
layout(location = 2) in vec2 vUV;

// Interpolating vertex attributes over the rasterizer
out VS_OUT
{
    vec4 vertexPosition; // interpolated vPosition
    vec2 vertexUV; // interpolated vUV
} vs_out;

void main()
{
    vs_out.vertexPosition = vec4( vPosition, 1.0f );
    vs_out.vertexUV = vUV;
    gl_Position = vs_out.vertexPosition;
}