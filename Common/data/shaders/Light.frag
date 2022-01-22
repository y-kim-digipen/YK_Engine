#version 420 core
struct Light
{
////Light
//    vec3 Pos = vec3(0.f, 0.f, -4.f);
//
////material
//    float Ka = 0.1f;
//    float Kd = 0.5f;
//    float Ks = 0.80f;
//    vec3 I_Emissive = vec3(50, 10, 10) ;
//    float ns = 3.f;
//
////Light
//    vec3 Ia = vec3(100, 100, 100) ;
//    vec3 Id = vec3(120, 250, 10) ;
//    vec3 Is = vec3(200, 200, 200) ;
//Light
    vec3 Pos;

//material
    float Ka;
    float Kd;
    float Ks;
    vec3 I_Emissive;
    float ns;

//Light
    vec3 Ia;
    vec3 Id;
    vec3 Is;
};