#version 450

in VS_OUT
{
    vec3 fragPos;
    vec3 fragNormal;
    vec3 fragUV;
} fs_in;

layout (location = 0) out vec3 vPos;
layout (location = 1) out vec3 vNormal;
layout (location = 2) out vec3 vUV;
layout (location = 3) out vec3 depthValue;

void main() {
    vPos        = fs_in.fragPos;
    vNormal     = fs_in.fragNormal;
    vUV         = fs_in.fragUV;
    float distFromCamera = length(fs_in.fragPos);
    depthValue  = vec3(distFromCamera);
}
