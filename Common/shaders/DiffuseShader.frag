#version 420 core

in VS_OUT
{
    vec3 fragDiffuse;
    vec3 fragNormal;

} fs_in;

out vec3 fragColor;

uniform vec3 directionalLight = vec3( 0.0, 1.0, 1.0 );

void main()
{
    vec3 ambient = fs_in.fragDiffuse * 0.1f;
    float N_dot_L = max( dot( normalize(directionalLight), normalize(fs_in.fragNormal)), 0.0f );
	fragColor = max(ambient, fs_in.fragDiffuse * N_dot_L);
}
