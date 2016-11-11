#version 450

layout(set = 0, binding = 0, std140) uniform ColorBuffer {
    vec4 color;
};

layout(location = 0) out vec4 outColor;

void main() {
    outColor = color;
}
