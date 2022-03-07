#version 430

#define MAX_NUM_TOTAL_LIGHTS 16

#define PI 3.14159265359

// input textures for the deferred shading lighting pass
layout (binding=0) uniform sampler2D tex_object0; //viewPosBuffer
layout (binding=1) uniform sampler2D tex_object1; //normalBuffer
layout (binding=2) uniform sampler2D tex_object2; //uvBuffer
layout (binding=3) uniform sampler2D tex_object3; //depthBuffer
layout (binding=4) uniform sampler2D tex_object4; //albedoBuffer
layout (binding=5) uniform sampler2D tex_object5; //materialBuffer
//vec3 LightColor = vec3(0.5f, 1.f, 0.3f);

uniform vec3 CameraPos_GUIX;

// Interpolating vertex attributes over the rasterizer
in VS_OUT
{
    vec4 vertexPosition; // interpolated vPosition
    vec2 vertexUV; // interpolated vUV
} fs_in;

out vec3 fragColor;

uniform float ao = 0.5;

struct Enviroment
{
    vec3 I_Fog;
    float zNear;
    vec3 GlobalAmbient;
    float zFar;
};

struct Light
{
//Light
    vec3 Pos;
    int type;

//material
    float Ka;
    float Kd;
    float Ks;
    float ns;

//Light
    vec3 Ia;
    float c1;
    vec3 Id;
    float c2;
    vec3 Is;
    float c3;

    vec3 dir;
    float theta;
    vec3 padding;
    float phi;

    vec3 DummyPadding;
    float P;
};

layout(std140, binding = 1) uniform LightBlock
{
    Light lights[MAX_NUM_TOTAL_LIGHTS];
    int NumActiveLights;
} light_block;

layout(std140, binding = 2) uniform EnvironmentBlock
{
    Enviroment data;
} environment_block;


float calculateAttenuation(float c1, float c2, float c3, float distance);
vec3  fresnelSchlick(float cosTheta, vec3 F0);
float DistributionGGX(vec3 N, vec3 H, float roughness);
float GeometrySchlickGGX(float NdotV, float roughness);
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness);
vec3 ApplyHDR(vec3 originalColor, float gamma, float exposure);

void main()
{
    vec3    albedo      = texture(tex_object4, fs_in.vertexUV).rgb;
    vec2    material    = texture(tex_object5, fs_in.vertexUV).xy;

    float   metallic    = material.x;
    float   roughness   = material.y;
    roughness = clamp(roughness, 0.03, 1.f);

    vec3 normal = texture(tex_object1, fs_in.vertexUV).rgb;
    vec3 worldPos = texture(tex_object0, fs_in.vertexUV).rgb;
    vec3 N = normalize(normal);
    vec3 V = normalize(CameraPos_GUIX - worldPos);

    vec3 Lo = vec3(0.f);

    for(int i = 0; i < light_block.NumActiveLights; ++i)
    {
        Light currentlight = light_block.lights[i];
        vec3 LightPos = currentlight.Pos;
        vec3 L = normalize(LightPos - worldPos);
        vec3 H = normalize(V + L);                  //Half vector

        float distance = length(LightPos - worldPos);
//        float attenuation = 1.f / (distance * distance);
        float attenuation = calculateAttenuation(currentlight.c1, currentlight.c2, currentlight.c2, distance);

        vec3 Wi = normalize(LightPos - V);
        float cosTheta = max(dot(N, Wi), 0.f);

        vec3 radiance = currentlight.Ia * attenuation * cosTheta;
        switch(currentlight.type)
        {
            case 0:
            {

                break;
            }
            case 1:
            {
                float LdotD = dot(L, normalize(currentlight.dir));
                float cosPhi = cos(currentlight.phi);
                float cosTheta = cos(currentlight.theta);
                float SpotLightEffect = 0.f;
                if(LdotD < cosPhi)
                {
                    SpotLightEffect = 0.f;
                }
                else if(LdotD > cosTheta)
                {
                    SpotLightEffect = 1.f;
                }
                else
                {
                    SpotLightEffect = pow((LdotD - cos(currentlight.phi)) / (cos(currentlight.theta) - cos(currentlight.phi)), currentlight.P);
                }
                radiance *= SpotLightEffect;
                break;
            }
            case 2:
            {


                break;
            }
        }

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
    }

    vec3 ambient = vec3(0.03f) * albedo * ao;
    vec3 color = ambient + Lo;

    vec3 HDRcolor = ApplyHDR(color, 2.2, 5.f);

    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0/2.2));
    fragColor = color;
}


float calculateAttenuation(float c1, float c2, float c3, float distance)
{
    return min(1.f/(c1 + c2 * distance + c3 * distance * distance), 1.f);
}

vec3 ApplyHDR(vec3 originalColor, float gamma, float exposure)
{
    // reinhard tone mapping
    vec3 mapped = originalColor / (originalColor + vec3(1.0));
    // gamma correction
    mapped = pow(mapped, vec3(1.0 / gamma));

    return mapped;
}

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
