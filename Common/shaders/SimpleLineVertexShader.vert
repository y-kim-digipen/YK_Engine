#version 410

uniform mat4  vertexTransform;

layout(location = 0) in vec3    vertexNormalPosition;
out vec3 rasterColor;
uniform vec3 uRasterColor;

void main() {
    rasterColor = uRasterColor;
    gl_Position = vertexTransform * vec4(vertexNormalPosition, 1.f);
}
