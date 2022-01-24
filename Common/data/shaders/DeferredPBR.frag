#version 430

#define MAX_NUM_TOTAL_LIGHTS 16

#define PI 3.14159265359

// input textures for the deferred shading lighting pass
layout (binding=0) uniform sampler2D tex_object0; //viewPosBuffer
layout (binding=1) uniform sampler2D tex_object1; //normalBuffer
layout (binding=2) uniform sampler2D tex_object2; //uvBuffer
layout (binding=3) uniform sampler2D tex_object3; //depthBuffer

uniform vec3 LightPos = vec3(0, 1, 1);
vec3 LightColor = vec3(23.47, 21.31, 20.79);

// Interpolating vertex attributes over the rasterizer
in VS_OUT
{
    vec4 vertexPosition; // interpolated vPosition
    vec2 vertexUV; // interpolated vUV
} fs_in;

out vec3 fragColor;

uniform vec3 camPos = vec3(0.f);

uniform vec3    albedo = vec3(0.2f, 0.15, 0.56);
uniform float   metallic = 0.3;
uniform float   roughness = 0.2;
uniform float   ao = 0.1;

//float calculateAttenuation(vec3 fragPos, vec3 LightPos);
vec3  fresnelSchlick(float cosTheta, vec3 F0);
float DistributionGGX(vec3 N, vec3 H, float roughness);
float GeometrySchlickGGX(float NdotV, float roughness);
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness);

void main()
{
    vec3 WorldPos = texture(tex_object1, fs_in.vertexUV).rgb;
    vec3 N = normalize(WorldPos);
    vec3 V = texture( tex_object0, fs_in.vertexUV ).rgb;

    vec3 L = normalize(LightPos - WorldPos);
    vec3 H = normalize(V + L);                  //Half vector

    float distance = length(LightPos - WorldPos);
    float attenuation = 1.f / (distance * distance);

    vec3 Wi = normalize(LightPos - V);
    float cosTheta = max(dot(N, Wi), 0.f);
//    float attenuation = calculateAttenuation(V, LightPos);
    vec3 radiance = LightColor * attenuation * cosTheta;

    vec3 Lo = vec3(0.f);



    vec3 F0 = vec3(0.04f);
    F0      = mix(F0, albedo, metallic);
    vec3 F  = fresnelSchlick(cosTheta, F0);

    float NDF   = DistributionGGX(N, H, roughness);
    float G     = GeometrySmith(N, V, L, roughness);

    //Cook-Torrance BRDF
    vec3 numerator = NDF * G * F;
    float denominator = 4.f * max(dot(N, V), 0.f) * max(dot(N, L), 0.f) + 0.0001;
    vec3 specular = numerator / denominator;

    vec3 kS = F;
    vec3 kD = vec3(1.f) - kS;

    kD *= 1.f - metallic;

    float NdotL = max(dot(N, L), 0.f);
    Lo += (kD * albedo / PI + specular) * radiance * NdotL;

    vec3 ambient = vec3(0.03f) * albedo * ao;
    vec3 color = ambient + Lo;

    fragColor = color;
}

//float calculateAttenuation(vec3 fragPos, vec3 LightPos)
//{
//
//}

vec3  fresnelSchlick(float cosTheta, vec3 F0){
    return F0 + (1.f - F0) * pow(clamp(1.f - cosTheta, 0.f, 1.f), 5.f);
}

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a         = roughness * roughness;
    float a2        = a * a;
    float NdotH     = max(dot(N, H), 0.f);
    float NdotH2    = NdotH * NdotH;

    float num       = a2;
    float denom     = (NdotH2 * (a2 - 1.f) + 1.f);
    denom = PI * denom * denom;
    return num / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness){
    float r = roughness + 1.f;
    float k = (r * r) / 8.f;

    float num = NdotV;
    float denom = (NdotV * (1.f - k) + k);
    return num / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.f);
    float NdotL = max(dot(N, L), 0.f);
    float ggx2  = GeometrySchlickGGX(NdotV, roughness);
    float ggx1  = GeometrySchlickGGX(NdotL, roughness);
    return ggx1 * ggx2;
}
