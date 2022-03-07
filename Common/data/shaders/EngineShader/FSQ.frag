#version 450 core

layout (binding = 0) uniform sampler2D tex_object0;
// Interpolating vertex attributes over the rasterizer
in VS_OUT
{
    vec4 vertexPosition; // interpolated vPosition
    vec2 vertexUV;
} fs_in;

out vec3 fragColor;

void main()
{
    vec3 tColor = texture( tex_object0, fs_in.vertexUV ).rgb;
    //    vec3 fColor = fs_in.vertexColor;
    //    fragColor = mix( tColor, fColor, 0.015f );
    fragColor = tColor;
    //    fragColor = vec3(0.5f);
}