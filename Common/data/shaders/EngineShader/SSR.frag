#version 450 core

layout (binding=0) uniform sampler2D tex_object0;//gFinalImage;
layout (binding=6) uniform sampler2D tex_object1;//gPosition;
layout (binding=7) uniform sampler2D tex_object2;//gNormal;
layout (binding=5) uniform sampler2D tex_object3;//gExtraComponents;

uniform mat4 invertViewMatrix;
uniform mat4 projectionMatrix;
uniform mat4 invertProjectionMatrix;
uniform mat4 viewMatrix;

in vec2 TexCoords;

out vec4 outColor;

//const float rayStep = 0.1;
//const float minRayStep = 0.01;
////const float maxSteps = 30;
//
//const float searchDistInv = 0.2;
//const int numBinarySearchSteps = 15;
//const float maxDDepth = 1.0;
//const float maxDDepthInv = 1.0;
//const float reflectionSpecularFalloffExponent = 3.0;

float Metallic;


void main()
{
    float maxDist = 8.f;
    float resolution  = 0.3;
    float thickness = 0.5;
    int   steps       = 10;

    float metallic = texture(tex_object3, TexCoords).x;


    vec2 texSize = textureSize(tex_object0, 0).xy;
    vec4 positionFrom = texture(tex_object1, TexCoords);

    if(metallic < 0.01 )
    {
        outColor = vec4(0.f, 0.f, 0.f, 1.f);
        return;
    }

    vec3 normal = normalize(texture(tex_object2, TexCoords).rgb);

    vec3 unitPositionFrom = normalize(positionFrom.xyz);

    vec3 reflect = normalize(reflect(unitPositionFrom, normal));

    vec4 startView = vec4(positionFrom.xyz                    , 1.f);
    vec4 endView   = vec4(positionFrom.xyz + reflect * maxDist, 1.f);

    vec4 startFrag = startView;
    startFrag = projectionMatrix * startFrag;
    startFrag.xyz /= startFrag.w;
    startFrag.xy = startFrag.xy * 0.5f + 0.5f;
    startFrag.xy *= texSize;

    vec4 endFrag = endView;
    endFrag = projectionMatrix * endFrag;
    endFrag.xyz /= endFrag.w;
    endFrag.xy = endFrag.xy * 0.5f + 0.5f;
    endFrag.xy *= texSize;

    vec4 uv = vec4(0.f);
    vec2 frag = startFrag.xy;
    uv.xy = frag / texSize;

    vec2 delta = endFrag.xy - startFrag.xy;

    float useX = abs(delta.x) >= abs(delta.y) ? 1.f : 0.f;
    float deltaDist = mix(abs(delta.y), abs(delta.x), useX) * clamp(resolution, 0, 1);

    vec2 increment = vec2(delta.x, delta.y) / max(deltaDist, 0.001);

    float search0 = 0;
    float search1 = 0;

    int hit0 = 0;
    int hit1 = 0;

    float viewDistance = startView.z;
    float depth = thickness;

    vec4 positionTo;

    float i = 0;

    for(i = 0; i < int(deltaDist); ++i)
    {
        frag += increment;
        uv.xy = frag / texSize;
        positionTo = texture(tex_object1, uv.xy);

        search1 = mix(
        (frag.y - startFrag.y) / delta.y, (frag.x - startFrag.x)
        / delta.x, useX);

        viewDistance = (startView.z * endView.z) / mix(endView.z, startView.z, search1);

        depth = viewDistance - positionTo.z;

        if(depth > 0.f && depth < thickness)
        {
            hit0 = 1;
            break;
        }
        else
        {
            search0 = search1;
        }
    }
    search1 = search0 + ((search1 - search0) / 2);
    steps *= hit0;

    for (i = 0; i < steps; ++i) {
        frag       = mix(startFrag.xy, endFrag.xy, search1);
        uv.xy      = frag / texSize;
        positionTo = texture(tex_object1, uv.xy);

        viewDistance = (startView.z * endView.z) / mix(endView.z, startView.z, search1);
        depth        = viewDistance - positionTo.z;

        if (depth > 0 && depth < thickness) {
            hit1 = 1;
            search1 = search0 + ((search1 - search0) / 2);
        } else {
            float temp = search1;
            search1 = search1 + ((search1 - search0) / 2);
            search0 = temp;
        }
    }

    float visibility =
    hit1
    * positionTo.w
    * ( 1
    - max
    ( dot(-unitPositionFrom, reflect)
    , 0
    )
    )
    * ( 1
    - clamp
    ( depth / thickness
    , 0
    , 1
    )
    )
    * ( 1
    - clamp
    (   length(positionTo - positionFrom)
    / maxDist
    , 0
    , 1
    )
    )
    * (uv.x < 0 || uv.x > 1 ? 0 : 1)
    * (uv.y < 0 || uv.y > 1 ? 0 : 1);

//    visibility = clamp(visibility, 0, 1);

    outColor = vec4(texture(tex_object0, uv.xy).xyz, 1.f);
    return;

}
