#version 450

layout(location = 0) in vec2 texCoords;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec3 tangeant;

layout(location = 0) out vec4 outColor;

void main() {
    outColor = vec4(0.5, 0.5, 0.5, 1);
}
