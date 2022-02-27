#version 450

in VS_OUT
{
    vec3 worldPos;
    vec3 worldNormal;
    vec3 fragUV;

    vec3 viewPos;
    vec3 viewNormal;

} fs_in;

layout (location = 0) out vec3 worldPos;
layout (location = 1) out vec3 worldNormal;
layout (location = 2) out vec3 vUV;
layout (location = 3) out vec3 depthValue;
layout (location = 4) out vec3 vAlbedo;
layout (location = 5) out vec3 vMaterial;
layout (location = 6) out vec3 viewPos;
layout (location = 7) out vec3 viewNormal;


uniform vec3 baseColor;
uniform vec3 material;

void main() {
    worldPos        = fs_in.worldPos;
    worldNormal     = fs_in.worldNormal;
    viewPos        = fs_in.viewPos;
    viewNormal     = fs_in.viewNormal;
    vUV         = fs_in.fragUV;
    float distFromCamera = length(fs_in.viewPos);
    depthValue  = vec3(distFromCamera);
    vAlbedo = baseColor;
    vMaterial = material;
}
