#version 450 core

uniform mat4 modelToWorldTransform;
uniform mat4 perspectiveMatrix;

//UV stuffs here
uniform bool UsingGPUUV_GUIX;
uniform int UVType_GUIX = 0; /*0. Planar, 1. Cylindarical, 2. Spherical, 3. CubeMap*/
uniform vec3 ModelScale_GUIX = vec3(1.f);
uniform vec3 MinBoundingBox_GUIX, MaxBoundingBox_GUIX;

in layout(location = 0) vec3 vPosition;
in layout(location = 1) vec3 vertexNormal;
in layout(location = 2) vec2 vUV;
//
//uniform vec3 LightPos = vec3(0.f, 0.f, -4.f);
//uniform vec3 CameraPos;

//Environment
//float zNear = 0.1, zFar = 50;
//uniform float c1 = 0.7, c2 = 0.4, c3 = 0.2;

out  PhongShadingData
{
    vec3 Position;
    vec3 Normal;
    vec2 UV;
} shading_data;


vec2 CalcGPU_UV(int type, vec3 vPositon, vec3 vNormal);

void main() {
    gl_Position = perspectiveMatrix * modelToWorldTransform * vec4(vPosition, 1.f);
    vec3 Position = (modelToWorldTransform * vec4(vPosition, 1.f)).xyz;
    vec3 Normal = mat3(modelToWorldTransform) *  vertexNormal;

    shading_data.Position = Position;
    shading_data.Normal = Normal;

    if(UsingGPUUV_GUIX)
    {
        shading_data.UV = CalcGPU_UV(UVType_GUIX, vPosition, vertexNormal);
    }
    else
    {
        shading_data.UV = vUV;
    }
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