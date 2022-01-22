#version 450 core

#define MAX_NUM_TOTAL_LIGHTS 16

layout (binding = 0) uniform sampler2D tex_object0;
layout (binding = 1) uniform sampler2D tex_object1;

uniform mat4 modelToWorldTransform;
uniform mat4 perspectiveMatrix;

uniform bool UsingGPUUV_GUIX;
uniform int UVType_GUIX = 0; /*0. Planar, 1. Cylindarical, 2. Spherical, 3. CubeMap*/
uniform vec3 ModelScale_GUIX = vec3(1.f);
uniform vec3 MinBoundingBox_GUIX, MaxBoundingBox_GUIX;

uniform vec3 CameraPos_GUIX;
uniform bool UsingTexture_GUIX;

//Environment
uniform vec3 EmissiveColor;

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

in layout(location = 0) vec3 vPosition;
in layout(location = 1) vec3 vertexNormal;
in layout(location = 2) vec2 vUV;

out vec3 Color;

vec2 CalcGPU_UV(int type, vec3 vPositon, vec3 vNormal);

void main() {
    gl_Position = perspectiveMatrix * modelToWorldTransform * vec4(vPosition, 1.f);
    vec3 Position = (modelToWorldTransform * vec4(vPosition, 1.f)).xyz;
    vec3 Normal = mat3(modelToWorldTransform) *  vertexNormal;
    vec2 UV;
    if(UsingGPUUV_GUIX)
    {
        UV = CalcGPU_UV(UVType_GUIX, vPosition, vertexNormal);
    }
    else
    {
        UV = vUV;
    }

    vec3 Sum_Local_Light = vec3(0.f);

    vec3 V = CameraPos_GUIX - Position;
    float CameraDistance = length(V);
    vec3 V_Normalized = V / CameraDistance;
    if(light_block.NumActiveLights > 0)
    {
        for(int i = 0; i < light_block.NumActiveLights; ++i)
        {
            Light currentlight = light_block.lights[i];

            vec3 Pos = currentlight.Pos;

            float CameraDistance = length(V);
            vec3 V_Normalized = V / CameraDistance;

            //material
            float Ka = currentlight.Ka;
            float Kd = currentlight.Kd;
            float Ks = currentlight.Ks;
            float ns = currentlight.ns;

            //Light
            vec3 Ia = currentlight.Ia;
            vec3 Id = currentlight.Id;
            vec3 Is = currentlight.Is;
            if(UsingTexture_GUIX)
            {
                Id = mix(texture(tex_object0, UV).rgb * 256.f, currentlight.Id, 0.4f);
                Is = mix(texture(tex_object1, UV).rgb * 256.f, currentlight.Is, 0.4f);
                ns = max(texture(tex_object1, UV).r, 0.01f);
            }

            vec3 L = Pos - Position;
            float LightDistance = length(L);
            vec3 L_Normalized = L / LightDistance;

            vec3 N = normalize(Normal);

            float NdotL = dot(N, L_Normalized);
            vec3 ReflectionVector = 2 * NdotL * N - L_Normalized;
            float RdotV = dot(ReflectionVector, V_Normalized);
            float Att = min(1.f/(currentlight.c1 + currentlight.c2 * LightDistance + currentlight.c3 * LightDistance * LightDistance), 1.f);

            vec3 I_Ambient = Ia * Ka;
            vec3 I_Diffuse = Id * Kd * max(NdotL, 0.f);
            vec3 I_Specular = Is * Ks * pow( max(RdotV, 0.f), ns);

            vec3 Local_Light = vec3(0.f);

            switch(currentlight.type)
            {
                case 0: /*PointLight*/
                {
                    Local_Light =  Att * (I_Ambient + I_Diffuse + I_Specular);
                    break;
                }
//                case 1: /*DirectionalLight*/
//                {
//                    break;
//                }
                case 1: /*SpotLight*/
                {
                    float P = 1.f;
                    float LdotD = dot(L_Normalized, normalize(currentlight.dir));
                    float SpotLightEffect = 0.f;
                    float cosPhi = cos(currentlight.phi);
                    float cosTheta = cos(currentlight.theta);
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
                    Local_Light =  Att * (I_Ambient + SpotLightEffect * (I_Diffuse + I_Specular));
                    break;
                }
            }
            Sum_Local_Light += Local_Light;
        }
    }

    float S = (environment_block.data.zFar - CameraDistance)/(environment_block.data.zFar - environment_block.data.zNear);

    vec3 I_Emissive = EmissiveColor * 256;
    vec3 I_Local = I_Emissive + Sum_Local_Light / light_block.NumActiveLights;
    vec3 I_Fianl = environment_block.data.GlobalAmbient  +S * I_Local + (1.f - S) * environment_block.data.I_Fog;

    Color = I_Fianl / vec3(256.f);

}

vec2 CalcGPU_UV(int type, vec3 vPositon, vec3 vNormal)
{
    vec2 UV;
    float theta, z, phi;
    vec3 centroidVec = normalize(vPositon - (MinBoundingBox_GUIX + MaxBoundingBox_GUIX) * 0.5f);
    vec3 normalVertex = (vPosition - MinBoundingBox_GUIX) / ModelScale_GUIX;
    switch(type)
    {
        case 0:/*Planar*/{
        UV = (normalVertex.xy - vec2(-1.f)) * 0.5f;
        break;
    }
        case 1:/*Cylindrical*/
        {
            theta = degrees(atan(centroidVec.z, centroidVec.x));
            theta += 180.0f;

            UV.x = theta / 360.f;
            UV.y = (centroidVec.y + 1.f) * 0.5f;
            //            UV.y = (vPosition.z - MinBoundingBox_GUIX.z) / (MaxBoundingBox_GUIX.z - MinBoundingBox_GUIX.z);
            break;
        }
        case 2:/*Spherical*/
        {
            theta = degrees(atan(centroidVec.z, centroidVec.x));
            theta += 180.0f;
            z = centroidVec.y;
            phi = degrees(acos(z / length(centroidVec)));
            UV.x = theta / 360.f;
            UV.y = 1.f - (phi / 180.f);
            break;
        }
        case 3:/*Cube*/
        {
            float x = centroidVec.x,
            y = centroidVec.y,
            z = centroidVec.z;
            float absX = abs(x),
            absY = abs(y),
            absZ = abs(z);

            int isXPositive = x > 0 ? 1 : 0;
            int isYPositive = y > 0 ? 1 : 0;
            int isZPositive = z > 0 ? 1 : 0;

            float maxAxis, uc, vc;

            // POSITIVE X
            if (bool(isXPositive) && (absX >= absY) && (absX >= absZ))
            {
                // u (0 to 1) goes from +z to -z
                // v (0 to 1) goes from -y to +y
                maxAxis = absX;
                uc = -z;
                vc = y;
            }

            // NEGATIVE X
            else if (!bool(isXPositive) && absX >= absY && absX >= absZ)
            {
                // u (0 to 1) goes from -z to +z
                // v (0 to 1) goes from -y to +y
                maxAxis = absX;
                uc = z;
                vc = y;
            }

            // POSITIVE Y
            else if (bool(isYPositive) && absY >= absX && absY >= absZ)
            {
                // u (0 to 1) goes from -x to +x
                // v (0 to 1) goes from +z to -z
                maxAxis = absY;
                uc = x;
                vc = -z;
            }

            // NEGATIVE Y
            else if (!bool(isYPositive) && absY >= absX && absY >= absZ)
            {
                // u (0 to 1) goes from -x to +x
                // v (0 to 1) goes from -z to +z
                maxAxis = absY;
                uc = x;
                vc = z;
            }

            // POSITIVE Z
            else if (bool(isZPositive) && absZ >= absX && absZ >= absY)
            {
                // u (0 to 1) goes from -x to +x
                // v (0 to 1) goes from -y to +y
                maxAxis = absZ;
                uc = x;
                vc = y;
            }

            // NEGATIVE Z
            else if (!bool(isZPositive) && absZ >= absX && absZ >= absY)
            {
                // u (0 to 1) goes from +x to -x
                // v (0 to 1) goes from -y to +y
                maxAxis = absZ;
                uc = -x;
                vc = y;
            }

            UV.x = 0.5f * ( uc / maxAxis + 1.f );
            UV.y = 0.5f * ( vc / maxAxis + 1.f );

            break;
        }
    }
    return UV;
}