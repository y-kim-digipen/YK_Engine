#version 410

uniform mat4  vertexTransform;

layout(location = 0) in vec3    FaceNormalPosition;
out vec3 rasterColor;
uniform vec3 uRasterColor;

void main() {
    rasterColor = vec3(1.f, 0.f, 0.f);
    gl_Position = vertexTransform * vec4(FaceNormalPosition, 1.f);
}
