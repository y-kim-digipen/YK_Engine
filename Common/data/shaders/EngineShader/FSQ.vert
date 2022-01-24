#version 450 core

uniform mat4  vertexTransform;

// input vertex data, different for all executions of this shader.
in layout(location = 0) vec3 vPosition;
in layout(location = 2) vec2 vUV;
// Interpolating vertex attributes over the rasterizer
out VS_OUT
{
    vec4 vertexPosition; // interpolated vPosition
    vec2 vertexUV;
} vs_out;

//out mat4  vTransform;

void main()
{
    //    vs_out.vertexPosition = vertexTransform *  vec4( vPosition, 1.0f );
    vs_out.vertexPosition = /*vertexTransform */ vec4( vPosition, 1.0f );
    vs_out.vertexUV = vUV;

//    vTransform = vertexTransform;

    gl_Position = vs_out.vertexPosition;
}